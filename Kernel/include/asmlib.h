#ifndef ASMLIB_H
#define ASMLIB_H

#include <stdint.h>

char *cpuVendor(char *result);

void _lrax(uint64_t rax);
uint64_t _rax();
uint64_t _rbx();
uint64_t _rcx();
uint64_t _rdx();
uint64_t _rbp();
uint64_t _rsi();
uint64_t _rdi();
uint64_t _rsp();
uint64_t _r8();
uint64_t _r9();
uint64_t _r10();
uint64_t _r11();
uint64_t _r12();
uint64_t _r13();
uint64_t _r14();
uint64_t _r15();
uint64_t _rip();

uint32_t _eax();
uint32_t _ebx();
uint32_t _ecx();
uint32_t _edx();

uint16_t _readfl();
void _hlt();
void _lhlt();
void _lidt(void * idtr);
void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);
void _cli();
void _sti();


void _irq00handler();
void _irq01handler();
void _syscall_handler();

char kb_read();
uint64_t rtc(uint64_t mode);

uint64_t _in(uint16_t port);
uint64_t _out(uint16_t port,uint16_t value);

void enter_region(uint64_t * lock);
void leave_region(uint64_t * lock);
uint64_t try_to_lock(uint64_t * lock);
#endif
