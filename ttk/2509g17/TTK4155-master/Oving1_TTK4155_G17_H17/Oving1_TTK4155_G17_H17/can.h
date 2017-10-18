/*
 * can.h
 *
 * Created: 16.10.2017 10:46:45
 *  Author: pamarton
 */ 


#ifndef CAN_H_
#define CAN_H_

//------------------------------------------//
//	INCLUDES								//
//------------------------------------------//

#include "MCP2515.h"
#include "MCP2515_driver.h"
#include "SPI.h"
#include <avr/interrupt.h>
#include "common.h"

//------------------------------------------//
//	DEFINITIONS								//
//------------------------------------------//

#define CANINTE 0x2B

//------------------------------------------//
//	STRUCTS									//
//------------------------------------------//

typedef struct CAN_message_t{
	int id;
	uint8_t length;
	uint8_t data[CAN_MESSAGE_LENGTH];
} CAN_message_t;

//------------------------------------------//
//	FUNCTIONS								//
//------------------------------------------//

void CAN_initialize(void);
CAN_message_t* CAN_message_receive(void);
void CAN_send_byte(CAN_message_t* message,uint8_t n);//Sends the data
void CAN_message_send(uint8_t *data);//array of uint8_t as input, sends the massage to the CAN_send_byte
void CAN_construct_meassage(int id, uint8_t length);//Used to redefine the ID and length of the messages
uint8_t CAN_transmit_complete(void);//Function that waits for the transmission to be complete

//------------------------------------------//
//	INTERRUPTS								//
//------------------------------------------//

ISR(INT2_vect);//Interrupt that fires when a message appears - you got mail!

#endif /* CAN_H_ */