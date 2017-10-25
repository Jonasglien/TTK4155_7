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
	
	
	MCP2515_bit_modify(MCP_RXB0CTRL,0b01100100,0xFF);
	MCP2515_bit_modify(MCP_CANINTE,0b00000001,1);
	//MCP2515_bit_modify(MCP_CANINTF,0b00000001,1);
	
	//set CAN mode
	MCP2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_LOOPBACK);   
	CAN_construct_message(CAN_ID,CAN_MESSAGE_LENGTH);
	
	printf("read CANSTAT\n");
	volatile uint8_t value = MCP2515_read(MCP_CANSTAT);
	printf("MCP_CANSTAT: %i\n", value);
}

CAN_message_t* CAN_message_receive(void){
	return &CAN_receive_buffer;
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
	//printf("%i\n",CAN_send_buffer.data[0]);
	
}

void CAN_send_byte(CAN_message_t* message,uint8_t n){
	MCP2515_write(MCP_TXBnSIDH+n*0x10,message->id>>3);		//8 most significant bits of the message ID
	MCP2515_write(MCP_TXBnSIDL+n*0x10,message->id<<5);		//3 least significant bits of the message ID
	MCP2515_write(MCP_TXBnDLC+n*0x10,(0b00001111)&(message->length));		//Message length

	for (uint8_t i = 0; i < message->length;i++){
		MCP2515_write(MCP_TXBnD0+n*0x10+i,message->data[i]);	//Message data
	}

	MCP2515_request_to_send(MCP_MERRF+(1<<n));
	printf("CAN_send_byte: ID %i Length: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_send_buffer.id,CAN_send_buffer.length,CAN_send_buffer.data[0],CAN_send_buffer.data[1],CAN_send_buffer.data[2],CAN_send_buffer.data[3],CAN_send_buffer.data[4],CAN_send_buffer.data[5],CAN_send_buffer.data[6],CAN_send_buffer.data[7]);

}

uint8_t CAN_read(uint8_t adr){
	return MCP2515_read(adr);
}

void CAN_data_receive() {
	printf("CAN receive\n");
	uint8_t n = CAN_transmit_complete();
	printf("CAN_transmit_complete: \%i\n", n);
	if (n >= 0){
		CAN_receive_buffer.id = ((CAN_read(RXBnSIDH + n*0x10)<<3)|(0b11100000 & (CAN_read(RXBnSIDL + n*0x10))>>5));
		CAN_receive_buffer.length = (CAN_read(RXBnDLC + n*0x10) & 0b00001111);
		for (uint8_t m = 0; m < CAN_receive_buffer.length; m++){
			CAN_receive_buffer.data[m] = CAN_read(RXBnDM + m);
		}
	}
	//printf("%i",CAN_receive_buffer.data[0]);
	printf("CAN_data_receive: ID %i L: %i DATA: %i %i %i %i %i %i %i %i\n",CAN_receive_buffer.id,CAN_receive_buffer.length,CAN_receive_buffer.data[0],CAN_receive_buffer.data[1],CAN_receive_buffer.data[2],CAN_receive_buffer.data[3],CAN_receive_buffer.data[4],CAN_receive_buffer.data[5],CAN_receive_buffer.data[6],CAN_receive_buffer.data[7]);
	MCP2515_bit_modify(MCP_CANINTF,0xFF,0); //clear all interrupts //CHANGE THIS SO IT JUST CLEARS THE CORRECT INTERRUPTS
}



ISR(INT2_vect){//interrupt incomming message
	cli();
	//IF there is a message in inbox 0, flag 1<<0, then if there is a message in inbox 1, flag 1<<1 this
	FLAG_new_message = 0b01;
	sei();
}


uint8_t CAN_transmit_complete(void){
	
	
	//

	
	
	volatile char interrupt = 1; //MCP2515_read(MCP_CANINTF);
	
	
	volatile char realInterrupt = MCP2515_read(MCP_CANINTF);
	printf("\n interrupt: \%i\n", realInterrupt);

		while((!((1 << MCP_RX0IF) ))&&(!((1 << MCP_RX1IF)))){
			interrupt = MCP2515_read(MCP_CANINTF);
		}
		printf("\n interrupt: \%i", interrupt);
		
		if((1 << MCP_RX0IF)){
			return 0;
		}
		else if((1 << MCP_RX1IF)){
			return 1;
		}
		else{
			return -1;
		}
	
		
	/*
	while((!((1 << MCP_RX0IF) & interrupt))&&(!((1 << MCP_RX1IF)&interrupt))){
		interrupt = MCP2515_read(MCP_CANINTF);
	}
	printf("interrupt: \%i", interrupt);
	
	if((1 << MCP_RX0IF)&interrupt){
		return 0;
	}
	else if((1 << MCP_RX1IF)&interrupt){
		return 1;
	}
	else{
		return -1;
	}
	*/

}