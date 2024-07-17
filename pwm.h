//*****************************************************************************
//
// pwm.h
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the pwm output.
//
//*****************************************************************************

#ifndef PWM_H_
#define PWM_H_

#define MAX_DUTY        69
#define MAIN_MIN_DUTY   3
#define TAIL_MIN_DUTY   3

//*****************************************************************************
// Initialise Rotors
void initPWM(void);

//*****************************************************************************
// Calculate the pulse period
// @param freq the pulse period frequency.
// @return The pulse period.
static uint32_t calcPulsePeriod (uint32_t freq);

//*****************************************************************************
// Calculate the pulse width
// @param period the pulse width period.
// @param duty the pulse width duty.
// @return The pulse width.
static uint32_t calcPulseWidth (uint32_t period, uint32_t duty);

//*****************************************************************************
// Initialise the main rotors PWM.
static void initMain();

//*****************************************************************************
// Set the main rotors PWM.
// @param targetPower the target pwm power.
void setMainPower(uint32_t targetPower);

//*****************************************************************************
// Start the main rotors PWM. 
// set the main power to the minimum main rotor duty.
void startMain(void);

//*****************************************************************************
// Stop the main rotors PWM. 
// set the main power to 0.
void stopMain(void);

//*****************************************************************************
// Get the main power. 
// @return mainPower the main power
uint32_t getMainPwr(void);


//*****************************************************************************
// Initialise the tail rotors PWM
static void initTail(void);

//*****************************************************************************
// Set the tail rotors PWM.
// @param targetPower the target pwm power.
void setTailPower(uint32_t targetPower);

//*****************************************************************************
// Start the tail rotors PWM. 
// set the tail power to the minimum tail rotor duty.
void startTail(void);

//*****************************************************************************
// Stop the tail rotors PWM. 
// set the tail power to 0.
void stopTail(void);

//*****************************************************************************
// Get the tail power. 
// @return tailPower the tail power
uint32_t getTailPwr(void);

#endif /* PWM_H_ */
