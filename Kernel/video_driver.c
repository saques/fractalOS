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
	draw_char((uint8_t*)fb+i*WIDTH*C_HEIGHT*bPP+j*C_WIDTH*bPP,
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
				tmp[0]=0xFF;tmp[1]=0xFF;tmp[2]=0xFF;
			} else {
				tmp[0]=0x00;tmp[1]=0x00;tmp[2]=0x00;
			}
			tmp+=3;
		}
        where += WIDTH*bPP;
        tmp=where;
    }
}
