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

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	NO_CLOCK, F_CPU_CLOCK, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024
} TIMER1_PRESCALER;

typedef enum {
	NORMAL, COMPARE = 2
} TIMER1_MODE;

typedef struct {
	TIMER1_MODE mode;
	uint16 initialValue;
	uint16 compareValue;
	TIMER1_PRESCALER prescaler;
} TIMER1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer driver
 * 	1. Set the required mode.
 * 	2. Set the required initial and compare value.
 * 	2. Set the required prescaler.
 * 	4. Initialize Timer1 Registers
 */
void Timer1_init(const TIMER1_ConfigType *Config_Ptr);

/*
 * Description: Function to disable the Timer1.
 */
void Timer1_deInit(void);

/*
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void (*a_ptr)(void));
#endif /* ICU_H_ */
