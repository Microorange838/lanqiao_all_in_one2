#include "utils.h"
#include "led.h"
#include "timer.h"
#include "smg.h"
#include "uart.h"
#include "ir_haier.h"

u16 xdata ir_buff_pos=0;
u16 xdata ir_send_buffer[200];
u16 data ir_send_pos=0;

void IR_send_load(u16 time_us){
    ir_send_buffer[ir_buff_pos] = time_us;
    ir_buff_pos++;
}

void IR_send_start(){
    u16 i;
    u8 level;

    syspulse_enter_critic();
    for ( i = 0; i < ir_buff_pos; i++)
    {
        if(i%2 == 0)
            level = LEVEL_HIGH;
        else
            level = LEVEL_LOW;
        PCA_IR_send_level(level, ir_send_buffer[i]);
        while(CCF2==0);
        CCF2=0;
        PCA_IR_send_stop();
    }
    syspulse_exit_critic();
    ir_send_pos = 0;
}


void IR_send_nonblock(){
    u8 level;
    if(ir_buff_pos){
        if(ir_send_pos%2==0)
            level = LEVEL_HIGH;
        else
            level = LEVEL_LOW;
        PCA_IR_send_level(level, ir_send_buffer[ir_send_pos]);

        ir_send_pos++;
        /// @brief 发送完毕
        if(ir_send_pos == ir_buff_pos){
            ir_buff_pos = 0;
            ir_send_pos = 0;
        }
    }
    
}

void IR_recv_start(){
    PCA_IR_recv_start();
}

void IR_recv_callback(HaierAcYrw02Protocol *protocol){
    smg_debug_DEC(protocol->pbit.Temp+kHaierAcYrw02MinTempC);
}

/*per time: 1 us
level : same as TRANSMITER!!!
*/
void IR_recv_isr(u8 level, u16 time){
    IR_haier_recv_single(level, time);
}