/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:36:10 by fmoulin           #+#    #+#             */
/*   Updated: 2026/04/29 15:29:38 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdlib.h>

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
	// TWCR est le registre de controle de I2C (22.9.2 p 239)
	
	// TWINT
		// Pour effacer le flag TWINT, il faut le mettre a 1 (C'est contre intuitif !)
			// on appelle ca un : Write 1 to clear
	// TWEN
		// ce bit active l'interface TWI
	// TWSTA
		// START condition bit
		// pour devenir le master I2C
		// Un maître prend le bus et commence une communication.
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	// TWINT = 0 → le module travaille
	// TWINT = 1 → le module a fini, résultat prêt
	while (!(TWCR & (1 << TWINT)))
		;
}

void	i2c_stop(void)
{
	// TWSTO
		// STOP condition bit
		// Communication terminée, bus libéré.
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void	i2c_write(unsigned char data)
{
	// on stock la data dans TWDR qui est le registre de data (22.9.4 p 241)
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	// TWINT = 0 → le module travaille
	// TWINT = 1 → le module a fini, résultat prêt
	while (!(TWCR & (1 << TWINT)))
		;
}

void i2c_read(void)
{
}

uint8_t i2c_read_ack(void)
{
	// ici on active le flag acknowledgment
	// cela sert a dire "je suis pret a recevoir un autre bit"
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	return (TWDR);
}

uint8_t i2c_read_nack(void)
{
	// ici c'est la meme fonction read mais sans l'acknowledgment car on utilise cette fonction pour le dernier bit
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	return (TWDR);
}

void print_hex_value(uint8_t c)
{
	uint8_t high;
	uint8_t low;

	high = c >> 4;
	low = c & 0x0F;

	if (high >= 0 && high <= 9)
		uart_tx('0' + high);
	else if ( high >= 10 && high <= 15)
		uart_tx('A' + high - 10);

	if (low >= 0 && low <= 9)
		uart_tx('0' + low);
	else if ( low >= 10 && low <= 15)
		uart_tx('A' + low - 10);
}

void communication_init(void)
{
	// le premier write sert a dire a quelle adresse on parle
		// on decale l'adresse de 1 bit vers la gauche
		// car le dernier bit sert a dire si on write ou on read
			// 0 = write
			// 1 = read
	i2c_write(0x38 << 1); // on ne touche pas au dernier bit donc write
	i2c_write(0xAC); // cet octet est le Trigger Measurement command (lance un conversion maintenant) (etape 2)
	i2c_write(0x33); // bit de configuration impose par le constructeur (etape 2)
	i2c_write(0x00); // bit de configuration impose par le constructeur (etape 2)
}

void read_datas(uint8_t *data)
{
	// le premier write sert a dire a quelle adresse on parle
		// on decale l'adresse de 1 bit vers la gauche
		// car le dernier bit sert a dire si on write ou on read
			// 0 = write
			// 1 = read
	i2c_write((0x38 << 1) | 1); // on met le dernier bit a 1 donc read
	for (int i = 0; i < 6; i++)
		data[i] = i2c_read_ack();	// on recupere les 6 premiers octets avec aknowledgement
	data[6] = i2c_read_nack(); // on recupere le 7eme octet sans acknowledgment
}

void raw_calculation(float *humidity, float *temperature)
{
	uint8_t		data[7];
	uint32_t 	humidity_raw;

	uint32_t 	temperature_raw;

	
	read_datas(data);
	
	humidity_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4);
	*humidity = humidity_raw * 100.00 / 1048576.0;
	// 1048576.0 est le max sur 20 bits et l'humidite va de 0 a 100%
	
	temperature_raw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | ((uint32_t)data[5]);
	*temperature = temperature_raw * 200.00 / 1048576.0 - 50.0;
	// 1048576.0 est le max sur 20 bits et la temperature sur AHT20 va de -50.C a 150.C
	// l'amplitude totale est donc de 200.C
		// c'est pour ca qu'on multiplie par 200
		// et ensuite on decale de -50 (car amplitude -50.C a 150.C)
}

void average_calculation(float humidity, float temperature, float *hum_avg, float *temp_avg)
{
	static float		temp_hist[3] = {0, 0, 0};
	static float		hum_hist[3] = {0, 0, 0};
	static int			idx = 0;
	static int			count = 0;
	
	hum_hist[idx] = humidity;
	temp_hist[idx] = temperature;

	idx = (idx + 1) % 3;
	if (count < 3)
		count++;
	
	*hum_avg = 0;
	*temp_avg = 0;
	for (int i = 0; i < count; i++)
	{
		*hum_avg += hum_hist[i];
		*temp_avg += temp_hist[i];
	}
	
	*hum_avg /= count;
	*temp_avg /= count;
}

void display_data(float hum_avg, float temp_avg)
{
	char		buf[10];
	
	uart_printstr("Temperature: ");
	uart_printstr(dtostrf(temp_avg, 0, 0, buf));
	uart_printstr(".C, Humidity: ");
	uart_printstr(dtostrf(hum_avg, 0, 1, buf));
	uart_tx('%');
	uart_tx('\r');
	uart_tx('\n');
}

int	main(void)
{
	float		humidity;
	float		temperature;
	float		temp_avg = 0;
	float		hum_avg = 0;
	
	uart_init(16);
	i2c_init();
	while (1)
	{
		i2c_start();
		communication_init();
		i2c_stop();
		
		_delay_ms(80); // 80 ms d'attente imposees par le constructeur (etape 3)
		
		i2c_start();
		raw_calculation(&humidity, &temperature);
		average_calculation(humidity, temperature, &hum_avg, &temp_avg);
		display_data(hum_avg, temp_avg);
		i2c_stop();
		
		_delay_ms(80);
	}
}


// datasheet de l'AHT20
	// https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf
// le Sensor reading process est explique en page 8/11
// Les 7 octets donnes par l'AHT20 correspondent a :
// [status] [humidity byte1] [humidity byte2] [humidity/temp mix] [temp byte1] [temp byte2] [CRC]