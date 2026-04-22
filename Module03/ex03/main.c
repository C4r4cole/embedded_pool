/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/22 11:12:51 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int	ft_atoi_hex(const char *nptr)
{
	size_t	i;
	int		sign;
	int		nb;

	i = 0;
	sign = 1;
	nb = 0;
	while (nptr[i] == 9 || nptr[i] == 10
		|| nptr[i] == 11 || nptr[i] == 12
		|| nptr[i] == 13 || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	if (nptr[i] == '0' && (nptr[i + 1] == 'x' || nptr[i + 1] == 'X'))
		i += 2;
	while ((nptr[i] >= '0' && nptr[i] <= '9')
		|| (nptr[i] >= 'a' && nptr[i] <= 'f')
		|| (nptr[i] >= 'A' && nptr[i] <= 'F'))
	{
		nb *= 16;
		if (nptr[i] >= '0' && nptr[i] <= '9')
			nb += nptr[i] - '0';
		else if (nptr[i] >= 'a' && nptr[i] <= 'f')
			nb += nptr[i] - 'a' + 10;
		else
			nb += nptr[i] - 'A' + 10;
		i++;
	}
	return (nb * sign);
}

void uart_init(unsigned int ubrr)
{
	UCSR0A |= (1 << U2X0);
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1 << RXCIE0);
	
	UCSR0C |= (1<<UCSZ00);
	UCSR0C |= (1<<UCSZ01);
	UCSR0C &= ~(1<<USBS0);
}

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
	PORTD &= ~((1 << PD3) | (1 << PD5) | (1 << PD6));
	timers_init();
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

int	main(void)
{
	char	buf[7];
	char	r[3];
	char	g[3];
	char	b[3];

	uart_init(16);
	init_rgb();
	while (1)
	{
		uart_tx('#');
		for (uint8_t i = 0; i < 6; i++)
		{
			buf[i] = uart_rx();
			uart_tx(buf[i]);
		}
		buf[6] = '\0';

		r[0] = buf[0];
		r[1] = buf[1];
		r[2] = '\0';
		g[0] = buf[2];
		g[1] = buf[3];
		g[2] = '\0';
		b[0] = buf[4];
		b[1] = buf[5];
		b[2] = '\0';

		set_rgb(ft_atoi_hex(r), ft_atoi_hex(g), ft_atoi_hex(b));
		uart_tx('\r');
		uart_tx('\n');
	}
}
