/******************************************************************************
 *
 * Module: Control ECU
 *
 * File Name: Control_ECU.c
 *
 * Description: Source file for the Control ECU Code
 *
 * Author: Youssef ElSayyad
 *
 *******************************************************************************/
#include "dc_motor.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "usart.h"
#include "twi.h"
#include "timer1.h"
#include <util/delay.h>
#define ERROR 0
#define SUCCESS 1
#define READY 0x11
#define COMMAND 0x12
#define NOT_MATCHING 0x13
#define MATCHING 0x14
#define OPEN_DOOR 0x15
#define HOLD 0x16
#define DOOR_OPENED 0x17
#define DOOR_CLOSED 0x18
#define CHANGE_PASSWORD 0x19
#define RECIEVE_PASSWORD 0x21
#define RECIEVE_CHECK 0x22
#define BUZZER_ON 0x23
#define BUZZER_OFF 0x24
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Arrays to store the password and setting default values to characters*/
uint8 g_recievedPassword[5] = { '!', '!', '!', '!', '!' };
uint8 g_recievedCheck[5] = { '%', '%', '%', '%', '%' };
/*Timer1 Configuration Pointer for Motor Control (3 secs compare Match)*/
TIMER1_ConfigType MOTORConfigurations_Ptr = { COMPARE, 0, 23436, F_CPU_1024 };
/*Timer1 Configuration Pointer for Buzzer Control (6 secs compare Match)*/
TIMER1_ConfigType BUZZERConfigurations_Ptr = { COMPARE, 0, 46875, F_CPU_1024 };
/*Variable to hold the status of the two passwords*/
uint8 status;
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/* Description : Function Receives the first and second password from the HMI ECU */
void RecieveBothPasswords(void) {
	/*Waiting for a response from HMI ECU to make sure they are Synched*/
	while (USART_recieveByte() != RECIEVE_PASSWORD) {}
	/*Recieveing Passwords from the HMI ECU*/
	for (int i = 0; i < 5; i++) {
		g_recievedPassword[i] = USART_recieveByte();
	}
	while (USART_recieveByte() != RECIEVE_CHECK) {}
	for (int i = 0; i < 5; i++) {
		g_recievedCheck[i] = USART_recieveByte();
	}
}
/*
 * Description : Function checks if the recieved passwords from the HMI ECU Match or not
 * and sends the result to the HMI ECU
 */
uint8 CheckMatching(void) {
	/*variable to count if the 5 elements of the password are matching*/
	uint8 check = 0;
	/*checks if the two passwords sent by HMI ECU match*/
	for (int i = 0; i < 5; i++) {
		if (g_recievedCheck[i] == g_recievedPassword[i]) {
			check++;
		}
	}
	/*if they match sending to HMI ECU they match and returns SUCCESS*/
	if (check == 5) {
		USART_sendByte(READY);
		USART_sendByte(MATCHING);
		return SUCCESS;
	}
	/*if they dont match sending to HMI ECU they dont match and returns ERROR*/
	else {
		USART_sendByte(READY);
		USART_sendByte(NOT_MATCHING);
		return ERROR;
	}
}
/*
 * Description : Function saves the recieved password from the HMI ECU */
void SavingPassword(void) {
	/*Saving the password sent by HMI ECU in the EEPROM and sets the Array to default value*/
	for (int i = 0; i < 5; i++) {
		EEPROM_writeByte((0x0311 + i), g_recievedPassword[i]);
		_delay_ms(10);
		g_recievedPassword[i] = '!';
	}
}
/*
 * Description : Function checks if they Entered password sent by the HMI ECU matches the
 * saved password in the EEPROM and sends the result to the HMI ECU
 */
uint8 CheckSavedPassword(void) {
	/*Array to store the password saved in the EEPROM*/
	uint8 savedPassword[5];
	/*variable to count if the 5 elements of the password are matching*/
	uint8 check = 0;
	/*waiting for HMI ECU to send a password to check it*/
	while (USART_recieveByte() != RECIEVE_PASSWORD) {}
	/*Receiving the password sent by the HMI ECU*/
	for (int i = 0; i < 5; i++) {
		g_recievedPassword[i] = USART_recieveByte();
	}
	/*Reading the saved password in the EEPROM*/
	for (int i = 0; i < 5; i++) {
		EEPROM_readByte((0x0311 + i), &savedPassword[i]);
		_delay_ms(10);
	}
	/*checking if the password sent by HMI ECU matches the one saved in the EEPROM and
	 * count the number of matching of the elements*/
	for (int i = 0; i < 5; i++) {
		if (savedPassword[i] == g_recievedPassword[i]) {
			check++;
		}
	}
	/*if they match sending to HMI ECU they match and returns SUCCESS*/
	if (check == 5) {
		USART_sendByte(READY);
		USART_sendByte(MATCHING);
		return SUCCESS;
	}
	/*if they dont match sending to HMI ECU they dont match and returns ERROR*/
	else {
		USART_sendByte(READY);
		USART_sendByte(NOT_MATCHING);
		return ERROR;
	}
}
/* Description : The Call back Function of Timer1 to control the door Motor*/
void MotorControl(void) {
	/*static variable to control the DC MOTOR*/
	static uint8 flag = 0;
	flag++;
	/*When is flag==4 (15 secs) sends to the HMI ECU that the door is opened and
	 * stops the motor for (3 secs) to leave it open*/
	if (flag == 4) {
		USART_sendByte(DOOR_OPENED);
		DcMotor_Rotate(STOP, 0);
	}
	/*When is flag==5 (18 secs) sends to the HMI ECU that the door is going to close and
	 * starts to close it*/
	if (flag == 5) {
		USART_sendByte(HOLD);
		DcMotor_Rotate(A_CLOCKWISE, 100);
	}
	/*When is flag==10 (33 secs) sends to the HMI ECU that the door is closed and
		 * stops the DC MOTOR and Deinitialize Timer1 and resets the flag variable*/
	if (flag == 10) {
		USART_sendByte(DOOR_CLOSED);
		DcMotor_Rotate(STOP, 0);
		Timer1_deInit();
		flag = 0;
	}
}
/*Description : Function to start Timer1 and Door Motor*/
void OPEN_DOOROption() {
	/*waiting for HMI ECU to be in synch*/
	while (USART_recieveByte() != READY) {}
	/*Starting Timer1 with the DC MOTOR Configurations*/
	Timer1_init(&MOTORConfigurations_Ptr);
	/*Starting the DC MOTOR With max speed*/
	DcMotor_Rotate(CLOCKWISE, 100);
}
/*Description : The Call back Function of Timer1 to control the Buzzer*/
void BuzzerControl(void) {
	/*static variable to control the Buzzer*/
	static uint8 flag = 0;
	/*When is flag==10 (60 secs) Turns the buzzer off and DEinitializie Timer1 and
	 * tells The HMI ECU that the buzzer is off*/
	flag++;
	if (flag == 10) {
		Buzzer_OFF();
		Timer1_deInit();
		USART_sendByte(BUZZER_OFF);
		flag = 0;
	}
}
/* Description : Function to recieve new password until they match and saves the new password
 * if they match in the EEPROM*/
void ChangeSystemPassword(void) {
	/*Waiting for a response from HMI ECU to make sure they are Synched*/
	while (USART_recieveByte() != READY) {}
	/*Recieveing Passwords and checks on them until they match and saves it inside EEPROM*/
	do {
		RecieveBothPasswords();
		status = CheckMatching();
	} while (status != SUCCESS);
	if (status == SUCCESS) {
		SavingPassword();
	}
}
/* Description : Function to recieve a password for maximum 2 times and checks if it matches
 * the password saved in EEPROM if it matches it calls the selected option(input for the function)
 *  if it doesnt match the system enters HOLD mode */
void WrongPasswordEntry(void (*func_ptr)(void)) {
	/*Variable to count the times for wrong password entry*/
	uint8 count = 0;
	/*checks if the password sent by HMI ECU matches the saved password in EEPROM maximum 2 times
	 and if it matches before this it exits the loop and calls the selected option function */
	do {
		status = CheckSavedPassword();
		count++;
	} while ((count < 2) && (status != SUCCESS));
	if (status == SUCCESS) {
		/*Calling the selected option function*/
		func_ptr();
	} else if (count == 2) {
		/*Setting the call back function for timer*/
		Timer1_setCallBack(BuzzerControl);
		/*waiting for HMI ECU to be in synch*/
		while (USART_recieveByte() != BUZZER_ON) {}
		/*Starting Timer1 with the Buzzer Configurations*/
		Timer1_init(&BUZZERConfigurations_Ptr);
		/*Starting the Buzzer*/
		Buzzer_ON();
	}
}

int main() {
	/*Variable to recieve the selected option sent from HMI ECU*/
	uint8 option;
	/*Initializing the DC MOTOR*/
	DcMotor_Init();
	/*Initializing the BUZZER*/
	Buzzer_init();
	/*Setting the call back of Timer1*/
	Timer1_setCallBack(MotorControl);
	/*Initializing the I2C*/
	TWI_init();
	/*USART Structure Configuration
	 * Mode:ASYNCHRONZIED
	 * Parity:DISABLED
	 * STOP_BIT: ONEBIT
	 * BIT-DATA: 8BITS
	 * BAUD RATE:9600			*/
	USART_ConfigType Config_Ptr = { ASYNCH, DISABLED, ONEBIT, EIGHTBITS, 9600 };
	/*Initializing the USART*/
	USART_init(&Config_Ptr);
	/*Send READY to the HMI ECU and Waiting for a response to make sure they are Synched*/
	USART_sendByte(READY);
	while (USART_recieveByte() != READY) {}
	/*Recieveing Passwords and checks on them until they match and saves it inside EEPROM*/
	do {
		RecieveBothPasswords();
		status = CheckMatching();
	} while (status != SUCCESS);
	if (status == SUCCESS) {
		SavingPassword();
	}
	while (1) {
		/*Waiting for the HMI ECU to send a command*/
		while (USART_recieveByte() != COMMAND) {}
		/*Receives the command sent by the HMI ECU*/
		option = USART_recieveByte();
		/*Switches on the command sent by the HMI ECU*/
		switch (option) {
		/*if the command is to OPEN DOOR checks if the Received password matches the one
		 * saved in the EEPROM and calls the function responsible for this if they dont match
		 * calls the function responsible for wrong password entry*/
		case OPEN_DOOR: {
			status = CheckSavedPassword();
			if (status == SUCCESS) {
				OPEN_DOOROption();
			} else if (status == ERROR) {
				WrongPasswordEntry(OPEN_DOOROption);
			}
			break;
		}
		/*if the command is to CHANGE PASSWORD checks if the Received password matches the one
		 * saved in the EEPROM and calls the function responsible for this if they dont match
		 * calls the function responsible for wrong password entry*/
		case CHANGE_PASSWORD: {
			status = CheckSavedPassword();
			if (status == SUCCESS) {
				ChangeSystemPassword();
			} else if (status == ERROR) {
				WrongPasswordEntry(ChangeSystemPassword);
			}
			break;
		}
		}
	}
}
