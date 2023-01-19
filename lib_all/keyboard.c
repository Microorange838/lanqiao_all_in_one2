#include "utils.h"
#include "led.h"
#include "smg.h"
#include "at24cxx.h"
#include "sonic.h"
#include "ir.h"
#include "uart.h"
#include "timer.h"
#include "ir_haier.h"

u8 key_cons = 0;
u16 key_time[4] = {0, 0, 0, 0};
HaierAcYrw02Protocol xdata ir_prt;

#define KEYN_MASK(n) (1<<n)

void key7_call_back(u8 is_long){
    u16 temp;
    if(is_long){
    }else{
        
    }
}

void key6_call_back(u8 is_long){
    if(is_long){
        led_set_single(0x03);
    }else{
        led_set_single(0x04);
        
    }
}

void key5_call_back(u8 is_long){
    if(is_long){
        led_set_single(0x05);
        IR_recv_start();
    }else{
        led_set_single(0x06);
        stateReset(&ir_prt);
        IR_haier_send(&ir_prt);
    }
}

void key4_call_back(u8 is_long){
    if(is_long){
        led_set_single(0x07);
    }else{
        led_set_single(0x08);

    }
}


/*execute cycle 10ms
*/
void key_scan(){
    u8 key = P3 ^ 0xFF;
    if(key & KEYN_MASK(0)){
        key_time[0]++;
        if(key_time[0] == 100)
            key7_call_back(1);
    }
    else{
        if(key_time[0]>3 && key_time[0]<80)
            key7_call_back(0);
        key_time[0] = 0;
    }

    if(key & KEYN_MASK(1)){
        key_time[1]++;
        if(key_time[1] == 100)
            key6_call_back(1);
    }
    else{
        if(key_time[1]>3 && key_time[1]<80)
            key6_call_back(0);
        key_time[1] = 0;
    }
        
    if(key & KEYN_MASK(2)){
        key_time[2]++;
        if(key_time[2] == 100)
            key5_call_back(1);
    }
    else{
        if(key_time[2]>3 && key_time[2]<80)
            key5_call_back(0);
        key_time[2] = 0;
    }

    if(key & KEYN_MASK(3)){
        key_time[3]++;
        if(key_time[3] == 100)
            key4_call_back(1);
    }
    else{
        if(key_time[3]>3 && key_time[3]<80)
            key4_call_back(0);
        key_time[3] = 0;
    }

}