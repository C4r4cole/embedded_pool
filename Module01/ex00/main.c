/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/15 17:10:02 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

int	main(void)
{
	DDRB |= (1 << PB1); // met la LED D2 du PORT B en sortie
	while (1)
	{
		for (volatile long i = 0; i < 400000; i++)
		{
		}
		PORTB ^= (1 << PB1);
	}
}

// une iteration de ma boucle for c'est :
	// lecture de i
	// comparaison 32 bits
	// incrément 32 bits
	// écriture de i
	// branchement
// comme mon i est un long (32 bits) et que avr est 8 bits
	// 32 / 8 = 4
	// chaque etape d'une iteration de ma boucle va etre faite 4 fois
// donc environ 4 x 5 etapes pour chaque iteration de ma boucle (soit environ 20 instructions par loop)
// 400 000 (loop) x 20 (instructions) = 8 000 000

// la frequence du CPU est de 16Mhz
	// cela signifie qu'il peut effectuer 16 000 000 d'instructions en 1 seconde
	// 8 000 000 d'instructions etant la moitie, on peut supposer que :
		// la boucle va "attendre" 0.5 sec
		// la LED va s'allumer les autres 0.5 sec