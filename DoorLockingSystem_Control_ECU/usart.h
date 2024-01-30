 /*************************************1*****************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef USART_H_
#define USART_H_

#include "std_types.h"
#define F_CPU 8000000
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	ASYNCH,SYNCH
}USART_MODE;
typedef enum
{
	DISABLED,EVENPARITY=2,ODDPARITY
}USART_ParityMode;

typedef enum
{
	ONEBIT,TWOBITS
}USART_StopSelect;
typedef enum
{
	FIVEBITS,SIXBITS,SEVENBITS,EIGHTBITS,NINEBITS=7
}USART_CharacterSize;
typedef struct
{
	USART_MODE mode;
	USART_ParityMode parity;
	USART_StopSelect stop;
	USART_CharacterSize size;
	uint16 BaudRate;
}USART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void USART_init(const USART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void USART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 USART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void USART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '$' symbol through UART from the other UART device.
 */
void USART_receiveString(uint8 *Str); // Receive until $

#endif /* USART_H_ */
