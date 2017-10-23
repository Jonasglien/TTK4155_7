/*
 * uart.c
 *
 * Created: 22.10.2017 12:09:13
 *  Author: jonasgl
 */ 

#include "uart.h"



void send_UART(char letter){
	//send UART melding
	while( !(UCSR0A & (1<<UDRE0)) );				/* UCSR0A = USART 0 Control and Status Register A, UDRE0 = UCSR0A bit definitions (5), wait for data to be transmitted (checks if the register is empty) */
	UDR0 = letter;									/* UDR0 = USART 0 Data Register */
}



void init_UART(unsigned int ubrr){
	
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	

 	
	fdevopen((int(*)(char, struct __file*))&send_UART, (int(*)(struct __file*))&recieve_UART);			//overrides printf("string");
}



unsigned int recieve_UART(void){
		
	while (!(UCSR0A & (1 << RXC0)));				/* UCSR0A = USART 0 Control and Status Register A, RXC0 = UCSR0A bit definition (7) , waiting for data to be transmitted (checks if the register is empty) */ 

	return UDR0;									/* UDR0 = USART 0 Data Register */
}





void testUart(void){
	printf("turn off? Y/N\n");
	
	unsigned char temp = ' ';
	while(1){
		temp = recieve_UART();
		if (temp != 0){
			send_UART(temp);
		}
		if (temp == 'Y'){
			break;
		}
	};
}