/*
* can.c
*
* Created: 16.10.2017 10:46:30
*  Author: pamarton
*/

#include "can.h"
#define F_CPU 16000000UL
#include <util/delay.h>

static CAN_message_t CAN_receive_buffer; 
static CAN_message_t CAN_send_buffer;
uint8_t FLAG_new_message = 0;

void CAN_initialize(void){

	MCP2515_initialize();

	// Turn mask/filters off; receive any message AND Enable Rollover
	MCP2515_bit_modify(MCP_RXB0CTRL,0b01100100,0xFF);
	MCP2515_bit_modify(MCP_RXB1CTRL,0b01100100,0xFF);
	
	// RX0IE Receive Buffer 0 FUll Interrupt Enable bit
	MCP2515_bit_modify(MCP_CANINTE,0x01,1);
	
	//set CAN mode
	MCP2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL); 
	
	// Initialize empty message for transmission
	CAN_construct_message(CAN_ID,CAN_MESSAGE_LENGTH);

	CAN_interrupt_setup();
	
	printf("MCP_CANSTAT: %i\n", MCP2515_read(MCP_CANSTAT));
	printf("CAN initialized.\n\n");

}

CAN_message_t* CAN_message_receive(void){						// What does this step do
	return &CAN_receive_buffer;						// What does this step do
}

void CAN_construct_message(int id, uint8_t length){
	CAN_send_buffer.id = id;
	CAN_send_buffer.length = length;
	for(uint8_t i = 0; i < length; i++){
		CAN_send_buffer.data[i] = 0;
	}
}

void CAN_message_send(int *data){//array of uint8_t as input
	for(uint8_t i = 0; i < CAN_send_buffer.length; i++){
		CAN_send_buffer.data[i] = data[i];
	}
	CAN_send_byte(&CAN_send_buffer,0);
}

void CAN_send_byte(CAN_message_t* message,uint8_t n){
	MCP2515_write(MCP_TXBnSIDH+n*0x10,message->id>>3);		//8 most significant bits of the message ID
	MCP2515_write(MCP_TXBnSIDL+n*0x10,message->id<<5);		//3 least significant bits of the message ID
	MCP2515_write(MCP_TXBnDLC+n*0x10,(0b00001111)&(message->length));		//Message length

	for (uint8_t i = 0; i < message->length;i++){
		MCP2515_write(MCP_TXBnD0+n*0x10+i,message->data[i]);	//Message data
	}

	//MCP2515_request_to_send(MCP_MERRF+(1<<n));
	MCP2515_request_to_send(n);
	printf("CAN_send_byte: \tID %i L: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_send_buffer.id,CAN_send_buffer.length,CAN_send_buffer.data[0],CAN_send_buffer.data[1],CAN_send_buffer.data[2],CAN_send_buffer.data[3],CAN_send_buffer.data[4],CAN_send_buffer.data[5],CAN_send_buffer.data[6],CAN_send_buffer.data[7]);

}

int CAN_read(uint8_t adr){
	return MCP2515_read(adr);
}

uint8_t receive_flag = 0;

void CAN_data_receive(void) {
	if(receive_flag){
		receive_flag = 0;
		uint8_t n = CAN_reception_complete();
		if (n >= 0){
			CAN_receive_buffer.id = ((CAN_read(RXBnSIDH + n*0x10)<<3)|(0b11100000 & (CAN_read(RXBnSIDL + n*0x10))>>5));
			CAN_receive_buffer.length = (CAN_read(RXBnDLC + n*0x10) & 0b00001111);
			for (uint8_t m = 0; m < CAN_receive_buffer.length; m++){
				CAN_receive_buffer.data[m] =  CAN_read(RXBnDM + m);
			}
		}
		printf("CAN_data_receive: \tID %i L: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_receive_buffer.id,CAN_receive_buffer.length,CAN_receive_buffer.data[0],CAN_receive_buffer.data[1],CAN_receive_buffer.data[2],CAN_receive_buffer.data[3],CAN_receive_buffer.data[4],CAN_receive_buffer.data[5],CAN_receive_buffer.data[6],CAN_receive_buffer.data[7]);
		
	}
}

ISR(INT4_vect){//interrupt incoming message
	
	//IF there is a message in inbox 0, flag 1<<0, then if there is a message in inbox 1, flag 1<<1 this
	receive_flag = 1;
}
	
void CAN_interrupt_setup(void){
	cli();

	// Use PE4 as external interrupt pin
	DDRE &= ~(1<<PINE4);
	
	// Trigger interrupt on r edge (Compare sec 15.2.2 of Atmega2560 data sheet).
	EICRB &= ~(1<<ISC40);	//x0
	EICRB |= (1<<ISC41);	//1x
	
	// Enable INT4 (Compare sec 15.2.3 of Atmega2560 data sheet).
	EIMSK |= (1<<INT4);

	//Enable global interrupt.
	sei();
}

uint8_t CAN_reception_complete(void){
	if(MCP2515_read(MCP_CANINTF) & MCP_RX0IF){
		MCP2515_bit_modify(MCP_CANINTF,MCP_RX0IF,0);
		return 0;
	}
	else if(MCP2515_read(MCP_CANINTF) & MCP_RX1IF){
		MCP2515_bit_modify(MCP_CANINTF,MCP_RX1IF,0);
		return 1;
	}
	else{
		return -1;
	}
}

uint8_t CAN_transmission_complete(void){
	if(MCP2515_read(MCP_CANINTF) & MCP_TX0IF){
		MCP2515_bit_modify(MCP_CANINTF,MCP_TX0IF,0);
		return 0;
	}
	else if(MCP2515_read(MCP_CANINTF) & MCP_TX1IF){
		MCP2515_bit_modify(MCP_CANINTF,MCP_TX1IF,0);
		return 1;
	}
	else if(MCP2515_read(MCP_CANINTF) & MCP_TX2IF){
		MCP2515_bit_modify(MCP_CANINTF,MCP_TX2IF,0);
		return 1;
	}
	else{
		return -1;
	}
}	
	
	
	
uint8_t CAN_transmit_complete(void){
	
	volatile char interrupt = MCP2515_read(MCP_CANINTF);

	while( (!( (1 << MCP_RX0IF) & interrupt) )  &&  (!((1 << MCP_RX1IF)&interrupt))){
		interrupt = MCP2515_read(MCP_CANINTF);
	}
	
	
	printf("Interrupt: %i", interrupt);

	if((1 << MCP_RX1IF)&interrupt){
		FLAG_new_message = 1;
		MCP2515_bit_modify(MCP_CANINTF,0xFF,1 << MCP_RX1IF); //RETURNS THE POSSITION OF THE ROLLED OVER MESSAGE, IF THERE IS ONE, and clears its interrupt
		return 1;
	}else if((1 << MCP_RX0IF)&interrupt){
		MCP2515_bit_modify(MCP_CANINTF,0xFF,1 << MCP_RX0IF); //clear all interrupts //CHANGE THIS SO IT JUST CLEARS THE CORRECT INTERRUPTS
		return 0;
	}else{
		MCP2515_bit_modify(MCP_CANINTF,0xFF,0); //clear all interrupts //CHANGE THIS SO IT JUST CLEARS THE CORRECT INTERRUPTS
		return -1;
	}
	
}