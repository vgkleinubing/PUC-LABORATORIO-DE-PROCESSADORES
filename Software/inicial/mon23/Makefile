#************************************************
# Monitor LPC2378
# Marcos Augusto Stemmer
#*************************************************/

SERIALDEV = /dev/ttyS0
TERMINAL=ltser
CLOCKFREQ=12000
TARGET=monarm
MODULOS = main.o monitor.o lcd.o iap.o crt.o
HEADERS = monarm.h lpc23xx.h
BAUDRATE=19200

#Nome do compilador C, assembler e linker
CC      = arm-elf-gcc
LD      = arm-elf-gcc
AS	= arm-elf-as
AFLAGS  = -mapcs-32 -mcpu=arm7tdmi
CFLAGS  = -Wall -O2 -mcpu=arm7tdmi-s -D BAUDRATE=$(BAUDRATE)
LFLAGS  = -nostartfiles

all: $(TARGET).hex

#Converte arquivo elf para hex
%.hex: %.elf
	arm-elf-objcopy -O ihex $< $@

#Chama o compilador c
%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) -o $@ $<

#Compila os modulos em assembly
%.o: %.S
	$(AS) $(AFLAGS) -o $@ $<

$(TARGET).elf: $(MODULOS)
	$(LD) -Tlpc2378_flash.ld $(LFLAGS) -o $(TARGET).elf $(MODULOS)

monr.elf: $(MODULOS)
	$(LD) -Tlpc2378_ram.ld $(LFLAGS) -o monr.elf $(MODULOS)

ram: monr.hex

tser: monr.hex
	ltser $(SERIALDEV) b=$(BAUDRATE) monr.hex

# Use 'make isp' para programar a memoria flash
isp: $(TARGET).hex
	lpc21isp $(TARGET).hex $(SERIALDEV) $(BAUDRATE) $(CLOCKFREQ)

clean:
	-rm -f *.o *.elf *~ *.bin *.map *.hex
