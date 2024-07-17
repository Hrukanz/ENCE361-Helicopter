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
#include "yaw.h"
#include "flyingState.h"

//****************************************************************
// Constants
//****************************************************************
#define BAUD_RATE           9600

// Uses UART0 module with Rx pin PA0 and Tx pin PA1.
#define UART_BASE           UART0_BASE
#define UART_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_GPIO_BASE      GPIO_PORTA_BASE
#define UART_GPIO_PIN_RX    GPIO_PIN_0
#define UART_GPIO_PIN_TX    GPIO_PIN_1
#define UART_GPIO_PINS      UART_GPIO_PIN_RX | UART_GPIO_PIN_TX

// Transmits with word length 8, one stop bit and no parity bits.
#define UART_WORD_LEN       UART_CONFIG_WLEN_8
#define UART_STOP_BIT       UART_CONFIG_STOP_ONE
#define UART_PAR_BIT        UART_CONFIG_PAR_NONE
#define UART_CONFIG         UART_WORD_LEN | UART_STOP_BIT | UART_PAR_BIT

// The number of characters to send over UART at a time.
#define STR_LEN             18



//*****************************************************************************
// Initialise the UART module
void initUart (void) {
    // Enable the UART0 module and GPIO port A, which is used for UART0 pins.
    SysCtlPeripheralEnable(UART_PERIPH_UART);
    SysCtlPeripheralEnable(UART_PERIPH_GPIO);

    // Configure the Rx and Tx pins for UART use.
    GPIOPinTypeUART(UART_GPIO_BASE, UART_GPIO_PINS);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Configure the UART clock rate, baud rate, word length, stop bits
    // and parity bits
    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), BAUD_RATE, UART_CONFIG);

    // Enable Tx and Rx buffers and the UART module itself.
    UARTFIFOEnable(UART_BASE);
    UARTEnable(UART_BASE);
}


//*****************************************************************************
// Transmits a message
void status(void) {
    char line[STR_LEN + 1];

    usnprintf(line, sizeof(line), "Alt: %3d [%3d]\r\n", ADCPercentage(), getAltitudeIn());
    send(line);

    usnprintf(line, sizeof(line), "Yaw: %2d.%1d [%3d]\r\n", yawAngle()/10, abs(yawAngle()%10), getYawIn(), getYawIn);
    send(line);

    usnprintf(line, sizeof(line), "Main: %4d%%\r\n", getMainPwr());
    send(line);

    usnprintf(line, sizeof(line), "Tail: %4d%%\r\n", getTailPwr());
    send(line);

    usnprintf(line, sizeof(line), "%16s\r\n", stateString());
    send(line);
}


//*****************************************************************************
// Transmit string via UART.
// Uses a blocking function for sending characters.
// @param string string to write to the UART Tx buffer.
void send(char *string) {
    while(*string) {
        // Write the next character to the UART Tx buffer.
        UARTCharPut(UART_BASE, *string);
        string++;
    }
}


