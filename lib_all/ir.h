#ifndef LANQIAO_IR
#define LANQIAO_IR
#include "ir_haier.h"

void IR_recv_isr(u8 level, u16 time);
void IR_recv_start();

void IR_send_load(u16 time_us);
void IR_send_nonblock();
void IR_recv_callback(HaierAcYrw02Protocol *protocol);
#endif