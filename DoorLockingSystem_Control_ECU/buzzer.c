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
#include "buzzer.h"
#include "std_types.h"
#include "gpio.h"
#include "common_macros.h"
/*
 * Description :Setup the direction for the buzzer pin as output pin through the GPIO driver
 Turn off the buzzer through the GPIO.
 */
void Buzzer_init(void) {
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PINID, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PINID, LOGIC_LOW);
}
/*
 * Description :Function to enable the Buzzer through the GPIO.
 */
void Buzzer_ON(void) {
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PINID, LOGIC_HIGH);

}
/*
 * Description :Function to disable the Buzzer through the GPIO.
 */
void Buzzer_OFF(void) {
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PINID, LOGIC_LOW);
}
