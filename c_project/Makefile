DEVICE  = atmega328

CLOCK   = 16000000

AVRDUDE = /home/soultaker/.arduino15/hardware/tools/avr/bin/avrdude -C/home/soultaker/.arduino15/hardware/tools/avr/etc/avrdude.conf -patmega328p -carduino -P/dev/ttyUSB9 -b57600 -D
OBJECTS = main.o

COMPILE = avr-gcc -Wall -Os -fexec-charset=cp1251 -std=c99 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf $(OBJECTS)

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
