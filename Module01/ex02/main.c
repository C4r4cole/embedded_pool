/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/16 15:48:25 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

int	main(void)
{
	DDRB |= (1 << PB1); // met PB1 en sortie
	TCCR1A |= (1 << COM1A1); // non-inverting mode sur OC1A // Page 140 / 16.11.1 / Table 16-1
	TCCR1B |= (1 << WGM13); // Mode fastPWM ICR1 en MAX & OCR1A en duty // Table 16-4
	TCCR1B |= (1 << WGM12); // Mode fastPWM ICR1 en MAX & OCR1A en duty // Table 16-4
	TCCR1A |= (1 << WGM11); // Mode fastPWM ICR1 en MAX & OCR1A en duty // Table 16-4
	TCCR1B |= (1 << CS12); // Prescaler a 256 // Pages 142-143 / 16.11.2 / Bit 2:0 / Table 16-5
	ICR1 = 62499;
	OCR1A = 6250;
	
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

	// le prescaler est un ralentisseur de temps !
	// le CPU fait 16 000 000 de cycles en 1 seconde
	// Le timer fait 16 bits
		// On n'a donc que 65 536 cycles max pour compter
		// 16 000 000 / 256 = 62 500
		// 62 500 ca rentre dans 65 536
		// grace au prescaler, on passe donc a 62.5 kHz (62 500 cycles par seconde)
		// un bip toutes les 0.5 seconde = 62 500 / 2 = 31 250 cycles
		

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
