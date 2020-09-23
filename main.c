/*
 * main.c
 *
 *  Created on: Apr 22, 2019
 *      Author: Think
 */
/*


the app




 */


#include <avr/io.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include"MCAL/UART.h"

#define TRUE  1
#define FALSE 0
#define ATMEA8_PAGE_SIZE  64

void boot_program_page (uint32_t page, uint8_t *buf);
unsigned char END_BOOT_LOADER_PROG_flag=FALSE;
unsigned char LOW_NIBBLE_flag=TRUE;

uint8_t array_uart[64];
int main(void) {

	init_UART();
	uint8_t* ptr=array_uart;
	uint16_t boot_loader_st_address=0X0000;
	uint8_t z=0,hex_number=0,number_of_write_iteration=0;
	uint8_t number_of_bytes_recieved=0;
	uint8_t number_of_data_in_the_line=0;
	char REC_CHAR=0;
	signed char number_of_char_recived=0;

	UART_SEND_string("start \r\n");

	while(1)
	{

		z=UART_getChar();

		if(z==':')
		{
			number_of_char_recived=0;
			REC_CHAR=UART_getChar();

			if((REC_CHAR >= '0') && (REC_CHAR <= '9'))
			{
				REC_CHAR -=48;
			}
			else if((REC_CHAR >= 'A') && (REC_CHAR <= 'F'))
			{
				REC_CHAR -=55;
			}

			number_of_data_in_the_line=REC_CHAR*16;

			REC_CHAR=UART_getChar();

			if((REC_CHAR >= '0') && (REC_CHAR <= '9'))
			{
				REC_CHAR -=48;
			}
			else if((REC_CHAR >= 'A') && (REC_CHAR <= 'F'))
			{
				REC_CHAR -=55;
			}

			number_of_data_in_the_line+=REC_CHAR;

			number_of_data_in_the_line*=2;

		}

		else if((number_of_char_recived>(6+number_of_data_in_the_line))||(number_of_char_recived<7))
		{

			if(number_of_char_recived==6)
			{
				if(z=='1')		END_BOOT_LOADER_PROG_flag=TRUE;
			}
		}



		else
		{
			//			UART_SendChar(z);
			if((z >= '0') && (z <= '9'))
			{
				z -=48;
			}
			else if((z >= 'A') && (z <= 'F'))
			{
				z -=55;
			}

			if(LOW_NIBBLE_flag==TRUE)
			{
				hex_number=z*16;
				LOW_NIBBLE_flag=FALSE;
			}
			else if(LOW_NIBBLE_flag==FALSE)
			{
				hex_number+=z;
				array_uart[number_of_bytes_recieved]=hex_number;
				number_of_bytes_recieved++;
				LOW_NIBBLE_flag=TRUE;
			}

			if(number_of_bytes_recieved>=ATMEA8_PAGE_SIZE)
			{
				UART_SEND_number((int)number_of_bytes_recieved);
				boot_program_page( boot_loader_st_address, ptr);
				number_of_bytes_recieved=0;
				boot_loader_st_address +=ATMEA8_PAGE_SIZE;

			}



		}

		number_of_char_recived++;
		if(	END_BOOT_LOADER_PROG_flag==TRUE)
		{
			UART_SEND_number((int)number_of_bytes_recieved);
			boot_program_page( boot_loader_st_address, ptr);
			break;
		}
	}



	UART_SEND_string("jump \r\n");
	_delay_ms(200);

	DE_init_UART();

	asm("CLR R30");
	asm("CLR R31");
	asm("IJMP");

}




void boot_program_page (uint32_t page, uint8_t *buf)
{


	uint16_t i;
	uint8_t sreg;

	// Disable interrupts.



	sreg = SREG;
	cli();

	eeprom_busy_wait ();

	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<ATMEA8_PAGE_SIZE; i+=2)
	{
		// Set up little-endian word.

		uint16_t w = *buf++;
		w += (*buf++) << 8;

		boot_page_fill (page + i, w);
	}

	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.

	boot_rww_enable ();

	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;


}



