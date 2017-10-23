/*
 * can.c
 *
 * Created: 16.10.2017 10:46:30
 *  Author: pamarton
 */ 

#include "can.h"
static CAN_message_t CAN_receive_buffer; //VOLATILE ??????????????????????????????
static CAN_message_t CAN_send_buffer;
uint8_t FLAG_new_message = 0;

void CAN_initialize(void){
	MCP2515_initialize();
	int i = 1;
	i++;
	EMCUCR &= ~(1<<ISC2); /*Extended MCU Control Register bit 0 ....turned off? */
	GICR |= (1<<INT2);   /* General Interrupt Control Register port int2 defined as on?*/
	MCP2515_bit_modify(CANINTE,0b00000001,0b00000001);
	CAN_construct_meassage(CAN_ID,CAN_MESSAGE_LENGTH);
}

CAN_message_t* CAN_message_receive(void){	
	return &CAN_receive_buffer;
}

void CAN_construct_meassage(int id, uint8_t length){
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
	MCP2515_write(0x31+n*0x10,message->id>>3);//8 most significant bits of the message ID
	MCP2515_write(0x32+n*0x10,message->id<<5);//3 least significant bits of the message ID
	MCP2515_write(0x35+n*0x10,(0b00001111)&(message->length));//Message length
	
	for (uint8_t i = 0; i < message->length;i++)
	{
		MCP2515_write(0x36+n*0x10+i,message->data[i]);//Message data
	}
	
	MCP2515_request_to_send(0x80+(1<<n));
	//printf("ID %i L: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_send_buffer.id,CAN_send_buffer.length,CAN_send_buffer.data[0],CAN_send_buffer.data[1],CAN_send_buffer.data[2],CAN_send_buffer.data[3],CAN_send_buffer.data[4],CAN_send_buffer.data[5],CAN_send_buffer.data[6],CAN_send_buffer.data[7]);
	
}

uint8_t CAN_read(uint8_t adr){
	return MCP2515_read(adr);
}

void CAN_data_receive() {
	uint8_t n = CAN_transmit_complete();
	if (n >= 0){
		CAN_receive_buffer.id = ((CAN_read(RXBnSIDH + n*0x10)<<3)|(0b11100000 & (CAN_read(RXBnSIDL + n*0x10))>>5));
		CAN_receive_buffer.length = (CAN_read(RXBnDLC + n*0x10) & 0b00001111);
		for (uint8_t m = 0; m < CAN_receive_buffer.length; m++){
			CAN_receive_buffer.data[m] = CAN_read(RXBnDM + m);
		}
		
	}
	printf("%i",CAN_receive_buffer.data[0]);
	MCP2515_bit_modify(MCP_CANINTF,0xFF,0); //clear all interrupts //CHANGE THIS SO IT JUST CLEARS THE CORRECT INTERRUPTS
}



ISR(INT2_vect){//interrupt incomming message
	cli();
	//IF there is a message in inbox 0, flag 1<<0, then if there is a message in inbox 1, flag 1<<1 this
	FLAG_new_message = 0b01;
	sei();
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