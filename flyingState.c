//*****************************************************************************
//
// flyingState.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the state the helicopter is in
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

static flyingState_t state; // Current state the helicopter is in.

//*****************************************************************************
// Set the current state
// @param stateIn input state to change the state to.
void setState(flyingState_t stateIn) {
    state = stateIn;
}

//*****************************************************************************
// Gets the current state
// @return state the current state
flyingState_t getState(void) {
    return state;
}
//*****************************************************************************
// Switch case to display the current state
// @return current state to display
char* stateString(void) {
    switch (state) {
        case LANDED: return "Landed";
        case FINDING_YAW_REF: return "Taking off";
        case FLYING: return "Flying";
        case LANDING_YAW: return "Landing Yaw";
        case LANDING_ALT: return "Landing Alt";
        default: return "";
    }
}
