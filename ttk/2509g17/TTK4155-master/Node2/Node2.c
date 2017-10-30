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
	cli();
	init();
	sei();
	
	uint8_t temp[8] = {1,2,3,4,5,6,7,8};
	uint8_t *data = temp;

	uint8_t sendCAN = 0;
	
	uint8_t receiveCAN = 1;
	
	/*
	CAN_message_send(data);
	_delay_ms(100);
	CAN_data_receive();
	*/
	
    while(1){
		if(sendCAN){
			data[7]++;
			CAN_message_send(data);
			CAN_data_receive();
			sendCAN = 1;
		}
		
		_delay_ms(5000);
		printf("Waiting for message ...\n");
		if(receiveCAN){
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