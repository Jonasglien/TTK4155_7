/*
 * uart.h
 *
 * Created: 22.10.2017 12:09:24
 *  Author: jonasgl
 */

#ifndef UART_H_
#define UART_H_

//------------------------------------------//
//	INCLUDES								//
//------------------------------------------//

//#include "new_printf.h"
#include <avr/io.h>
#include <stdio.h>

//------------------------------------------//
//	DEFINITIONS								//
//------------------------------------------//

#define FOSC 16000000// Clock Speed
#define BAUDRATE 9600 // Valgt verdi data sendt pr sekund
//#define UBBR 32 - 1 //F_CPU/(16*BAUDRATE)-1 //USART BAUD RATE REGISTER VALUE
#define UBRR 103    //F_CPU (called FOSC here)/(16*BAUDRATE)-1 

//------------------------------------------//
//	FUNCTIONS								//
//------------------------------------------//

void send_UART(char letter);
void init_UART(unsigned int ubrr);
unsigned int recieve_UART(void);
void testUart(void);

#endif /* UART_H_ */