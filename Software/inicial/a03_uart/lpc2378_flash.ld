
/* Identifica o ponto de entrada  */

ENTRY(_startup)

/* Especifica as areas de memoria do LPC2378  */
MEMORY 
{
flash		: ORIGIN = 0,          LENGTH = 524288	/* FLASH ROM	*/
ram		: ORIGIN = 0x40000000, LENGTH = 32736	/* Memoria RAM */
ram_isp_high(A)	: ORIGIN = 0x40007FE0, LENGTH = 32	/* Variaveis usadas pelo Boot Loader da ROM*/
}

/* define a global symbol _stack_end  */
_stack_end = 0x40007EDC;

/* now define the output sections  */
SECTIONS 
{
	. = 0;	/* set location counter to address zero  */
	
	startup : { *(.startup)
		    _estartup = .;
	} >flash	/* the startup code goes into FLASH */
	
	

	.text :		/* collect all sections that should go into FLASH after startup  */ 
	{
		_text = .;
		crt.o(.text)		/* all .text sections (code)  */
		crt.o(.rodata)		/* all .rodata sections (constants, strings, etc.)  */
		crt.o(.rodata*)		/* all .rodata* sections (constants, strings, etc.)  */
		crt.o(.glue_7)		/* all .glue_7 sections  (no idea what these are) */
		crt.o(.glue_7t)		/* all .glue_7t sections (no idea what these are) */
		_etext = .;		/* define a global symbol _etext just after the last code byte */
	} >flash 			/* put all the above into FLASH */

	.kex :
	{
		_kex = .;
		*(.text)
		*(.rodata)
		*(.rodata*)
		*(.glue_7)
		*(.glue_7t)
		_ekex = .;
	} >ram AT >flash

	.data :			/* As secçoes .data sao dados inicializados  */ 
	{
		_data = .;	/* O simbolo _data marca o inicio da secção .data */
		*(.data)	/* all .data sections  */
		_edata = .;	/* _edata marca o fim de .data section  */
	} >ram AT >flash	/* put all the above into RAM (but load the LMA copy into FLASH) */

	.bss :			/* collect all uninitialized .bss sections that go into RAM  */
	{
		_bss_start = .;	/* define a global symbol marking the start of the .bss section */
		*(.bss)		/* all .bss sections  */
	} >ram			/* put all the above in RAM (it will be cleared in the startup code */

	. = ALIGN(4);		/* advance location counter to the next 32-bit boundary */
	_bss_end = . ;		/* define a global symbol marking the end of the .bss section */
}
	_end = .;		/* define a global symbol marking the end of application RAM */
