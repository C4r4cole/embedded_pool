/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/16 14:38:52 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

int	main(void)
{
	DDRB |= (1 << PB1); // met PB1 en sortie
	TCCR1A |= (1 << COM1A0); // Toggle sur OC1A // Page 140 / 16.11.1 / Table 16-1
	TCCR1B |= (1 << WGM12); // Mode CTC // Table 16-4
	TCCR1B |= (1 << CS12); // Prescaler a 256 // Pages 142-143 / 16.11.2 / Bit 2:0 / Table 16-5
	OCR1A = 31249;
	
	while (1)
	{
	}
}

/***********************************************************************************************/
/****                                        TCNT1                                          ****/
/***********************************************************************************************/


// c'est le registre principal (le compteur)
// il s'incremente tout seul (de 0 a 65535 car 16 bits)

/***********************************************************************************************/
/****                                        TCCR1A                                         ****/
/***********************************************************************************************/

// ce registre (8 bits) controle le comportement des sorties (OC1A)
// il contient notamment les bits COM1A1 & COM1A0
	// ces 2 bits controlent ce que fait OC1A
		// quand COM1A1 est a 0
		// et que COM1A0 est a 1
		// Toggle !!!!!

/***********************************************************************************************/
/****                                        TCCR1B                                         ****/
/***********************************************************************************************/

// Ce registre (8 bits) controle le mode (CTC) et le prescaler

// CTC en WGM12
// Prescaler en CS12

// c'est le registre qui contient les bits qui choisisse la vitesse
	// Prescaler	Vitesse
	// 1			16 MHz
	// 8			2 MHz
	// 64			250 kHz
	// 256			62.5 kHz
	// 1024			15.6 kHz
	
	// plus le prescaler est grand, plus le timer est lent


/***********************************************************************************************/
/****                                        OCR1A                                          ****/
/***********************************************************************************************/

// Output Compare Register 1A
// c'est une valeur cible (la valeur que TCNT1 doit regoindra apres plusieurs incrementation)
// quand TCNT1 == OCR1A alors quelque chose se produit


/***********************************************************************************************/
/****                                         OC1A                                          ****/
/***********************************************************************************************/

// c'est une sortie speciale liee au timer
// sur ATmega328P : OC1A == PB1
// cela veut dire qui ma LED D1 (qui est aussi en PB1) est pilotable directement par le timer

/***********************************************************************************************/
/****                                       MODE CTC                                        ****/
/***********************************************************************************************/

// Le timer compte
// atteint OCR1A
// revient a 0 automatiquement
// recommence

// Cela cree un cycle regulier

