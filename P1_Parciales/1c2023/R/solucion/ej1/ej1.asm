global acumuladoPorCliente_asm
global en_blacklist_asm
global blacklistComercios_asm


section .data

PAGO_CLIENTE_OFFSET  EQU  0
PAGO_MONTO_OFFSET    EQU  8
PAGO_APROBADO_OFFSET EQU  16
PAGO_SIZE            EQU  24



;########### SECCION DE TEXTO (PROGRAMA)
section .text


acumuladoPorCliente_asm:
	; rdi -> uint8_t cantidadPagos
	; rsi -> pagos_t* arr_pagos

	push rbp
	mov rbp, rsp
	
	push r12
	push r13
	push r14
	push r15

	mov r12, rdi ; cantidadPagos
	mov r13, rsi ; arr_pagos
	xor r14, r14 ;pagosAprobadosPorCliente
	xor r15, r15 ; iterador

	mov rdi, 10
	imul rdi, 4


	call malloc

	mov rax, r14 ;pagosAprobadosPorCliente

	mov rdi, r13
	mov rsi, 0
	mov rdx, 10
	imul rdx, 4

	call memset 

	.while:
		cmp r15, r12; es cantidadPagos == iterador ?
		je .end

		movzx rsi, PAGO_SIZE
		imul rsi, r15; iterador * PAGO_SIZE
		mov rdi, [r13 + rsi]
		; arr_pagos[i].cliente
		movzx rdx, byte [rdi + PAGO_CLIENTE_OFFSET]
		; arr_pagos[i].aprobado
		movzx rcx, byte [rdi + PAGO_APROBADO_OFFSET]

		; pagosAprobadosPorCliente[cliente] += arr_pago[i].aprobado
		add [r14 + rdx * 4] , rcx

		inc r15; iterador ++
		jmp .while

	.end:

	mov rax, r14

	pop r15
	pop r14
	pop r13
	pop r12
	
	pop rbp
	ret

en_blacklist_asm:
	; rdi -> char* comercio
	; rsi -> char** lista_comercios
	; rdx -> uint8_t n
	push rbp
	mov rbp, rsp
	
	push r12
	push r13
	push r14
	push rbx


	mov r12, rdi; char* comercio
	mov r13, rsi; char** lista_comercios
	mov r14. rdx; uint8_t n


	xor rbx, rbx
	.while:
		cmp rbx, r14
		je .end

		mov rdi,[r13 + rbx*8]
		mov rsi, r12

		call strcmp

		cmp rax, 0
		jne .sigIteracion

		mov rax, 1
		jmp .end

		.sigIteracion:
		inc rbx
		xor rax, rax
		jmp .while

	.end:
	mov rbp


	pop rbx
	pop r14
	pop r13
	pop r12

	ret

blacklistComercios_asm:
	; rdi -> uint8_t cantidad_pagos
	; rsi -> pago_t* arr_pagos
	; rdx -> char** arr_comercios
	; uint8_t size_comercios

	push rbp
	mov rbp, rsp

	push r12
	push r13
	push r14
	push r15

	mov rdi, r12
	mov rsi, r13

	call malloc

	.while:
		cmp 

	.end:

	pop r15
	pop r14
	pop r13
	pop r12

	pop rbp
	ret
