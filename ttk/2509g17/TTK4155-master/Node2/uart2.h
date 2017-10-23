/**
* File              : uart.h
 
* authors           : Ali Al-Jumaili, Amund Høivik, Erlend Hårstand
* date              : 20/11/2016
 
 
* Description       : Header file for uart.c
*                     Include this file in the application.
*/
 
 
#ifndef UART2_H_
#define UART2_H_
 
//Function declarations
void uart_init( unsigned int ubrr );
unsigned int uart_transmit(unsigned char data);
unsigned int uart_receive(void);
 
#endif /* UART2_H_ */