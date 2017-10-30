/*
 * Oving1_TTK4155_G17_H17.c
 *
 * Created: 04.09.2017 11:50:29
 *  Author: pamarton
 */ 

#include "common.h" //common constants
#include <stdio.h>
#include "adc.h"
#include <avr/io.h>

//#include <avr/interrupt.h>
#include "uart.h"
#include "i_o.h"
#include "input_conversion.h"
#include "button_interrupts.h"


void SRAM_test(void);
void initalize(void);
void bootscreen(void);
#include "oled.h"
#include "menu.h"
#include "can.h"

int main(void)
{
	initalize();
	oled_goto_line(7);
	sram_write_string(" - BYGGERN - G17");
	
		//uint8_t temp[8] = {1,8,3,4,5,3,7,6};
		//uint8_t *data = temp;
		
	int8_t temp[8] = {0,1,2,3,4,5,6,7};//{'H','A','L','L','O','!',' ','§'};
	int8_t *data = temp;

	uint8_t sendCAN = 1;
		
	while(1){
		if(sendCAN){
			//data[7]++;
			
			//CAN_data_receive();
			
			data[0] = read_control_input('X');
			data[1] = read_control_input('Y');
			data[2] = read_control_input('L');
			data[3] = read_control_input('R');
			
			
			CAN_message_send(data);
			
			_delay_ms(1000);
			
			
			
		
		}
		//printf("X: %i ; Y: %i\n", read_control_input('X'), read_control_input('Y'));
		sram_write_int(data[2] = read_control_input('L'));
		sram_push();
		
		//menu_update();
		//unsigned int temp = recieve_UART();
		//send_UART(temp);
		
		//printf("Node 1 sending message...\n");
		//CAN_message_send(data);
		//_delay_ms(1000);
		//printf("Node 1 waiting for message...\n");
		//printf("Status: %x\n",MCP2515_read_status());
		//printf("Error?:%i\n",MCP2515_read(MCP_EFLG));	
		//CAN_data_receive();
		//_delay_ms(1000);
		
	}
	
	
	
}






void initalize(void){
	cli();//disable interrupts
	init_UART(UBBR);
	printf("LOADING g17_%s %s %s\nINITIALIZING...\n\nUART successfully initialized\n\n", VERSION,__DATE__,__TIME__);
	
	BIT_ON(MCUCR,SRE); //SET THIS IN SOME INITALIZE FUNBCTION
	BIT_ON(SFIOR,XMM2);//HVORFOR GJORDE DE DETTE I OLED?

	oled_ini();
	sram_init();
	bootscreen();
	sram_push();
	
	initialize_menu();
	
	initialize_control_input();
	
	initalize_interrupts();
	
	initialize_timer(FPS);//60 FPS
	
	CAN_initialize();
	
	sei();//enable interrupts
}

#include <stdlib.h>
void SRAM_test(void)//CAN BE REMOVED, IN CASE OF LOW STORAGE
{
	volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
	uint16_t ext_ram_size = 0x800;
	uint16_t write_errors = 0;
	uint16_t retrieval_errors = 0;
	printf("Starting SRAM test...\n");
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i,	retreived_value, some_value);
			write_errors++;
		}
	}
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed);
	// reset the PRNG to the state it had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n",i, retreived_value, some_value);
			retrieval_errors++;
		}
	}
	printf("SRAM test completed with\n%4d errors in write phase and\n%4d errors	in retrieval phase\n\n", write_errors, retrieval_errors);
}



void bootscreen(void){
	int x = 24;
	int y = 61;
	sram_draw_line(x,y,x-15,y-60);//letter G
	sram_draw_line(x-15,y-60,x+15,y-60);
	sram_draw_line(x+15,y-60,x+12,y-48);
	sram_draw_line(x+12,y-48,x-6,y-48);
	sram_draw_line(x-6,y-48,x,y-24);
	sram_draw_line(x,y-24,x+3,y-36);
	sram_draw_line(x+3,y-36,x+9,y-36);
	sram_draw_line(x+9,y-36,x,y);//end of letter
	
	
	
	x += 40;
	sram_draw_rectangle(x-5,y,x+5,y-60);

	x += 40;
	sram_draw_line(x,y,x+15,y-60);//number 7
	sram_draw_line(x-15,y-60,x+15,y-60);
	sram_draw_line(x-15,y-60,x-12,y-48);
	sram_draw_line(x-12,y-48,x+6,y-48);
	sram_draw_line(x+6,y-48,x+3,y-36);
	sram_draw_line(x+3,y-36,x-9,y-36);
	sram_draw_line(x-9,y-36,x-6,y-24);
	sram_draw_line(x-6,y-24,x,y-24);
	sram_draw_line(x,y-24,x-3,y-12);
	sram_draw_line(x-3,y-12,x,y);//end of number
	sram_push();
	_delay_ms(1000);
}