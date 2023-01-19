#include "utils.h"
#include "ir.h"
#include "ir_haier.h"
#include "STRING.H"
#include "uart.h"

/// Reset the internal state to a fixed known good state.
void stateReset(HaierAcYrw02Protocol *yrw02_protocol) {
  memset(yrw02_protocol->raw, 0, kHaierACYRW02StateLength);
  yrw02_protocol->pbit.Model = kHaierAcYrw02ModelA;
  yrw02_protocol->pbit.Temp = kHaierAcYrw02DefTempC - kHaierAcYrw02MinTempC;
  yrw02_protocol->pbit.Health = false;
  setSwingV(yrw02_protocol, kHaierAcYrw02SwingVMiddle);
  setFan(yrw02_protocol, kHaierAcYrw02FanAuto);
  yrw02_protocol->pbit.Power = true;
  yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonPower;
}

/// Sum all the bytes of an array and return the least significant 8-bits of
/// the result.
/// @param[in] start A ptr to the start of the byte array to calculate over.
/// @param[in] length How many bytes to use in the calculation.
/// @return The 8-bit calculated result of all the bytes and init value.
uint8_t sumBytes(const uint8_t * const start, const uint16_t length) {
  uint8_t checksum = 0;
  const uint8_t *ptr;
  for (ptr = start; ptr - start < length; ptr++) checksum += *ptr;
  return checksum;
}

void checksum(HaierAcYrw02Protocol *yrw02_protocol) {
  yrw02_protocol->pbit.Sum = sumBytes(yrw02_protocol->raw, kHaierACYRW02StateLength - 1);
}

/// Get a PTR to the internal state/code for this protocol.
/// @return PTR to a code for this protocol based on the current internal state.
uint8_t* getRaw(HaierAcYrw02Protocol *yrw02_protocol) {
  checksum(yrw02_protocol);
  return yrw02_protocol->raw;
}

/// Set the internal state from a valid code for this protocol.
/// @param[in] new_code A valid code for this protocol.
void setRaw(HaierAcYrw02Protocol *yrw02_protocol, const uint8_t new_code[]) {
  memcpy(yrw02_protocol->raw, new_code, kHaierACYRW02StateLength);
}

/// Set the Button/Command setting of the A/C.
/// @param[in] button The value of the button/command that was pressed.
void setButton(HaierAcYrw02Protocol *yrw02_protocol, uint8_t button) {
  switch (button) {
    case kHaierAcYrw02ButtonTempUp:
    case kHaierAcYrw02ButtonTempDown:
    case kHaierAcYrw02ButtonSwingV:
    case kHaierAcYrw02ButtonSwingH:
    case kHaierAcYrw02ButtonFan:
    case kHaierAcYrw02ButtonPower:
    case kHaierAcYrw02ButtonMode:
    case kHaierAcYrw02ButtonHealth:
    case kHaierAcYrw02ButtonTurbo:
    case kHaierAcYrw02ButtonSleep:
    case kHaierAcYrw02ButtonLock:
    case kHaierAcYrw02ButtonCFAB:
      yrw02_protocol->pbit.Button = button;
  }
}

/// Get/Detect the model of the A/C.
/// @return The enum of the compatible model.
u8 getModel(HaierAcYrw02Protocol *yrw02_protocol) {
  return yrw02_protocol->pbit.Model;
}

/// Set the model of the A/C to emulate.
/// @param[in] model The enum of the appropriate model.
void setModel(HaierAcYrw02Protocol *yrw02_protocol, u8 model) {
  yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonCFAB;
  yrw02_protocol->pbit.Model = model;
}

/// Get the Button/Command setting of the A/C.
/// @return The value of the button/command that was pressed.
uint8_t getButton(HaierAcYrw02Protocol *yrw02_protocol){
  return yrw02_protocol->pbit.Button;
}

/// Set the operating mode of the A/C.
/// @param[in] mode The desired operating mode.
void setMode(HaierAcYrw02Protocol *yrw02_protocol, uint8_t mode) {
  switch (mode) {
    case kHaierAcYrw02Auto:
    case kHaierAcYrw02Dry:
    case kHaierAcYrw02Fan:
      // Turbo & Quiet is only available in Cool/Heat mode.
      yrw02_protocol->pbit.Turbo = false;
      yrw02_protocol->pbit.Quiet = false;
      // FALL-THRU
    case kHaierAcYrw02Cool:
    case kHaierAcYrw02Heat:
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonMode;
      yrw02_protocol->pbit.Mode = mode;
      break;
    default:
      setMode(yrw02_protocol, kHaierAcYrw02Auto);  // Unexpected, default to auto mode.
  }
}

/// Get the operating mode setting of the A/C.
/// @return The current operating mode setting.
uint8_t getMode(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Mode; }

/// Set the default temperature units to use.
/// @param[in] on Use Fahrenheit as the units.
///   true is Fahrenheit, false is Celsius.
void setUseFahrenheit(HaierAcYrw02Protocol *yrw02_protocol, const bool on) { yrw02_protocol->pbit.UseFahrenheit = on; }

/// Get the default temperature units in use.
/// @return true is Fahrenheit, false is Celsius.
bool getUseFahrenheit(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.UseFahrenheit; }

/// Set the temperature.
/// @param[in] degree The temperature in degrees.
/// @param[in] fahrenheit Use units of Fahrenheit and set that as units used.
void setTemp(HaierAcYrw02Protocol *yrw02_protocol, const uint8_t degree, const bool fahrenheit) {
  uint8_t old_temp = getTemp(yrw02_protocol);
  uint8_t temp;
  if (old_temp == degree) return;

  if (yrw02_protocol->pbit.UseFahrenheit == fahrenheit) {
    if (old_temp > degree)
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonTempDown;
    else
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonTempUp;
  } else {
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonCFAB;
  }
  yrw02_protocol->pbit.UseFahrenheit = fahrenheit;

  temp = degree;
  if (fahrenheit) {
    if (temp < kHaierAcYrw02MinTempF)
      temp = kHaierAcYrw02MinTempF;
    else if (temp > kHaierAcYrw02MaxTempF)
      temp = kHaierAcYrw02MaxTempF;
    if (degree >= 77) { temp++; }
    if (degree >= 79) { temp++; }
    // See at getTemp() comments for clarification
    yrw02_protocol->pbit.ExtraDegreeF = temp % 2;
    yrw02_protocol->pbit.Temp = (temp - kHaierAcYrw02MinTempF -yrw02_protocol->pbit.ExtraDegreeF) >> 1;
  } else {
    if (temp < kHaierAcYrw02MinTempC)
      temp = kHaierAcYrw02MinTempC;
    else if (temp > kHaierAcYrw02MaxTempC)
      temp = kHaierAcYrw02MaxTempC;
    yrw02_protocol->pbit.Temp = temp - kHaierAcYrw02MinTempC;
  }
}

/// Get the current temperature setting.
/// The unit of temperature is specified by UseFahrenheit value.
/// @return The current setting for temperature.
uint8_t getTemp(HaierAcYrw02Protocol *yrw02_protocol){
  uint8_t degree;
  if (!yrw02_protocol->pbit.UseFahrenheit) { return yrw02_protocol->pbit.Temp + kHaierAcYrw02MinTempC; }
  degree = yrw02_protocol->pbit.Temp*2 + kHaierAcYrw02MinTempF + yrw02_protocol->pbit.ExtraDegreeF;
  // The way of coding the temperature in degree Fahrenheit is
  // kHaierAcYrw02MinTempF + Temp*2 + ExtraDegreeF, for example
  // Temp = 0b0011, ExtraDegreeF = 0b1, temperature is 60 + 3*2 + 1 = 67F
  // But around 78F there is unconsistency, see table below
  //
  // | Fahrenheit | Temp   | ExtraDegreeF |
  // |    60F     | 0b0000 |     0b0      |
  // |    61F     | 0b0000 |     0b1      |
  // |    62F     | 0b0001 |     0b0      |
  // |    63F     | 0b0001 |     0b1      |
  // |    64F     | 0b0010 |     0b0      |
  // |    65F     | 0b0010 |     0b1      |
  // |    66F     | 0b0011 |     0b0      |
  // |    67F     | 0b0011 |     0b1      |
  // |    68F     | 0b0100 |     0b0      |
  // |    69F     | 0b0100 |     0b1      |
  // |    70F     | 0b0101 |     0b0      |
  // |    71F     | 0b0101 |     0b1      |
  // |    72F     | 0b0110 |     0b0      |
  // |    73F     | 0b0110 |     0b1      |
  // |    74F     | 0b0111 |     0b0      |
  // |    75F     | 0b0111 |     0b1      |
  // |    76F     | 0b1000 |     0b0      |
  // |  Not Used  | 0b1000 |     0b1      |
  // |    77F     | 0b1001 |     0b0      |
  // |  Not Used  | 0b1001 |     0b1      |
  // |    78F     | 0b1010 |     0b0      |
  // |    79F     | 0b1010 |     0b1      |
  // |    80F     | 0b1011 |     0b0      |
  // |    81F     | 0b1011 |     0b1      |
  // |    82F     | 0b1100 |     0b0      |
  // |    83F     | 0b1100 |     0b1      |
  // |    84F     | 0b1101 |     0b0      |
  // |    86F     | 0b1110 |     0b0      |
  // |    85F     | 0b1101 |     0b1      |
  if (degree >= 77) { degree--; }
  if (degree >= 79) { degree--; }
  return degree;
}

/// Set the Health (filter) setting of the A/C.
/// @param[in] on true, the setting is on. false, the setting is off.
void setHealth(HaierAcYrw02Protocol *yrw02_protocol,const bool on) {
  yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonHealth;
  yrw02_protocol->pbit.Health = on;
}

/// Get the Health (filter) setting of the A/C.
/// @return true, the setting is on. false, the setting is off.
bool getHealth(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Health; }

/// Get the value of the current power setting.
/// @return true, the setting is on. false, the setting is off.
bool getPower(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Power; }

/// Change the power setting.
/// @param[in] on true, the setting is on. false, the setting is off.
void setPower(HaierAcYrw02Protocol *yrw02_protocol, const bool on) {
  yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonPower;
  yrw02_protocol->pbit.Power = on;
}

/// Change the power setting to On.
void on(HaierAcYrw02Protocol *yrw02_protocol) { setPower(yrw02_protocol, true); }

/// Change the power setting to Off.
void off(HaierAcYrw02Protocol *yrw02_protocol) { setPower(yrw02_protocol ,false); }

/// Get the Sleep setting of the A/C.
/// @return true, the setting is on. false, the setting is off.
bool getSleep(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Sleep; }

/// Set the Sleep setting of the A/C.
/// @param[in] on true, the setting is on. false, the setting is off.
void setSleep(HaierAcYrw02Protocol *yrw02_protocol, const bool on) {
  yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonSleep;
  yrw02_protocol->pbit.Sleep = on;
}

/// Get the Turbo setting of the A/C.
/// @return The current turbo setting.
bool getTurbo(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Turbo; }

/// Set the Turbo setting of the A/C.
/// @param[in] on The desired turbo setting.
/// @note Turbo & Quiet can't be on at the same time, and only in Heat/Cool mode
void setTurbo(HaierAcYrw02Protocol *yrw02_protocol, const bool on) {
  switch (getMode(yrw02_protocol)) {
    case kHaierAcYrw02Cool:
    case kHaierAcYrw02Heat:
      yrw02_protocol->pbit.Turbo = on;
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonTurbo;
      if (on) yrw02_protocol->pbit.Quiet = false;
  }
}

/// Get the Quiet setting of the A/C.
/// @return The current Quiet setting.
bool getQuiet(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Quiet; }

/// Set the Quiet setting of the A/C.
/// @param[in] on The desired Quiet setting.
/// @note Turbo & Quiet can't be on at the same time, and only in Heat/Cool mode
void setQuiet(HaierAcYrw02Protocol *yrw02_protocol, const bool on) {
  switch (getMode(yrw02_protocol)) {
    case kHaierAcYrw02Cool:
    case kHaierAcYrw02Heat:
      yrw02_protocol->pbit.Quiet = on;
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonTurbo;
      if (on) yrw02_protocol->pbit.Turbo = false;
  }
}

/// Get the current fan speed setting.
/// @return The current fan speed.
uint8_t getFan(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Fan; }

/// Set the speed of the fan.
/// @param[in] speed The desired setting.
void setFan(HaierAcYrw02Protocol *yrw02_protocol, uint8_t speed) {
  switch (speed) {
    case kHaierAcYrw02FanLow:
    case kHaierAcYrw02FanMed:
    case kHaierAcYrw02FanHigh:
    case kHaierAcYrw02FanAuto:
      yrw02_protocol->pbit.Fan = speed;
      yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonFan;
  }
}

/// For backward compatibility. Use getSwingV() instead.
/// Get the Vertical Swing position setting of the A/C.
/// @return The native position/mode.
uint8_t getSwing(HaierAcYrw02Protocol *yrw02_protocol){
  return getSwingV(yrw02_protocol);
}

/// Get the Vertical Swing position setting of the A/C.
/// @return The native position/mode.
uint8_t getSwingV(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.SwingV; }

/// Set the Vertical Swing mode of the A/C.
/// @param[in] pos The position/mode to set the vanes to.
void setSwingV(HaierAcYrw02Protocol *yrw02_protocol, uint8_t pos) {
  uint8_t newpos = pos;
  switch (pos) {
    case kHaierAcYrw02SwingVOff:
    case kHaierAcYrw02SwingVAuto:
    case kHaierAcYrw02SwingVTop:
    case kHaierAcYrw02SwingVMiddle:
    case kHaierAcYrw02SwingVBottom:
    case kHaierAcYrw02SwingVDown: yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonSwingV; break;
    default: return;  // Unexpected value so don't do anything.
  }
  // Heat mode has no MIDDLE setting, use BOTTOM instead.
  if (pos == kHaierAcYrw02SwingVMiddle && yrw02_protocol->pbit.Mode == kHaierAcYrw02Heat)
    newpos = kHaierAcYrw02SwingVBottom;
  // BOTTOM is only allowed if we are in Heat mode, otherwise MIDDLE.
  if (pos == kHaierAcYrw02SwingVBottom && yrw02_protocol->pbit.Mode != kHaierAcYrw02Heat)
    newpos = kHaierAcYrw02SwingVMiddle;
  yrw02_protocol->pbit.SwingV = newpos;
}

/// Get the Horizontal Swing position setting of the A/C.
/// @return The native position/mode.
uint8_t getSwingH(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.SwingH; }

/// Set the Horizontal Swing mode of the A/C.
/// @param[in] pos The position/mode to set the vanes to.
void setSwingH(HaierAcYrw02Protocol *yrw02_protocol, uint8_t pos) {
  switch (pos) {
    case kHaierAcYrw02SwingHMiddle:
    case kHaierAcYrw02SwingHLeftMax:
    case kHaierAcYrw02SwingHLeft:
    case kHaierAcYrw02SwingHRight:
    case kHaierAcYrw02SwingHRightMax:
    case kHaierAcYrw02SwingHAuto: yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonSwingH; break;
    default: return;  // Unexpected value so don't do anything.
  }
  yrw02_protocol->pbit.SwingH = pos;
}


/// Set the Timer operating mode.
/// @param[in] mode The timer mode to use.
void setTimerMode(HaierAcYrw02Protocol *yrw02_protocol, const uint8_t mode) {
  yrw02_protocol->pbit.TimerMode = (mode > kHaierAcYrw02OffThenOnTimer) ? kHaierAcYrw02NoTimers
                                                     : mode;
  switch (yrw02_protocol->pbit.TimerMode) {
    case kHaierAcYrw02NoTimers:
      setOnTimer(yrw02_protocol, 0);  // Disable the On timer.
      setOffTimer(yrw02_protocol, 0);  // Disable the Off timer.
      break;
    case kHaierAcYrw02OffTimer:
      setOnTimer(yrw02_protocol, 0);  // Disable the On timer.
      break;
    case kHaierAcYrw02OnTimer:
      setOffTimer(yrw02_protocol, 0);  // Disable the Off timer.
      break;
  }
}

/// Get the Timer operating mode.
/// @return The mode of the timer is currently configured to.
uint8_t getTimerMode(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.TimerMode; }

/// Set the number of minutes of the On Timer setting.
/// @param[in] mins Nr. of Minutes for the Timer. `0` means disable the timer.
void setOnTimer(HaierAcYrw02Protocol *yrw02_protocol, const uint16_t mins) {
  const uint16_t nr_mins = MIN((uint16_t)(23 * 60 + 59), mins);
  const bool enabled = (nr_mins > 0);
  uint8_t mode;
  yrw02_protocol->pbit.OnTimerHrs = nr_mins / 60;
  yrw02_protocol->pbit.OnTimerMins = nr_mins % 60;

  
  mode = getTimerMode(yrw02_protocol);
  switch (mode) {
    case kHaierAcYrw02OffTimer:
      mode = enabled ? kHaierAcYrw02OffThenOnTimer : mode;
      break;
    case kHaierAcYrw02OnThenOffTimer:
    case kHaierAcYrw02OffThenOnTimer:
      mode = enabled ? kHaierAcYrw02OffThenOnTimer : kHaierAcYrw02OffTimer;
      break;
    default:
      // Enable/Disable the On timer for the simple case.
      mode = enabled << 1;
  }
  yrw02_protocol->pbit.TimerMode = mode;
}

/// Get the number of minutes of the On Timer setting.
/// @return Nr of minutes.
uint16_t getOnTimer(HaierAcYrw02Protocol *yrw02_protocol){
  return yrw02_protocol->pbit.OnTimerHrs * 60 + yrw02_protocol->pbit.OnTimerMins;
}

/// Set the number of minutes of the Off Timer setting.
/// @param[in] mins Nr. of Minutes for the Timer. `0` means disable the timer.
void setOffTimer(HaierAcYrw02Protocol *yrw02_protocol, const uint16_t mins) {
  const uint16_t nr_mins = MIN((uint16_t)(23 * 60 + 59), mins);
  bool enabled;
  uint8_t mode;

  yrw02_protocol->pbit.OffTimerHrs = nr_mins / 60;
  yrw02_protocol->pbit.OffTimerMins = nr_mins % 60;

  enabled = (nr_mins > 0);
  mode = getTimerMode(yrw02_protocol);
  switch (mode) {
    case kHaierAcYrw02OnTimer:
      mode = enabled ? kHaierAcYrw02OnThenOffTimer : mode;
      break;
    case kHaierAcYrw02OnThenOffTimer:
    case kHaierAcYrw02OffThenOnTimer:
      mode = enabled ? kHaierAcYrw02OnThenOffTimer : kHaierAcYrw02OnTimer;
      break;
    default:
      // Enable/Disable the Off timer for the simple case.
      mode = enabled;
  }
  yrw02_protocol->pbit.TimerMode = mode;
}

/// Get the number of minutes of the Off Timer setting.
/// @return Nr of minutes.
uint16_t getOffTimer(HaierAcYrw02Protocol *yrw02_protocol){
  return yrw02_protocol->pbit.OffTimerHrs * 60 + yrw02_protocol->pbit.OffTimerMins;
}

/// Get the Lock setting of the A/C.
/// @return true, the setting is on. false, the setting is off.
bool getLock(HaierAcYrw02Protocol *yrw02_protocol){ return yrw02_protocol->pbit.Lock; }

/// Set the Lock setting of the A/C.
/// @param[in] on true, the setting is on. false, the setting is off.
void setLock(HaierAcYrw02Protocol *yrw02_protocol, const bool on) {
  yrw02_protocol->pbit.Button = kHaierAcYrw02ButtonLock;
  yrw02_protocol->pbit.Lock = on;
}


void IR_haier_send(HaierAcYrw02Protocol *yrw02_protocol){
    u8 byte_cnt=0,b_cnt=0;
    u8 *raw = getRaw(yrw02_protocol);
    // head
    IR_send_load(kHaierAcHdr);
    IR_send_load(kHaierAcHdr);
    IR_send_load(kHaierAcHdr);
    IR_send_load(kHaierAcHdrGap);

    for(byte_cnt=0;byte_cnt<kHaierACYRW02StateLength;byte_cnt++){
        uart1_transmit(raw[byte_cnt]);
        for(b_cnt=0;b_cnt<8;b_cnt++){
            //send byte MSB
            if(raw[byte_cnt] & (0x80>>b_cnt)){
                //bit 1
                IR_send_load(kHaierAcBitMark);
                IR_send_load(kHaierAcOneSpace);
            }
            else{
                //bit 0
                IR_send_load(kHaierAcBitMark);
                IR_send_load(kHaierAcZeroSpace);
            }
        }
    }

    IR_send_load(kHaierAcBitMark);
    IR_send_load(kHaierAcMinGap);
    
    IR_send_nonblock();
}

HaierAcYrw02Protocol haier_recv_buffer;

void IR_haier_recv_single(u8 level, u16 time){
    static u8 state = HAIER_IR_RECV_STATE_IDLE;
    static u8 now_pos=0;
    switch (state)
    {
    case HAIER_IR_RECV_STATE_IDLE:
        if(level == LEVEL_HIGH && time<=3500 && time>=2500)
            state = HAIER_IR_RECV_STATE_HEAD1;
        else
            state = HAIER_IR_RECV_STATE_IDLE;

        break;
    
    case HAIER_IR_RECV_STATE_HEAD1:
        if(level == LEVEL_LOW && time<=3500 && time>=2500)
            state = HAIER_IR_RECV_STATE_HEAD2;
        else
            state = HAIER_IR_RECV_STATE_IDLE;

        break;

    case HAIER_IR_RECV_STATE_HEAD2:
        if(level == LEVEL_HIGH && time<=3500 && time>=2500)
            state = HAIER_IR_RECV_STATE_HEAD3;
        else
            state = HAIER_IR_RECV_STATE_IDLE;

        break;


    case HAIER_IR_RECV_STATE_HEAD3:
        if(level == LEVEL_LOW && time<=5000 && time>=4000){
            state = HAIER_IR_RECV_STATE_DATA;
            memset(haier_recv_buffer.raw, 0, kHaierACYRW02StateLength);
            now_pos=0;}
        else
            state = HAIER_IR_RECV_STATE_IDLE;

        break;

    case HAIER_IR_RECV_STATE_DATA:
        if(level == LEVEL_LOW){
            if(time >= 300 && time <=1000){
                //receive 0
                now_pos++;
            }else if(time>1000 && time<=2000){
                //receive 1 MSB
                haier_recv_buffer.raw[now_pos/8] |= (0x80>>(now_pos%8));
                now_pos++;
            }
            if(now_pos == (kHaierACYRW02StateLength*8)){
                IR_recv_callback(&haier_recv_buffer);
                state=HAIER_IR_RECV_STATE_IDLE;
            }
        }
        break;

    default:
        break;
    }
}


