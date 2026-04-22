/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/21 14:02:10 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

int	main(void)
{
	DDRD |= (1 << PD3) | (1 << PD5) | (1<< PD6); // met PD3, PD5 & PD6 en sortie
	
	while (1)
	{
		PORTD &= ~(1 << PD3); // off blue
		PORTD |= (1 << PD5); // on red
		_delay_ms(1000);
		PORTD &= ~(1 << PD5); // off red
		PORTD |= (1 << PD6); // on green
		_delay_ms(1000);
		PORTD &= ~(1 << PD6); // off green
		PORTD |= (1 << PD3); // on blue
		_delay_ms(1000);
	}
}
