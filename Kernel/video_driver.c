#include <stdint.h>
#include <font_bitmaps.h>

#define WIDTH 1024
#define HEIGHT 768
#define bPP 3
#define C_WIDTH 8
#define C_HEIGHT 16


static void draw_char(uint8_t *where, uint8_t character) ;


uint8_t * fb;
static int i=0;
static int j=0;

/*
 * This function MUST be
 * called before using any
 * other functions of the
 * driver
 */
void initialize_driver(){
	/*
	* Linear framebuffer as found in
	* Pure64 docs
	*/
	fb= (uint8_t *)(*(uint32_t *)0x5080);
}

/*
 * Test function:
 * it prints all over
 * the screen
 */
void naive_print(){
	for(i=0;i<WIDTH*HEIGHT*bPP;i++){
		fb[i]=(uint8_t)i;
	}
	i=0;
}

void clear_screen(){
	for(i=0;i<WIDTH*HEIGHT*bPP;i++){
		fb[i]=0;
	}
	i=0;
	j=0;
}

void print_char(uint8_t character){
	draw_char((uint8_t*)fb+(i*WIDTH*C_HEIGHT+j*C_WIDTH)*bPP,
			  character);
	j++;
	if(j>=(WIDTH/C_WIDTH)){
		j=0;
		i++;
		if(i>=(HEIGHT/C_HEIGHT)){
			clear_screen();
		}
	}
}
void print(const uint8_t * sptr, uint8_t length) {
		while (length--) {
			switch (*sptr) {
				case '\b':
					print_backspace();
					break;
				case '\n':
					print_newline();
					break;
				default:
					print_char(*sptr);
					break;
				}
			sptr++;
		}
}

void print_string(char * str){
	while(*str!=0){
		print_char(*str);
		str++;
	}
}

void print_newline(){
	int bk=j,count=WIDTH/C_WIDTH;
	while(bk<count){
		print_char(' ');
		bk++;
	}
}

void print_backspace() {
	if (j != 0) {
		j--;
		print_char(' ');
		j--;
	}
	else {
		if (i != 0) {
			i--;
			j = WIDTH / C_WIDTH - 1;
			print_char(' ');
			i--;
			j = WIDTH / C_WIDTH - 1;
		}
	}
}

/*
 * p row
 * q column
 * ccoord: 0x00BBGGRR
 */
void print_pixel(uint32_t x,uint32_t y,uint32_t ccoord){
	uint8_t * pixel = fb + (x*WIDTH+y)*bPP;
	pixel[0]=(uint8_t)(ccoord>>16);
	pixel[1]=(uint8_t)(ccoord>>8);
	pixel[2]=(uint8_t)(ccoord);
}

/*
 * Adaptation of example found at osdev.org
 * by saques
 */
static void draw_char(uint8_t *where, uint8_t character) {
    int q;
    int row;
    uint8_t * tmp=where;
    uint8_t row_data;
    const uint8_t *font_data_for_char = &__font_bitmap__[(character - 0x20)*0x10];
    for (row = 0; row < C_HEIGHT; row++) {
        row_data = font_data_for_char[row];
		for(q=0;q<C_WIDTH;q++){
			if((row_data>>(8-q-1))&0x01){
				tmp[0]=0xFF;tmp[1]=0XFF;tmp[2]=0XFF;
			} else {
				tmp[0]=0x00;tmp[1]=0x00;tmp[2]=0x00;
			}
			tmp+=3;
		}
        where += WIDTH*bPP;
        tmp=where;
    }
}

void pulse() {
	static uint8_t b = 0;
	static uint8_t count = 0;
	if (count > PULSE_TIME) {
		count = 0;
		if (b) {
			draw_char((uint8_t*)fb+(i*WIDTH*C_HEIGHT+j*C_WIDTH)*bPP,
					  '|');
			b = 0;
		}
		else {
			draw_char((uint8_t*)fb+(i*WIDTH*C_HEIGHT+j*C_WIDTH)*bPP,
					  ' ');
			b = 1;
		}
	}
	else {
		count++;
	}

}