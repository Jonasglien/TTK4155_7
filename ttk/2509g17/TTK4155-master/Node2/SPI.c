/*
 * SPI.c
 *
 * Created: 23.10.2017 09:48:45
 *  Author: jonasgl
 */ 
#include "SPI.h"



void SPI_initialize(void){

	/* Set MOSI and SCK output, all others input */
	DDRB |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS)|(1<<PB0);
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
	//while(!(SPSR & (1<<SPIF)));

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
//	SPI_send(0b00001111);
//	printf("MCP2515_testSPI");
	for (int i = 0; i< 256; i++){
		SPI_send(i);
		printf("Value of i: %i\t value of reg:\t%i\n",i,SPDR);
	}	
}




	/*
	void SPI_initialize(void)
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
	
	
		/*
		// Set MOSI and SCK output, all others input
		DDRB |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
		
		// Enable SPI, Master, set clock rate fck/16
		SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
		
		// Set MISO as input
		DDRB &= ~(1<<SPI_MISO);
		*/
	
	
	
	/*
void SPI_MasterInit(void){
	// Set MOSI and SCK output, all others input
	DDRB = (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
*/
