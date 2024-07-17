//*****************************************************************************
//
// display.h
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for displaying 
// relevant information about the helicopter on the microcontroller.
//
//*****************************************************************************


#ifndef DISPLAY_H_
#define DISPLAY_H_



//*****************************************************************************
// intialise the Orbit OLED display
void initDisplay (void);

//*****************************************************************************
// Function to display ADC percentage, Yaw angle, main power, and tail power.
void display(void);


#endif /* DISPLAY_H_ */
