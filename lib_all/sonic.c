#include "utils.h"
#include "timer.h"
#include "INTRINS.H"

#define SONIC_PIN_SEND P10
#define SONIC_PIN_RECV P11

void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void sonic_send(){
    u8 i=0;
    for (i = 0; i < 8; i++)
    {
        SONIC_PIN_SEND = 0;
        Delay12us();
        SONIC_PIN_SEND = 1;
        Delay12us();
    }
    
}

void sonic_detect(){
    P11 = 1;
    PCA_sonic_start();
    sonic_send();
    CR = 1;
}

/* must be called after detect+180us
*/
float sonic_read_distance(){
    float temp;
    if((CL == 0) && (CR == 0))
        return 999;
    temp = ((float)CCAP0H*256 + (float)CCAP0L) * 0.017;
    CR = 0; // not must
    return temp;
}

