/*
 * main.c
 *
 *  Created on: Apr 22, 2019
 *      Author: Think
 */
/*


the app




 */


#include"bootloader.h"
#include"MCAL/UART.h"
#include<avr/eeprom.h>



unsigned long number_of_bytes_have_to_be_written=0,number_of_bytes_written=0;
unsigned char array_uart[64];
unsigned char decimal_number=0,number_of_bytes_recieved=0;


int main(void) {

	unsigned char*    ptr                       =array_uart;
	unsigned long     boot_loader_st_address    =0X0000;
	unsigned char     decimal_nibble            =0,number_of_data_bytes_in_the_line    =0;
	unsigned char     REC_CHAR                  =0,numbers_of_char_recived             =0;
	unsigned char     boot_loader_flag          =FALSE;

	WDT_off();
	init_UART();
	UART_SEND_string("read bootloader flag \r\n");

	boot_loader_flag=eeprom_read_byte((uint8_t *)BOOT_LOADER_FLAG_ADDRESS);

	if(boot_loader_flag==FALSE)    start_the_main_program();

	UART_SEND_string("start boot loader \r\n");
	while(1)
	{

		REC_CHAR=UART_getChar();

		if(REC_CHAR==DETECT_NEW_LINE)
		{
			numbers_of_char_recived=0;
			number_of_data_bytes_in_the_line=get_data_length_in_the_line();
			number_of_bytes_have_to_be_written+=number_of_data_bytes_in_the_line;
		}

		else if((numbers_of_char_recived<=THE_BEGINING_OF_REAL_DATA)||(numbers_of_char_recived>(THE_BEGINING_OF_REAL_DATA+number_of_data_bytes_in_the_line)))
		{
			if(numbers_of_char_recived==6)
			{
				if(REC_CHAR==DETECT_FILE_END)
				{
					UART_SEND_number((int)number_of_bytes_recieved);
					number_of_bytes_written+=number_of_bytes_recieved;
					boot_program_page( boot_loader_st_address, ptr);
					break;
				}

			}
		}

		else
		{
			decimal_nibble=convert_from_hex_to_decimal(REC_CHAR);
			fill_the_receiving_array(decimal_nibble);

			if(number_of_bytes_recieved>=ATMEA8_PAGE_SIZE)
			{
				UART_SEND_number((int)number_of_bytes_recieved);
				boot_program_page( boot_loader_st_address, ptr);
				number_of_bytes_written+=ATMEA8_PAGE_SIZE;
				number_of_bytes_recieved=0;
				boot_loader_st_address +=ATMEA8_PAGE_SIZE;
			}
		}

		numbers_of_char_recived++;


	}

	eeprom_write_byte((uint8_t *)BOOT_LOADER_FLAG_ADDRESS,FALSE);

	UART_SEND_string("\r\n number_of_bytes_have_to_be_written : ");
	UART_SEND_long_number((int) number_of_bytes_have_to_be_written/2);
	UART_SEND_string("\r\n number_of_bytes_written            : ");
	UART_SEND_long_number((int) number_of_bytes_written);

	UART_SEND_string("\r\njump\r\n");

	start_the_main_program();


}



