/******************************************************************************
 *
 * Module: Human Machine Interface
 *
 * File Name: HMI_ECU.c
 *
 * Description: Source file for the HMI ECU Code
 *
 * Author: Youssef ElSayyad
 *
 *******************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "usart.h"
#include <util/delay.h>
/*******************************************************************************
 *                           Global Variables & Defines                        *
 *******************************************************************************/
uint8 key;
uint8 PasswordStatus;
#define READY 0x11
#define COMMAND 0x12
#define NOT_MATCHING 0x13
#define MATCHING 0x14
#define OPEN_DOOR 0x15
#define HOLD 0x16
#define DOOR_OPENED 0x17
#define DOOR_CLOSED 0x18
#define CHANGE_PASSWORD 0x19
#define SEND_PASSWORD 0x21
#define SEND_CHECK 0x22
#define BUZZER_ON 0x23
#define BUZZER_OFF 0x24
#define OPEN_DOOR_OPTION '+'
#define CHANGE_PASSWORD_OPTION '-'
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function creates a password for the system by taking the password two times and
 * sending it to the Control ECU to check if they match
 */
void CreatingSystemPassword(void) {
	/*Arrays to hold the password first and second entry*/
	uint8 password[5] = { '@', '@', '@', '@', '@' };
	uint8 passwordCheck[5] = { '*', '*', '*', '*', '*' };
	key = 0;
	LCD_displayString("Enter NEW Pass:");
	LCD_moveCursor(1, 0);
	/*while loop to fill the first password entry array*/
	while (key != '=') {
		key = KEYPAD_getPressedKey();
		if ((key >= 0 && key < 10) && (password[0] == '@')) {
			password[0] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[1] == '@')) {
			password[1] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[2] == '@')) {
			password[2] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[3] == '@')) {
			password[3] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[4] == '@')) {
			password[4] = key;
			LCD_displayCharacter('*');
		}
		_delay_ms(200);
	}/*sending the first password array to the Control ECU*/
	USART_sendByte(SEND_PASSWORD);
	for (int i = 0; i < 5; i++) {
		USART_sendByte(password[i]);
	}
	LCD_clearScreen();
	key = 0;
	LCD_displayString("Re-Enter the");
	LCD_moveCursor(1, 0);
	LCD_displayString("same pass:");
	/*while loop to fill the second password entry array*/
	while (key != '=') {
		key = KEYPAD_getPressedKey();
		if ((key >= 0 && key < 10) && (passwordCheck[0] == '*')) {
			passwordCheck[0] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (passwordCheck[1] == '*')) {
			passwordCheck[1] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (passwordCheck[2] == '*')) {
			passwordCheck[2] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (passwordCheck[3] == '*')) {
			passwordCheck[3] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (passwordCheck[4] == '*')) {
			passwordCheck[4] = key;
			LCD_displayCharacter('*');
		}
		_delay_ms(200);
	}
	/*sending the second password array to the Control ECU*/
	USART_sendByte(SEND_CHECK);
	for (int i = 0; i < 5; i++) {
		USART_sendByte(passwordCheck[i]);
	}

}
/*Description : Function for Entering the saved password and sending it to the Control ECU*/
uint8 EnteringPassword(void) {
	key = 0;
	/*Array to store the password*/
	uint8 password[5] = { '@', '@', '@', '@', '@' };
	uint8 status;
	LCD_clearScreen();
	LCD_displayString("Plz Enter Pass:");
	LCD_moveCursor(1, 0);
	/*while loop to fill password entry array*/
	while (key != '=') {
		key = KEYPAD_getPressedKey();
		if ((key >= 0 && key < 10) && (password[0] == '@')) {
			password[0] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[1] == '@')) {
			password[1] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[2] == '@')) {
			password[2] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[3] == '@')) {
			password[3] = key;
			LCD_displayCharacter('*');
		} else if ((key >= 0 && key < 10) && (password[4] == '@')) {
			password[4] = key;
			LCD_displayCharacter('*');
		}
		_delay_ms(300);
	}
	/*sending the first password array to the Control ECU*/
	USART_sendByte(SEND_PASSWORD);
	for (int i = 0; i < 5; i++) {
		USART_sendByte(password[i]);
	}
	/*Waiting for a response to make sure they are Synched*/
	while (USART_recieveByte() != READY) {}
	/*Receiving the status from the Control ECU if the password matches the saved password or not*/
	status = USART_recieveByte();
	return status;
}
/* Description : Function displaying status for the open door option*/
void OpeningDoorOption(void) {
	/*Send READY to the Control ECU and Waiting for a response to make sure they are Synched*/
	USART_sendByte(READY);
	LCD_clearScreen();
	LCD_displayString("DOOR UNLOCKING");
	while (USART_recieveByte() != DOOR_OPENED) {}
	/*Waiting for a response to make sure they are Synched*/
	LCD_clearScreen();
	LCD_displayString("DOOR OPEN");
	while (USART_recieveByte() != HOLD) {}
	/*Waiting for a response to make sure they are Synched*/
	LCD_clearScreen();
	LCD_displayString("DOOR LOCKING");
	while (USART_recieveByte() != DOOR_CLOSED) {}
	LCD_clearScreen();
}
/* Description : Function displaying status and takes input from user
 *  for change password option*/
void ChangePassOption(void) {
	/*Send READY to the Control ECU to make sure they are Synched*/
	USART_sendByte(READY);
	/*Takes input Passwords and sends them to the Control ECU to check on them
	 * until the control responds that they match and displays that the password is created*/

	do {
		LCD_clearScreen();
		CreatingSystemPassword();
		while (USART_recieveByte() != READY) {
		}
		PasswordStatus = USART_recieveByte();
	} while (PasswordStatus != MATCHING);
	LCD_clearScreen();
	LCD_displayString("Password Created");
	_delay_ms(1000);
	LCD_clearScreen();
}
void WrongPasswordEntry(void (*func_ptr)(void)) {
	/*Variable to count the times for password entry*/
	uint8 count = 0;
	/*sends the input password to Control ECU to check if it matches the saved password
	 *  maximum 2 times and if it matches before this it exits the loop
	 *  and calls the selected option function if not it displays ERROR*/
	do {
		LCD_clearScreen();
		PasswordStatus = EnteringPassword();
		count++;
	} while ( (count < 2) && (PasswordStatus != MATCHING) );
	LCD_clearScreen();
	if (count == 2 && (PasswordStatus != MATCHING) ) {
		/*displaying ERROR*/
		LCD_displayString("ERROR");
		USART_sendByte(BUZZER_ON);
		/*Waiting for the Control ECU to turn off buzzer so the system goes to normal state*/
		while (USART_recieveByte() != BUZZER_OFF) {}
		LCD_clearScreen();
	} else if (PasswordStatus == MATCHING) {
		/*Calling the selected option function*/
		func_ptr();
	}
}

/* Description : Function that displays the menu options */
void MainOptions(void) {
	key = 0;
	PasswordStatus = 0;
	LCD_displayString("+ : Open Door");
	LCD_moveCursor(1, 0);
	LCD_displayString("- : Change Pass");
	/*taking input from user for the selected option*/
	while ((key != OPEN_DOOR_OPTION) && (key != CHANGE_PASSWORD_OPTION)) {
		key = KEYPAD_getPressedKey();
		_delay_ms(300);
	}
	/*Switches on the selected option by the user*/
	switch (key) {
	/*if the selected option is to OPEN DOOR checks sends to Control ECU
	 * that a command is selected and sends it too and asks the user for the system password
	 * if it matches calls the function to open door if not it calls the function responsible
	 * for wrong password entry*/
	case OPEN_DOOR_OPTION: {
		USART_sendByte(COMMAND);
		USART_sendByte(OPEN_DOOR);
		PasswordStatus = EnteringPassword();
		if (PasswordStatus == MATCHING) {
			OpeningDoorOption();
		} else if (PasswordStatus == NOT_MATCHING) {
			WrongPasswordEntry(OpeningDoorOption);
		}
		break;
	}
	/*if the selected option is to CHANGE PASSWORD checks sends to Control ECU
	 * that a command is selected and sends it too and asks the user for the system password
	 * if it matches calls the function to Change Password if not
	 * it calls the function responsible for wrong password entry*/
	case CHANGE_PASSWORD_OPTION: {
		USART_sendByte(COMMAND);
		USART_sendByte(CHANGE_PASSWORD);
		PasswordStatus = EnteringPassword();
		if (PasswordStatus == MATCHING) {
			ChangePassOption();
		}else if(PasswordStatus == NOT_MATCHING){
			WrongPasswordEntry(ChangePassOption);
		}
		break;
	}
	}
	LCD_clearScreen();
}

int main() {
	/*USART Structure Configuration
		 * Mode:ASYNCHRONZIED
		 * Parity:DISABLED
		 * STOP_BIT: ONEBIT
		 * BIT-DATA: 8BITS
		 * BAUD RATE:9600*/
	USART_ConfigType Config_Ptr = { ASYNCH, DISABLED, ONEBIT, EIGHTBITS, 9600 };
	/*Initializing the USART*/
	USART_init(&Config_Ptr);
	/*Initializing the LCD*/
	LCD_init();
	/*Send READY to the Control ECU and Waiting for a response to make sure they are Synched*/
	USART_sendByte(READY);
	while (USART_recieveByte() != READY) {}
	/*Creating system password for first time*/
	do {
		LCD_clearScreen();
		CreatingSystemPassword();
		while (USART_recieveByte() != READY) {}
		PasswordStatus = USART_recieveByte();
	} while (PasswordStatus != MATCHING);
	LCD_clearScreen();
	LCD_displayString("Password Created");
	_delay_ms(1000);
	LCD_clearScreen();
	while (1) {
		/*Main Options Function so that LCD Always Displays the system option*/
		MainOptions();
	}
}
