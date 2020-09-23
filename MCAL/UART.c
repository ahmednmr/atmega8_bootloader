/*
 * UART.c
 *
 *  Created on: Oct 11, 2019
 *      Author: Nmr
 */
#include <avr/io.h>
#include "UART.h"



//  0x019f   1200 baudrate
//  0x0681   2400 baud rate


void init_UART(void)
{
		UCSRB = (1 << RXEN) | (1 << TXEN);
       UCSRC = (1 << URSEL) | (3 << UCSZ0);
		UBRRL =0x9F;
		UBRRH =0x01;
}

void DE_init_UART(void)
{
		UCSRB =0;
       UCSRC =0;
		UBRRL =0;
		UBRRH =0;
}


void UART_SendChar(char data)
{
	while (!( UCSRA & (1<<UDRE))) ;  /* Wait for empty transmit buffer */

	UDR = data;                     /* Put data into buffer, sends the data */
}


char UART_getChar(void)
{
	while (!(UCSRA & (1<<RXC)));   /* Wait until reveived flag set */

	return UDR;                   /* Return data in received buffer */
}


void UART_SEND_string(char *array)
{
	int i=0;
    while(array[i]!='\0')
    {

    	UART_SendChar(array[i]);
    	i++;
    }

}




void UART_SEND_number_binary(int number)
{
    char c=0,k=0;

    for (c = 16; c > 0; c--)
     {
       k = number >> (c-1);

       if (k & 1)
    	   UART_SendChar('1');
       else
    	   UART_SendChar('0');

     }


}




void UART_SEND_number(int number)
{
   char buffer[20];

   itoa(number,buffer,10);
   UART_SEND_string(buffer);


}


