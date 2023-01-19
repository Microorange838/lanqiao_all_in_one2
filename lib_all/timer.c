#include "utils.h"
#include "ir.h"
#include "timer.h"
#include "uart.h"
#include "smg.h"

/*  @note:Timer 1
    @note:Used for syspulse
*/
void syspulse_init(){ 
    AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
    ET1 = 1; //开中断
    PT1 = 0;
    EA = 1;
}

void syspulse_enter_critic(){
    ET1 = 0;
}

void syspulse_exit_critic(){
    ET1 = 1;
}

/*  @note:Timer0 used for freq
*/
void Timer_freq_start(){

	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
    TMOD |= 0x04;    //配置为计数器
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x00;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

}

u16 Timer_freq_result(){
    TR0 = 0;
    return (TH0*256 + TL0);
}

void PCA_sonic_start(){
    CMOD = 0;  //syscl/12(1us)  关主定时器中断
    CF = 0;
    CR = 0; //关PCA
    CCAPM0 = 1<<4;  //CCP通道0 输入捕获 下降沿 关中断
    CL = 0; //清空PCA计数器
    CH = 0;
    CCAP0L = 0; //清空 捕获比较计数器
    CCAP0H = 0;
    AUXR1 &= 0xCF; //第一组脚位
}

void PCA_IR_recv_start(){
    CMOD = 0x00; //sysclk/12(1us) 关主定时器中断
    CF = 0;
    CR = 0;
    CL = 0;
    CH = 0;
    CCAPM0 = 0x11; //CCP通道0 输入捕获 下降沿 开中断
    CCF0 = 0; //CCP中断flag
    CCAP0L = 0;
    CCAP0H = 0;
    AUXR1 &= 0xCF; //第一组脚位
}

void PCA_IR_recv_up_start(){
    CR = 0; //重置PCA主定时器
    CL = 0;
    CH = 0;
    CCAPM0 = 0x21; //CCP通道0 输入捕获 上升沿 开中断
    CR = 1;
}

void PCA_IR_recv_down_start(){
    CR = 0; //重置PCA主定时器
    CL = 0;
    CH = 0;
    CCAPM0 = 0x11; //CCP通道0 输入捕获 下降沿 开中断
    CR = 1;
}

void PCA_IR_recv_stop(){
    PCA_reset();
}

/* 红外发送相关
*/
//比较输出翻转间隔
#define PCA_IR_38K_CYCLE 26 //38kHz
//微秒转换为分频后tick因子
#define PCA_IR_US_FACTOR 2
u16 data PCA_IR_compare_value = 0;
u8 suc_bit = 0;

void PCA_IR_send_level(u8 level, u16 time_us){
    time_us *= PCA_IR_US_FACTOR;
    P10 = 1;   //又是反的  红外LED脚 不用PP
    CR = 0;
    CMOD = 0x0C; //sysclk/6 (0.5us) 关主定时器中断
    PPCA = 1; //必须加！！！必须加 复位不为1
    CF = 0;
    CL = 0;
    CH = 0;
    if(level == LEVEL_HIGH)
        CCAPM1 = 0x0D; //CCP通道1 输出比较Toggle 开中断
    else
        CCAPM1 = 0x00; //CCP通道1 Disable
    CCAPM2 = 0x09; //CCP通道2 软定时器 开中断
    CCF1 = 0; //CCP中断flag
    CCF2 = 0;
    PCA_IR_compare_value += PCA_IR_38K_CYCLE;
    CCAP1L = 0; // 确保触发第一次中断
    CCAP1H = 0;
    CCAP2L = time_us; //设置为发送完毕时间
    CCAP2H = time_us>>8;
    AUXR1 &= 0xCF; //第一组脚位
    CR = 1;
    
}


void PCA_IR_send_stop(){
    CR = 0;
    CCAPM0 = 0;
    CCAPM1 = 0;
    CCAPM2 = 0;
    PCA_IR_compare_value = 0;
}

void PCA_debug(){
    CR = 0;
    CMOD = 0x0C; //sysclk/6 (0.5us) 关主定时器中断
    PPCA = 1; //必须加！！！必须加 复位不为1
    CF = 0;
    CL = 0;
    CH = 0;
    CCF1 = 0; //CCP中断flag
    CCF2 = 0;
    CCAPM1 = 0x0D; //CCP通道1 输出比较Toggle 开中断 关比较！！！ 写CCAP1时比较自动打开
    CCAPM2 = 0x09; //CCP通道1 输出比较Toggle 开中断 关比较！！！ 写CCAP1时比较自动打开
    PCA_IR_compare_value += PCA_IR_38K_CYCLE;
    CCAP1L = 0; // 确保触发第一次中断
    CCAP1H = 0;
    CCAP2L = 0xCF;
    CCAP2H = 0xCF;
    AUXR1 &= 0xCF; //第一组脚位
    CR = 1;
}

void PCA_reset(){
    CR = 0;
    CMOD = 0;
    CL = 0;
    CH = 0;
    CCAPM0 = 0;
    CCAPM1 = 0;
    CCAPM2 = 0;
}


void PCA_isr() interrupt 7{
    if(CCF1 == 1){
        CCF1 = 0;//CCP通道1 发送通道
        CCAP1L = PCA_IR_compare_value;
        CCAP1H = PCA_IR_compare_value>>8;
        PCA_IR_compare_value += PCA_IR_38K_CYCLE;
    }
    if(CCF0 == 1){
        CCF0 = 0; //CCP通道0 中断标志
        if(CCAPM0 & (1<<4)){
            //下降沿  
            //注意1838接收头与发送电平相反
            IR_recv_isr(0, (CCAP0H*256+CCAP0L));
            PCA_IR_recv_up_start();
        }else{
            //上升沿 
            IR_recv_isr(1, (CCAP0H*256+CCAP0L));
            PCA_IR_recv_down_start();
        }

    }
    if(CCF2 == 1){
        CCF2 = 0;
        PCA_IR_send_stop();
        IR_send_nonblock();
    }

}




