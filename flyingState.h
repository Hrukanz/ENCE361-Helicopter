//*****************************************************************************
//
// flyingState.h
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the state the helicopter is in
//
//*****************************************************************************

#ifndef FLYINGSTATE_H_
#define FLYINGSTATE_H_

//*****************************************************************************
// Flying states enumerator
enum flyingStates {LANDED = 0,
                   FINDING_YAW_REF,
                   FLYING,
                   LANDING_YAW,
                   LANDING_ALT};
typedef enum flyingStates flyingState_t;

//*****************************************************************************
// Set the current state
// @param stateIn input state to change the state to.
void setState(flyingState_t stateIn);

//*****************************************************************************
// Gets the current state
// @return state the current state
flyingState_t getState(void);

//*****************************************************************************
// Switch case to display the current state
// @return current state to display
char* stateString(void);

#endif /* FLYINGSTATE_H_ */
