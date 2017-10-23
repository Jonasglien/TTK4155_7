/*
 * uart.c
 *
 * Created: 11.09.2017 08:43:21
 *  Author: jonasgl
 */ 
#include "uart.h"

void send_UART(char letter){
	//send UART melding
	while( !(UCSR0A & (1<<UDRE0)) );				/* UCSR0A = USART 0 Control and Status Register A, UDRE0 = UCSR0A bit definitions (5) */
	UDR0 = letter;									/* UDR0 = USART 0 Data Register */
}

void init_UART(unsigned int ubrr){
	//set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);				/* UBBR0H = USART 0 Baud-Rate Register High Byte, Shared with UCSR0C */
	UBRR0L = (unsigned char)(ubrr);					/* UBBR0L = USART 0 Baud-Rate Register Low Byte */
	//enable receiver and transmitter				
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);					/* UCSR0B = USART 0 Control and Status Register B, RXEN0 & TXEN0 = UCSR0B bit definitions */
	fdevopen((int(*)(char, struct __file*))&send_UART, (int(*)(struct __file*))&recieve_UART); //overrides printf("string");
}

unsigned int recieve_UART(void){
	while (!(UCSR0A & (1 << RXC0)));				/* UCSR0A = USART 0 Control and Status Register A, RXC0 = UCSR0A bit definition (7) */
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