#************************************************
# Makefile LPC2378 - Labproc 2009-2
# Marcos Augusto Stemmer
#*************************************************/

#Configuracoes especificas deste projeto
SERIALDEV = /dev/ttyS0
CLOCKFREQ = 12000
TARGET = a08smcard
MODULOS = uart.o lcd.o i2c2.o suporte.o crt.o
HEADERS = vic_cpsr.h lcd.h
TERMINAL = ltser
BAUDRATE = 19200

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

#Compila o main.c para executar na RAM
mainr.o: main.c $(HEADERS)
	$(CC) -c $(CFLAGS) -D RAM_RUN -o mainr.o main.c

#Chama o linker/loader para juntar os módulos gerando o arquivo elf
$(TARGET).elf: $(MODULOS) main.o
	$(LD) $(LFLAGS) -Tlpc2378_flash.ld -o $(TARGET).elf $(MODULOS) main.o

$(TARGET)r.elf: $(MODULOS) mainr.o
	$(LD) $(LFLAGS) -Tlpc2378_ram.ld -o $(TARGET)r.elf $(MODULOS) mainr.o

#Chama o terminal e executa o programa na RAM (necessita do mon23)
tser: $(TARGET)r.hex
	$(TERMINAL) $(SERIALDEV) b=$(BAUDRATE) $(TARGET)r.hex

# Use 'make isp' para programar a memoria flash
isp: $(TARGET).hex
	lpc21isp $(TARGET).hex $(SERIALDEV) $(BAUDRATE) $(CLOCKFREQ)

#Limpa, apagando os arquivos gerados pela compilacao
clean:
	-rm -f *.o *.elf *~ *.bin *.map *.hex
