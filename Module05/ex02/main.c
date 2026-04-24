/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/24 14:51:48 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

void uart_init(unsigned int ubrr)
{
	/*Set baud rate */
	UCSR0A |= (1 << U2X0); // Table 20-7 page 199
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

void uart_tx(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!( UCSR0A & (1<<UDRE0))) //20.11.2 page 200 / UDREn = Bit 5
	;
	// quand UDRE0 = 1, le data register est prêt à recevoir un nouveau byte
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void uart_printstr(const char* str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}


char	*itoa(int n, char *str, int base)
{
	long	nb;
	int		i;
	int		sign;

	if (base < 2 || base > 16)
		return (0);
	nb = n;
	sign = 0;
	i = 0;
	if (nb < 0 && base == 10)
	{
		sign = 1;
		nb = -nb;
	}
	if (nb == 0)
		str[i++] = '0';
	while (nb > 0)
	{
		if (nb % base < 10)
			str[i++] = (nb % base) + '0';
		else
			str[i++] = (nb % base) - 10 + 'A';
		nb /= base;
	}
	if (sign)
		str[i++] = '-';
	str[i] = '\0';
	i--;
	sign = 0;
	while (sign < i)
	{
		char tmp = str[sign];
		str[sign] = str[i];
		str[i] = tmp;
		sign++;
		i--;
	}
	return (str);
}

void	adc_init()
{
	DDRC &= ~(1 << PC0); // ADC0 en entree
	DDRC &= ~(1 << PC1); // ADC1 en entree
	DDRC &= ~(1 << PC2); // ADC2 en entree
	
	// Initializing ADMUX = choix de la tension de reference
	//						choix du canal
	//						choix de l'alignement du resultat
	// REFS1 REFS0 ADLAR MUX3 MUX2 MUX1 MUX0
	
	// 1 - Choix de la tension de reference
	ADMUX |= (1 << REFS0);
		// Activer le bit REFS0 parametre ADMUX sur AVcc
		// AVcc est la valeur de reference
		// si la carte est a 5V alors la valeur de reference est de 5V
	
	// 2 - choix de l'alignement du resultat
	ADMUX &= ~(1 << ADLAR);
		// ADLAR bit a 0 = alignement a droite

	// 3 - choix du canal avec MUX Table 24-4 p 258
	// ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
		// tous les bits a 0 donc ADC0 (ADC0 correspond a la pin du potentiometre)

	// Initializing ADCSRA // C'est le registre de controle de l'ADC
	// ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0
	
	// 1 - Activer l'ADC avec ADEN
	ADCSRA |= (1 << ADEN);
	
	// 2 - Parametrer le prescaler ADC avec ADPS2 ADPS1 et ADPS0
		// La datasheet recommande généralement une fréquence d’horloge ADC
		// autour de 50 kHz à 200 kHz pour obtenir la pleine résolution.
		// Avec F_CPU = 16 MHz, on choisit souvent prescaler 128 (Table 24-5 p259)
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRB = 0;
}

int	main(void)
{
	uint16_t 	result;
	char		buf[8];
	uart_init(16);
	adc_init();
	while (1)
	{
		ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
		// Lance la conversion analog to digital
		ADCSRA |= (1 << ADSC);
		// Tant qu'une conversion est en cours
		while (ADCSRA & (1 << ADSC))
			;
		result = ADC; // Il faut lire le resultat seulement apres la conversion of course !!!
		uart_printstr(itoa(result, buf, 10));
		uart_printstr(", ");

		ADMUX &= ~((1<<MUX1)|(1<<MUX2)|(1<<MUX3));
		ADMUX |= (1<<MUX0);
		// Lance la conversion analog to digital
		ADCSRA |= (1 << ADSC);
		// Tant qu'une conversion est en cours
		while (ADCSRA & (1 << ADSC))
			;
		result = ADC; // Il faut lire le resultat seulement apres la conversion of course !!!
		uart_printstr(itoa(result, buf, 10));
		uart_printstr(", ");

		ADMUX &= ~((1<<MUX0)|(1<<MUX2)|(1<<MUX3));
		ADMUX |= (1<<MUX1);
		// Lance la conversion analog to digital
		ADCSRA |= (1 << ADSC);
		// Tant qu'une conversion est en cours
		while (ADCSRA & (1 << ADSC))
			;
		result = ADC; // Il faut lire le resultat seulement apres la conversion of course !!!
		uart_printstr(itoa(result, buf, 10));
		uart_printstr("\r\n");
		
		_delay_ms(20);
	}
}
