#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

extern void ramRead(unsigned long addr, unsigned char *b);
extern void ramWrite(unsigned long addr, unsigned char val);
extern void __vector_13();	//we call it directly :)

void ramInit() {
	cli();

		asm("cli");
	
	//timer
	TCCR1A = 0x00;
	TCCR1B = 0x0B;		//reset on match with OCR1A, clk= clk/64 -> overflow every 13.1 ms (faster than 4x as fast as we need to refresh)
	OCR1A = 0x493E;		//match every 60ms
	TIMSK1 = 2;		//interrupt when we match
	
	//RAM PORT

	DDRB |= 0x3F;	//out: A0..A7
	PORTB |=  0xC0;	//lo: A0..A7    - portb 0x25  -  
	DDRC |= 0x04;	//out: nRAS
	DDRC |= 0x02;	//out: nWE
	DDRC |= 0x01;	//out: nCAS
	DDRC |= 0x08;	//out: led
		
	PORTC |= 0x04;	//hi: nRAS    - 0x28 - pinc 0x06
	PORTC |= 0x01;	//hi: nCAS
	PORTC |= 0x02;	//hi: nWE

	DDRD &= 0x00;	//in:  D0..D7
	PORTD = 0x00;	//no pullups: data  portd 0x2b - pind 0x09

	unsigned char t;
	

	_delay_us(200);	//as per init instructions
	for(t = 0; t < 8; t++) __vector_13();
	
	sei();	//enable refresh

}