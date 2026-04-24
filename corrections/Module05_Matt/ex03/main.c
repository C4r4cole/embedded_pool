/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: macarnie <macarnie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 15:05:59 by macarnie          #+#    #+#             */
/*   Updated: 2026/04/21 19:39:43 by macarnie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>

#ifndef F_CPU
# define F_CPU	16000000UL
#endif

#ifndef BAUD
# define BAUD	115200
#endif

#define TX_BUF_SIZE	64

/*
** Ring-buffer UART TX path: ADC ISR produces bytes, UDRE ISR consumes them.
** Must stay power-of-two size for mask-based wrapping in tx_next().
** Producer: __vector_21 (ADC). Consumer: __vector_19 (UDRE).
** head = next write position, tail = next read position.
** Empty: head == tail. Full: (head + 1) & mask == tail.
*/

/*
** Internal Temperature Sensor : ADC8 (MUX[3:0] = 1000), decimal format "xxxx\r\n"
** Tos and K are calibration constants for converting raw ADC to temperature in °C.
*/

#define Tos 345 // ADC value at 25°C (from board testing with boot.h)

static volatile uint8_t	adc_dec[6] = {0, 0, 0, 0,
									   '\r', '\n'};
static volatile uint8_t	tx_buf[TX_BUF_SIZE];
static volatile uint8_t	tx_head = 0;
static volatile uint8_t	tx_tail = 0;

/*
** Configure UART for TX-only at configured baud rate.
** U2X0 enables double-speed mode for better accuracy at 115200.
** UDRIE0 (TX interrupt) is enabled only when data is queued (lazy approach).
*/
static void	uart_init(uint16_t ubrr)
{
	UBRR0H = (uint8_t)(ubrr >> 8); // Set baud rate high byte
	UBRR0L = (uint8_t)ubrr; // Set baud rate low byte

	UCSR0A = (1 << U2X0); // Enable double speed mode for more accurate baud rate
	UCSR0B = (1 << TXEN0); // Enable transmitter, no receiver

	UCSR0C =  (0 << UMSEL01) | (0 << UMSEL00) // Asynchronous USART
			| (0 << UPM01)   | (0 << UPM00) // No parity
			| (0 << USBS0) // 1 stop bit
			| (1 << UCSZ01)  | (1 << UCSZ00); // 8 data bits
}

/*
** Ring-buffer wrap: advance index with modulo using bit-mask.
** Only works because TX_BUF_SIZE is power-of-two (64).
*/
static uint8_t	tx_next(uint8_t idx)
{
	return ((uint8_t)((idx + 1) & (TX_BUF_SIZE - 1)));
}

/*
** Enable UART Data Register Empty interrupt.
** Safe to call multiple times (setting already-set bit is a no-op).
** This wakes the TX consumer ISR if queue was idle.
*/
static void	uart_tx_kick(void)
{
	UCSR0B |= (1 << UDRIE0);
}

/*
** Enqueue single byte; returns 0 if buffer full, 1 if success.
** Non-blocking: never waits. Returns immediately to avoid ISR latency.
** Kicks UDRIE0 to ensure consumer wakes up if idle.
*/
static uint8_t	uart_tx_push(uint8_t c)
{
	uint8_t	next = tx_next(tx_head);

	if (next == tx_tail)
		return (0);
	tx_buf[tx_head] = c;
	tx_head = next;
	uart_tx_kick();
	return (1);
}

/*
** Enqueue variable-length buffer; drops silently if insufficient space.
** Non-blocking to keep ISR latency bounded; samples lost if queue full.
** Each call to uart_tx_push checks free slots and kicks TX ISR.
*/
static void	uart_tx_push_buf(volatile uint8_t *data, uint8_t len)
{
	uint8_t	free_slots;
	uint8_t	i;

	if (tx_head >= tx_tail)
		free_slots = (uint8_t)(TX_BUF_SIZE - (tx_head - tx_tail) - 1);
	else
		free_slots = (uint8_t)(tx_tail - tx_head - 1);
	if (free_slots < len)
		return ;

	if (len > TX_BUF_SIZE - 1)
		len = TX_BUF_SIZE - 1;

	i = 0;

	while (i < len)
	{
		uart_tx_push(data[i]);
		if (data[i] >= '0' && data[i] <= '9')
			data[i] = 0;
		++i;
	}
}

/*
** Configure ADC: 10-bit right-adjusted mode, 1.1V reference, timer-triggered.
** Auto-trigger disabled; Timer1 ISR manually starts conversions.
** ADC complete interrupt enabled to process results immediately.
** Prescaler 128 gives 125 kHz ADC clock (safe for 10-bit, we use 8-bit).
** Digital input disabled on ADC0, ADC1, ADC2 to minimize noise coupling.
*/
static void	adc_init(void)
{
	ADMUX = (1 << REFS1) |(1 << REFS0) | (1 << MUX3); // 1.1V reference, ADC8
	ADCSRA =  (1 << ADEN)
			| (1 << ADIE)
			| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRB = 0;
	DIDR0 = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D);
}

/*
** Configure Timer1: CTC mode at 50 Hz (20 ms period).
** Compare A interrupt fires every 40000 counts (prescaler 8: 16M/8 = 2M counts/s).
** ISR kicks ADC conversion to maintain steady sensor sampling rate.
*/
static void timer1_init(void)
{
	TCCR1A = 0b00000000;
	TCCR1B = 0b00001001;

	TCNT1 = 0;

	OCR1A = 40000;

	TIMSK1 = (1 << OCIE1A);
}

/*
** Convert 4-bit value (0-15) to ASCII hex character (0-9, a-f).
** Used to format 8-bit ADC results as two hex chars for UART output.
*/
static void	get_decimal(volatile uint8_t *buf, uint16_t val)
{
	if (val > 1023)
		val = 1023;
	if (val >= 1000)
		buf[0] = '0' + (val / 1000);
	if (val >= 100)
		buf[1] = '0' + ((val / 100) % 10);
	if (val >= 10)
		buf[2] = '0' + ((val / 10) % 10);
	buf[3] = '0' + (val % 10);
}

void	__vector_21(void) __attribute__((signal, used));
void	__vector_19(void) __attribute__((signal, used));
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
** Reads ADC (10-bit right-adjusted result) in ~2 CPU cycles.
** Converts to decimal chars + CRLF frame (\r\n) via uart_tx_push_buf().
** Drop policy: sample lost if TX queue cannot fit 6 bytes (real-time first).
*/
void	__vector_21(void) // ADC conversion complete ISR
{
	uint16_t res = ADC - Tos; // Read ADC value (10-bit)

	get_decimal(adc_dec, res);
	uart_tx_push_buf(adc_dec, 6);
}

/*
** USART Data Register Empty interrupt: drain TX queue to hardware.
** Called each time UDR0 becomes ready (every ~87 µs @ 115200 baud).
** Sends one byte per ISR; auto-disables UDRIE0 when queue empty.
** Paces output at UART bitrate with zero busy-waiting.
*/
void	__vector_19(void) // USART data register empty ISR
{
	if (tx_head == tx_tail)
	{
		UCSR0B &= ~(1 << UDRIE0);
		return ;
	}
	if (tx_buf[tx_tail])
		UDR0 = tx_buf[tx_tail];
	tx_tail = tx_next(tx_tail);
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
	uint16_t	ubrr = F_CPU / (8 * BAUD) - 1;

	uart_init(ubrr);
	timer1_init();
	adc_init();

	local_sei();

	while (1)
		;
}
