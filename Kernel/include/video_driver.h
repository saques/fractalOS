#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H
#define PULSE_TIME 12
#include <stdint.h>
void initialize_driver();
void naive_print();
void clear_screen();
void print_char(uint8_t character);
void print_string(char * str);
void print_newline();
void print_pixel(uint32_t p,uint32_t q,uint32_t ccoord);
void pulse();
void sys_set_color(uint8_t r, uint8_t g, uint8_t b);
void set_back_color(uint8_t r, uint8_t g, uint8_t b);
#endif
