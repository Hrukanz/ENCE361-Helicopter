/*
 * yaw.c
 *
 *  Created on: 18/05/2023
 *      Author: hya73
 */



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
#include "flyingState.h"


// Yaw channels A and B uses Pins PB0 & PB1
#define YAW_CHANNEL_A_PIN           GPIO_PIN_0
#define YAW_CHANNEL_B_PIN           GPIO_PIN_1
// Yaw Gpio Base for port B
#define GPIO_BASE                   GPIO_PORTB_BASE

#define YAW_REFERENCE_GPIO_PERIPH   SYSCTL_PERIPH_GPIOC
#define YAW_REFERENCE_GPIO_BASE     GPIO_PORTC_BASE
#define YAW_REFERENCE_PIN           GPIO_PIN_4

#define CIRCLE_SLOTS 112
#define YAW_PER_SLOT 4
#define CIRCLE_DEGREES 360

static int16_t yawChange = 0;   // Yaw value relative to reference, each slot corresponds to a yaw change of 4
static int32_t yawIn = 0;

//*****************************************************************************
// Interrupt handler for yaw reference pin
static void yawRefIntHandler() {
    if (getState() == FINDING_YAW_REF) {
        yawChange = 0;
        yawIn = 0;
        setState(FLYING);
    }

    GPIOIntClear(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
}

//*****************************************************************************
// Interrupt handler for yaw channels A and B
void yawChannelsIntHandler(void)
{
    GPIOIntClear(GPIO_BASE, YAW_CHANNEL_A_PIN);
    GPIOIntClear(GPIO_BASE, YAW_CHANNEL_B_PIN);
   // Keep prev yaw channels
   static bool prevChannelA = false;
   static bool prevChannelB = false;

   // Check whether each yaw channel is currently HIGH.
  bool curChannelA = (GPIOPinRead(GPIO_BASE, YAW_CHANNEL_A_PIN) == YAW_CHANNEL_A_PIN);
  bool curChannelB = (GPIOPinRead(GPIO_BASE, YAW_CHANNEL_B_PIN) == YAW_CHANNEL_B_PIN);

  // Uses the previous and current values of channel A and channel B to
  // determine the direction of rotation, and update the yaw value.
  if (!prevChannelA &&  !prevChannelB) {
      // Case 1: Channel A and B were both low in the previous state
      if (!curChannelA && curChannelB) {
          // Channel A stayed low, while B went high -> clockwise rotation
          yawChange += 1;
      } else if (curChannelA && !curChannelB) {
          // Channel B stayed low, while A went high -> counterclockwise rotation
          yawChange -= 1;
      }
  } else if (!prevChannelA && prevChannelB) {
      // Case 2: Channel A was low and B was high in the previous state
      if (curChannelA && curChannelB) {
          // Both channels are now high -> clockwise rotation
          yawChange += 1;
      } else if (!curChannelA && !curChannelB) {
          // Both channels are now low -> counterclockwise rotation
          yawChange -= 1;
      }
  } else if (prevChannelA && !prevChannelB) {
      // Case 3: Channel A was high and B was low in the previous state
      if (!curChannelA && !curChannelB) {
          // Both channels are now low -> clockwise rotation
          yawChange += 1;
      } else if (curChannelA && curChannelB) {
          // Both channels are now high -> counterclockwise rotation
          yawChange -= 1;
      }
  } else {
      // Case 4: Channel A and B were both high in the previous state
      if (curChannelA && !curChannelB) {
          // Channel B went low, while A stayed high -> clockwise rotation
          yawChange += 1;
      } else if (!curChannelA && curChannelB) {
          // Channel A went low, while B stayed high -> counterclockwise rotation
          yawChange -= 1;
      }
  }
  // Update the previous channel states to the current channel states
  prevChannelA = curChannelA;
  prevChannelB = curChannelB;

}

//*****************************************************************************
// Function to initialize yaw
void initYaw(void) {
    // The GPIOB peripheral must be enabled for configuration and use
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);

    // Config a pin change interrupt triggered by rising or a falling edge on both channels.
    GPIOIntRegister(GPIO_BASE, yawChannelsIntHandler);

    GPIOIntTypeSet(GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN, GPIO_BOTH_EDGES);

    // Enable the interrupts configured.
    GPIOIntEnable(GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);


    SysCtlPeripheralEnable(YAW_REFERENCE_GPIO_PERIPH);
    GPIOPinTypeGPIOInput(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
    GPIOPadConfigSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntRegister(YAW_REFERENCE_GPIO_BASE, yawRefIntHandler);
    GPIOIntTypeSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
}

//*****************************************************************************
// Function to convert yaw to range of -180 degrees to 180 degrees
// @param yaw yaw to convert
// @return yaw converted yaw
int32_t convert(int32_t yaw) {
    if (yaw>(CIRCLE_DEGREES/2)) {
        yawChange = -223;
        return (-CIRCLE_DEGREES/2) + (yaw - (CIRCLE_DEGREES/2));
    } else if (yaw<(-CIRCLE_DEGREES/2)) {
        yawChange = 224;
        return (CIRCLE_DEGREES/2) + (yaw + (CIRCLE_DEGREES/2));
    } else {
        return yaw;
    }
}

//*****************************************************************************
// Function to calculate the yaw angle.
// @return yaw angle after calculating.
int32_t yawAngle(void) {
    int32_t yawAngle = (yawChange * CIRCLE_DEGREES * 10) / (CIRCLE_SLOTS * YAW_PER_SLOT);
    return yawAngle;
}

//*****************************************************************************
// Function to set yaw input
// @param yawAmount amount to add/subrtract from the yawIn
void setYawInput(int32_t yawAmount) {
    yawIn = convert(yawIn + yawAmount);
}

//*****************************************************************************
// Get the yaw Error
// @return wanted yaw - current yaw angle 
int32_t yawError(void) {
    return convert(yawIn - yawAngle()/10);
}

//*****************************************************************************
// Change yaw landing with specified amount
// @param amount amount the subtract or add from the yaw in.
void yawLanding(int16_t amount) {
    if (yawIn < 0) {
        yawIn += amount;
        if (yawIn > 0) {
            yawIn = 0;
        }
    } else if (yawIn > 0) {
        yawIn -= amount;
        if (yawIn < 0) {
            yawIn = 0;
        }
    } else {    // yawIn == 0
        if (yawError() == 0) {
            setState(LANDING_ALT);
        }
    }
}

//*****************************************************************************
// Change yaw reference with specified amount
// @param amount amount the subtract or add from the yaw in.
void yawRef(int16_t amount) {
    if (yawIn < 0) {
        yawIn += amount;
        if (yawIn > 0) {
            yawIn = 0;
        }
    } else if (yawIn > 0) {
        yawIn -= amount;
        if (yawIn < 0) {
            yawIn = 0;
        }
    } else {    // yawIn == 0
        if (yawError() == 0) {
            setState(FLYING);
        }
    }
}

//*****************************************************************************
// Get the yaw In
// @return yaw in the yaw input.
int32_t getYawIn(void) {
    return yawIn;
}





