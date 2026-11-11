/*****************************************************************************
 *	    File: Lab08.c
 *  Lab Name: RS232 Communication Server
 *    Author: Dr. Greg Nordstrom
 *   Created: 12/30/17
 *  Mod Date: 3/28/19, 1/17/21
 *  Modified:
 *   ModDate:
 * 
 * Processor: ATmega128A (on the ReadyAVR board)
 *
 * This program implements a communication server. Normally the server
 * simply echoes what was sent. However, there are two cases where
 * different behavior occurs:
 *    1. Pressing the ESC causes a soft reset (redisplays the title text)
 *    2. Typing the letters 'end' contiguously displays 'Bye' and halts
 *
 *****************************************************************************/

// define CPU speed (7.3728 MHz for the ATmega128A)
#define F_CPU   7372800UL

#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

#include <avr/io.h>
#include <util/delay.h>
#include "UARTLibrary.h"  // Double quotes for local library.

#define BAUDRATE 1200 // standard values from 1200 to 115200
#define DATABITS    8 // 5-8
#define STOPBITS    1 // 1,2
#define PARITY      0 // 0=none, 1=odd, 2=even

/*
 * Corrected function declaration for the custom UART string helper.
 * This avoids compiler warnings/errors from using a function before it is declared.
 */
void UART_Write_String(char *str);

int main(void)
{

    /* See the UARTLibrary.h for available calls.
	 *     	  // initialize the UART
	 *     	  uart_init(BAUDRATE, DATABITS, STOPBITS, PARITY);
	 *
	 *        // transmit a value (0x41) to the comm port
	 *        c = 0x41;
	 *        uart_tx(c);
	 *
	 *        // receive a byte from the comm port
	 *        c = uart_rx();
	 */

	uart_init(BAUDRATE, DATABITS, STOPBITS, PARITY);
	
	DDRA |= (1<< DDA0);
	PORTA |= (1 << PORTA0);
	
	uart_tx(CR);
	uart_tx(LF);
	uart_tx(CR);
	uart_tx(LF);
	
	UART_Write_String("RS232 Communications Server\r\n");
	UART_Write_String("Status: Active\r\n");
	UART_Write_String("Andrew Firestone\r\n");
	UART_Write_String("EECE3624\r\n");
	UART_Write_String("Fall 2026\r\n");
	
	
	uart_tx(CR);
	uart_tx(LF);
	uart_tx(CR);
	uart_tx(LF);
	
	unsigned char c;
	int count = 0;
	
	while(1)
	{
		c = uart_rx();
		
		/*
		 * ESC reset: reset the 'end' detection state and redisplay the title text.
		 * This makes the server restart cleanly instead of carrying an old match state.
		 */
		/*
		 * ESC pressed: restart the terminal display and reset the 'end' sequence tracker.
		 * This prevents stale input from triggering the shutdown sequence after a reset.
		 */
		if (c == ESC){
			UART_Write_String("\r\nRS232 Communications Server\r\n");
			UART_Write_String("Status: Active\r\n");
			UART_Write_String("Andrew Firestone\r\n");
			UART_Write_String("EECE3624\r\n");
			UART_Write_String("Fall 2026\r\n");
			count = 0;  // clear the 'end' detection state
			continue;   // restart the loop without echoing the ESC key
		}
		
		/*
		 * Carriage return handling: send both CR and LF so the terminal lines up properly.
		 * Any other character is echoed back to the user exactly as received.
		 */
		if (c == CR){
			uart_tx(CR);
			uart_tx(LF);
		}
		else{
			uart_tx(c);
		}
		
		/*
		 * Corrected sequence check for the 'end' command.
		 * The original code used bitwise '&' instead of logical '&&'.
		 * Using '&' caused incorrect comparisons and prevented proper detection.
		 */
		if (c == 'e'){
			count = 1;
		}
		else if (count == 1 && c == 'n'){
			count++;
		}
		else if (count == 2 && c == 'd'){
			count++;
		}
		else{
			count = 0;
		}
		
		if (c != 0){
			PORTA &= ~(1 << PORTA0);
			_delay_ms(75);
			PORTA |= (1 << PORTA0);
		}
		
		if (count == 3){
			UART_Write_String("\r\n\r\nBye\r\n");
			while (1){
				;
			}
		}
	}
	return 0;
}

void UART_Write_String(char *str) {
    while (*str != '\0') {      // Loop until the end of the string
        uart_tx(*str);  // Send the current character
        str++;                  // Move to the next character pointer
    }
}