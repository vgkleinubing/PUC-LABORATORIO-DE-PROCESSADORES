#************************************************
# Monitor LPC2378
# Marcos Augusto Stemmer
#*************************************************/

#Configuracoes especificas deste projeto
SERIALDEV = /dev/ttyS0
CLOCKFREQ = 12000
TARGET = arm02
MODULOS =  main.o crt.o
TERMINAL = ltser
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

#Chama o linker/loader para juntar os módulos gerando o arquivo elf
$(TARGET).elf: $(MODULOS)
	$(LD) $(LFLAGS) -Tlpc2378_flash.ld -o $(TARGET).elf $(MODULOS)

$(TARGET)r.elf: $(MODULOS)
	$(LD) $(LFLAGS) -Tlpc2378_ram.ld -o $(TARGET)r.elf $(MODULOS)

#Chama o terminal e executa o programa na RAM (necessita do mon23)
tser: $(TARGET)r.hex
	$(TERMINAL) $(SERIALDEV) b=$(BAUDRATE) $(TARGET)r.hex

# Use 'make isp' para programar a memoria flash
isp: $(TARGET).hex
	lpc21isp $(TARGET).hex $(SERIALDEV) $(BAUDRATE) $(CLOCKFREQ)

#Limpa, apagando os arquivos gerados pela compilacao
clean:
	-rm -f *.o *.elf *~ *.bin *.map *.hex
