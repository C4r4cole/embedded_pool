/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/14 19:52:49 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

int	main(void)
{
	DDRB |= (1 << 0);
	DDRD &= ~(1 << 2);
	PORTD |= (1 << 2);
	int	prev = PIND & (1 << 2);
	while (1)
	{
		int current = PIND & (1 << 2);
		if (prev != 0 && current == 0)
		{
			PORTB ^= (1 << 0);
			_delay_ms(50);
		}
		prev = current;
	}
}
