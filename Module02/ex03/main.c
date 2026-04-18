/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/18 12:01:47 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

void USART_RX_vect(void) __attribute__((signal, used));

void uart_tx(unsigned char data)
{
	while (!( UCSR0A & (1<<UDRE0)))
	;
	UDR0 = data;
}

unsigned char uart_rx(void)
{
	while (!( UCSR0A & (1<<RXC0)))
	;
	return (UDR0);
}

void uart_init(unsigned int ubrr)
{
	UCSR0A |= (1 << U2X0);
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); // page 201 / 20.11.3
	
	UCSR0C |= (1<<UCSZ00);
	UCSR0C |= (1<<UCSZ01);
	UCSR0C &= ~(1<<USBS0);
}

void USART_RX_vect(void)
{
	unsigned char c;
	
	c = uart_rx();
	if (c == '\r')
	{
		uart_tx('\r');
		uart_tx('\n');
	}
	if (c >= 'a' && c <= 'z')
		c -= 32;
	else if (c >= 'A' && c <= 'Z')
		c += 32;
	uart_tx(c);	
}

int	main(void)
{
	uart_init(16);
	SREG |= (1 << SREG_I);
	while (1)
	{
	}
}

/**************************************************************************************/
/*                                     TIMSK1                                         */
/**************************************************************************************/

// Time Interrupt MaSK Register
// c'est le registre qui permet de declenche l'interruption
	// il contient le bit OCIE1A que l'on passe a 1
	// OCIE = Output Compare Iterrupt Enable
	// en faisant cela, on permet au CPU de declencher une interruption quand le Timer
	// match avec OCR1A
	
/**************************************************************************************/
/*                                       ISR                                          */
/**************************************************************************************/

// Interrupt Service Routine
// C'est la fonction qui va etre declancher par le CPU au moment de l'interruption
// Il faut definir ce qu'elle fait

// comme on ne peut pas appele cette fonction car elle est interdite dans l'ennonce
	// il faut trouver l'alternative :
		// declarer un fonction avec le nom exact du vecteur qui nous interesse
			// vecteur 12 : TIMER_COMPA_vect
		// lui donner les attibute signal et used
			// signal : // - permet de specifier que ce n'est pas une fonction normal
			  			// - le compilateur doit l'interpreter comme une fonction
						// prevue par AVR
			// used :	// permet de dire au compilateur de ne pas supprimer la fonction
						// meme si il n'y a pas d'appel a cette fonction dans le code

/**************************************************************************************/
/*                                       SREG                                         */
/**************************************************************************************/

// Status REGister
// C'est le registre dans lequel on peut activer l'interruption
// SREG_I est le bit 7
// Le bit 7 = Global Interrupt Enable
