#ifndef LANQIAO_TIMER
#define LANQIAO_TIMER

void syspulse_init();
void syspulse_enter_critic();
void syspulse_exit_critic();

void Timer_freq_start();
u16 Timer_freq_result();

void PCA_sonic_start();

void PCA_IR_recv_start();

void PCA_IR_send_level(u8 level, u16 time_us);
void PCA_IR_send_stop();

void PCA_debug();
void PCA_reset();
#endif