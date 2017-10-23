/*
 * SPI.h
 *
 * Created: 23.10.2017 09:49:09
 *  Author: jonasgl
 */ 

#ifndef SPI_H_
#define SPI_H_

//------------------------------------------//
//	INCLUDES								//
//------------------------------------------//

#include <avr/io.h>
#include "uart.h" //REMOVE AFTER USE
#include "i_o.h"
#include "MCP2515.h"

//------------------------------------------//
//	FUNCTIONS								//
//------------------------------------------//

void SPI_initialize(void);
void SPI_send(char data);
char SPI_read(void);
void SPI_select(void);
void SPI_deselect(void);

#endif /* SPI_H_ */