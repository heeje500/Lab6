/*	Author: eeje Lee
 *	Lab: 6
 *  Partner(s) Name: 
 *	Lab Section: 
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif

enum States{init, zero, one, two, wait, restart}state;

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


void Tick() {

unsigned char count; 
	switch(state) { //transititons

	case init:
	PORTB = 0x00;
	state = zero;
	break;

	case zero:
	if ((~PINA & 0x01) == 0x01) {
		state = wait;
	}

	else {
		state = one;
	}
	count = 0x01;
	break;

	case one:
	if ((~PINA & 0x01) == 0x01) {
		state = wait;
	}

	else {
		if (count == 0) {
			state = zero;
		}

		else if (count == 1) {
			state = two;
		}
	}
	break;

	case two:
	if ((~PINA & 0x01) == 0x01) {
		state = wait;
	}

	else {
		state = one;
	}
	count = 0x00;
	break;

	case wait: 
	if ((~PINA & 0x01) == 0x01) {
		state = wait;
	}

	else {
		state = restart;
	}
	break;

	case restart:
	if ((~PINA & 0x01) == 0x01) {
		state = zero;
	}
	else {
		state = restart;
	}
	break;
		
	default:
	break;
}

switch(state) { //actions
	case init:
	break;

	case zero:
	PORTB = 0x01;
	break;

	case one:
	PORTB = 0x02;
	break;

	case two:
	PORTB = 0x04;
	break;

	case wait:
	break;

	case restart:
	break;
	default:
	break;

	} 
}

int main() {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB= 0xFF; PORTB = 0x00;
	TimerSet(300);
	TimerOn();
	state = init;
	while (1) {
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}
