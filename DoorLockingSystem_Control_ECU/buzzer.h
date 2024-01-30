/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: header file for the BUZZER driver
 *
 * Author: Youssef ELSayyad
 *
 *******************************************************************************/
#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                DC Motor Pins                                *
 *******************************************************************************/
#define BUZZER_PORT_ID  PORTD_ID

#define BUZZER_PINID PIN6_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :Setup the direction for the buzzer pin as output pin through the GPIO driver
Turn off the buzzer through the GPIO.
 */
void Buzzer_init(void);
/*
 * Description :Function to enable the Buzzer through the GPIO.
 */
void Buzzer_ON(void);
/*
 * Description :Function to disable the Buzzer through the GPIO.
 */
void Buzzer_OFF(void);

#endif /* BUZZER_H_ */
