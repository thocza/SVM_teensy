# SVM_teensy
Space Vector Modulation for teensy 4.0 and 4.1 boards

version: 0.0.1
author: Thomas Czaja

## Introduction
[SVM](https://en.wikipedia.org/wiki/Space_vector_modulation) is the standard puls with modulation method in moderen power electronic drive systems. This library provides an easy to use SVM class to utilize the FLEX PWM timers of the i.MX RT1060 processor used on the teensy 4.0 and 4.1 boards to creat a space vector modulated output signal. The teensy 4s have 4 independent FLEX PWM timers. Each timer has 4 synchronized timers, which can be configured to creat an SVM signal. Each sub-timer is hard wired to two pins that can be used independently. So dynamic pin allocation is not possible. For an example see SVM_example.ino.
On how to install libraries from GitHub to Arduino IDE see [here](https://forum.arduino.cc/t/tutorial-with-screen-shots-how-to-download-a-library-from-github-and-installing-a-zip-library/994122).

## Syntax
Include library to your project file:
```c++
#include "SVMPWM_teensy.h"
```
### Constructor
The SVM class has to constructors
Constructor 1 creats the object with the selcted pins and the switching frequency but SVM is in disabled and has to be enabled before it starts to creat the SVM signals:
```c++
SVMPWM SVMobj(A_PIN, B_PIN, C_PIN, swfrequency);
```
Constructor 2 creats the object and starts the SVM signal with the set duty-cycles:
```c++
SVMPWM SVMobj(A_PIN, B_PIN, C_PIN, swfrequency, dutyA, dutyB, dutyC);
```
As the pins are connected to eahc sub-timer and only sub-timer of one FLEX timer are synchronized the pins that can be used for the SVM are connected to the following FLEX timer.
|FLEX Timer |Pins on teensy |
|-----------|---------------|
|1          |7, 8, 42, 43, 44, 45, 46, 47|
|2          |4, 5, 6, 9, 33, 36, 37|
|3          |28, 29, 51, 54|
|4          |2, 3, 22, 23|

### En- and Disable SVM
The SVM can be enabled with the following functions. There is no difference between both functions
```c++
SVMobj.begin();
SVMobj.resume();
```
The SVM can be disabled as followed. Currently the signal status of each pin is not defined when disabling. It can either be high or low.
```c++
SVMobj.stop();
```
### Set duty-cycles
The duty cycles can either be set in a three phase space
```c++
SVMobj.abcWrite(dutyA, dutyB, dutyC);
```
or in alpha beta space
```c++
SVMobj.abWrite(phaseAlpha, phaseBeta);
```
Duty-cycles have are floats from -1. to 1.
