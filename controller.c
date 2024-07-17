//*****************************************************************************
//
// controller.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   21/3/23
//
// Module for controlling the gains through a PID controller sysystem.
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
#include "yaw.h"
#include "flyingState.h"


#define MAIN_KP 7       // Proportional gain for main rotor altitude control
#define MAIN_KI 2       // Integral gain for main rotor altitude control
#define MAIN_KD 2       // Derivative gain for main rotor altitude control

#define TAIL_KP 10      // Proportional gain for tail rotor yaw control
#define TAIL_KI 3       // Integral gain for tail rotor yaw control
#define TAIL_KD 2       // Derivative gain for tail rotor yaw control


static int32_t updateRate;

static int32_t mainErrorIntegrated = 0;
static int32_t mainErrorPrev = 0;

static int32_t tailErrorIntegrated = 0;
static int32_t tailErrorPrev = 0;

//*****************************************************************************
// Initialize controller with update rate.
// @param updateRateIn update rate to initialise.
void initController(int32_t updateRateIn) {
    updateRate = updateRateIn;
}

//*****************************************************************************
// Altitude PID Controller.
static void altitudeController(void) {
    int32_t error = altitudeError();
    int32_t newMainErrorIntegrated = mainErrorIntegrated + error / updateRate;
    int32_t errorDerivative = (error - mainErrorPrev) * updateRate;

    int32_t control = (error * MAIN_KP + newMainErrorIntegrated * MAIN_KI + errorDerivative * MAIN_KD);

    mainErrorPrev = error;

    // Limit control output to prevent exceeding maximum duty cycle
    if (control > MAX_DUTY) {
        control = MAX_DUTY;
    } else if (control < MAIN_MIN_DUTY) {
        control = MAIN_MIN_DUTY;
    } else {
        // Only accumulate error signal if output is within its limits,
        // to prevent integral windup.
        mainErrorIntegrated = newMainErrorIntegrated;
    }

    setMainPower(control);

}

//*****************************************************************************
// Yaw PID Controller.
static void yawController(void) {
    int32_t error = yawError();
    int32_t newTailErrorIntegrated = tailErrorIntegrated + error / updateRate;
    int32_t errorDerivative = (error - tailErrorPrev) * updateRate;

    int32_t control = (error * TAIL_KP + newTailErrorIntegrated * TAIL_KI + errorDerivative * TAIL_KD);

    tailErrorPrev = error;

    // Limit control output to prevent exceeding maximum duty cycle
    if (control > MAX_DUTY) {
        control = MAX_DUTY;
    } else if (control < TAIL_MIN_DUTY) {
        control = TAIL_MIN_DUTY;
    } else {
        // Only accumulate error signal if output is within its limits,
        // to prevent integral windup.
        tailErrorIntegrated = newTailErrorIntegrated;
    }

    setTailPower(control);
}

//*****************************************************************************
// Update PID Controllers.
void updateController(void) {
    altitudeController();
    yawController();
}
