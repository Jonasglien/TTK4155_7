/*
 * MCP2515_driver.c
 *
 * Created: 23.10.2017 10:40:15
 *  Author: jonasgl
 */ 

#include "MCP2515_driver.h"
#include "SPI.h"

void MCP2515_initialize(void){
	SPI_initialize();
	MCP2515_reset();
}

void MCP2515_write(uint8_t adr, int data){
	SPI_select();
	SPI_send(MCP_WRITE);
	SPI_send(adr);
	SPI_send(data);
	SPI_deselect();
}

void MCP2515_read_buffer(int data){
	SPI_select();
	SPI_send(MCP_READ);
	SPI_send(0x00);
	SPI_deselect();
}

int MCP2515_read(uint8_t adr){
	SPI_select();
	SPI_send(MCP_READ);
	SPI_send(adr);
	int received_data = (int) SPI_receive();

	SPI_deselect();

	return received_data;
}

void MCP2515_testSPI(void){
	printf("MCP2515_testSPI");
	for (int i = 0; i< 256; i++){
		MCP2515_write(0b00000000,i);
		printf("Value of i: %i\t value of reg:\t%i\n",i,MCP2515_read(0b00000000));
	}
}

void MCP2515_request_to_send(uint8_t buffer){
	SPI_select();
	SPI_send(0x80|(1<<buffer));
	SPI_deselect();
}

uint8_t MCP2515_read_status(void){
	SPI_select();
	SPI_send(MCP_READ_STATUS);
	uint8_t recieved_status = SPI_receive();
	SPI_deselect();
	return recieved_status;
}

void MCP2515_bit_modify(uint8_t adr, uint8_t mask, int data){
	SPI_select();
	SPI_send(MCP_BITMOD);
	SPI_send(adr);
	SPI_send(mask);
	SPI_send(data);
	SPI_deselect();
}


void MCP2515_reset(void){
	printf("Resetting MCP2515\n");
	SPI_select();
	SPI_send(MCP_RESET);
	SPI_deselect();
}

