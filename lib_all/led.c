#include "led.h"


u8 led_pattern1[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
u8 led_pattern2[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
u8 led_pattern3[8] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
u8 led_pattern4[8] = {0x81, 0x42, 0x24, 0x18, 0x81, 0x42, 0x24, 0x18};
u8 led_pattern5[8] = {0x18, 0x24, 0x42, 0x81, 0x18, 0x24, 0x42, 0x81};
u8 led_clear[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

u8 brightness = 2;
#define LED_MAX_BRIGHTNESS 5

u8 led_mode = LED_SINGLE;
u8 *now_pattern = led_clear;
u8 now_led = 0x00;

/*execute cycle 1ms
*/
void led_update(){
    u8 brightness_count = 0;
    if(brightness >= brightness_count){
        P0 = ~now_led;
        IO_LED;
        IO_RELEASE;
    }else{
        P0 = 0xFF;
        IO_LED;
        IO_RELEASE;
    }
    brightness_count++;
    if(brightness_count == LED_MAX_BRIGHTNESS)
        brightness_count = 0;
}

/*execute cylce ($pattern)ms
*/
void led_pattern_update(){
    static u8 pattern_cnt = 0;
    if (led_mode == LED_PATTERN)
    {
        now_led = now_pattern[pattern_cnt];
    }
    pattern_cnt++;
    if (pattern_cnt == 8)
        pattern_cnt = 0;
}

void led_set_pattern(u8 id){
    led_mode = LED_PATTERN;
    switch (id)
    {
    case 1:
        now_pattern = led_pattern1;
        break;
    case 2:
        now_pattern = led_pattern2;
        break;
    case 3:
        now_pattern = led_pattern3;
        break;
    case 4:
        now_pattern = led_pattern4;
        break;
    case 5:
        now_pattern = led_pattern5;
        break;        
    default:
        now_pattern = led_clear;
        break;
    }
}   

void led_set_single(u8 p0){
    led_mode = LED_SINGLE;
    now_led = p0;
}
