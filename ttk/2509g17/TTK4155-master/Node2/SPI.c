/*
 * SPI.c
 *
 * Created: 23.10.2017 09:48:45
 *  Author: jonasgl
 */ 
#include "SPI.h"

#define		SPI_SS			PB0
#define		SPI_MOSI		PB2
#define		SPI_MISO		PB3
#define		SPI_SCK			PB1

void SPI_initialize(void){
	/*
	DDRB |=	(1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);
	DDRB &= ~(1 << SPI_MISO);
	//BIT_ON(SPCR,SPIE);		//SPI INTERRUPT ENABLE
	//BIT_OFF(SPCR,DORD);		//transmit the most significant bit first, change to ON if we transmit the Least significant first
	BIT_ON(SPCR,MSTR);		//MASTER/SLAVE SELECT, set to 1 for master, 0 for slave (master wanted)
	//BIT_ON(SPCR,CPOL);		//CLOCK POLARITY, NO IDEA WHAT WE NEED!!!!!!!!!!!!!
	//BIT_ON(SPCR,CPHA);		//CLOCK PHASE, NO IDEA!!!!!!!!!!!
	//BIT_ON(SPCR,SPR1);		//SPI CLOCK RATE SELECT, BIT 1
	//BIT_ON(SPCR,SPR0);		//SPI CLOCK RATE SELECT, BIT 0
	BIT_ON(SPCR,SPE);		//SPI ENABLE, overrides the normal pins function (we want this)
	SPSR |= (1 << SPI2X); //ADD COMMENT------------------------------------------------------------------------------------------------------------
	//sei();
	*/
	
		// Set MOSI and SCK output, all others input
		DDRB = (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
		// Enable SPI, Master, set clock rate fck/16
		SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
		
		
	SPI_deselect();
	printf("SPI communication initialized.\n");
}

void SPI_test(void) {
	SPI_send(0b00001111);
}

void SPI_send(char data) {
	//Start the transmission
	SPDR = data;
	
	//Wait for data to be transmitted (checks if the register is empty)
	while(!(SPSR & (1<<SPIF)));
}





/*
void SPI_MasterInit(void){
	// Set MOSI and SCK output, all others input
	DDRB = (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


void SPI_MasterTransmit(char cData){
	// Start transmission
	SPDR = cData;
	// Wait for transmission complete
	while (!(SPSR & (1<<SPIF)));
}
*/

char SPI_read(void){
	//Send dummy data to read from slave
	SPI_send(0);
	
	//Wait for data to be received
	while(!(SPSR & (1<<SPIF)));

	return SPDR;
}

void SPI_select(void){
	PORTB &= ~(1 << SPI_SS);
}

void SPI_deselect(void){
	PORTB |= (1 << SPI_SS);
}



