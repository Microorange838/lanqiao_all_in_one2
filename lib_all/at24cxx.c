#include "iic.h"
#include "utils.h"

#define AT24C02_WRITE_ADDR 0xA0
#define AT24C02_READ_ADDR 0xA1

void at24c02_write_byte(u8 addr, u8 val){
    IIC_Start();
    IIC_SendByte(AT24C02_WRITE_ADDR);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
    IIC_SendByte(val);
    IIC_WaitAck();
    IIC_Stop();
}

u8 at24c02_read_byte(u8 addr){
    u8 temp;
    IIC_Start();
    IIC_SendByte(AT24C02_WRITE_ADDR);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
    IIC_Start();
    IIC_SendByte(AT24C02_READ_ADDR);
    IIC_WaitAck();
    temp = IIC_RecByte();
    IIC_SendAck(IIC_NOACK);
    IIC_Stop();
    return temp;
}