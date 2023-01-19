#include "menu.h"
#include "led.h"
#include "timer.h"
#include "uart.h"

void init_all(){
    syspulse_init();
    uart1_init();
}

int main(){
    init_all();
    while (1)
    {
        sys_loop();
    }
}