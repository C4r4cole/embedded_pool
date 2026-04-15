/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/15 11:24:49 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>

int	main(void)
{
	DDRB |= (1 << 0); 
	DDRD &= ~(1 << 2);
	PORTD |= (1 << 2);
	while (1)
	{
		if (!(PIND & (1 << 2)))
			PORTB |= (1 << 0);
		else
			PORTB &= ~(1 << 0);
	}
}

// Le 3 registres GPIO (DDR, PORT, PIN)

// DDR -> Direction register
	// il definie si une pin est une entree ou une sortie
	// 1 = sortie (envoie d'un signal)
	// 0 = entree (lecture d'un signal)

// PORT -> Output ou pull-up Register
	// son role depend du mode
	// si DDR = 1
		// PORT = 1 -> envoie du courant
		// PORT = 0 -> coupe le courant
	// si DDR = 0
		// avec PORT on active la pull-up pour envoyer constamment 1
			// Pull-up ≠ bouton
			// Pull-up = stabilisation + logique par défaut

// PIN -> Input Register
	// sert a lire l'etat du pin
	// 1 -> HIGH
	// 0 -> LOW

// Les lettres A, B, C et D sont des PORT
// A chaque PORT sont attribues 8 pin
// par exemple PORTA va de PA0 a PA7

// Pour resumer de maniere tres simple :
	// DDR  → entrée / sortie
	// PORT → écrire (ou pull-up)
	// PIN  → lire