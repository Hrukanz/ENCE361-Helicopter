//*****************************************************************************
//
// uart.c
//
// Author: Daniel Neal & Haruka Yamamoto
// Last modified:   20/3/23
//
// Module for controlling the Uart Display.
// Uses Lab code.
//
//*****************************************************************************


#ifndef UART_H_
#define UART_H_

//*****************************************************************************
// Initialise the UART module
void initUart (void);

//*****************************************************************************
// Transmits a message
void status(void);

//*****************************************************************************
// Transmit string via UART.
// Uses a blocking function for sending characters.
// @param string string to write to the UART Tx buffer.
void send(char *string);

#endif /* UART_H_ */
