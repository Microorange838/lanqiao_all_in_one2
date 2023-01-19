#include "utils.h"
#include "iic.h"


#define PCF8591_ADDR_WRITE 0x90
#define PCF8591_ADDR_READ 0x91

void pcf8591_read_adc(u8 channel){
    u8 temp;
    IIC_Start();
    IIC_SendByte(PCF8591_ADDR_WRITE);
    IIC_WaitAck();
    IIC_SendByte(channel);
    IIC_WaitAck();
    IIC_Start();
    IIC_SendByte(PCF8591_ADDR_READ);
    IIC_WaitAck();
    IIC_RecByte();
    IIC_SendAck(IIC_ACK);
    temp = IIC_RecByte();
    IIC_SendAck(IIC_NOACK);
    IIC_Stop();
}

void pcf8591_write_dac(u8 votage){
    IIC_Start();
    IIC_SendByte(PCF8591_ADDR_WRITE);
    IIC_WaitAck();
    IIC_SendByte(1<<6);
    IIC_WaitAck();
    IIC_SendByte(votage);
    IIC_WaitAck();
    IIC_SendByte(votage);
    IIC_WaitAck();
    IIC_Stop();
}