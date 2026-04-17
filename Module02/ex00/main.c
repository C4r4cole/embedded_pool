/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/17 15:24:25 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

void uart_tx(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!( UCSR0A & (1<<UDRE0))) //20.11.2 page 200 / UDREn = Bit 5
	;
	// quand UDRE0 = 1, le data register est prêt à recevoir un nouveau byte
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void uart_init(unsigned int ubrr)
{
	/*Set baud rate */
	UCSR0A &= ~(1 << U2X0); // Table 20-7 page 199
	UBRR0H = (unsigned char)(ubrr>>8);
		// on decale de 8 bits vers la droite pour pouvoir stocker les bits fort (H pour High)
			// unsigned int est sur 16 bits mais tous les registres sont sur 8 Bits
			// on ne peut pas tous stocker d'un coup
	UBRR0L = (unsigned char)ubrr;
		// on garde les 8 bits faibles
	/* Enable transmitter */
	UCSR0B = (1<<TXEN0); // 20.11.13 page 201
	/* Set frame format: 8data, 1stop bit */
	UCSR0C |= (1<<UCSZ00); // 20.11.4 page 202 / Table 20-11
	UCSR0C |= (1<<UCSZ01);
	UCSR0C &= ~(1<<USBS0);
}

int	main(void)
{
	uart_init(8); 
	while (1)
	{
		uart_tx('Z');
		_delay_ms(1000);
	}
}

/**************************************************************************************/
/*                             Concepts importants                                    */
/*                               F_CPU & BAUD rate                                    */
/**************************************************************************************/

// - La F_CPU de 16Mhz, c'est la vitesse interne du CPU
// 		- cela veut dire 16 000 000 de cycles par seconde
// - Le BAUD rate de 115200, c'est la vitesse de communication
// 		- En UART : 1 baud = 1 bit
// 		- cela veut dire 115200 bits / seconde

/**************************************************************************************/
/*                                UBRR0H & UBRR0L                                     */
/**************************************************************************************/

// Uart Baud Rate Register -> UBRR
// Ce sont les registres du baud rate generator

/**************************************************************************************/
/*                                     UCSR0A                                         */
/**************************************************************************************/

// C’est le registre des flags d’état.
 
//quand UDRE0 = 1, le data register est prêt à recevoir un nouveau byte
// donc on peut écrire dans UDR0

// contient aussi RXC0, TXC0, FE0, DOR0, UPE0, U2X0

/**************************************************************************************/
/*                                     UCSR0B                                         */
/**************************************************************************************/

// c'est le registre qui gere (entre autre) les parametres reception/transmission

/**************************************************************************************/
/*                                     UCSR0C                                         */
/**************************************************************************************/

// c'est le registre qui gere (entre autre) le nombre de bits envoye/recu par le
// transmetteur/recepteur
