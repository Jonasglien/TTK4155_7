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


int main(void)
{
	init_UART(UBRR);
			
	printf("Initializing...\n");
	
	
	CAN_initialize();
	

	//CAN_receive_buffer =  CAN_message_receive();
	
	
	uint8_t temp[8] = {1,10,3,4,5,6,7,8};
	uint8_t *data = temp;
	//CAN_message_send(data);
	
	//SPI_test();

	sei();

	uint8_t sendCAN = 1;
	

    while(1){
		if(sendCAN){
			printf("Sending message!\n");
			CAN_message_send(data);
			_delay_ms(100);
			CAN_data_receive();
			sendCAN = 1;
			_delay_ms(200);
			
		}
		
		
	}
	cli();

}