extern malloc

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

ATTACKUNIT_CLASE_OFFSET EQU 0
ATTACKUNIT_COMBUSTIBLE_OFFSET EQU 12
ATTACKUNIT_REFERENCE_OFFSET EQU 14
ATTACKUNIT_SIZE EQU 16


COLS EQU 255
ROWS EQU 255



section .text
; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - optimizar
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - contarCombustibleAsignado
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1C como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - modificarUnidad
global EJERCICIO_1C_HECHO
EJERCICIO_1C_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

global optimizar
optimizar:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi r/m64 = mapa_t           mapa
	; rsi r/m64 = attackunit_t*    compartida
	; edx r/m64 = uint32_t*        fun_hash(attackunit_t*)

	push rbp
	mov rbp, rsp

	push r12 ; mapa
	push r13 ; compartida
	push r14 ; fun_hash

	push r15 ; hash_compartida
	push rbx ; index
	push rbx ; index

	xor rbx, rbx ;index = 0

	;hash_compatida 
	mov rdi, r13
	call r14
	mov r15, rax ; 

	.while:
	;unidad_actual
	mov rdi, [r12 + 8 * rbx]

	.esUnidadNula:
	cmp rdi, 0
	je .sigIteracion

	.esCompartida:
	call r14
	cmp rax, r15 	; hash_actual -> rax
	jne .sigIteracion


	inc BYTE[r13 + ATTACKUNIT_REFERENCE_OFFSET]
	mov rdi ,[r12 + 8 * rbx]
	dec BYTE[rdi + ATTACKUNIT_REFERENCE_OFFSET]
	mov [r12 + 8 * rbx], r13	
	
	.noTieneReferencias:
	cmp BYTE[rdi + ATTACKUNIT_REFERENCE_OFFSET], 0
	jne .sigIteracion
	call free 

	
	.sigIteracion:
	inc rbx
	cmp rbx, ROWS*COLS
	jl .while

	.end:
	pop rbx
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12

	pop rbp
	ret

global contarCombustibleAsignado
contarCombustibleAsignado:
	; rdi r/m64 = mapa_t           mapa
	; rsi r/m64 = uint16_t*        fun_combustible(char*)


	push rbp 
	mov rbp, rsp

	push r12 ; mapa
	push r13 ; fun_combustible
	push r14 ; res
	push rbx ; index


	mov r12, rdi
	mov r13, rsi 

	xor r14, r14
	xor rbx, rbx

	.while:
	mov rdi, [r12 + 8 * rbx ]

	.esUnidadNula:
	cmp r12, 0
	je .sigIteracion


	;COMBUSTIBLE BASE
	call r13
	movzx rax, ax

	;COMBUSTIBLE ACTUAL
	movzx rsi, WORD[rdi + ATTACKUNIT_COMBUSTIBLE_OFFSET]

	;COMBUSTIBLE UTILIZADO
	sub rax, rsi

	;COMOBUSTIBLE TOTAL
	mov r14, rax
	.sigIteracion:
	inc rbx
	cmp rbx, ROW*COLS
	jl .while
	.end:

	pop rbx
	pop r14
	pop r13
	pop r12


	pop rbp
	ret

global modificarUnidad
modificarUnidad:
	; rsi r/m64 = mapa_t           mapa
	; rdi r/m8  = uint8_t          x
	; rdx r/m8  = uint8_t          y
	; rcx r/m64 = void*            fun_modificar(attackunit_t*)

	push rbp
	mov rbp, rsp

	push r12
	push r13
	push r14
	push r15
	push rbx
	push rbx

	mov r12, rdi
	mov r13, rsi
	mov r14, rdx
	mov r15, rcx


	;[Matriz + (x*COLS + y) * size] 

	;x*COLS
	movzx r13, sil
	movzx r14, dl

	imul r13, COLS
	add r13, r14 ; + y

	;rbx -> (x*COLS + y)
	mov rbx, r13

	; UNIDAD_ACTUAL
	mov rdi, [r12 + 8 * rbx]

	.esUnidadNula:
	cmp rdi, 0
	je .end

	.estaOptimizada:
	mov rsi , [rdi + ATTACKUNIT_REFERENCE_OFFSET]
	cmp rsi, 1
	jnl .end

	dec BYTE [rdi + ATTACKUNIT_REFERENCE_OFFSET]

	; NUEVA_UNIDAD
	mov rdi, ATTACKUNIT_SIZE
	call malloc

	; NUEVA_UNIDAD = UNIDAD_ACTUAL
	mov rsi, [r12 + 8 * rbx] 
	
	; copia los bits 0 a 8
	mov rdi, [rsi]
	mov [rax], rdi
	; copia los bits 8 a 16
	mov rdi, [rsi + 8]
	mov [rax + 8], rdi
	
	mov [rax + ATTACKUNIT_REFERENCE_OFFSET], BYTE 1
	mov [r12 + 8 * rbx], rax

	; FUN_MODIFICAR(unidad_actual) 
	mov rdi, [r12 + 8 * rbx]
	call rcx

	.end:
	pop rbx
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12

	pop rbp
	ret