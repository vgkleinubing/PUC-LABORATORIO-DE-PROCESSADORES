#************************************************
# Monitor LPC2378
# Marcos Augusto Stemmer
#*************************************************/

#Configuracoes especificas deste projeto
SERIALDEV = /dev/ttyS0
CLOCKFREQ = 12000
TARGET  = tmp100
MODULOS = lcd.o i2c.o crt.o
HEADERS = lcd.h
TERMINAL = ltser
BAUDRATE = 19200

#Nome do compilador C, assembler e linker
CC      = arm-elf-gcc
AS      = arm-elf-as
LD      = arm-elf-ld
#AFLAGS  = -ahls -mapcs-32 -mcpu=arm7tdmi -o crt.o
AFLAGS  =-mapcs-32 -mcpu=arm7tdmi
CFLAGS  = -Wall -O2 -mcpu=arm7tdmi-s -D BAUDRATE=$(BAUDRATE)
LFLAGS  =-Tlpc2378_flash.ld

all: $(TARGET).hex

#Converte arquivo elf para hex
%.hex: %.elf
	arm-elf-objcopy -O ihex $< $@

#Compila o main.c para executar na RAM
mainr.o: main.c $(HEADERS)
	$(CC) -c $(CFLAGS) -D RAM_RUN -o mainr.o main.c

#Chama o compilador c
%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) -o $@ $<

#Compila os modulos em assembly
%.o: %.s
	$(AS) $(AFLAGS) -o $@ $<

#Chama o linker/loader para juntar os módulos gerando o arquivo elf
$(TARGET).elf: $(MODULOS) main.o
	$(LD) $(LFLAGS) -o $(TARGET).elf $(MODULOS) main.o

$(TARGET)r.elf: $(MODULOS) mainr.o
	$(LD) -Tlpc2378_ram.ld -o $(TARGET)r.elf $(MODULOS) mainr.o

tser: $(TARGET)r.hex
	$(TERMINAL) $(SERIALDEV) b=$(BAUDRATE) $(TARGET)r.hex

# Use 'make isp' para programar a memoria flash
isp: $(TARGET).hex
	lpc21isp $(TARGET).hex $(SERIALDEV) $(BAUDRATE) $(CLOCKFREQ)

#Limpa, apagando os arquivos gerados pela compilacao
clean:
	-rm -f *.o *.elf *~ *.bin *.map *.hex
