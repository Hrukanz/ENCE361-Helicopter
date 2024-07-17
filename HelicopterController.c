//*****************************************************************************
//
// HelicopterController.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   17/3/23
//
// Main helicopter controller program.
//
//*****************************************************************************

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "buttons4.h"
#include "pwm.h"
#include "altitude.h"
#include "controller.h"
#include "uart.h"
#include "yaw.h"
#include "flyingState.h"
#include "switch.h"
#include "display.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define MAX_STR_LEN 16

#define YAW_INCREMENT 15
#define ALTITUDE_INCREMENT 10

#define BUF_SIZE 20
#define SAMPLE_RATE_HZ 400

#define CONTROL_UPDATE_RATE 40

#define SYSTICK 400
#define SWITCH_TICK 10
#define BUTTON_TICK 40
#define CONTROLLER_TICK 20
#define TAKE_OFF_LANDING_TICK 80
#define DISPLAY_TICK 40
#define UART_TICK 40

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3); 
    updateButtons();
    updateSwitch();
    updateController();

   // tasksTick();

}


//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
void initClock (void) {
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


//*****************************************************************************
// Check if any buttons have been pressed.
void buttonCheck(void) {
    if (checkButton(UP) == PUSHED) {
        setAltitudeInput(ALTITUDE_INCREMENT);
    }

    if (checkButton(DOWN) == PUSHED) {
        setAltitudeInput(-ALTITUDE_INCREMENT);
    }

    if (checkButton(LEFT) == PUSHED) {
        setYawInput(YAW_INCREMENT);
    }

    if (checkButton(RIGHT) == PUSHED) {
        setYawInput(-YAW_INCREMENT);
    }
}

//*****************************************************************************
// Check if the switch has changed states.
void switchCheck(void) {
    switchState_t switchState = checkSwitch();

    if (switchState == SWITCH_UP && getState() == LANDED) {
        startMain();
        startTail();
        setState(FINDING_YAW_REF);

    } else if (switchState == SWITCH_DOWN && getState() == FLYING) {
        setState(LANDING_YAW);
    }
}

//*****************************************************************************
// Update Yaw input/ Altitude input depending on the current state of the helicopter.
void updateTakeOffOrLanding(void) {
    if (getState() == FINDING_YAW_REF) {
        setYawInput(YAW_INCREMENT);
    } else
    if (getState() == LANDING_YAW) {
        yawLanding(YAW_INCREMENT);
    } else if (getState() == LANDING_ALT) {
        altLanding(ALTITUDE_INCREMENT);
    }
}

//*****************************************************************************
// Delay the Uart print in for easy viewing on console.
void delayUART() {
    static int ticks = 0;
    if (ticks>40) {
        status();
        ticks = 0;
    } else {
        ticks++;
    }
}


//*****************************************************************************
// Main Application Loop.
int main(void) {

    initClock();
    initYaw();
    initAltitude();
    initPWM();
    initUart();
    initSwitch();
    initController(CONTROL_UPDATE_RATE);
    initDisplay();
    initButtons();
    initScheduler(6);
    setState(LANDED);

    // Enable interrupts to the processor.
    IntMasterEnable();

    //Wait for buffer to be filled
    circBuffer();

    while(true) {
        buttonCheck();
        switchCheck();
        updateTakeOffOrLanding();
        delayUART();
        display();

    }

}

