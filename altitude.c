//*****************************************************************************
//
// altitude.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for measuring the altitude of the helicopter.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "circBufT.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "altitude.h"
#include "flyingState.h"
#include "pwm.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 10
#define MAX_ALTITUDE 100
#define MIN_ALTITUDE 0
// Calculated as: 4095 * (1V / 3.3V)
#define ADC_RANGE              1241

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;    // Circular buffer for storing ADC samples
static int32_t meanADC;         // Current mean ADC value
static int32_t minADC;          // ADC value for the 'Landed' altitude
static int32_t sumADC;          // Current sum of the ADC samples in the buffer
static int32_t altitudeIn = 0;
// Number of ADC samples taken, used to check whether buffer is filled yet.
static uint32_t samplesTaken = 0;

//*****************************************************************************
// Calls the initialisers for the altitude class
void initAltitude(void) {
    initCircBuf (&g_inBuffer, BUF_SIZE);
    initADC();
}


//*****************************************************************************
// Initializes the ADC for single-ended sampling of channel 9 
// with interrupts enabled.
void initADC (void) {
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister(ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

//*****************************************************************************
// Interrupt handler for the ADC conversion complete interrupt. 
// Writes the ADC sample to the circular buffer 
// and calculates the mean ADC value.
void ADCIntHandler(void) {
        // Oldest circular buffer value, newest circular buffer Value
        uint32_t newValue;
        
        // Get the single sample from ADC0.  ADC_BASE is defined in
        // inc/hw_memmap.h
        ADCSequenceDataGet(ADC0_BASE, 3, &newValue);
        samplesTaken++;
        
        // Get the oldest value from the circular buffer (before it is overwritten).
        // oldValue = readCircBuf (&g_inBuffer);

        
        // Place it in the circular buffer (advancing write index)
        writeCircBuf (&g_inBuffer, newValue);

        sumADC = 0;
        int8_t i;
        for (i = 0; i < BUF_SIZE; i++)
            sumADC = sumADC + readCircBuf (&g_inBuffer);
        meanADC = (2 * sumADC + BUF_SIZE) / 2 / BUF_SIZE;

        
        // Clean up, clearing the interrupt
        ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
// Function to sample the ADC and set the mean ADC value.
void meanSample(void) {

    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer and display it, together with the sample number.
    meanADC = ((2 * sumADC + BUF_SIZE) / 2 / BUF_SIZE);
}

//*****************************************************************************
// Waits for the circular buffer to be filled to initialise the minimum ADC
void circBuffer(void) {
    while(samplesTaken<BUF_SIZE*10){}
    minADC = meanADC;
}

//*****************************************************************************
// Function to calculate the ADC percentage.
// @return The ADC percentage calculated from the mean.
int32_t ADCPercentage(void)
{
    //Return the ADC percentage calculated from the mean.
    return ((minADC - meanADC) * 100) / ADC_RANGE;
}

//*****************************************************************************
// Sets the altitude input based on the specified altitude change.
// @param altChange The altitude change to apply to the input.
void setAltitudeInput(int32_t altChange) {
    altitudeIn += altChange;
    if (altitudeIn >= MAX_ALTITUDE) {
        altitudeIn = MAX_ALTITUDE;
    } else if (altitudeIn < MIN_ALTITUDE) {
        altitudeIn = MIN_ALTITUDE;
    }
}

//*****************************************************************************
// Calculates the altitude error as the difference between the altitude
// input and the ADC percentage.
// @return The altitude error.
int32_t altitudeError(void) {
    return altitudeIn - ADCPercentage();
}

//*****************************************************************************
// Gets the current mean ADC value.
// @return The mean ADC value.
int32_t getMeanADC(void) {
    return meanADC;
}

//*****************************************************************************
// Gets the current altitude input.
// @return The altitude input.
int32_t getAltitudeIn(void) {
    return altitudeIn;
}

//*****************************************************************************
// Performs landing logic based on the current altitude input and error.
// If the altitude input is above the minimum altitude, it is decreased by the specified amount.
// If the altitude input is at the minimum altitude and 
// the altitude error is zero, the main and tail motors are stopped 
// and the state is set to LANDED.
// @param amount The amount to decrease the altitude input by.
void altLanding(int16_t amount) {
    if (altitudeIn > MIN_ALTITUDE) {
        altitudeIn -= amount;
        if (altitudeIn < MIN_ALTITUDE) {
            altitudeIn = MIN_ALTITUDE;
        }
    } else if (altitudeIn == MIN_ALTITUDE) {
        if (altitudeError() == 0) {
            stopMain();
            stopTail();
            setState(LANDED);
        }
    }
}

