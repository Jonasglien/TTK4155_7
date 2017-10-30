/*
 * can.c
 *
 * Created: 16.10.2017 10:46:30
 *  Author: pamarton
 */ 

#include "can.h"
//#include <util/delay.h>

static CAN_message_t CAN_receive_buffer;
static CAN_message_t CAN_send_buffer;
uint8_t receive_flag = 0;



void CAN_initialize(void){
	MCP2515_initialize();

	
	
	// Initialize empty message for transmission
	CAN_construct_message(CAN_ID,CAN_MESSAGE_LENGTH); 
	
	// Turn mask/filters off; receive any message AND Enable Rollover
	MCP2515_bit_modify(MCP_RXB0CTRL,0b01100100,0xFF);
	MCP2515_bit_modify(MCP_RXB1CTRL,0b01100100,0xFF);
	
	// RX0IE Receive Buffer 0 FUll Interrupt Enable bit
	MCP2515_bit_modify(MCP_CANINTE,0x01,1);
	
	//set CAN mode
	MCP2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);

	// Set up interrupts.
	CAN_interrupt_setup();
	
	printf("MCP_CANSTAT: %i\n", MCP2515_read(MCP_CANSTAT));//comment out later
	printf("CAN initialized.\n\n");
}



CAN_message_t* CAN_message_receive(void){						// What does this step do //NOT NECCECARY, but if we want to reference the recieved data from another part of the program this is necceccary
	return &CAN_receive_buffer;						// What does this step do
}

	
void CAN_construct_message(int id, uint8_t length){
	CAN_send_buffer.id = id;
	CAN_send_buffer.length = length;
	for(uint8_t i = 0; i < length; i++){
		CAN_send_buffer.data[i] = 0;
	}
}

	//array of int8_t as input (we chose int8_t over uint8_t so that we can send negative values
void CAN_message_send(int8_t *data){
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
	printf("\nSENT MESSAGE:");
	CAN_print(CAN_send_buffer);
}

int CAN_read(uint8_t adr){
	return MCP2515_read(adr);
}



void CAN_data_receive(void) {
	if(receive_flag){
		receive_flag = 0;
		uint8_t n = CAN_reception_complete();
		if (n >= 0){
			CAN_receive_buffer.id = ((CAN_read(RXBnSIDH + n*0x10)<<3)|(0b11100000 & (CAN_read(RXBnSIDL + n*0x10))>>5));
			CAN_receive_buffer.length = (CAN_read(RXBnDLC + n*0x10) & 0b00001111);
			for (uint8_t m = 0; m < CAN_receive_buffer.length; m++){
				CAN_receive_buffer.data[m] = CAN_read(RXBnDM + m);
			}
		}
		printf("\nRECIEVED MESSAGE:");
		CAN_print(CAN_receive_buffer);
	}
}

void CAN_print(CAN_message_t message){
	printf("\nID %i\tL: %i\tDATA:\n",message.id,message.length);
	for(uint8_t i = 0; i < message.length; i++){
		printf("%i\t",message.data[i]);
	}
}


ISR(INT2_vect){//interrupt incoming message
	receive_flag = 1;
}

void CAN_interrupt_setup(void){
	cli();

	//Extended MCU Control Register bit 0 ....turned off?
	EMCUCR &= ~(1<<ISC2);
	
	//General Interrupt Control Register port int2 defined as on?
	GICR |= (1<<INT2);

	//Enable global interrupt.
	sei();
}

int CAN_reception_complete(void){
	printf("MCP_CANINTF = %i",MCP2515_read(MCP_CANINTF));
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


uint8_t CAN_transmit_complete(void){


	volatile char interrupt = MCP2515_read(MCP_CANINTF);

	while((!((1 << MCP_RX0IF) & interrupt))&&(!((1 << MCP_RX1IF)&interrupt))){
		interrupt = MCP2515_read(MCP_CANINTF);
	}
	

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