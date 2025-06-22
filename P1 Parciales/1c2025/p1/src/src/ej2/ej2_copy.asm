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


	xor r15, r15 	;i=0
	while: 

	cmp rdi, NULL
	je end
	
	; directory_entry_list
	mov r14, [rdi + FANTASTRUCO_DIR_OFFSET]
	; directory_len
	mov r13, [rdi + FANTASTRUCO_ENTRIES_OFFSET] 

	for: 
		push rdi
		push rsi
	
		cmp r15, r13 ; i ?= directory_len
		je skip
		 
		mov rdi, r14
		mov rsi, rsi
		call strcmp

		cmp rax, NULL
		jne endfor
		
		pop rsi
		pop rdi

		call [r14 + DIRENTRY_PTR_OFFSET]

		jmp end

	 	endfor:   
		pop rsi
		pop rdi


		inc r14

	 jmp for

	skip:
	;carta = carta->__archetype;
	mov rdi, [rdi + FANTASTRUCO_ARCHETYPE_OFFSET]
	jmp while

	end:

	pop r12
	pop r13
	pop r14
	pop r15

	pop rbp
	ret ;No te olvides el ret!
