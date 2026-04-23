/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/23 15:30:11 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

volatile uint8_t up = 1;
volatile uint16_t value = 0;

void	timer1_brightness()
{
	DDRB |= (1 << PB1); // met PB1 en sortie
	
	// Waveform Generation Mode : fast PWM with TOP at ICR1 // Table 16-4 p 142
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	TCCR1A |= (1 << WGM11);
	
	// prescaler a 8
	// pour que le scintillement soit invisible a l'oeil nu, il faut etre superieur a 2kHz
	// Formule : F_CPU / (prescaler * (1 + TOP)
	// 16 000 000 / ( 8 * 1001) = 2000 (soit 2kHz)
	TCCR1B |= (1 << CS11);
	// TCCR1B |= (1<< CS10) | (1 << CS11); // test avec prescaler a 64

	
	TCCR1A |= (1 << COM1A1);

	ICR1 = 1000;
	// ICR1 = 124; // test avec prescaler a 64
	OCR1A = 0;
}

void	timer0_brightness_update()
{
	// Waveform Generation Mode : CTC with TOP at OCRA Table 15-8 p 115
	TCCR0A |= (1 << WGM01);

	// Prescaler 64 Table 15-9
	TCCR0B |= (1 << CS00) | (1 << CS01);

	// Interruption on OCRA section 15.9.6 p 118
	TIMSK0 |= (1 << OCIE0A);
	
	// Avec prescaler a 64 : 16 000 000 / 64 = 250 000
	// Le Timer compte donc jusqu'a 250 000 en 1 sec
	// Si par exemple on choisi 1000 interruptions par secondes alors : 
	OCR0A = 250000 / 1000;
}

void TIMER0_COMPA_vect(void) __attribute__((signal, used));

void TIMER0_COMPA_vect()
{
    static uint16_t count = 0;

    count++;

    if (count >= 10) // toutes les 10 ms (donc on fait qqch toutes les 10 interruptions)
    {
        count = 0;

        if (up)
        {
            value += 20;
            if (value >= 1000)
            {
                value = 1000;
                up = 0;
            }
        }
        else
        {
            if (value > 20)
                value -= 20;
            else
            {
                value = 0;
                up = 1;
            }
        }

        OCR1A = value;
    }
}

int	main(void)
{
	timer1_brightness();
	timer0_brightness_update();
	SREG |= (1 << SREG_I);
	while (1)
	{
	}
}
