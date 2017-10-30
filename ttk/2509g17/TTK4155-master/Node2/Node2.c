/*
 * Node2.c
 *
 * Created: 22.10.2017 12:06:39
 *  Author: jonasgl
 */ 


#include <avr/io.h>

#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "can.h"

void init(void);

int main(void)
{
	cli();
	init();
	sei();
	
	int temp[8] = {1,2,3,4,5,6,7,8};
	int *data = temp;

	uint8_t sendCAN = 0;

    while(1){
		if(sendCAN){
			data[7]++;
			CAN_message_send(data);
			//CAN_data_receive();
			sendCAN = 1;
		}
		
		
		if(~sendCAN){
			//printf("Waiting for message ...\n");
			//printf("Status: %x\n",MCP2515_read_status());
			//printf("Error?:%i\n",MCP2515_read(MCP_EFLG));
			CAN_data_receive();
		}
	}
	
	return 1;
}


void init(void){
	printf("Initializing...\n");
	init_UART(UBRR);		
	CAN_initialize();
}