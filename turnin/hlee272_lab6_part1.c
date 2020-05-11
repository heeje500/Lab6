/*	Author: Heeje Lee
 *	Lab: 6
 *  Partner(s) Name: 
 *	Lab Section: 
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	https://youtu.be/5KhgPkl5Vyo
 */ 
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif

enum States{Start, zero, one, two}state;

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
	switch(state) { //transititons

	case Start: 
	state = zero;
	break;

	case zero:
	state = one;
	break;

	case one:
	state = two;
	break;

	case two:
	state = zero;
	break;

	default:
	state = Start;
	break;
}

switch(state) { //actions
	case Start:
	PORTB = 0x00;
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
	} 
}

void main() {
	DDRB= 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	unsigned char tmpB = 0x00;
	state = Start;
	while (1) {
		tmpB = ~tmpB;
		PORTB = tmpB;
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}
