#include "SVMPWM_teensy.h"

#define SQRT3 sqrt(3)


/***************************************************************************************
*See SVMPWM for introductury comment
*autho: Thomas Czaja
*date: 27.02.2025
*version: 0.0.1
***************************************************************************************/
SVMPWM::SVMPWM(uint pina, uint pinb, uint pinc, float frequency){
  // Constructor 1 sets the 3 pins and the frequency but SVMPWM is still disabled
  setpins(pina, pinb, pinc);
  setfrequency(frequency);
  initialize();
}

SVMPWM::SVMPWM(uint pina, uint pinb, uint pinc, float frequency, float dutya, float dutyb, float dutyc){
  //Constructor 2 sets the 3 pins, the frequency and the dutycyle and enables the SVMPWM
  setpins(pina, pinb, pinc);
  setfrequency(frequency);
  initialize();
  abcWrite(dutya, dutyb, dutyc);
  begin();
}

uint SVMPWM::pin2flex(uint pin){
  //returns the Flex PWM Timer for each pin on the teensy
  if(pin==45 || pin==44 || pin==43 || pin==42 || pin==47 || pin==46 || pin==8 || pin==7) return 1;
  if(pin==4 || pin==33 || pin==5 || pin==6 || pin==9 || pin==36 || pin==37) return 2;
  if(pin==54 || pin==29 || pin==28 || pin==51) return 3;
  if(pin==22 || pin==23 || pin==2 || pin==3) return 4;
  return 0;
}

volatile uint32_t *SVMPWM::getpinout(uint pin){
  //returns the pointer to the GLIO Mux Register to the used pin
  if(pin == 2) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04;         //FLEX4.2A
  else if(pin == 3) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_05;    //FLEX4.2B
  else if(pin == 4) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06;    //FLEX2.0A
  else if(pin == 5) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_08;    //FLEX2.1A
  else if(pin == 6) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_10;     //FLEX2.2A
  else if(pin == 7) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01;     //FLEX1.3B
  else if(pin == 8) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00;     //FLEX1.3A
  else if(pin == 9) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11;     //FLEX2.2B
  else if(pin == 22) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_08; //FLEX4.0A
  else if(pin == 23) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_09; //FLEX4.1A
  else if(pin == 28) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_32;   //FLEX3.1B
  else if(pin == 29) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_31;   //FLEX3.1A
  else if(pin == 33) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_07;   //FLEX2.0B
  else if(pin == 36) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02;    //FLEX2.3A
  else if(pin == 37) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_03;    //FLEX2.3B
  else if(pin == 42) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_03; //FLEX1.1B
  else if(pin == 43) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_02; //FLEX1.1A
  else if(pin == 44) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_00; //FLEX1.0B
  else if(pin == 45) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_00; //FLEX1.0A
  else if(pin == 47) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_05; //FLEX1.2B
  else if(pin == 47) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_04; //FLEX1.2A
  else if(pin == 51) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_29;   //FLEX3.0A
  else if(pin == 54) return &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_22;   //FLEX3.3B
  return 0;
}

int SVMPWM::getpinoutvalue(uint pin){
  //returns the GPIO Mux register value to connect pin to FLEX timer output
  if(pin==2 || pin==3 || pin==4 || pin==5 || pin==22 || pin==23 || pin==28 || pin==29 || pin==33 || pin==42 || pin==43 || pin==44 || pin==45 || pin==46 || pin==47 || pin==51 || pin==54) return 1;
  else if(pin==6 || pin==9) return 2;
  else if(pin==7 || pin==8 ||pin==36 || pin==37) return 6;
  return 0;
}

uint16_t SVMPWM::getpinenable(uint pin){
  //returns the register value to enable pin out for each pin
  if(pin == 2) return FLEXPWM_OUTEN_PWMA_EN(4);         //FLEX4.2A
  else if(pin == 3) return FLEXPWM_OUTEN_PWMB_EN(4);    //FLEX4.2B
  else if(pin == 4) return FLEXPWM_OUTEN_PWMA_EN(1);    //FLEX2.0A
  else if(pin == 5) return FLEXPWM_OUTEN_PWMA_EN(2);    //FLEX2.1A
  else if(pin == 6) return FLEXPWM_OUTEN_PWMA_EN(4);    //FLEX2.2A
  else if(pin == 7) return FLEXPWM_OUTEN_PWMB_EN(8);    //FLEX1.3B
  else if(pin == 8) return FLEXPWM_OUTEN_PWMA_EN(8);    //FLEX1.3A
  else if(pin == 9) return FLEXPWM_OUTEN_PWMB_EN(4);    //FLEX2.2B
  else if(pin == 22) return FLEXPWM_OUTEN_PWMA_EN(1);   //FLEX4.0A
  else if(pin == 23) return FLEXPWM_OUTEN_PWMA_EN(2);   //FLEX4.1A
  else if(pin == 28) return FLEXPWM_OUTEN_PWMB_EN(2);   //FLEX3.1B
  else if(pin == 29) return FLEXPWM_OUTEN_PWMA_EN(2);   //FLEX3.1A
  else if(pin == 33) return FLEXPWM_OUTEN_PWMB_EN(1);   //FLEX2.0B
  else if(pin == 36) return FLEXPWM_OUTEN_PWMA_EN(8);   //FLEX2.3A
  else if(pin == 37) return FLEXPWM_OUTEN_PWMB_EN(8);   //FLEX2.3B
  else if(pin == 42) return FLEXPWM_OUTEN_PWMB_EN(2);   //FLEX1.1B
  else if(pin == 43) return FLEXPWM_OUTEN_PWMA_EN(2);   //FLEX1.1A
  else if(pin == 44) return FLEXPWM_OUTEN_PWMB_EN(1);   //FLEX1.0B
  else if(pin == 45) return FLEXPWM_OUTEN_PWMA_EN(1);   //FLEX1.0A
  else if(pin == 47) return FLEXPWM_OUTEN_PWMB_EN(4);   //FLEX1.2B
  else if(pin == 47) return FLEXPWM_OUTEN_PWMA_EN(4);   //FLEX1.2A
  else if(pin == 51) return FLEXPWM_OUTEN_PWMA_EN(1);   //FLEX3.0A
  else if(pin == 54) return FLEXPWM_OUTEN_PWMB_EN(8);   //FLEX3.3B
  return 0;
}

volatile uint16_t *SVMPWM::getpinvalue1(uint pin){
  //returns the pointer to the register to set the value 1 for the PWM
  if(pin == 2) return &FLEXPWM4_SM2VAL2;         //FLEX4.2A
  else if(pin == 3) return &FLEXPWM4_SM2VAL4;    //FLEX4.2B
  else if(pin == 4) return &FLEXPWM2_SM0VAL2;    //FLEX2.0A
  else if(pin == 5) return &FLEXPWM2_SM1VAL2;    //FLEX2.1A
  else if(pin == 6) return &FLEXPWM2_SM2VAL2;    //FLEX2.2A
  else if(pin == 7) return &FLEXPWM1_SM3VAL4;    //FLEX1.3B
  else if(pin == 8) return &FLEXPWM1_SM3VAL2;    //FLEX1.3A
  else if(pin == 9) return &FLEXPWM2_SM2VAL4;    //FLEX2.2B
  else if(pin == 22) return &FLEXPWM4_SM0VAL2;   //FLEX4.0A
  else if(pin == 23) return &FLEXPWM4_SM1VAL2;   //FLEX4.1A
  else if(pin == 28) return &FLEXPWM3_SM1VAL4;   //FLEX3.1B
  else if(pin == 29) return &FLEXPWM3_SM1VAL2;   //FLEX3.1A
  else if(pin == 33) return &FLEXPWM2_SM0VAL4;   //FLEX2.0B
  else if(pin == 36) return &FLEXPWM2_SM3VAL2;   //FLEX2.3A
  else if(pin == 37) return &FLEXPWM2_SM3VAL4;   //FLEX2.3B
  else if(pin == 42) return &FLEXPWM1_SM1VAL4;   //FLEX1.1B
  else if(pin == 43) return &FLEXPWM1_SM1VAL2;   //FLEX1.1A
  else if(pin == 44) return &FLEXPWM1_SM0VAL4;   //FLEX1.0B
  else if(pin == 45) return &FLEXPWM1_SM0VAL2;   //FLEX1.0A
  else if(pin == 47) return &FLEXPWM1_SM2VAL4;   //FLEX1.2B
  else if(pin == 47) return &FLEXPWM1_SM2VAL2;   //FLEX1.2A
  else if(pin == 51) return &FLEXPWM3_SM0VAL2;   //FLEX3.0A
  else if(pin == 54) return &FLEXPWM3_SM3VAL4;   //FLEX3.3B
  return 0;
}

volatile uint16_t *SVMPWM::getpinvalue2(uint pin){
  //returns the pointer to the register to set value 2 for the PWM
  if(pin == 2) return &FLEXPWM4_SM2VAL3;         //FLEX4.2A
  else if(pin == 3) return &FLEXPWM4_SM2VAL5;    //FLEX4.2B
  else if(pin == 4) return &FLEXPWM2_SM0VAL3;    //FLEX2.0A
  else if(pin == 5) return &FLEXPWM2_SM1VAL3;    //FLEX2.1A
  else if(pin == 6) return &FLEXPWM2_SM2VAL3;    //FLEX2.2A
  else if(pin == 7) return &FLEXPWM1_SM3VAL5;    //FLEX1.3B
  else if(pin == 8) return &FLEXPWM1_SM3VAL3;    //FLEX1.3A
  else if(pin == 9) return &FLEXPWM2_SM2VAL5;    //FLEX2.2B
  else if(pin == 22) return &FLEXPWM4_SM0VAL3;   //FLEX4.0A
  else if(pin == 23) return &FLEXPWM4_SM1VAL3;   //FLEX4.1A
  else if(pin == 28) return &FLEXPWM3_SM1VAL5;   //FLEX3.1B
  else if(pin == 29) return &FLEXPWM3_SM1VAL3;   //FLEX3.1A
  else if(pin == 33) return &FLEXPWM2_SM0VAL5;   //FLEX2.0B
  else if(pin == 36) return &FLEXPWM2_SM3VAL3;   //FLEX2.3A
  else if(pin == 37) return &FLEXPWM2_SM3VAL5;   //FLEX2.3B
  else if(pin == 42) return &FLEXPWM1_SM1VAL5;   //FLEX1.1B
  else if(pin == 43) return &FLEXPWM1_SM1VAL3;   //FLEX1.1A
  else if(pin == 44) return &FLEXPWM1_SM0VAL5;   //FLEX1.0B
  else if(pin == 45) return &FLEXPWM1_SM0VAL3;   //FLEX1.0A
  else if(pin == 47) return &FLEXPWM1_SM2VAL5;   //FLEX1.2B
  else if(pin == 47) return &FLEXPWM1_SM2VAL3;   //FLEX1.2A
  else if(pin == 51) return &FLEXPWM3_SM0VAL3;   //FLEX3.0A
  else if(pin == 54) return &FLEXPWM3_SM3VAL5;   //FLEX3.3B
  return 0;
}

void SVMPWM::initialize(){
  //initializes the used FLEX PWM Timer and connects the FLEX PWM output to the pin
  if(flex == 1){
    FLEXPWM1_FCTRL0 |= FLEXPWM_FCTRL0_FLVL(15); // logic high = fault
	  FLEXPWM1_FSTS0 = 15;//0x008; // clear fault status
	  FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex11enable + 4*flex12enable + 8*flex13enable); //enables the 3 FLEX timers needed, FLEX 1 allways need to be active as reference

    FLEXPWM1_SM0CTRL2 = FLEXPWM_SMCTRL2_INDEP; //indipendend pin output
    FLEXPWM1_SM0CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
    //init Flex1.0
    FLEXPWM1_SM0INIT = -period; //PWM Start
    FLEXPWM1_SM0VAL0 = 0;
    FLEXPWM1_SM0VAL1 = period; //PWM end
    FLEXPWM1_SM0VAL2 = 0;
    FLEXPWM1_SM0VAL3 = 0;
    FLEXPWM1_SM0VAL4 = 0;
    FLEXPWM1_SM0VAL5 = 0;

    if(flex11enable){
      FLEXPWM1_SM1CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM1_SM1CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex1.1
      FLEXPWM1_SM1INIT = -period;
      FLEXPWM1_SM1VAL0 = 0;
      FLEXPWM1_SM1VAL1 = period;
      FLEXPWM1_SM1VAL2 = 0;
      FLEXPWM1_SM1VAL3 = 0;
      FLEXPWM1_SM1VAL4 = 0;
      FLEXPWM1_SM1VAL5 = 0;
    }
    if(flex12enable){
      FLEXPWM1_SM2CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM1_SM2CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex1.2
      FLEXPWM1_SM2INIT = -period;
      FLEXPWM1_SM2VAL0 = 0;
      FLEXPWM1_SM2VAL1 = period;
      FLEXPWM1_SM2VAL2 = 0;
      FLEXPWM1_SM2VAL3 = 0;
      FLEXPWM1_SM2VAL4 = 0;
      FLEXPWM1_SM2VAL5 = 0;
    }
    if(flex13enable){
      FLEXPWM1_SM3CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM1_SM3CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex1.3
      FLEXPWM1_SM3INIT = -period;
      FLEXPWM1_SM3VAL0 = 0;
      FLEXPWM1_SM3VAL1 = period;
      FLEXPWM1_SM3VAL2 = 0;
      FLEXPWM1_SM3VAL3 = 0;
      FLEXPWM1_SM3VAL4 = 0;
      FLEXPWM1_SM3VAL5 = 0;
    }
  }
  if(flex == 2){
    FLEXPWM2_FCTRL0 |= FLEXPWM_FCTRL0_FLVL(15); // logic high = fault
	  FLEXPWM2_FSTS0 = 15;//0x008; // clear fault status
	  FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex21enable + 4*flex22enable + 8*flex23enable);//enables the 3 FLEX timers needed, FLEX 1 allways need to be active as reference

    FLEXPWM2_SM0CTRL2 = FLEXPWM_SMCTRL2_INDEP; //indipendend pin output
    FLEXPWM2_SM0CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
    //init Flex2.0
    FLEXPWM2_SM0INIT = -period;
    FLEXPWM2_SM0VAL0 = 0;
    FLEXPWM2_SM0VAL1 = period;
    FLEXPWM2_SM0VAL2 = 0;
    FLEXPWM2_SM0VAL3 = 0;
    FLEXPWM2_SM0VAL4 = 0;
    FLEXPWM2_SM0VAL5 = 0;

    if(flex21enable){
      FLEXPWM2_SM1CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM2_SM1CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex2.1
      FLEXPWM2_SM1INIT = -period;
      FLEXPWM2_SM1VAL0 = 0;
      FLEXPWM2_SM1VAL1 = period;
      FLEXPWM2_SM1VAL2 = 0;
      FLEXPWM2_SM1VAL3 = 0;
      FLEXPWM2_SM1VAL4 = 0;
      FLEXPWM2_SM1VAL5 = 0;
    }
    if(flex22enable){
      FLEXPWM2_SM2CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM2_SM2CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex2.2
      FLEXPWM2_SM2INIT = -period;
      FLEXPWM2_SM2VAL0 = 0;
      FLEXPWM2_SM2VAL1 = period;
      FLEXPWM2_SM2VAL2 = 0;
      FLEXPWM2_SM2VAL3 = 0;
      FLEXPWM2_SM2VAL4 = 0;
      FLEXPWM2_SM2VAL5 = 0;
    }
    if(flex23enable){
      FLEXPWM2_SM3CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM2_SM3CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex2.3
      FLEXPWM2_SM3INIT = -period;
      FLEXPWM2_SM3VAL0 = 0;
      FLEXPWM2_SM3VAL1 = period;
      FLEXPWM2_SM3VAL2 = 0;
      FLEXPWM2_SM3VAL3 = 0;
      FLEXPWM2_SM3VAL4 = 0;
      FLEXPWM2_SM3VAL5 = 0;
    }
  }
  if(flex == 3){
    FLEXPWM3_FCTRL0 |= FLEXPWM_FCTRL0_FLVL(15); // logic high = fault
	  FLEXPWM3_FSTS0 = 15;//0x008; // clear fault status
	  FLEXPWM3_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex31enable + 8*flex33enable); //enables the 3 FLEX timers needed, FLEX 1 allways need to be active as reference

    FLEXPWM3_SM0CTRL2 = FLEXPWM_SMCTRL2_INDEP; //indipendend pin output
    FLEXPWM3_SM0CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
    //init Flex3.0
    FLEXPWM3_SM0INIT = -period;
    FLEXPWM3_SM0VAL0 = 0;
    FLEXPWM3_SM0VAL1 = period;
    FLEXPWM3_SM0VAL2 = 0;
    FLEXPWM3_SM0VAL3 = 0;
    FLEXPWM3_SM0VAL4 = 0;
    FLEXPWM3_SM0VAL5 = 0;

    if(flex31enable){
      FLEXPWM3_SM1CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM3_SM1CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex3.1
      FLEXPWM3_SM1INIT = -period;
      FLEXPWM3_SM1VAL0 = 0;
      FLEXPWM3_SM1VAL1 = period;
      FLEXPWM3_SM1VAL2 = 0;
      FLEXPWM3_SM1VAL3 = 0;
      FLEXPWM3_SM1VAL4 = 0;
      FLEXPWM3_SM1VAL5 = 0;
    }
    if(flex33enable){
      FLEXPWM3_SM3CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM3_SM3CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex3.3
      FLEXPWM3_SM3INIT = -period;
      FLEXPWM3_SM3VAL0 = 0;
      FLEXPWM3_SM3VAL1 = period;
      FLEXPWM3_SM3VAL2 = 0;
      FLEXPWM3_SM3VAL3 = 0;
      FLEXPWM3_SM3VAL4 = 0;
      FLEXPWM3_SM3VAL5 = 0;
    }
  }
  if(flex == 4){
    FLEXPWM4_FCTRL0 |= FLEXPWM_FCTRL0_FLVL(15); // logic high = fault
	  FLEXPWM4_FSTS0 = 15;//0x008; // clear fault status
	  FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex41enable + 4*flex42enable ); //enables the 3 FLEX timers needed, FLEX 1 allways need to be active as reference

    FLEXPWM4_SM0CTRL2 = FLEXPWM_SMCTRL2_INDEP; //indipendend pin output
    FLEXPWM4_SM0CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
    //init Flex4.0
    FLEXPWM4_SM0INIT = -period;
    FLEXPWM4_SM0VAL0 = 0;
    FLEXPWM4_SM0VAL1 = period;
    FLEXPWM4_SM0VAL2 = 0;
    FLEXPWM4_SM0VAL3 = 0;
    FLEXPWM4_SM0VAL4 = 0;
    FLEXPWM4_SM0VAL5 = 0;

    if(flex41enable){
      FLEXPWM4_SM1CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM4_SM1CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex4.1
      FLEXPWM4_SM1INIT = -period;
      FLEXPWM4_SM1VAL0 = 0;
      FLEXPWM4_SM1VAL1 = period;
      FLEXPWM4_SM1VAL2 = 0;
      FLEXPWM4_SM1VAL3 = 0;
      FLEXPWM4_SM1VAL4 = 0;
      FLEXPWM4_SM1VAL5 = 0;
    }
    if(flex42enable){
      FLEXPWM4_SM2CTRL2 = FLEXPWM_SMCTRL2_INDEP | FLEXPWM_SMCTRL2_INIT_SEL(2); //indipendend pin output & user mastersync
      FLEXPWM4_SM2CTRL = FLEXPWM_SMCTRL_HALF | FLEXPWM_SMCTRL_PRSC(prescale); // set prescale
      //init Flex4.2
      FLEXPWM4_SM2INIT = -period;
      FLEXPWM4_SM2VAL0 = 0;
      FLEXPWM4_SM2VAL1 = period;
      FLEXPWM4_SM2VAL2 = 0;
      FLEXPWM4_SM2VAL3 = 0;
      FLEXPWM4_SM2VAL4 = 0;
      FLEXPWM4_SM2VAL5 = 0;
    }
  }

  if(flex == 1) FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex11enable + 4*flex12enable + 8*flex13enable); //enable Flex 1
  else if(flex == 2) FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex21enable + 4*flex22enable + 8*flex23enable); //enable Flex 2
  else if(flex == 3) FLEXPWM3_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex31enable + 8*flex33enable); //enable Flex 3
  else if(flex == 4) FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex41enable + 4*flex42enable ); //enable Flex 4

  if(flex == 1) FLEXPWM1_OUTEN |= pinaenable | pinbenable | pincenable; //enable outputpins FLEX 1
  else if(flex == 2) FLEXPWM2_OUTEN |= pinaenable | pinbenable | pincenable; //enable outputpins FLEX 2
  else if(flex == 3) FLEXPWM3_OUTEN |= pinaenable | pinbenable | pincenable; //enable outputpins FLEX 3
  else if(flex == 4) FLEXPWM4_OUTEN |= pinaenable | pinbenable | pincenable; //enable outputpins FLEX 4

  *pinapinout = pinapinoutvalue; //enable GPIO MUX to pin A
  *pinbpinout = pinbpinoutvalue; //enable GPIO MUX to pin B
  *pincpinout = pincpinoutvalue; //enable GPIO MUX to pin C
}

bool SVMPWM::setpins(uint pina, uint pinb, uint pinc){
  // setups all to chosen pins
  if(pina != pinb && pina != pinc && pinb != pinc){
      //no equal pins
    flex = pin2flex(pina); // flex of Pin A
    if(flex == pin2flex(pinb) && flex == pin2flex(pinc)){
      //all pins in same flex
      pinapinout = getpinout(pina); //gets pointer fo MUX register
      pinbpinout = getpinout(pinb); //gets pointer fo MUX register
      pincpinout = getpinout(pinc); //gets pointer fo MUX register
      pinapinoutvalue = getpinoutvalue(pina); //get value to set MUX register
      pinbpinoutvalue = getpinoutvalue(pinb); //get value to set MUX register
      pincpinoutvalue = getpinoutvalue(pinc); //get value to set MUX register
      pinaenable = getpinenable(pina); //gets value to enable pin
      pinbenable = getpinenable(pinb); //gets value to enable pin
      pincenable = getpinenable(pinc); //gets value to enable pin
      pinaval1 = getpinvalue1(pina); //gets pointer to pin A value 1 register
      pinaval2 = getpinvalue2(pina); //gets pointer to pin A value 2 register
      pinbval1 = getpinvalue1(pinb); //gets pointer to pin B value 1 register
      pinbval2 = getpinvalue2(pinb); //gets pointer to pin B value 2 register
      pincval1 = getpinvalue1(pinc); //gets pointer to pin C value 1 register
      pincval2 = getpinvalue2(pinc); //gets pointer to pin B value 2 register
      if(pina==42 || pina==43 || pinb==42 || pinb==43 || pinc==42 || pinc==43) flex11enable = true; //one pin is connected to FLEX 1.1
      if(pina==46 || pina==47 || pinb==46 || pinb==47 || pinc==46 || pinc==47) flex12enable = true; //one pin is connected to FLEX 1.2
      if(pina==7 || pina==8 || pinb==7 || pinb==8 || pinc==7 || pinc==8) flex13enable = true; //one pin is connected to FLEX 1.3
      if(pina==5 || pinb==5 || pinc==5) flex21enable = true; //one pin is connected to FLEX 2.1
      if(pina==6 || pina==9 || pinb==6 || pinb==9 || pinc==6 || pinc==9) flex22enable = true; //one pin is connected to FLEX 2.2
      if(pina==36 || pina==37 || pinb==36 || pinb==37 || pinc==36 || pinc==37) flex23enable = true; //one pin is connected to FLEX 2.3
      if(pina==28 || pina==29 || pinb==28 || pinb==29 || pinc==28 || pinc==29) flex31enable = true; //one pin is connected to FLEX 3.1
      //FLEX 3.2 is not connected to pin on teensy board
      if(pina==51 || pinb==51 || pinc==51) flex33enable = true; //one pin is connected to FLEX 3.3
      if(pina==23 || pinb==23 || pinc==23) flex41enable = true; //one pin is connected to FLEX 4.1
      if(pina==2 || pina==3 || pinb==2 || pinb==3 || pinc==2 || pinc==3) flex42enable = true; //one pin is connected to FLEX 4.2
      //FLEX 4.3 is not connected to pin on teensy board
      return true;
    }
  }
  return false;
}

void SVMPWM::begin(){
  //enables the SVMPWM
  if(flex == 1) FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_RUN(1 + 2*flex11enable + 4*flex12enable + 8*flex13enable); //enable FLEX 1
  else if(flex == 2) FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_RUN(1 + 2*flex21enable + 4*flex22enable + 8*flex23enable); //enable FLEX 2
  else if(flex == 3) FLEXPWM3_MCTRL |= FLEXPWM_MCTRL_RUN(1 + 2*flex31enable + 8*flex33enable); //enable FLEX 3
  else if(flex == 4) FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_RUN(1 + 2*flex41enable + 4*flex42enable ); //enable FLEX 4
}

void SVMPWM::setfrequency(float frequency){
  //calculates the period for the PWM and the prescaler needed
  period = (float)F_BUS_ACTUAL/frequency;
  prescale = 0;
  while(period > 32767){
    period = period >> 1;
    prescale++;
    if(prescale > 7){
      prescale = 7;
      period = 32767;
    }
  }
}

void SVMPWM::abcWrite(float dutya, float dutyb, float dutyc){
  //set dutycyles for three phase output inputs must be between 0 and 1
  if(dutya > 1) dutya = 1;
  else if(dutya < -1) dutya = -1;
  if(dutyb > 1) dutyb = 1;
  else if(dutyb < -1) dutyb = -1;
  if(dutyc > 1) dutyc = 1;
  else if(dutyc < -1) dutyc = -1;
  uint da = uint((dutya + 1.)/2.*period);
  uint db = uint((dutyb + 1.)/2.*period);
  uint dc = uint((dutyc + 1.)/2.*period);
  if(flex == 1) FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex11enable + 4*flex12enable + 8*flex13enable);
  if(flex == 2) FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex21enable + 4*flex22enable + 8*flex23enable);
  if(flex == 3) FLEXPWM3_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex31enable + 8*flex33enable);
  if(flex == 4) FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_CLDOK(1 + 2*flex41enable + 4*flex42enable );
  *pinaval1 = -da;
  *pinaval2 = da;
  *pinbval1 = -db;
  *pinbval2 = db;
  *pincval1 = -dc;
  *pincval2 = dc;
  if(flex == 1) FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex11enable + 4*flex12enable + 8*flex13enable);
  if(flex == 2) FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex21enable + 4*flex22enable + 8*flex23enable);
  if(flex == 3) FLEXPWM3_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex31enable + 8*flex33enable);
  if(flex == 4) FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_LDOK(1 + 2*flex41enable + 4*flex42enable );
}

void SVMPWM::abWrite(float dutyalpha, float dutybbeta){
  //set dutycyle in alpha beta
  //inputs between -0.5 and 0.5
  float b = -0.5*dutyalpha + SQRT3*0.5*dutybbeta;
  float c = -0.5*dutyalpha - SQRT3*0.5*dutybbeta;
  abcWrite(dutyalpha, b, c);
}

void SVMPWM::stop(){
  //stops the SVMPWM
  if(flex == 1) FLEXPWM1_MCTRL &= ~FLEXPWM_MCTRL_RUN(1 + 2*flex11enable + 4*flex12enable + 8*flex13enable);
  else if(flex == 2) FLEXPWM2_MCTRL &= ~FLEXPWM_MCTRL_RUN(1 + 2*flex21enable + 4*flex22enable + 8*flex23enable);
  else if(flex == 3) FLEXPWM3_MCTRL &= ~FLEXPWM_MCTRL_RUN(1 + 2*flex31enable + 8*flex33enable);
  else if(flex == 4) FLEXPWM4_MCTRL &= ~FLEXPWM_MCTRL_RUN(1 + 2*flex41enable + 4*flex42enable );
}

void SVMPWM::resume(){
  //enables PWM after stop
  begin();
}