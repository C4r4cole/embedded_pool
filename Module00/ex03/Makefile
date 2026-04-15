# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/14 13:58:00 by fmoulin           #+#    #+#              #
#    Updated: 2026/04/14 19:45:35 by fmoulin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

F_CPU = 16000000UL # en face de XTAL1 & XTAL2 sur le schema du DevKit 16Mhz
MCU = atmega328p # c'est la cible, le microcontroller, la puce
PROGRAMMER = arduino # c'est l'interface entre le PC et la puce qui permet la communication entre les 2
PORT = /dev/ttyUSB0 # ls /dev/tty*
BAUD = 115200

all: hex flash

hex:
	avr-gcc -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) main.c -o main.bin
	avr-objcopy -O ihex main.bin main.hex

flash:
	avrdude -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:main.hex

clean:
	rm -f main.hex main.bin