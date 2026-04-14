/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/14 18:51:55 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

int	main(void)
{
	DDRB |= (1 << 0);
	// on passe le bit 0 du registre DDRB a 1 afin de specifier la direction (sortie)
	
	PORTB |= (1 << 0);
	// on passe le bit 0 du registre PORTB a 1 afin de mettre l'etat de la LED sur ON
	
	while (1)
	{	
	}
}