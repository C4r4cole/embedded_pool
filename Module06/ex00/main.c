/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/27 18:46:11 by fmoulin          ###   ########.fr       */
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

void	i2c_init(void)
{
	// SCL = F_CPU / (16 + 2 * TWBR * prescaler) // formule p 222
	// 100000 = 16000000 / (16 + 2 * TWBR)
	// TWBR = 72
	TWBR = 72;

	// Prescaler a 1
	TWSR &= ~((1 << TWPS1) | (1<< TWPS0));
}

void	i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR & (1 << TWINT)))
		;
}

void	i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void	i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
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

int	main(void)
{
	uint8_t	status;
	char	buf[8];
	uart_init(16);
	i2c_init();
	while (1)
	{
		i2c_start();

		status = TWSR & 0xF8;
		uart_printstr(itoa(status, buf, 10));
		uart_printstr("\r\n");

		i2c_write((0x38 << 1));
		status = TWSR & 0xF8;
		uart_printstr(itoa(status, buf, 10));
		uart_printstr("\r\n");
		
		i2c_stop();

		_delay_ms(20);
	}
}
