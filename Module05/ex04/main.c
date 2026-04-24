/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/24 17:30:25 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

void	timers_init()
{
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00); // Table 15-5 p114 & Table 15-8 p115
	TCCR0B |= (1 << CS00);

	TCCR2A |= (1 << COM2B1) | (1 << WGM20); // Table 18.7 p163 & Table 18.8 p164
	TCCR2B |= (1 << CS20);
}

void init_rgb()
{
	DDRD |= (1 << PD3) | (1 << PD5) | (1<< PD6); // met PD3, PD5 & PD6 en sortie
	PORTD &= ~(1 << PD3) | (1 << PD5) | (1<< PD6);
	timers_init();
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

void	adc_init()
{
	DDRC &= ~(1 << PC0); // ADC0 en entree
	
	// Initializing ADMUX = choix de la tension de reference
	//						choix du canal
	//						choix de l'alignement du resultat
	// REFS1 REFS0 ADLAR MUX3 MUX2 MUX1 MUX0
	
	// 1 - Choix de la tension de reference
	ADMUX |= (1 << REFS0);
		// Activer le bit REFS0 parametre ADMUX sur AVcc
		// AVcc est la valeur de reference
		// si la carte est a 5V alors la valeur de reference est de 5V
	
	// 2 - choix de l'alignement du resultat
	ADMUX &= ~(1 << ADLAR);
		// ADLAR bit a 0 = alignement a droite

	// 3 - choix du canal avec MUX Table 24-4 p 258
	ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
		// tous les bits a 0 donc ADC0 (ADC0 correspond a la pin du potentiometre)

	// Initializing ADCSRA // C'est le registre de controle de l'ADC
	// ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0
	
	// 1 - Activer l'ADC avec ADEN
	ADCSRA |= (1 << ADEN);
	
	// 2 - Parametrer le prescaler ADC avec ADPS2 ADPS1 et ADPS0
		// La datasheet recommande généralement une fréquence d’horloge ADC
		// autour de 50 kHz à 200 kHz pour obtenir la pleine résolution.
		// Avec F_CPU = 16 MHz, on choisit souvent prescaler 128 (Table 24-5 p259)
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRB = 0;
}

void	init_D_leds()
{
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4); // les 4 LEDs D en sortie
}

int	main(void)
{
	uint16_t 	result;
	adc_init();
	init_D_leds();
	init_rgb();
	while (1)
	{
		// Lance la conversion analog to digital
		ADCSRA |= (1 << ADSC);
		// Tant qu'une conversion est en cours
		while (ADCSRA & (1 << ADSC))
			;
		result = ADC; // Il faut lire le resultat seulement apres la conversion of course !!!
		wheel(result / 4);
		PORTB = 0;
		if (result >= 256)		PORTB |= (1 << PB0);
		if (result >= 2*256)	PORTB |= (1 << PB1);
		if (result >= 3*256)	PORTB |= (1 << PB2);
		if (result == 1023)		PORTB |= (1 << PB4);
	}
}
