/*********************************************************************
* SVMPWM (Space-vector puls width modulation) for Teensy 4.0 and 4.1
* This file give a short example how to use the SVMPWM_teensy library 
* 
* The example creats a SVM on the pins 22, 23 and 3 on the teensy board
* and uses a switching frequency of 5000 Hz
* a 50 Hz signal will be modulated for 1 s then the signal will swithc of
* for 1 s and then turn on again.
* 
* Created: 10.03.2025
* Author: Thomas Czaja
* Modified: --.--.----
* Author: -
* Version 0.0.1
*
* 
*********************************************************************/

#include "SVMPWM_teensy.h"

// defines outputpins
const uint A_PIN = 22;
const uint B_PIN = 23;
const unit C_PIN = 3;

//set switchingfrequency to 5kHz
float swfrequency = 5000;

//modulate a 50 Hz signal
float outputfrequency = 50;
float w = 0;

// creat SVMPWM object in disabled state 
SVMPWM exampeobj(A_PIN, B_PIN, C_PIN, swfrequency);

void setup() {
  //set duty cycles for three phases to 0.5 -> output voltage 0V
  exampeobj.abcWrite(0.5, 0.5, 0.5);
  //enable SVMPWM
  exampeobj.begin();
}

void loop() {
  //delay 1 ms
  delay(1);
  //update voltage angle
  w = w + 2*PI*outputfrequency*0.001;
  //calculte duty cycles for each phase
  float phaseA = 0.5*sin(w);
  float phaseB = 0.5*sin(w - PI/3);
  float phaseC = 0.5*sin(w + PI/3);
  //set dutycyles
  exampeobj.abcWrite(phaseA, phaseB, phaseC);
  //after 1 s disable PWM
  if(w > 315){
    exampeobj.stop();
  }
  //after 2 s enable PWM again
  else if(w > 630){
    w = 0;
    exampeobj.resume();
  }
}
