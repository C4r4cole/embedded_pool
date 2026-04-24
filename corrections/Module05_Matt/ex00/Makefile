# _____________             _____                               
# ___  __ \__(_)_______________(_)___________                   
# __  /_/ /_  /__  ___/  ___/_  /__  __ \  _ \                  
# _  ____/_  / _(__  )/ /__ _  / _  / / /  __/                  
# /_/     /_/  /____/ \___/ /_/  /_/ /_/\___/                   
#                                                               
# __________          ______      _________________    _________
# ___  ____/______ ______  /____________  /_____  /__________  /
# __  __/  __  __ `__ \_  __ \  _ \  __  /_  __  /_  _ \  __  / 
# _  /___  _  / / / / /  /_/ /  __/ /_/ / / /_/ / /  __/ /_/ /  
# /_____/  /_/ /_/ /_//_.___/\___/\__,_/  \__,_/  \___/\__,_/  

# === Controller ===
MCU			= atmega328p
F_CPU		= 16000000UL
BAUD		= 115200

# === Compiler and Flags ===
CC			= avr-gcc
CFLAGS		= -Wall -Wextra -Werror -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Os

# === avrdude ===
PROGRAMMER	= arduino
PORT		= /dev/ttyUSB0

# === Files ===
BIN			= main.bin
HEX			= main.hex

# === Rules ===
all: hex flash

hex: $(HEX)

$(HEX): $(BIN)
	avr-objcopy -O ihex -R .eeprom $(BIN) $(HEX)

$(BIN): main.c
	$(CC) $(CFLAGS) -o $(BIN) main.c

flash: $(HEX)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX):i

screen:
	screen $(PORT) $(BAUD)

clean:
	rm -f $(BIN) $(HEX)

.PHONY: all hex flash screen clean
