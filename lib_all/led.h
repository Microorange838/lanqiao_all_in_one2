#ifndef LANQIAO_LED
#define LANQIAO_LED

#include "utils.h"

enum {
    LED_PATTERN,
    LED_SINGLE
};

void led_set_single(u8 p0);
void led_set_pattern(u8 id);


void led_update();
void led_pattern_update();

#endif