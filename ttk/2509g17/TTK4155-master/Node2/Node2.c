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
#include "SPI.h"
#include "MCP2515.h"
#include "MCP2515_driver.h"

int main(void)
{
	init_UART(UBRR);
			
	printf("Initializing...\n");
	
	CAN_initialize();
	

	//CAN_receive_buffer =  CAN_message_receive();
	
	
	uint8_t temp[8] = {1,2,3,4,5,6,7,8};
	uint8_t *data = &temp;
	CAN_message_send(data);
	
	

	

	
	//CAN_message_t *rcv_message = CAN_message_receive();

	//printf("%/i", rcv_message->data);
    	while(1){
			SPI_test();
				//CAN_data_receive();
				//CAN_message_send(data);
		//printf("hei ");
	    //_delay_ms(10);
		//printf("p� deg\n\n");
		//_delay_ms(500);
			//unsigned int temp = recieve_UART();
			//send_UART(temp);
    	}
}