/*
 * SPI.c
 *
 * Created: 23.10.2017 09:48:45
 *  Author: jonasgl
 */ 
#include "SPI.h"



void SPI_initialize(void){

	/* Set MOSI and SCK output, all others input */
	DDRB |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS)|(1<<PB0); //PB0 is not connected, but has to be set to output in order for SPI-comm to work
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
		
	SPI_deselect();
	printf("SPI communication initialized.\n");
}


void SPI_send(char data) {
	
	// Start the transmission
	SPDR = data;
	// Wait for transmission complete (checks if the register is empty)
	while(!(SPSR & (1<<SPIF)));

}


char SPI_receive(void){
	
	// Send dummy data to receive from slave
	SPI_send(0);
	
	// Wait for reception complete
	while(!(SPSR & (1<<SPIF)));

	// Return Data Register
	return SPDR;
}

void SPI_select(void){
	PORTB &= ~(1 << SPI_SS);
}

void SPI_deselect(void){
	PORTB |= (1 << SPI_SS);
}

void SPI_test(void) {
	for (int i = 0; i< 256; i++){
		SPI_send(i);
		printf("Value of i: %i\t value of reg:\t%i\n",i,SPDR);
	}	
}