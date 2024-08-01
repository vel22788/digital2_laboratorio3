/*
 * main.c
 *
 * Created: 7/31/2024 6:22:43 PM
 *  Author: Luis Angel Velasquez
 */ 

#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include "UART/UART.h"
#include "SPI/SPI.h"

void MostrarLEDs(uint8_t a);

int unir(int d1, int d2, int d3){
	return ((d1*100) + (d2*10) + d3);}
	
int CharToInt(char num){
	return num - '0';}

volatile int d1, d2, d3;
volatile char Rv1, Rv2, Rv3;

volatile uint8_t  bufferRX;
uint8_t contador_valor_recibido, ValueReceived;

uint8_t valorSPI0 = 0;
uint8_t valorSPI1 = 0;

int main(void)
{
	cli();						// Se apagan las interrupciones globales
	contador_valor_recibido = 0;
	DDRC |= (1<<DDC0)|(1<<DDC1)|(1<<DDC2)|(1<<DDC3)|(1<<DDC4)|(1<<DDC5);
	DDRD |= 0b00011100;
	PORTD = 0;
	PORTC = 0;
	initUART(9600, 0, 0, 0, 0, 1, 8, 103);
	
	spiInit(SPI_MASTER_OSC_DIV16, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	sei();

	while (1)
	{
		PORTB &= ~(1<<PORTB2);   // SLAVE SELECT
		
		spiWrite('c');
		valorSPI0 = spiRead();
		_delay_ms(100);
		spiWrite('d');
		valorSPI1 = spiRead();
		
		
		d1=CharToInt(Rv1);
		d2=CharToInt(Rv2);
		d3=CharToInt(Rv3);
		ValueReceived = unir(d1,d2,d3);
		MostrarLEDs(ValueReceived);
		
		PORTB |= (1<<PORTB2);	// SLAVE SELECT
		
	}
}


ISR(USART_RX_vect)
{
	// Aumentar dicho contador
	contador_valor_recibido ++;
	
	// Si es la primera recepciton almacenarlo en el Rv1, si es la segunda en Rv2.
	if(contador_valor_recibido==1){
		Rv1 = UDR0;
	} else if (contador_valor_recibido==2)
	{
		Rv2 = UDR0;
		} else {
		Rv3 = UDR0;
		contador_valor_recibido = 0;
		char mensaje1[]="P1\n";
		writeUART_Text(mensaje1);
		writeUART_car(valorSPI0);
		char mensaje2[]="P2\n";
		writeUART_Text(mensaje2);
		writeUART_car(valorSPI1);
	}
}

void MostrarLEDs(uint8_t a){
	if(a & 1)
	PORTC |= (1<<PORTC0);
	else
	PORTC &= ~(1<<PORTC0);
	
	if(a & 2)
	PORTC |= (1<<PORTC1);
	else
	PORTC &= ~(1<<PORTC1);
	if(a & 4)
	PORTC |= (1<<PORTC2);
	else
	PORTC &= ~(1<<PORTC2);
	if(a & 8)
	PORTC |= (1<<PORTC3);
	else
	PORTC &= ~(1<<PORTC3);
	if(a & 16)
	PORTC |= (1<<PORTC4);
	else
	PORTC &= ~(1<<PORTC4);
	if(a & 32)
	PORTC |= (1<<PORTC5);
	else
	PORTC &= ~(1<<PORTC5);
	if(a & 64)
	PORTD |= (1<<PORTD3);
	else
	PORTD &= ~(1<<PORTD3);
	if(a & 128)
	PORTD |= (1<<PORTD2);
	else
	PORTD &= ~(1<<PORTD2);
	
}
