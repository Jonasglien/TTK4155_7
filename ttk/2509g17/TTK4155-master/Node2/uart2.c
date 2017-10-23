/*
* UART.c
*
* Created: 07.09.2016 09:06:26
*  Author: amundho
*/



#include <avr/io.h> //for registers
#include <stdio.h>
#include "uart2.h"


static FILE uart_stdio = FDEV_SETUP_STREAM(&uart_transmit, &uart_receive, _FDEV_SETUP_RW);

/**
* This method will initialize the UART and connect the transmit and receive functions to printf
* @authors Ali Al-Jumaili, Amund Høivik, Erlend Hårstand
* @param channel the desired adc channel
* @return void
* @date 7/9/2016
*/
void uart_init( unsigned int ubrr )
{
	//Set baud rate
	UBRR0H = (unsigned char)    (ubrr>>8);
	UBRR0L =    (unsigned char) ubrr ;
	
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);                   //Enable(set) the receiver and transmitter
	UCSR0C &= ~(1 << UMSEL00);                        // Choose UART not USART (clears the bit)
	UCSR0C &= ~(1 << UMSEL01);                        // Choose UART not USART (clears the bit)
	UCSR0C = (1<<USBS0)| (1<<UCSZ00) |(1<<UCSZ01); //Set frame format: 8data, 2stop bit
	
	stdout =&uart_stdio;                            //connect printf to uart_Transmit
	stdin  =&uart_stdio;                            //connect printf to uart_recieve
}

/**
* This method will return 0 if the transmission is completed
* @authors Ali Al-Jumaili, Amund Høivik, Erlend Hårstand
* @param data is the data to be transmitted
* @return 0 when transmission is completed
* @date 7/9/2016
*/
unsigned int uart_transmit(unsigned char data)
{
	while( !(UCSR0A & (1<<UDRE0)) ); //loop_until_bit_is_set(UCSR0A,UDRE0);//wait by checking the flag until transmitter buffer is empty
	UDR0 = data;//puts data into buffer and sends it
	return 0; //transmit ok
}

/**
* This method will return the received data from UART
* @authors Ali Al-Jumaili, Amund Høivik, Erlend Hårstand
* @param void
* @return URD0 register containing the data recieved
* @date 7/9/2016
*/
unsigned int uart_receive(void)
{
	//loop_until_bit_is_set(UCSR0A,UDRE0);        //wait by checking the flag until transmitter buffer is empty
	while (!(UCSR0A & (1 << UDRE0)));
	return UDR0;
}