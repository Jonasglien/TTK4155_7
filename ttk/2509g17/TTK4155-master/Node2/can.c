/*
* can.c
*
* Created: 16.10.2017 10:46:30
*  Author: pamarton
*/

#include "can.h"

#define F_CPU 16000000UL
#include <util/delay.h>
static CAN_message_t CAN_receive_buffer; //VOLATILE ??????????????????????????????
static CAN_message_t CAN_send_buffer;
uint8_t FLAG_new_message = 0;

void CAN_initialize(void){

	MCP2515_initialize();

	// Turn mask/filters off; receive any message AND Enable Rollover
	MCP2515_bit_modify(MCP_RXB0CTRL,0b01100100,0xFF);
	
	// RX0IE Receive Buffer 0 FUll Interrupt Enable bit
	MCP2515_bit_modify(MCP_CANINTE,0x01,1);
	//MCP2515_bit_modify(MCP_CANINTF,0b00000001,1);
	
	//set CAN mode
	MCP2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_LOOPBACK); 
	
	// Initialize empty message for transmission
	CAN_construct_message(CAN_ID,CAN_MESSAGE_LENGTH);
	
	volatile uint8_t value = MCP2515_read(MCP_CANSTAT);
	printf("MCP_CANSTAT: %i\n", value);	
	
	CAN_interrupt_setup();
	
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

void CAN_message_send(uint8_t *data){//array of uint8_t as input
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

	MCP2515_request_to_send(MCP_MERRF+(1<<n));
	//printf("CAN_send_byte: ID %i Length: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_send_buffer.id,CAN_send_buffer.length,CAN_send_buffer.data[0],CAN_send_buffer.data[1],CAN_send_buffer.data[2],CAN_send_buffer.data[3],CAN_send_buffer.data[4],CAN_send_buffer.data[5],CAN_send_buffer.data[6],CAN_send_buffer.data[7]);

}

uint8_t CAN_read(uint8_t adr){
	return MCP2515_read(adr);
}

uint8_t recieve_flag = 0;

void CAN_data_receive(void) {
	//printf("CAN receive\n");
	if(recieve_flag){
		uint8_t n = CAN_transmit_complete();
		//printf("CAN_transmit_complete: \%i\n", n);
		if (n >= 0){
			CAN_receive_buffer.id = ((CAN_read(RXBnSIDH + n*0x10)<<3)|(0b11100000 & (CAN_read(RXBnSIDL + n*0x10))>>5));
			CAN_receive_buffer.length = (CAN_read(RXBnDLC + n*0x10) & 0b00001111);
			for (uint8_t m = 0; m < CAN_receive_buffer.length; m++){
				CAN_receive_buffer.data[m] = CAN_read(RXBnDM + m);
			}
		}
		//printf("%i",CAN_receive_buffer.data[0]);
		//printf("CAN_data_receive: ID %i L: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_receive_buffer.id,CAN_receive_buffer.length,CAN_receive_buffer.data[0],CAN_receive_buffer.data[1],CAN_receive_buffer.data[2],CAN_receive_buffer.data[3],CAN_receive_buffer.data[4],CAN_receive_buffer.data[5],CAN_receive_buffer.data[6],CAN_receive_buffer.data[7]);
		recieve_flag = 0;
	}
}

ISR(INT4_vect){//interrupt incoming message
	//cli();
	//IF there is a message in inbox 0, flag 1<<0, then if there is a message in inbox 1, flag 1<<1 this
	//FLAG_new_message = 0b01;
	recieve_flag = 1;
	_delay_ms(100);
	//printf('MSG');
	//PORTE |= (1<<PE4);
	//sei();
	MCP2515_bit_modify(MCP_CANINTF,0xFF,0); //clear all interrupts //CHANGE THIS SO IT JUST CLEARS THE CORRECT INTERRUPTS
}
	
void CAN_interrupt_setup(void){
	cli();
	// Use PE4 as external interrupt pin
	DDRE &= ~(1<<PINE4);
	
	// Trigger interrupt on any signal change (Compare sec 15.2.2 of Atmega2560 data sheet).
	//EICRB |= (1<<ISC40);
	EICRB &= ~(1<<ISC40);
	EICRB &= ~(1<<ISC41);
	//EICRB &= ~(1<<ISC41);
	
	// Enable INT4 (Compare sec 15.2.3 of Atmega2560 data sheet).
	EIMSK |= (1<<INT4);
	
	//Enable global interrupt.
	sei();
}


uint8_t CAN_transmit_complete(void){
	
	
	// AMUND CAN YOU EXPLAIN WHAT HAPPENS HERE
	

	volatile char interrupt = MCP2515_read(MCP_CANINTF); ; //MCP2515_read(MCP_CANINTF);
		/*
		while((!((1 << MCP_RX0IF) ))&&(!((1 << MCP_RX1IF)))){ 
			interrupt = MCP2515_read(MCP_CANINTF);
		}
		if((1 << MCP_RX0IF)){
			return 0;
		}
		else if((1 << MCP_RX1IF)){
			return 1;
		}
		else{
			return -1;
		}
	
		*/
	
	while((!((1 << MCP_RX0IF) & interrupt))&&(!((1 << MCP_RX1IF)&interrupt))){
		interrupt = MCP2515_read(MCP_CANINTF);
	}
	//printf("interrupt: \%i", interrupt);
	
	if((1 << MCP_RX0IF)&interrupt){
		return 0;
	}
	else if((1 << MCP_RX1IF)&interrupt){
		return 1;
	}
	else{
		return -1;
	}
	

}