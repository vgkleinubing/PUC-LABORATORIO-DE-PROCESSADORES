
/******************************************************
crt.s
Este modulo em assembly tem os vetores de interrupcao
e os procedimentos de inicializacao
***************************************************** */
/* Tamanhos das pilhas */
.set  UND_STACK_SIZE, 0x00000004	/* 4 bytes para Undefined Instruction */
.set  ABT_STACK_SIZE, 0x00000004	/* 4 bytes para "abort"	*/
.set  FIQ_STACK_SIZE, 0x00000040	/* 64 bytes de pilha para "FIQ" */
.set  IRQ_STACK_SIZE, 0X00000040	/* 64 bytes de pilha para "IRQ" */
.set  SVC_STACK_SIZE, 0x00000040	/* 64 bytes de pilha no modo supervisor	*/

/* Padroes de bits de Modo e Interrupcao (I & F) nos PSRs
 (program status registers) */
.set  MODE_USR,	0x10	/* Normal User Mode				*/
.set  MODE_FIQ,	0x11	/* FIQ Processing Fast Interrupts Mode		*/
.set  MODE_IRQ,	0x12	/* IRQ Processing Standard Interrupts Mode	*/
.set  MODE_SVC,	0x13	/* Supervisor Processing Software Interrupts Mode */
.set  MODE_ABT,	0x17	/* Abort Processing memory Faults Mode	*/
.set  MODE_UND,	0x1B	/* Undefined Processing	Undefined Instructions Mode	*/
.set  MODE_SYS,	0x1F	/* System Running Priviledged Operating	System Tasks  Mode	*/

.set  I_BIT, 0x80	/* when	I bit is set, IRQ is disabled (program status registers) */
.set  F_BIT, 0x40	/* when	F bit is set, FIQ is disabled (program status registers) */
.set  SORP, 0x40000200

.text
.arm

.global	Reset_Handler
.global	_startup
.func	_startup

_startup:

# Exception Vectors

_vectors:	ldr	PC, Reset_Addr
		ldr	PC, Undef_Addr
		ldr	PC, SWI_Addr
		ldr	PC, PAbt_Addr
		ldr	PC, DAbt_Addr
		nop	/* Reservado para a soma (checksum) dos vetores	*/
		ldr	PC, [PC,#-0x0120]	
/* see page 71 of "Insiders Guide to the Philips ARM7-Based Microcontrollers" by Trevor	Martin	*/
		ldr	PC, FIQ_Addr

Reset_Addr:	.word	Reset_Handler	/* definido neste modulo */
Undef_Addr:	.word	UNDEF_Routine	/* definido no main.c  */
SWI_Addr:	.word	SWI_Routine	/* definido no main.c  */
PAbt_Addr:	.word	UNDEF_Routine	/* definido no main.c  */
DAbt_Addr:	.word	UNDEF_Routine	/* definido no main.c  */
FIQ_Addr:	.word	FIQ_Routine	/* definido no main.c  */
		.word	0	/* Arrdonda para um total de 16 vetores  */

Reset_Handler:
/* MEMMAP = 1 Vetores na Flash a partir de 0x00000000		*/
/* MEMMAP = 2 Vetores remapeados para a RAM em 0x40000000	*/
		ldr	r0, = 1		/*Vetores na Flash */
		ldr	r1, = 0xE01FC040	/* MEMMAP	*/
		str	r0, [r1]

/* Configura a pilha para cada modo de operacao. */
		ldr	r0, =_stack_end
		msr	CPSR_c,	#MODE_UND|I_BIT|F_BIT	/* Undefined Instruction Mode  */
		mov	sp, r0
		sub	r0, r0,	#UND_STACK_SIZE
		msr	CPSR_c,	#MODE_ABT|I_BIT|F_BIT	/* Abort Mode */
		mov	sp, r0
		sub	r0, r0,	#ABT_STACK_SIZE
		msr	CPSR_c,	#MODE_FIQ|I_BIT|F_BIT	/* FIQ Mode */
		mov	sp, r0	
		sub	r0, r0,	#FIQ_STACK_SIZE
		msr	CPSR_c,	#MODE_IRQ|I_BIT|F_BIT	/* IRQ Mode */
		mov	sp, r0
		sub	r0, r0,	#IRQ_STACK_SIZE
		msr	CPSR_c,	#MODE_SVC|I_BIT|F_BIT	/* Supervisor Mode */
		mov	sp, r0
		sub	r0, r0,	#SVC_STACK_SIZE
		msr	CPSR_c,	#MODE_SYS|I_BIT|F_BIT	/* User	Mode */
		mov	sp, r0
/* copy .kex section (Copy from ROM to RAM) */
		ldr	R1, =_etext
		ldr	R2, =_kex
		ldr	R3, =_ekex
7:		cmp	R2, R3
		ldrlo	R0, [R1], #4
		strlo	R0, [R2], #4
		blo	7b

/* copy .data section (Copy from ROM to RAM) */
		ldr	R1, =_ekex
		ldr	R2, =_data
		ldr	R3, =_edata
1:		cmp	R2, R3
		ldrlo	R0, [R1], #4
		strlo	R0, [R2], #4
		blo	1b

/* Clear .bss section (Zero init)  */
		mov	R0, #0
		ldr	R1, =_bss_start
		ldr	R2, =_bss_end
2:		cmp	R1, R2
		strlo	R0, [R1], #4
		blo	2b

/******** Configura clock (Acrescentado por Marcos Stemmer)*/
		ldr	r3, = 0xE01FC100
		mov	r2, #0x21	/* Liga clock 12M */
		str	r2, [r3, #0xa0]	/* SCS = 0x21	*/
Lp2:
		ldr	r2, [r3, #0xa0]	/* Espera estabilizar		*/
		ands	r2, r2, #0x40	/* while(SCS & 0x40);		*/
		beq	Lp2
		mov	r2, #1		/* Usa clock 12M para a CPU	*/
		str	r2, [r3, #0x0c]	/* CLKSRCSEL = 1;		*/
		mov	r2, #0		/* Divisor por 1 na CPU 	*/
		str	r2, [r3, #0x04]	/* CCLKCFG=0;			*/
/* Divisor por 1 nos perifericos */
		ldr	r2, = 0x55555555
		str	r2, [r3, #0xA8]	/* PCLKSEL0 = 0x55555555;	*/
		str	r2, [r3, #0xAC] /* PCLKSEL1 = 0x55555555;	*/
/* Configura o MAM */
		sub	r3,r3,#100
		mov	r2, #0
		str	r2, [r3, #0]	/* MAMCR=0 */
		mov	r2, #3
		str	r2, [r3, #4]	/* MAMTIM=3 */
		mov	r2, #2
		str	r2, [r3, #0]	/* MAMCR=2 */
/****** Fim da configuracao do clock *******/

/* Entra na funcao main() do programa em C  */
		LDR	PC, =main

.endfunc
.end
