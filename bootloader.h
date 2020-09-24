/*
 * bootloader.h
 *
 *  Created on: Sep 24, 2020
 *      Author: embeddedfab
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include <avr/io.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include"MCAL/UART.h"


#define TRUE  1
#define FALSE 0
#define ATMEA8_PAGE_SIZE  64
#define DETECT_NEW_LINE    ':'
#define THE_BEGINING_OF_REAL_DATA  6
#define DETECT_FILE_END      '1'
#define BOOT_LOADER_FLAG_ADDRESS      245


void boot_program_page (unsigned long page, unsigned char *buf);
unsigned char get_data_length_in_the_line(void);
unsigned char convert_from_hex_to_decimal(char hex_num);
void fill_the_receiving_array(unsigned char decimal_nibble);
void start_the_main_program(void);
void WDT_off(void);


#endif /* BOOTLOADER_H_ */
