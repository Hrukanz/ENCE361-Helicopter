//*****************************************************************************
//
// switch.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the switch state changes.
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


#define SWITCH_PERIPH     SYSCTL_PERIPH_GPIOA
#define SWITCH_PORT_BASE  GPIO_PORTA_BASE
#define SWITCH_PIN        GPIO_PIN_7


static bool switchState;

static bool stateChange = false;

//*****************************************************************************
// Initialise Switch State.
void initSwitch(void) {
    SysCtlPeripheralEnable(SWITCH_PERIPH);
    GPIOPinTypeGPIOInput(SWITCH_PORT_BASE, SWITCH_PIN);
    GPIOPadConfigSet(SWITCH_PORT_BASE, SWITCH_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    switchState = (GPIOPinRead(SWITCH_PORT_BASE, SWITCH_PIN) == SWITCH_PIN);
}

//*****************************************************************************
// Update Switch State.
void updateSwitch(void) {
    bool state = (GPIOPinRead(SWITCH_PORT_BASE, SWITCH_PIN) == SWITCH_PIN);
    if (state != switchState) {
        stateChange = true;
    }
    switchState = state;
}

//*****************************************************************************
// Check the switch state and update it accordingly
// @return Switch State Change.
switchState_t checkSwitch(void) {
    if (stateChange) {
        stateChange = false;
        if (switchState) {
            return SWITCH_UP;
        } else {
            return SWITCH_DOWN;
        }
    } else {
        return SAME;
    }
}


