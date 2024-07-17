//*****************************************************************************
//
// display.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for displaying 
// relevant information about the helicopter on the microcontroller.
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
#include "scheduler.h"

//*****************************************************************************
// intialise the Orbit OLED display
void initDisplay (void) {
    
    OLEDInitialise();
}



//*****************************************************************************
// Function to display ADC percentage, Yaw angle, main power, and tail power.
void display(void) {

    char line1[17];
    char line2[17];
    char line3[17];
    char line4[17];

    usnprintf(line1, sizeof(line1), "Alt: %3d%%", ADCPercentage());
    usnprintf(line2, sizeof(line2), "Yaw: %2d.%1d deg", yawAngle()/10, abs(yawAngle()%10));
    usnprintf(line3, sizeof(line3), "Main: %2d%%", getMainPwr());
    usnprintf(line4, sizeof(line4), "Tail: %2d%%", getTailPwr());

    OLEDStringDraw(line1, 0, 0);
    OLEDStringDraw (line2, 0, 1);
    OLEDStringDraw (line3, 0, 2);
    OLEDStringDraw (line4, 0, 3);
}



