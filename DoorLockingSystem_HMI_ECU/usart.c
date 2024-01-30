/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Youssef ElSayyad
 *
 *******************************************************************************/

#include "usart.h"
#include <avr/io.h> /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/interrupt.h>
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*static volatile uint8 receiveddata=0;*/
/* Global variables to hold the address of the call back function in the application */
//static volatile uint8 recievedByte = 0;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
/*
 ISR(USART_RXC_vect){
 receiveddata=UDR;
 UCSRA|=(1<<RXC);
 }*/
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void USART_init(const USART_ConfigType *Config_Ptr) {
	uint16 ubrr_value = 0;
	/* U2X = 1 for double transmission speed */
	UCSRA = (1 << U2X);
	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/
	UCSRB = (1 << RXEN) | (1 << TXEN);
	/************************** UCSRC Description **************************
	 * URSEL     The URSEL must be one when writing the UCSRC
	 * UMSEL     Asynchronous Operation
	 * UPM1:0    Disable parity bit
	 * USBS      One stop bit
	 * UCSZ1:0   For 8-bit data mode
	 * UCPOL     Used with the Synchronous operation only
	 ***********************************************************************/
	UCSRC = (1 << URSEL);
	UCSRC = (UCSRC & 0xBF) | ((Config_Ptr->mode) << 6);
	UCSRC = (UCSRC & 0xCF) | ((Config_Ptr->parity) << 5);
	UCSRC = (UCSRC & 0xF7) | ((Config_Ptr->stop) << 3);
	UCSRC = (UCSRC & 0xF9) | ((Config_Ptr->size) << 1);
	/* Calculate the UBRR register value */
	ubrr_value = (uint16) (((F_CPU / ((Config_Ptr->BaudRate) * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value >> 8;
	UBRRL = ubrr_value;
}
void USART_sendByte(uint8 data) {
	UDR = data;
	while (BIT_IS_CLEAR(UCSRA, TXC)) {
	} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA, TXC); // Clear the TXC flag
}
uint8 USART_recieveByte(void) {
	while (BIT_IS_CLEAR(UCSRA, RXC)) {
	}
	return UDR;
}
void USART_sendString(const uint8 *Str) {
	uint8 i = 0;
	/* Send the whole string */
	while (Str[i] != '\0') {
		USART_sendByte(Str[i]);
		i++;
	}
}
void USART_receiveString(uint8 *Str) {
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = USART_recieveByte();

	/* Receive the whole string until the '$' */
	while (Str[i] != '$') {
		i++;
		Str[i] = USART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
