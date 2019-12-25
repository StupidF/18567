#ifndef _ds1302_h_
#define _ds1302_h_


#include "reg52.h"
#include <intrins.h>

#define uchar unsigned char
#define uint  unsigned int


void Init_timer2(void);
void ds1302_read_time(void);
void ds1302_write_time(void);
uchar ds1302_read_byte(uchar addr) ;
void ds1302_write_byte(uchar addr, uchar d);
#endif
