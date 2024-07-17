/*
 * yaw.h
 *
 *  Created on: 18/05/2023
 *      Author: hya73
 */

#ifndef YAW_H_
#define YAW_H_

//*****************************************************************************
// Function to initialize yaw
void initYaw(void);

//*****************************************************************************
// Interrupt handler for yaw channels A and B
void yawChannelsIntHandler(void);

//*****************************************************************************
// Function to convert yaw to range of -180 degrees to 180 degrees
// @param yaw yaw to convert
// @return yaw converted yaw
int32_t convert(int32_t yaw);

//*****************************************************************************
// Function to calculate the yaw angle.
// @return yaw angle after calculating.
int32_t yawAngle(void);

//*****************************************************************************
// Function to set yaw input
// @param yawAmount amount to add/subrtract from the yawIn
void setYawInput(int32_t yawAmount);

//*****************************************************************************
// Get the yaw Error
// @return wanted yaw - current yaw angle 
int32_t yawError(void);

//*****************************************************************************
// Change yaw landing with specified amount
// @param amount amount the subtract or add from the yaw in.
void yawLanding(int16_t amount);

//*****************************************************************************
// Change yaw reference with specified amount
// @param amount amount the subtract or add from the yaw in.
void yawRef(int16_t amount);

//*****************************************************************************
// Get the yaw In
// @return yaw in the yaw input.
int32_t getYawIn(void);

#endif /* YAW_H_ */
