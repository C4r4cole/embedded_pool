/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/22 16:42:54 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

volatile uint8_t g_wait_release = 0;

void	led_d1_init()
{
	DDRB |= (1 << PB0); // met PB0 en sortie
}

void	button_sw1_init()
{
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2); // active la pull-up sur SW1
	
	EIMSK |= (1 << INT0); // on enable interrupt sur INT0
	EICRA &= ~((1 << ISC01) | (1 << ISC00)); // ligne importante (on reset)
	// et ensuite :
	EICRA |= (1 << ISC01); // parametrage sur falling edge (Table13-2 p80)
	
}

void timer1_init(void)
{
	TCCR1A = 0;
	TCCR1B = 0;
	// on les reset pour eviter de risquer de garder des bits indesirables des boucles precedentes
	
	TCCR1B |= (1 << WGM12);  // mode CTC

	// Pour F_CPU = 16 MHz, prescaler 64 :
	// 16 MHz / 64 = 250000 Hz
	// 20 ms => 5000 ticks => OCR1A = 4999
	OCR1A = 4999;

	TIMSK1 |= (1 << OCIE1A); // Enable interruption compare A
}

void INT0_vect(void) __attribute__((signal, used)); // page 74 / Table 12-6

void INT0_vect(void)
{
	EIMSK &= ~(1 << INT0); // désactive INT0 pendant debounce

	if (g_wait_release == 0)
	{
		// on vient de détecter l'appui
		PORTB ^= (1 << PB0);
		g_wait_release = 1;

		// prochaine interruption : rising edge
		EICRA &= ~((1 << ISC01) | (1 << ISC00));
		EICRA |= (1 << ISC01) | (1 << ISC00);
	}
	else
	{
		// on vient de détecter le relâchement
		g_wait_release = 0;

		// prochaine interruption : falling edge
		EICRA &= ~((1 << ISC01) | (1 << ISC00));
		EICRA |= (1 << ISC01);
	}

	TCNT1 = 0;
	TCCR1B |= (1 << CS11) | (1 << CS10); // start timer1, prescaler 64
}


void TIMER1_COMPA_vect(void) __attribute__((signal, used));

void	TIMER1_COMPA_vect()
{
	// stop timer
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));

	EIFR |= (1 << INTF0);    // clear flag éventuel
	EIMSK |= (1 << INT0);    // réactive INT0
}

int	main(void)
{
	led_d1_init();
	button_sw1_init();
	timer1_init();
	SREG |= (1 << SREG_I);
	while (1)
	{
	}
}
