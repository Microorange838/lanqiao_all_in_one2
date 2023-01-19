#include "utils.h"
#include "uart.h"

#define UART_TRANSMIT_DONE_MASK 0x02
#define UART_RECEIVE_DONE_MASK 0x01


void uart1_init(){		//115200bps@12.000MHz

	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE6;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}

void uart1_transmit(u8 val){
    SBUF = val;
    while(!(SCON&UART_TRANSMIT_DONE_MASK));
    SCON &= (~UART_TRANSMIT_DONE_MASK);
}

