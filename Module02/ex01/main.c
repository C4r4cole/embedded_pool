/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/17 18:33:46 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

void TIMER1_COMPA_vect(void) __attribute__((signal, used)); // page 74 / Table 12-6

void uart_tx(unsigned char data)
{
	while (!( UCSR0A & (1<<UDRE0)))
	;
	UDR0 = data;
}

void uart_printstr(const char *str)
{
	while (*str)
	{
		uart_tx(*str);
		str++;
	}
	uart_tx('\r');
	uart_tx('\n');
}

void uart_init(unsigned int ubrr)
{
	UCSR0A &= ~(1 << U2X0);
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	UCSR0B = (1<<TXEN0);
	
	UCSR0C |= (1<<UCSZ00);
	UCSR0C |= (1<<UCSZ01);
	UCSR0C &= ~(1<<USBS0);
}

void timer_init(void)
{
	TCCR1B |= (1 << WGM12); // Mode CTC // Table 16-4
	TCCR1B |= (1 << CS10); // Prescaler a 1024 // Pages 142-143 / 16.11.2 / Bit 2:0 / Table 16-5
	TCCR1B |= (1 << CS12); // Prescaler a 1024 // Pages 142-143 / 16.11.2 / Bit 2:0 / Table 16-5
	OCR1A = 31249;
	TIMSK1 |= (1 << OCIE1A); // Pages 144-145 
	
	// 16 000 000 / 1024 = 15 625
	// donc 15 625 cycles par seconde
	// 15 625 x 2 = 31 250 pour 2 secondes
}

void TIMER1_COMPA_vect(void) // page 74 / Table 12-6
{
	uart_printstr("Hello World!");
}

int	main(void)
{
	uart_init(8);
	timer_init();
	SREG |= (1 << SREG_I); // 7.3.1 page 20
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
