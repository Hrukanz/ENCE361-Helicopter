//*****************************************************************************
//
// altitude.h
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for measuring the altitude of the helicopter.
//
//*****************************************************************************

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

//*****************************************************************************
// Calls the initialisers for the altitude class
void initAltitude(void);

//*****************************************************************************
// Initializes the ADC for single-ended sampling of channel 9 
// with interrupts enabled.
void initADC (void);

//*****************************************************************************
// Interrupt handler for the ADC conversion complete interrupt. 
// Writes the ADC sample to the circular buffer 
// and calculates the mean ADC value.
void ADCIntHandler(void);

//*****************************************************************************
// Function to sample the ADC and set the mean ADC value.
void meanSample(void);

//*****************************************************************************
// Waits for the circular buffer to be filled to initialise the minimum ADC
void circBuffer(void);

//*****************************************************************************
// Function to calculate the ADC percentage.
// @return The ADC percentage calculated from the mean.
int32_t ADCPercentage(void);

//*****************************************************************************
// Sets the altitude input based on the specified altitude change.
// @param altChange The altitude change to apply to the input.
void setAltitudeInput(int32_t altChange);

//*****************************************************************************
// Calculates the altitude error as the difference between the altitude
// input and the ADC percentage.
// @return The altitude error.
int32_t altitudeError(void);

//*****************************************************************************
// Gets the current mean ADC value.
// @return The mean ADC value.
int32_t getMeanADC(void);

//*****************************************************************************
// Gets the current altitude input.
// @return The altitude input.
int32_t getAltitudeIn(void);

//*****************************************************************************
// Performs landing logic based on the current altitude input and error.
// If the altitude input is above the minimum altitude, it is decreased by the specified amount.
// If the altitude input is at the minimum altitude and 
// the altitude error is zero, the main and tail motors are stopped 
// and the state is set to LANDED.
// @param amount The amount to decrease the altitude input by.
void altLanding(int16_t amount);

#endif /* ALTITUDE_H_ */
