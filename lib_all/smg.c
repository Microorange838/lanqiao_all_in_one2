#include "smg.h"

u8 xdata smg_model_num[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
u8 smg_duan[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
u8 smg_wei[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


void smg_update(){
    static u8 now_smg=0;
    P0 = 0x00;
    IO_COM;
    IO_RELEASE;
    P0 = smg_wei[now_smg];
    IO_SMG;
    IO_RELEASE;
    P0 = smg_duan[now_smg];
    IO_COM;
    IO_RELEASE;
    now_smg++;
    if(now_smg == 8)
        now_smg=0;
}

void smg_set_num(u8 num, u8 pos, bool is_point){
    u8 temp = smg_model_num[num];
    if(is_point)
        temp &= 0x7F;
    smg_wei[pos] = temp;
}

void smg_debug_DEC(u16 num){
    smg_set_num(GET_DEC_1(num), 4, false);
    smg_set_num(GET_DEC_10(num), 3, false);
    smg_set_num(GET_DEC_1e2(num), 2, false);
    smg_set_num(GET_DEC_1e3(num), 1, false);
    smg_set_num(GET_DEC_1e4(num), 0, false);
}
