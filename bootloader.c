/*
 * bootloader.c
 *
 *  Created on: Sep 24, 2020
 *      Author: embeddedfab
 */

#include"bootloader.h"


unsigned char LOW_NIBBLE_flag=TRUE;
extern unsigned char array_uart[64];
extern unsigned char decimal_number,number_of_bytes_recieved;



void  boot_program_page (unsigned long page, unsigned char *buf)
{


	unsigned int i;
	unsigned char sreg;

	// Disable interrupts.



	sreg = SREG;
	cli();

	eeprom_busy_wait ();

	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<ATMEA8_PAGE_SIZE; i+=2)
	{
		// Set up little-endian word.

		unsigned int w = *buf++;
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


unsigned char get_data_length_in_the_line(void)
{

	char REC_CHAR=0,decimal_nibble=0;
	unsigned char number_of_data_bytes_in_the_line=0;

	REC_CHAR=UART_getChar();
	decimal_nibble=convert_from_hex_to_decimal(REC_CHAR);

	number_of_data_bytes_in_the_line=decimal_nibble*16;

	REC_CHAR=UART_getChar();
	decimal_nibble=convert_from_hex_to_decimal(REC_CHAR);

	number_of_data_bytes_in_the_line+=decimal_nibble;
	number_of_data_bytes_in_the_line*=2;


	return number_of_data_bytes_in_the_line;

}


unsigned char convert_from_hex_to_decimal(char hex_num)
{
	unsigned char decimal_nibble=0;

	if((hex_num >= '0') && (hex_num <= '9'))
	{
		decimal_nibble=hex_num-48;
	}
	else if((hex_num >= 'A') && (hex_num <= 'F'))
	{
		decimal_nibble =hex_num-55;
	}

	return decimal_nibble;

}




void fill_the_receiving_array(unsigned char decimal_nibble)
{

	if(LOW_NIBBLE_flag==TRUE)
	{
		decimal_number=decimal_nibble*16;
		LOW_NIBBLE_flag=FALSE;
	}
	else if(LOW_NIBBLE_flag==FALSE)
	{
		decimal_number+=decimal_nibble;
		array_uart[number_of_bytes_recieved]=decimal_number;
		number_of_bytes_recieved++;
		LOW_NIBBLE_flag=TRUE;
	}


}


void start_the_main_program(void)
{


	DE_init_UART();
	_delay_ms(100);
	asm("CLR R30");
	asm("CLR R31");
	asm("IJMP");

}


void WDT_off(void)
{

wdt_reset();
WDTCR |= (1<<4) | (1<<3);
WDTCR = 0x00;

}

