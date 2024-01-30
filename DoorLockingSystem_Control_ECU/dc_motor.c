/******************************************************************************
 *
 * Module: DC_MOTOR
 *
 * File Name: dc_motor.c
 *
 * Description: source file for the DC MOTOR driver
 *
 * Author: Youssef ELSayyad
 *
 *******************************************************************************/

#include "dc_motor.h"
#include "common_macros.h"
#include "pwm.h"
#include "timer1.h"
/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void) {
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID, DC_MOTOR_IN1_PINID, PIN_OUTPUT);

	GPIO_setupPinDirection(DC_MOTOR_PORT_ID, DC_MOTOR_IN2_PINID, PIN_OUTPUT);

	GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN1_PINID, LOGIC_LOW);

	GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN2_PINID, LOGIC_LOW);
}

/*
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor
 * based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed) {
	PWM_Timer0_Start(speed);
	switch (state) {
	case STOP: {
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN1_PINID, LOGIC_LOW);

		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN2_PINID, LOGIC_LOW);
		break;
	}
	case CLOCKWISE: {
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN1_PINID, LOGIC_HIGH);

		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN2_PINID, LOGIC_LOW);
		break;
	}
	case A_CLOCKWISE: {
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN1_PINID, LOGIC_LOW);

		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_IN2_PINID, LOGIC_HIGH);
		break;
	}
	}
}
