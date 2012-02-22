/*******************************************************************************
* This file provides the functions for the UART module for MC40SE using PIC16F887
*
* Author: Ober Choo Sui Hong @ Cytron Technologies Sdn. Bhd.
*******************************************************************************/



#include <htc.h>
#include "system.h"
#include "uart.h"



/*******************************************************************************
* PUBLIC FUNCTION: uart_init
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Initialize the UART module.
*
*******************************************************************************/
void uart_init(void)
{
#if defined (_16F887) //if this file is compile for PIC16F887
	BRG16 = 0;									// Use 8 bit BRG
	SPBRGH = 0;
#endif
	BRGH = 1;									// Select high speed baud rate.
	SPBRG = (_XTAL_FREQ / 16 / UART_BAUD) - 1;	// Configure the baud rate.
	SPEN = 1;									// Enable serial port.
	CREN = 1;									// Enable reception.
	TXEN = 1;									// Enable transmission.
	SYNC = 0;									// Asynchronous communication
}



/*******************************************************************************
* PUBLIC FUNCTION: uart_tx
*
* PARAMETERS:
* ~ uc_data		- The data that we want to transmit.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* This function will transmit one byte of data using UART. This is a blocking
* function, if the transmit buffer is full, we will wait until the
* data in the buffer has been sent out before we move in the new data.
*
*******************************************************************************/
void uart_tx(unsigned char uc_data)
{
	// Wait until the transmit buffer is ready for new data.
	while (TXIF == 0);
	
	// Transmit the data.
	TXREG = uc_data;
}



/*******************************************************************************
* PUBLIC FUNCTION: uc_uart_rx
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ The data received from the UART.
*
* DESCRIPTIONS:
* This function will receive one byte of data using UART. This is a blocking
* function because if the receive buffer is empty, we will wait until the
* data is received.
*
*******************************************************************************/
unsigned char uc_uart_rx(void)
{
	unsigned char temp = 0;
	// If there is overrun error...
	if (OERR == 1) {
		// Clear the flag by disable and enable back the reception.
		CREN = 0;
		CREN = 1;
		temp = RCREG;	// clear RC register
	}	
	
	// Wait until there is data available in the receive buffer.
	while (RCIF == 0);
	
	// Return the received data.
	return RCREG;
}



/*******************************************************************************
* PUBLIC FUNCTION: uart_putstr
*
* PARAMETERS:
* ~ csz_string	- The null terminated string to transmit.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Transmit a string using the UART.
*
*******************************************************************************/
void uart_putstr(const char* csz_string)
{
	// Loop until the end of string.
	while (*csz_string != '\0') {
		uart_tx(*csz_string);
			
		// Point to next character.
		csz_string++;
	}
}