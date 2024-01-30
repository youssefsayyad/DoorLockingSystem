/******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.h
 *
 * Description: Header file for the AVR TIMER driver
 *
 * Author: Youssef ElSayyad
 *
 *******************************************************************************/
#include <avr/io.h> /* To use ICU/Timer1 Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "timer1.h"
#include <avr/interrupt.h> /* For TIMER1 ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function*/
static void (*g_TIMER1_callBack_ptr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect) {
	if (g_TIMER1_callBack_ptr != NULL_PTR) {
		/* Call the Call Back function in the application interrupt */
		(*g_TIMER1_callBack_ptr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_OVF_vect) {
	if (g_TIMER1_callBack_ptr != NULL_PTR) {
		/* Call the Call Back function in the application interrupt */
		(*g_TIMER1_callBack_ptr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer driver
 * 	1. Set the required mode.
 * 	2. Set the required initial and compare value.
 * 	2. Set the required prescaler.
 * 	4. Initialize Timer1 Registers
 */
void Timer1_init(const TIMER1_ConfigType *Config_Ptr) {
	switch (Config_Ptr->mode) {
	case 0: {
		/* Timer1 operates in Normal Mode (Non-PWM) */
		TCCR1A = (1 << FOC1A) | (1 << FOC1B);
		/* Initial Value for Timer1 */
		TCNT1 = Config_Ptr->initialValue;
		/* Enable the Overflow interrupt to generate an interrupt when the register overflows*/
		SET_BIT(TIMSK, TOIE1);
		/* Enable the global interrupt flag*/
		SREG |= (1 << 7);
		/*insert the required clock value in the first three bits (CS10, CS11 and CS12) of TCCR1B Register*/
		TCCR1B = Config_Ptr->prescaler;
		break;
	}
	case 2: {
		/* Timer1 operates in Compare Mode on (Non-PWM) */
		TCCR1A = (1 << FOC1A);
		/* Initial Value for Timer1 */
		TCNT1 = Config_Ptr->initialValue;
		/* Compare Value for Timer1 */
		OCR1A = Config_Ptr->compareValue;
		/* Enable the Timer1 Output Compare A interrupt to generate an interrupt when the register Matches the value */
		TIMSK = (1 << OCIE1A);
		/* Enable the global interrupt flag*/
		SREG |= (1 << 7);
		/*insert the required clock value in the first three bits (CS10, CS11 and CS12) of TCCR1B Register
		 * and the WGM12 BIT for CTC mode
		 */
		TCCR1B = ((Config_Ptr->mode) << 2) | (Config_Ptr->prescaler);
	}
	}
}

void Timer1_setCallBack(void (*a_ptr)(void)) {
	/* Save the address of the Call back function in a global variable */
	g_TIMER1_callBack_ptr = a_ptr;
}

void Timer1_deInit(void) {
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCNT1 = 0;
	OCR1A = 0;
	TCCR1B = 0;
	/* Disable the Output Compare A interrupt */
	CLEAR_BIT(TIMSK, OCIE1A);
	/* Disable the Overflow interrupt */
	CLEAR_BIT(TIMSK, TOIE1);
	/* Reset the global pointer value */
	g_TIMER1_callBack_ptr = NULL_PTR;
}
