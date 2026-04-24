/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 15:05:59 by macarnie          #+#    #+#             */
/*   Updated: 2026/04/22 14:34:52 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>

#define DIM 5

/*
** Configure GPIO: set LEDS D1-D4 (PB0-PB2, PB4), and RGB D5 (PD3, PD5, PD6) as outputs.
** Start with all LEDs off.
*/
static void	gpio_init(void)
{
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));

	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	PORTD &= ~((1 << PD3) | (1 << PD5) | (1 << PD6));
}

/*
** Configure ADC: 8-bit left-adjusted mode, AVCC reference, timer-triggered.
** Auto-trigger disabled; Timer1 ISR manually starts conversions.
** ADC complete interrupt enabled to process results immediately.
** Prescaler 128 gives 125 kHz ADC clock (safe for 10-bit, we use 8-bit).
** Digital input disabled on ADC0 to minimize noise coupling.
*/
static void	adc_init(void)
{
	ADMUX = (1 << REFS0 | (1 << ADLAR));
	ADCSRA =  (1 << ADEN)
			| (1 << ADIE)
			| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRB = 0;
	DIDR0 = (1 << ADC0D);
}

/*
** Configure Timer1: CTC mode at 50 Hz (20 ms period).
** Compare A interrupt fires every 40000 counts (prescaler 8: 16M/8 = 2M counts/s).
** ISR kicks ADC conversion to maintain steady sensor sampling rate.
*/
static void timer1_init(void)
{
	TCCR1A = 0b00000000;
	TCCR1B = 0b00001010;

	TCNT1 = 0;

	OCR1A = 40000;

	TIMSK1 = (1 << OCIE1A);
}

/*
** Configure Timer0 & Timer1: Fast PWM mode, non-inverting output on OC0A, OC0B & OC2B at ~60 Hz
** Fast PWM OB (red), OA (blue) & 2B duty cycles handled by OCRXn / ICRX (prescaler 1024: 16M/1024 = 15625 counts/s)
*/
static void	timer_rgb_init(void)
{
	TCCR0A = 0b10100011;
	TCCR0B = 0b00000101;

	TCCR2A = 0b00100011;
	TCCR2B = 0b00000101;

	OCR0A = 0;
	OCR0B = 0;
	OCR2B = 0;
}

static void	display_gauge(uint8_t val)
{
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));
	PORTB |=  ((val >> 6 > 0) << PB0)
			| ((val >> 6 > 1) << PB1)
			| ((val >> 6 > 2) << PB2)
			| ((val == 0xFF) << PB4);
}

static void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r / DIM;
	OCR0A = g / DIM;
	OCR2B = b / DIM;
}

static void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
		set_rgb(255 - pos * 3, 0, pos * 3);
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	}
	else
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

void	__vector_21(void) __attribute__((signal, used));
void	__vector_11(void) __attribute__((signal, used));

/*
** Timer1 Compare A interrupt: periodic trigger (50 Hz).
** Starts one ADC conversion per period by setting ADSC.
** Keeps sampling rate stable and independent of UART backpressure.
*/
void	__vector_11(void) // Timer1 compare match A ISR
{
	ADCSRA |= (1 << ADSC);
}

/*
** ADC conversion complete: format result and enqueue for transmission.
** Reads ADCH (8-bit left-adjusted result) in ~2 CPU cycles.
** Converts to 2 hex chars + CRLF frame (\r\n) via uart_tx_push_buf().
** Drop policy: sample lost if TX queue cannot fit 4 bytes (real-time first).
*/
void	__vector_21(void) // ADC conversion complete ISR
{
	uint8_t res = ADCH; // Read ADC value (8-bit)

	display_gauge(res);
	wheel(res);
}

static inline void	local_sei(void)
{
	__asm__ volatile ("sei" ::: "memory");
}

/*
** Main: initialize all peripherals, enable interrupts, then idle.
** All work is ISR-driven: Timer triggers ADC, ADC fills TX queue,
** UART ISR drains queue. Main loop intentionally empty.
** System runs in low-power idle state, interrupt-responsive.
*/
int main	(void)
{
	gpio_init();
	timer1_init();
	timer_rgb_init();
	adc_init();

	local_sei();

	while (1)
		;
}