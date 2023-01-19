#ifndef LANQIAO_IR_HAIER
#define LANQIAO_IR_HAIER
#include "utils.h"

#define kHaierAcYrw02MinTempC 16
#define kHaierAcYrw02MaxTempC 30
#define kHaierAcYrw02MinTempF 60
#define kHaierAcYrw02MaxTempF 86
#define kHaierAcYrw02DefTempC 25

#define kHaierAcYrw02ModelA 0xA6
#define kHaierAcYrw02ModelB 0x59
#define kHaierAc176Prefix 0xB7
#define kHaierAc160Prefix 0xB5

#define kHaierAcYrw02SwingVOff 0x0
#define kHaierAcYrw02SwingVTop 0x1
#define kHaierAcYrw02SwingVMiddle 0x2  // Not available in heat mode.
#define kHaierAcYrw02SwingVBottom 0x3  // Only available in heat mode.
#define kHaierAcYrw02SwingVDown 0xA
#define kHaierAcYrw02SwingVAuto 0xC  // Airflow

#define kHaierAc160SwingVOff     0x00
#define kHaierAc160SwingVTop     0x01
#define kHaierAc160SwingVHighest 0x02
#define kHaierAc160SwingVHigh    0x04
#define kHaierAc160SwingVMiddle  0x06
#define kHaierAc160SwingVLow     0x08
#define kHaierAc160SwingVLowest  0x03
#define kHaierAc160SwingVAuto    0x0C  // Airflow

#define kHaierAcYrw02SwingHMiddle 0x0
#define kHaierAcYrw02SwingHLeftMax 0x3
#define kHaierAcYrw02SwingHLeft 0x4
#define kHaierAcYrw02SwingHRight 0x5
#define kHaierAcYrw02SwingHRightMax 0x6
#define kHaierAcYrw02SwingHAuto 0x7

#define kHaierAcYrw02FanHigh 1
#define kHaierAcYrw02FanMed  2
#define kHaierAcYrw02FanLow  3
#define kHaierAcYrw02FanAuto 5  // HAIER_AC176 uses `0` in Fan2

#define kHaierAcYrw02Auto 0  // 0
#define kHaierAcYrw02Cool 1  // 1
#define kHaierAcYrw02Dry  2  // 2
#define kHaierAcYrw02Heat 4  // 4
#define kHaierAcYrw02Fan  5  // 5


#define kHaierAcYrw02ButtonTempUp   0x00
#define kHaierAcYrw02ButtonTempDown 0x01
#define kHaierAcYrw02ButtonSwingV   0x02
#define kHaierAcYrw02ButtonSwingH   0x03
#define kHaierAcYrw02ButtonFan      0x04
#define kHaierAcYrw02ButtonPower    0x05
#define kHaierAcYrw02ButtonMode     0x06
#define kHaierAcYrw02ButtonHealth   0x07
#define kHaierAcYrw02ButtonTurbo    0x08
#define kHaierAcYrw02ButtonSleep    11
#define kHaierAcYrw02ButtonTimer    16
#define kHaierAcYrw02ButtonLock     20
#define kHaierAc160ButtonLight      21
#define kHaierAc160ButtonAuxHeating 22
#define kHaierAc160ButtonClean      25
#define kHaierAcYrw02ButtonCFAB     26

#define kHaierAcYrw02NoTimers       0
#define kHaierAcYrw02OffTimer       1
#define kHaierAcYrw02OnTimer        2
#define kHaierAcYrw02OnThenOffTimer 4
#define kHaierAcYrw02OffThenOnTimer 5

#define kHaierACYRW02StateLength 14
#define kHaierACYRW02Bits 112


#define kHaierAcHdr 3000
#define kHaierAcHdrGap 4300
#define kHaierAcBitMark 520
#define kHaierAcOneSpace 1650
#define kHaierAcZeroSpace 600
#define kHaierAcMinGap 30000  // Completely made up value.

typedef struct {
    // Byte 0
    uint8_t Model       :8;
    // Byte 1
    uint8_t SwingV      :4;
    uint8_t Temp        :4;  // 16C~30C
    // Byte 2
    uint8_t             :5;
    uint8_t SwingH      :3;
    // Byte 3
    uint8_t             :1;
    uint8_t Health      :1;
    uint8_t             :3;
    uint8_t TimerMode   :3;
    // Byte 4
    uint8_t             :6;
    uint8_t Power       :1;
    uint8_t             :1;
    // Byte 5
    uint8_t OffTimerHrs :5;
    uint8_t Fan         :3;
    // Byte 6
    uint8_t OffTimerMins:6;
    uint8_t Turbo       :1;
    uint8_t Quiet       :1;
    // Byte 7
    uint8_t OnTimerHrs  :5;
    uint8_t Mode        :3;
    // Byte 8
    uint8_t OnTimerMins :6;
    uint8_t             :1;
    uint8_t Sleep       :1;
    // Byte 9
    uint8_t             :8;
    // Byte 10
    uint8_t ExtraDegreeF :1;
    uint8_t              :4;
    uint8_t UseFahrenheit:1;
    uint8_t              :2;
    // Byte 11
    uint8_t             :8;
    // Byte 12
    uint8_t Button      :5;
    uint8_t Lock        :1;
    uint8_t             :2;
    // Byte 13
    uint8_t Sum         :8;
} HaierAcYrw02PBit;


typedef union{
    uint8_t raw[kHaierACYRW02StateLength];  ///< The state in native form
    HaierAcYrw02PBit pbit;
}HaierAcYrw02Protocol;

typedef enum {
    HAIER_IR_RECV_STATE_IDLE,
    HAIER_IR_RECV_STATE_HEAD1,
    HAIER_IR_RECV_STATE_HEAD2,
    HAIER_IR_RECV_STATE_HEAD3,
    HAIER_IR_RECV_STATE_DATA
}Haier_IR_recv_state;

void stateReset(HaierAcYrw02Protocol *yrw02_protocol);
void checksum(HaierAcYrw02Protocol *yrw02_protocol);
uint8_t* getRaw(HaierAcYrw02Protocol *yrw02_protocol);
void setRaw(HaierAcYrw02Protocol *yrw02_protocol, const uint8_t new_code[]);

void setButton(HaierAcYrw02Protocol *yrw02_protocol, uint8_t button);
u8 getModel(HaierAcYrw02Protocol *yrw02_protocol);
void setModel(HaierAcYrw02Protocol *yrw02_protocol, u8 model);
uint8_t getButton(HaierAcYrw02Protocol *yrw02_protocol);
void setMode(HaierAcYrw02Protocol *yrw02_protocol, uint8_t mode);
uint8_t getMode(HaierAcYrw02Protocol *yrw02_protocol);
void setUseFahrenheit(HaierAcYrw02Protocol *yrw02_protocol, const bool on);
bool getUseFahrenheit(HaierAcYrw02Protocol *yrw02_protocol);
void setTemp(HaierAcYrw02Protocol *yrw02_protocol, const uint8_t degree, const bool fahrenheit);
uint8_t getTemp(HaierAcYrw02Protocol *yrw02_protocol);
void setHealth(HaierAcYrw02Protocol *yrw02_protocol,const bool on);
bool getHealth(HaierAcYrw02Protocol *yrw02_protocol);
bool getPower(HaierAcYrw02Protocol *yrw02_protocol);
void setPower(HaierAcYrw02Protocol *yrw02_protocol, const bool on);
void on(HaierAcYrw02Protocol *yrw02_protocol);
void off(HaierAcYrw02Protocol *yrw02_protocol);
bool getSleep(HaierAcYrw02Protocol *yrw02_protocol);
void setSleep(HaierAcYrw02Protocol *yrw02_protocol, const bool on);
bool getTurbo(HaierAcYrw02Protocol *yrw02_protocol);
void setTurbo(HaierAcYrw02Protocol *yrw02_protocol, const bool on);
bool getQuiet(HaierAcYrw02Protocol *yrw02_protocol);
void setQuiet(HaierAcYrw02Protocol *yrw02_protocol, const bool on);
uint8_t getFan(HaierAcYrw02Protocol *yrw02_protocol);
void setFan(HaierAcYrw02Protocol *yrw02_protocol, uint8_t speed);
uint8_t getSwing(HaierAcYrw02Protocol *yrw02_protocol);
uint8_t getSwingV(HaierAcYrw02Protocol *yrw02_protocol);
void setSwingV(HaierAcYrw02Protocol *yrw02_protocol, uint8_t pos);
uint8_t getSwingH(HaierAcYrw02Protocol *yrw02_protocol);
void setSwingH(HaierAcYrw02Protocol *yrw02_protocol, uint8_t pos);
void setTimerMode(HaierAcYrw02Protocol *yrw02_protocol, const uint8_t mode);
uint8_t getTimerMode(HaierAcYrw02Protocol *yrw02_protocol);
void setOnTimer(HaierAcYrw02Protocol *yrw02_protocol, const uint16_t mins);
void setOffTimer(HaierAcYrw02Protocol *yrw02_protocol, const uint16_t mins);
uint16_t getOnTimer(HaierAcYrw02Protocol *yrw02_protocol);
uint16_t getOffTimer(HaierAcYrw02Protocol *yrw02_protocol);
bool getLock(HaierAcYrw02Protocol *yrw02_protocol);
void setLock(HaierAcYrw02Protocol *yrw02_protocol, const bool on);

void IR_haier_send(HaierAcYrw02Protocol *yrw02_protocol);
void IR_haier_recv_single(u8 level, u16 time);
#endif