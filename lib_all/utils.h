#ifndef LANQIAO_UTILS
#define LANQIAO_UTILS

#include "STC/STC15F2K60S2.H"
#include "MATH.H"

typedef unsigned char t_flag;
typedef unsigned char bool;
typedef unsigned char u8;
typedef unsigned char uint8_t;
typedef unsigned short u16;
typedef unsigned short uint16_t;

#define true 1
#define false 0
#define LEVEL_HIGH 1
#define LEVEL_LOW 0

#define OSC_FREQ 12000000L

#define SET_BIT(VAL,BIT) ((VAL)|(1<<(BIT)))
#define RESET_BIT(VAL,BIT) ((VAL)&(~(1<<(BIT))))

#ifndef BIT
#define BIT(x, n) (((x) >> (n)) & 1)
#endif

#define IO_RELEASE {P2&=0x1F;}
#define IO_LED {P2&=0x1F;P2|=0x80;}
#define IO_OTH {P2&=0x1F;P2|=0xA0;}
#define IO_COM {P2&=0x1F;P2|=0xC0;}
#define IO_SMG {P2&=0x1F;P2|=0xE0;}

#define SET_FLAG(X) (X=1)
#define READ_FLAG(X) ((X)==0?0:((X)--))

#define GET_DEC_1(X) ((X)%10)
#define GET_DEC_10(X) (((X)/10)%10)
#define GET_DEC_1e2(X) (((X)/100)%10)
#define GET_DEC_1e3(X) (((X)/1000)%10)
#define GET_DEC_1e4(X) (((X)/10000)%10)
#define GET_DEC_1e5(X) (((X)/100000)%10)

//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

#define MAX(x, y)  ((x) > (y) ? (x) : (y))
#define MIN(x, y)  ((x) < (y) ? (x) : (y))

#endif