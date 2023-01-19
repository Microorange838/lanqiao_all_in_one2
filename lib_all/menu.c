#include "utils.h"
#include "keyboard.h"
#include "led.h"
#include "smg.h"
#include "timer.h"

t_flag led_update_flag, led_pattern_flag, freq_flag;
u16 led_pattern_cycle = 400;

void sys_loop(){
    u16 data temp;
    if(READ_FLAG(led_update_flag)){
        led_update();
    }
    if(READ_FLAG(led_pattern_flag)){
        led_pattern_update();
    }
    if(READ_FLAG(freq_flag)){
        temp = Timer_freq_result();
        Timer_freq_start();
        
    }
}

/*
execute in interrupt: real-time  less resource
execute through event: less important  more resource
    @note:syspulse software prescaler
*/
void syspulse_callback() interrupt 3
{
    static u16 prescaler_10=0;
    static u16 prescaler_100=0;
    static u16 prescaler_1000=0;
    static u16 prescaler_led_pattern=0;
    prescaler_10++;
    prescaler_100++;
    prescaler_1000++;
    prescaler_led_pattern++;
    /*1ms
    */
    smg_update();
    SET_FLAG(led_update_flag);
    if(prescaler_10 == 10){
        prescaler_10=0;
        key_scan();
        
    }
    if (prescaler_100 == 100)
    {
        prescaler_100=0;

    }
    if(prescaler_1000 == 1000){
        prescaler_1000=0;
        SET_FLAG(freq_flag);
        
    }
    if (prescaler_led_pattern == led_pattern_cycle)
    {
        prescaler_led_pattern=0;
        SET_FLAG(led_pattern_flag);
    }
    
}