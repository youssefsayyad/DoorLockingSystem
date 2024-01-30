/******************************************************************************
 *
 * Module: DC_MOTOR
 *
 * File Name: dc_motor.h
 *
 * Description: header file for the DC MOTOR driver
 *
 * Author: Youssef ELSayyad
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                DC Motor Pins                                *
 *******************************************************************************/
#define DC_MOTOR_PORT_ID  PORTB_ID

#define DC_MOTOR_IN1_PINID PIN5_ID
#define DC_MOTOR_IN2_PINID PIN6_ID
#define DC_MOTOR_EN1_PINID PIN3_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	STOP,CLOCKWISE,A_CLOCKWISE
}DcMotor_State;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void);

/*
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor
 * based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
