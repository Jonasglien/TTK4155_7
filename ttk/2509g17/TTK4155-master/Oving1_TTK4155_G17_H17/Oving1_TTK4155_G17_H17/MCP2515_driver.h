/*
 * MCP2515_driver.h
 *
 * Created: 10.10.2017 18:09:00
 *  Author: pamarton
 */ 


#ifndef MCP2515_DRIVER_H_
#define MCP2515_DRIVER_H_

//------------------------------------------//
//	FINCLUDES								//
//------------------------------------------//

#include "SPI.h"

//------------------------------------------//
//	FUNCTIONS								//
//------------------------------------------//

void MCP2515_write(uint8_t adr, int8_t data);
void MCP2515_read_buffer(uint8_t data);
uint8_t MCP2515_read(uint8_t adr);
void MCP2515_request_to_send(uint8_t buffer);
void MCP2515_initialize(void);
void MCP2515_bit_modify(uint8_t adr, uint8_t mask, int8_t data);
uint8_t MCP2515_read_status(void);
void MCP2515_testSPI(void);
void MCP2515_reset(void);
#endif /* MCP2515_DRIVER_H_ */

