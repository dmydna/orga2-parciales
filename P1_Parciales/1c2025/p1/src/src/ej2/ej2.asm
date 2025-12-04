extern strcmp
global invocar_habilidad

; Completar las definiciones o borrarlas (en este ejercicio NO serán revisadas por el ABI enforcer)

DIRENTRY_NAME_OFFSET EQU 0
DIRENTRY_PTR_OFFSET EQU 16
DIRENTRY_SIZE EQU 24

FANTASTRUCO_DIR_OFFSET EQU 0
FANTASTRUCO_ENTRIES_OFFSET EQU 8
FANTASTRUCO_ARCHETYPE_OFFSET EQU 16
FANTASTRUCO_FACEUP_OFFSET EQU 24
FANTASTRUCO_SIZE EQU 32

PTR_SIZE EQU 8
NULL EQU 0

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

section .text

; void invocar_habilidad(void* carta, char* habilidad);
invocar_habilidad:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = void*    card ; Vale asumir que card siempre es al menos un card_t*
	; r/m64 = char*    habilidad

	;rdi -> carta
	;rsi -> habilidad
	
	push rbp 
	mov rbp, rsp

	push r12
	push r13
	push r14
	push r15

	mov r12, [rdi + FANTASTRUCO_DIR_OFFSET]
	xor r13, r13
	mov r13w, word [rdi + FANTASTRUCO_ENTRIES_OFFSET]

	mov r14, rdi
	mov r15, rsi

	.while: 
		cmp r13, 0
		je .no_hay_mas_entradas

		mov rdi, r15
		mov rsi, [r12]

		call strcmp

		cmp rax, 0
		jz .encontrada

		dec r13
		add r12, 8
		jmp .while

	.encontrada:

		mov r8, [r12]
		mov r9, [r8 + DIRENTRY_PTR_OFFSET]

		mov rdi, r15
		mov rsi, r14
		call r9
		jmp .fin

	.no_hay_mas_entradas:
	
		mov r10, [rdi + FANTASTRUCO_ARCHETYPE_OFFSET]
		cmp 10, 0
		jz .fin
		mov rdi, r10
		mov rsi, r15
		call invocar_habilidad 


	end:

	pop r12
	pop r13
	pop r14
	pop r15

	pop rbp
	ret ;No te olvides el ret!
