//*****************************************************************************
//
// pwm.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the pwm output.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "flyingState.h"

#include "pwm.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define DIVIDER_PROCESSOR      SYSCTL_PWMDIV_4
#define DIVIDER                4


// Main Rotor.
#define MAIN_FREQ        200
#define MAIN_BASE        PWM0_BASE
#define MAIN_GEN         PWM_GEN_3
#define MAIN_OUTNUM      PWM_OUT_7
#define MAIN_OUTBIT      PWM_OUT_7_BIT
#define MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define MAIN_GPIO_PIN    GPIO_PIN_5
#define MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC





// Tail Rotor.
#define TAIL_FREQ        200
#define TAIL_BASE        PWM1_BASE
#define TAIL_GEN         PWM_GEN_2
#define TAIL_OUTNUM      PWM_OUT_5
#define TAIL_OUTBIT      PWM_OUT_5_BIT
#define TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define TAIL_GPIO_PIN    GPIO_PIN_1
#define TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF



//*****************************************************************************
// Static variables
//*****************************************************************************

// Current duty cycle of the two motors.
static uint32_t mainPower = 0;
static uint32_t tailPower = 0;

//*****************************************************************************
// Static function forward declarations
//*****************************************************************************
static uint32_t calcPulsePeriod(uint32_t freq);
static uint32_t calcPulseWidth (uint32_t period, uint32_t duty);
static void initMain();
static void initTail();

//*****************************************************************************
// Initialise Rotors
void initPWM(void) {
    SysCtlPWMClockSet(DIVIDER_PROCESSOR);
    initMain();
    initTail();
}

//*****************************************************************************
// Calculate the pulse period
// @param freq the pulse period frequency.
// @return The pulse period.
static uint32_t calcPulsePeriod (uint32_t freq) {
    return SysCtlClockGet() / DIVIDER / freq;
}

//*****************************************************************************
// Calculate the pulse width
// @param period the pulse width period.
// @param duty the pulse width duty.
// @return The pulse width.
static uint32_t calcPulseWidth (uint32_t period, uint32_t duty) {
    return period * duty / 100;
}

//*****************************************************************************
// set the PWM power
// @param ui32Freq the pwm frequency.
// @param ui32Duty the pwm duty.
void setPWM (uint32_t ui32Freq, uint32_t ui32Duty) {
    // Calculate the PWM period corresponding to the freq.
    uint32_t targetPower =
      SysCtlClockGet() / DIVIDER / ui32Freq;

    PWMGenPeriodSet(MAIN_BASE, MAIN_GEN, targetPower);
    PWMPulseWidthSet(MAIN_BASE, MAIN_OUTNUM,
                     targetPower * ui32Duty / 100);
}

//*****************************************************************************
// Initialise the main rotors PWM.
static void initMain(void) {
    SysCtlPeripheralEnable(MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(MAIN_PERIPH_GPIO);

    GPIOPinConfigure(MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(MAIN_GPIO_BASE, MAIN_GPIO_PIN);

    PWMGenConfigure(MAIN_BASE, MAIN_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the initial PWM parameters
    setPWM (MAIN_FREQ, MAIN_MIN_DUTY);

    PWMGenEnable(MAIN_BASE, MAIN_GEN);

    // Disable PWM output until main needs to start.
    PWMOutputState(MAIN_BASE, MAIN_OUTBIT, false);
}


//*****************************************************************************
// Set the main rotors PWM.
// @param targetPower the target pwm power.
void setMainPower(uint32_t targetPower) {
   if (targetPower < MAIN_MIN_DUTY) {
       targetPower = MAIN_MIN_DUTY;
    } else if (targetPower > MAX_DUTY) {
        targetPower = MAX_DUTY;
    }
    uint32_t pulsePeriod = calcPulsePeriod(MAIN_FREQ);
    uint32_t pulseWidth = calcPulseWidth(pulsePeriod, targetPower);
    PWMGenPeriodSet(MAIN_BASE, MAIN_GEN, pulsePeriod);
    PWMPulseWidthSet(MAIN_BASE, MAIN_OUTNUM, pulseWidth);
    mainPower = targetPower;

}

//*****************************************************************************
// Start the main rotors PWM. 
// set the main power to the minimum main rotor duty.
void startMain(void) {
    setMainPower(MAIN_MIN_DUTY);
    PWMOutputState(MAIN_BASE, MAIN_OUTBIT, true);
}

//*****************************************************************************
// Stop the main rotors PWM. 
// set the main power to 0.
void stopMain(void) {
    PWMOutputState(MAIN_BASE, MAIN_OUTBIT, false);
    mainPower = 0;
}


//*****************************************************************************
// Get the main power. 
// @return mainPower the main power
uint32_t getMainPwr(void) {
    return mainPower;
}

//*****************************************************************************
// Initialise the tail rotors PWM
static void initTail() {
    SysCtlPeripheralEnable(TAIL_PERIPH_PWM);
     SysCtlPeripheralEnable(TAIL_PERIPH_GPIO);

    GPIOPinConfigure(TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(TAIL_GPIO_BASE, TAIL_GPIO_PIN);

    PWMGenConfigure(TAIL_BASE, TAIL_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(TAIL_BASE, TAIL_GEN);

    //Disable PWM output until rotor needs to start.
    PWMOutputState(TAIL_BASE, TAIL_OUTBIT, false);
}

//*****************************************************************************
// Set the tail rotors PWM.
// @param targetPower the target pwm power.
void setTailPower(uint32_t targetPower) {
   if (targetPower < TAIL_MIN_DUTY) {
       targetPower = TAIL_MIN_DUTY;
    } else if (targetPower > MAX_DUTY) {
        targetPower = MAX_DUTY;
    }
    uint32_t pulsePeriod = calcPulsePeriod(TAIL_FREQ);
    uint32_t pulseWidth = calcPulseWidth(pulsePeriod, targetPower);
    PWMGenPeriodSet(TAIL_BASE, TAIL_GEN, pulsePeriod);
    PWMPulseWidthSet(TAIL_BASE, TAIL_OUTNUM, pulseWidth);
    tailPower = targetPower;
}

//*****************************************************************************
// Start the tail rotors PWM. 
// set the tail power to the minimum tail rotor duty.
void startTail(void) {
    setTailPower(TAIL_MIN_DUTY);
    PWMOutputState(TAIL_BASE, TAIL_OUTBIT, true);
}

//*****************************************************************************
// Stop the tail rotors PWM. 
// set the tail power to 0.
void stopTail(void) {
    PWMOutputState(TAIL_BASE, TAIL_OUTBIT, false);
    tailPower = 0;
}

//*****************************************************************************
// Get the tail power. 
// @return tailPower the tail power
uint32_t getTailPwr(void) {
    return tailPower;
}

