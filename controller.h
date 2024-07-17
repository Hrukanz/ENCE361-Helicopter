//*****************************************************************************
//
// controller.h
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   21/3/23
//
// Module for controlling the gains through a PID controller sysystem.
//
//*****************************************************************************


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Initialize controller with update rate.
// @param updateRateIn update rate to initialise.
void initController(int32_t updateRateIn);

//*****************************************************************************
// Update PID Controllers.
void updateController(void);

#endif /* CONTROLLER_H_ */
