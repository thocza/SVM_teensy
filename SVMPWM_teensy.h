#ifndef SVMPWM_H
#define SVMPWM_H
#include "Arduino.h"

#if defined(__arm__) && defined(TEENSYDUINO) && defined(__IMXRT1062__) //compile if board is teensy

/*********************************************************************
* SVMPWM (Space-vector puls width modulation) for Teensy 4.0 and 4.1
* This library profides a class for easy integration of a SVMPWM
* See SPVM_sketch.ino for an example
* 
* The following pins can be used. Only pins for one timer can be used at the same time
* Timer 1: 7, 8, 42, 43, 44, 45, 46, 47
* Timer 2: 4, 5, 6, 9, 33, 36, 37
* Timer 3: 28, 29, 51, 54
* Timer 4: 2, 3, 22, 23
* 
* Created: 27.02.2025
* Author: Thomas Czaja
* Modified: --.--.----
* Author: -
* Version 0.0.1
*
* 
*********************************************************************/


class SVMPWM{

    uint32_t period = 0; //period of PWM timer
    uint32_t prescale = 0; //prescaler for timer

    uint flex = 1; //FLEX timer used

    volatile uint32_t* pinapinout = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04; //pointer to output mux register for pin A, initialzed values is random
    volatile uint32_t* pinbpinout = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_05;  //pointer to output mux register for pin B, initialzed values is random
    volatile uint32_t* pincpinout = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06;  //pointer to output mux register for pin C, initialzed values is random

    int pinapinoutvalue = 5; // vlaue to output mux register to activate pin A
    int pinbpinoutvalue = 5; // vlaue to output mux register to activate pin B
    int pincpinoutvalue = 5; // vlaue to output mux register to activate pin C

    uint16_t pinaenable = 0; //value to enable pin A
    uint16_t pinbenable = 0; //value to enable pin B
    uint16_t pincenable = 0; //value to enable pin C

    volatile uint16_t* pinaval1 = &FLEXPWM4_SM2VAL2; //PWM value 1 for pin A, initialzed values is random
    volatile uint16_t* pinaval2 = &FLEXPWM4_SM2VAL3; //PWM value 2 for pin A, initialzed values is random
    volatile uint16_t* pinbval1 = &FLEXPWM4_SM2VAL4; //PWM value 1 for pin B, initialzed values is random
    volatile uint16_t* pinbval2 = &FLEXPWM4_SM2VAL5; //PWM value 2 for pin B, initialzed values is random
    volatile uint16_t* pincval1 = &FLEXPWM2_SM0VAL2; //PWM value 1 for pin V, initialzed values is random
    volatile uint16_t* pincval2 = &FLEXPWM2_SM0VAL3; //PWM value 2 for pin V, initialzed values is random

    bool flex11enable = false, flex12enable = false, flex13enable = false; //enabled FLEX1 timers
    bool flex21enable = false, flex22enable = false, flex23enable = false; //enabled FLEX1 timers
    bool flex31enable = false, flex33enable = false; //enabled FLEX1 timers
    bool flex41enable = false, flex42enable = false; //enabled FLEX1 timers

    //see SVMPWM_teensy.cpp for information to functions
    uint pin2flex(uint);
    volatile uint32_t *getpinout(uint);
    int getpinoutvalue(uint);
    uint16_t getpinenable(uint);
    volatile uint16_t *getpinvalue1(uint);
    volatile uint16_t *getpinvalue2(uint);

  public:
    SVMPWM(uint, uint, uint, float);
    SVMPWM(uint, uint, uint, float, float, float, float);
    void initialize();
    bool setpins(uint, uint, uint);
    void begin();
    void setfrequency(float);
    void abcWrite(float, float, float);
    void abWrite(float, float);
    void stop();
    void resume();

};
#else
  #error "Only Teensy 4.0 and 4.1 board are supported"
#endif
#endif