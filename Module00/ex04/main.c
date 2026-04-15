/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/15 14:31:39 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

int	main(void)
{
	int	i = 0;
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4); // met les LED D1, D2, D3 et D4 du PORT B en sortie
	DDRD &= ~(1 << PD2); // met le PORT D en entree
	PORTD |= (1 << PD2) | (1 << PD4); // active les pull-up des SW1 et SW2
	int	prev_SW1 = PIND & (1 << PD2); // garde l'etat de l'occurence precedente de SW1
	int	prev_SW2 = PIND & (1 << PD4); // garde l'etat de l'occurence precedente de SW2
	while (1)
	{
		int current_SW1 = PIND & (1 << PD2); // etat de l'occurence actuelle de SW1
		int current_SW2 = PIND & (1 << PD4); // etat de l'occurence actuelle de SW2
		if (prev_SW1 != 0 && current_SW1 == 0)
		{
			i++;
			_delay_ms(50);
		} // on incremente i de 1 sur le bouton SW1 est presse
		if (prev_SW2 != 0 && current_SW2 == 0)
		{
			i--;
			_delay_ms(50);
		} // on decremente i de 1 sur le bouton SW2 est presse
		if (i & (1 << 0)) // resultat 0 ou 1
			PORTB |= (1 << PB0); // si 1
		else
			PORTB &= ~(1 << PB0); // si 0
		if (i & (1 << 1)) // resultat 0 ou 2
			PORTB |= (1 << PB1); // si 2
		else
			PORTB &= ~(1 << PB1); // si 0
		if (i & (1 << 2)) // resultat 0 ou 4
			PORTB |= (1 << PB2); // si 4
		else
			PORTB &= ~(1 << PB2); // si 0
		if (i & (1 << 3)) // resultat 0 ou 8
			PORTB |= (1 << PB4); // si 8
		else
			PORTB &= ~(1 << PB4); // si 0
		prev_SW1 = current_SW1;
		prev_SW2 = current_SW2;
	}
}
