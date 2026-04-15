/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/15 12:04:23 by fmoulin          ###   ########.fr       */
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

// int	prev = PIND & (1 << 2);

	// en faisant ca on ne s'interesse qu'au bit numero 2
	// (1 << 2) cree un masque 00000100
	// PIND est soit 00000000 soit 00000100
		// si PIND et le masque sont tous les 2 a 00000100
			// prev = 4 (parce que 00000100 = 4)
		// si PIND et le masque sont differents ou tous les 2 a 00000000 
			// prev = 0