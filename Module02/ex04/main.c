/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/21 13:50:38 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define USER_BUF_SIZE 32
#define PASS_BUF_SIZE 32

#define STATE_USERNAME 0
#define STATE_PASSWORD 1

volatile uint8_t g_state = STATE_USERNAME;
volatile uint8_t g_user_i = 0;
volatile uint8_t g_pass_i = 0;

char g_user_buf[USER_BUF_SIZE];
char g_pass_buf[PASS_BUF_SIZE];

const char g_true_user[] = "spectre";
const char g_true_pass[] = "hello";

/*   GPIO   */

void leds_on(void)
{
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
	while (1)
	{
		PORTB ^= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
		_delay_ms(50);
	}
}

/*   UART   */

void uart_tx(unsigned char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}

void uart_print(const char *str)
{
	while (*str)
	{
		uart_tx(*str);
		str++;
	}
}

void uart_println(const char *str)
{
	uart_print(str);
	uart_tx('\r');
	uart_tx('\n');
}

void uart_init(unsigned int ubrr)
{
	UCSR0A = (1 << U2X0);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

int	ft_strcmp(char *s1, const char *s2)
{
	int	i = 0;

	while (s1[i] == s2[i] && s1[i] && s2[i])
		i++;
	if (s1[i] != s2[i])
		return (0);
	if (s1[i] == '\0' && s2[i] == '\0')
		return (1);
	return (0);
}

void	reset_login(void)
{
	g_user_i = 0;
	g_pass_i = 0;
	g_user_buf[0] = '\0';
	g_pass_buf[0] = '\0';
	g_state = STATE_USERNAME;

	uart_println("");
	uart_println("Enter your login:");
	uart_print("username: ");
}

void	uart_erase_last_char(void)
{
	uart_tx('\b');
	uart_tx(' ');
	uart_tx('\b');
}

void USART_RX_vect(void) __attribute__((signal, used));

void USART_RX_vect(void)
{
	char	c;

	c = UDR0;

	/* BACKSPACE */
	if (c == 127 || c == '\b')
	{
		if (g_state == STATE_USERNAME && g_user_i > 0)
		{
			g_user_i--;
			g_user_buf[g_user_i] = '\0';
			uart_erase_last_char();
		}
		else if (g_state == STATE_PASSWORD && g_pass_i > 0)
		{
			g_pass_i--;
			g_pass_buf[g_pass_i] = '\0';
			uart_erase_last_char();
		}
		return;
	}

	/* ENTER */
	if (c == '\r')
	{
		uart_tx('\r');
		uart_tx('\n');

		if (g_state == STATE_USERNAME)
		{
			g_user_buf[g_user_i] = '\0';
			g_state = STATE_PASSWORD;
			uart_print("password: ");
			return;
		}
		else if (g_state == STATE_PASSWORD)
		{
			g_pass_buf[g_pass_i] = '\0';

			if (ft_strcmp(g_user_buf, g_true_user) && ft_strcmp(g_pass_buf, g_true_pass))
			{
				uart_print("Hello ");
				uart_print(g_user_buf);
				uart_tx('!');
				uart_tx('\r');
				uart_tx('\n');
				uart_println("Shall we play a game?");
				leds_on();
			}
			else
			{
				uart_println("Bad combinaison username/password");
				reset_login();
			}
			return;
		}
	}

	/* caractère normal */
	if (g_state == STATE_USERNAME)
	{
		if (g_user_i < USER_BUF_SIZE - 1)
		{
			g_user_buf[g_user_i] = c;
			g_user_i++;
			g_user_buf[g_user_i] = '\0';
			uart_tx(c);
		}
	}
	else if (g_state == STATE_PASSWORD)
	{
		if (g_pass_i < PASS_BUF_SIZE - 1)
		{
			g_pass_buf[g_pass_i] = c;
			g_pass_i++;
			g_pass_buf[g_pass_i] = '\0';
			uart_tx('*');
		}
	}
}

int	main(void)
{
	uart_init(16);
	reset_login();
	SREG |= (1 << SREG_I);
	while (1)
	{
	}
}

/**************************************************************************************/
/*                                       ISR                                          */
/**************************************************************************************/

// Interrupt Service Routine
// C'est la fonction qui va etre declancher par le CPU au moment de l'interruption
// Il faut definir ce qu'elle fait

// comme on ne peut pas appele cette fonction car elle est interdite dans l'ennonce
	// il faut trouver l'alternative :
		// declarer un fonction avec le nom exact du vecteur qui nous interesse
			// vecteur 12 : TIMER_COMPA_vect
		// lui donner les attibute signal et used
			// signal : // - permet de specifier que ce n'est pas une fonction normal
			  			// - le compilateur doit l'interpreter comme une fonction
						// prevue par AVR
			// used :	// permet de dire au compilateur de ne pas supprimer la fonction
						// meme si il n'y a pas d'appel a cette fonction dans le code

/**************************************************************************************/
/*                                       SREG                                         */
/**************************************************************************************/

// Status REGister
// C'est le registre dans lequel on peut activer l'interruption
// SREG_I est le bit 7
// Le bit 7 = Global Interrupt Enable
