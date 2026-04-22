/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/21 19:10:01 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

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

int	main(void)
{
	uint8_t pos = 0;
	init_rgb();
	while (1)
	{
		wheel(pos++);
		_delay_ms(20);
	}
}
