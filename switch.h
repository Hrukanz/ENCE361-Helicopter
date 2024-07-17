//*****************************************************************************
//
// switch.h
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the switch state changes.
//
//*****************************************************************************


#ifndef SWITCH_H_
#define SWITCH_H_

enum switchStates {SAME = 0, SWITCH_UP, SWITCH_DOWN};
typedef enum switchStates switchState_t;

//*****************************************************************************
// Initialise Switch State.
void initSwitch(void);

//*****************************************************************************
// Update Switch State.
void updateSwitch(void);

//*****************************************************************************
// Check the switch state and update it accordingly
// @return Switch State Change.
switchState_t checkSwitch(void);


#endif /* SWITCH_H_ */
