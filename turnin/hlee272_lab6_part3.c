/*	Author:Heeje Lee
 *	Lab: 6
 *  Partner(s) Name: 
 *	Lab Section: 
 *	Assignment: Lab #6  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *    https://youtu.be/W33clYRARjY
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif

enum States{Start, init, wait, inc, dec, zero}state;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
	}

unsigned char count;
void Tick() {
 
	switch(state) { //transititons

	case Start:
	state = init;
	break;

	case init:
	if ((~PINA & 0x01) == 0x01) {
		state = inc;
	}

	else if ((~PINA & 0x02) == 0x02) {
		state = dec;
	}

	else if ((~PINA & 0x03) == 0x03) {
		state = zero;
	}

	else {
		state = init;
	}
	break;

	case wait:
	if (((~PINA & 0x03) == 0x01) || ((~PINA & 0x03) == 0x02)) {
		state = wait;
	}

	else if ((~PINA & 0x03) == 0x03) {
		state = zero;
	}

	else {
		state = init;
	}
	break;

	case inc:
	state = wait;
	break;

	case dec:
	state = wait;
	break;

	case zero:
	if (((~PINA & 0x03) == 0x01) || ((~PINA & 0x03) == 0x02)) {
		state = zero;
	}
	else {
		state = init;
	}
	break;
		
	default:
	state = Start;
	break;
}

switch(state) { //actions

	case Start:
	PORTB = 0x07;
	break;

	case init:
	break;

	case wait:
	break;

	case inc: 
	if (PORTB < 0x09) {
		PORTB = PORTB + 0x01;
	}

	else {
		PORTB = PORTB;
	}
	break;

	case dec:
	if (PORTB > 0x00) {
		PORTB = PORTB - 0x01;
	}

	else {
		PORTB = PORTB;
	}
	break;

	case zero:
	PORTB = 0x00;
	break;

	default:
	break;
	} 
}

int main() {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB= 0xFF; PORTB = 0x07;
	TimerSet(100);
	TimerOn();
	state = Start;
	while (1) {
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}
