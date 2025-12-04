section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el filtro
ALIGN 16


packed_128: times 4 dd 128, 128, 128, 128
packed_32: times  4 dd 32, 32, 32, 32
packed_255: times 4 dd 255, 255, 255, 255

section .text

; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 2A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej2a
global EJERCICIO_2A_HECHO
EJERCICIO_2A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 2B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej2b
global EJERCICIO_2B_HECHO
EJERCICIO_2B_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 2C (opcional) como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej2c
global EJERCICIO_2C_HECHO
EJERCICIO_2C_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Dada una imagen origen ajusta su contraste de acuerdo a la parametrización
; provista.
;
; Parámetros:
;   - dst:    La imagen destino. Es RGBA (8 bits sin signo por canal).
;   - src:    La imagen origen. Es RGBA (8 bits sin signo por canal).
;   - width:  El ancho en píxeles de `dst`, `src` y `mask`.
;   - height: El alto en píxeles de `dst`, `src` y `mask`.
;   - amount: El nivel de intensidad a aplicar.
global ej2a
ej2a:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi /m64 = rgba_t*  dst
	; rsi /m64 = rgba_t*  src
	; rdx /m32 = uint32_t width
	; rcx/m32 = uint32_t height
	; r8/m8  = uint8_t  amount

	push rsp
	mov rbp, rsp
	cvtsi2ss xmm4, r8

	mov rax, rdx
	mul rcx ; height * width

	.while:
		cmp rax, 0
		je .end

		movdqu xmm0, [rsi] ;

		movdqu xmm1, xmm0
		movdqu xmm2, xmm0
		movdqu xmm3, xmm0

		; Extrae valores rojos
		pmovzxbd xmm1, xmm1

		; Extrae valores verdes
		psrld xmm2, 8
		pmovzxbd xmm2, xmm2   ; uint8_t -> uint32_t

		; Extrae valores azules
		psrld xmm3, 16
		pmovzxbd xmm3, xmm3  ; uint8_t -> uint32_t

		; Empaqueto Contraste
		PACKUSWB xmm4, xmm4 ;
		pmovzxbd xmm4, xmm4
		
		; resto 128 a canales packed
		PSUBD xmm1, [packed_128]
		PSUBD xmm2, [packed_128]
		PSUBD xmm3, [packed_128]


		; convierto enteros a floats
		CVTDQ2PS xmm1, xmm1
		CVTDQ2PS xmm2, xmm2
		CVTDQ2PS xmm3, xmm3

		; Multiplo por contraste , divido 32 y sumo 128 
		MULPS xmm1, xmm4
		DIVPS xmm1, [packed_32]
		ADDPS xmm1, [packed_128]

		MULPS xmm2, xmm4
		DIVPS xmm3, [packed_32]
		ADDPS xmm3, [packed_128]

		MULPS xmm3, xmm4
		DIVPS xmm3, [packed_32]
		ADDPS xmm3, [packed_128]

		; convierto float a enteros
		CVTPS2DQ xmm1, xmm1
		CVTPS2DQ xmm2, xmm2
		CVTPS2DQ xmm3, xmm3

		; SATURACION ::
		pxor xmm0, xmm0 ; packed_0
		movdqu xmm6, [packed_255]

		; satura rojos
		pmaxud xmm0, xmm1     
		pminud xmm0, xmm6     
		movdqu xmm1, xmm0
		; satura verdes
		pxor xmm0, xmm0
		pmaxud xmm0, xmm2     
		pminud xmm0, xmm6     
		movdqu xmm2, xmm0
		; satura azules
		pxor xmm0, xmm0
		pmaxud xmm0, xmm3     
		pminud xmm0, xmm6     
		movdqu xmm3, xmm0

		; EMPAQUETO TODO en RGBA
		por xmm1, xmm2
		por xmm1, xmm3
		por xmm1, [packed_255]


		; guardo cambios
		movdqu [rsi], xmm1


		add rdi, 16       ; dst ++
		add rsi, 16       ; src ++

		sub rax, 4
		jmp .while

	.end:
	mov rsp, rbp
	pop rbp
	ret






; Dada una imagen origen ajusta su contraste de acuerdo a la parametrización
; provista.
;
; Parámetros:
;   - dst:    La imagen destino. Es RGBA (8 bits sin signo por canal).
;   - src:    La imagen origen. Es RGBA (8 bits sin signo por canal).
;   - width:  El ancho en píxeles de `dst`, `src` y `mask`.
;   - height: El alto en píxeles de `dst`, `src` y `mask`.
;   - amount: El nivel de intensidad a aplicar.
;   - mask:   Una máscara que regula por cada píxel si el filtro debe o no ser
;             aplicado. Los valores de esta máscara son siempre 0 o 255.
global ej2b
ej2b:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = rgba_t*  dst
	; r/m64 = rgba_t*  src
	; r/m32 = uint32_t width
	; r/m32 = uint32_t height
	; r/m8  = uint8_t  amount
	; r/m64 = uint8_t* mask

	ret

; [IMPLEMENTACIÓN OPCIONAL]
; El enunciado sólo solicita "la idea" de este ejercicio.
;
; Dada una imagen origen ajusta su contraste de acuerdo a la parametrización
; provista.
;
; Parámetros:
;   - dst:     La imagen destino. Es RGBA (8 bits sin signo por canal).
;   - src:     La imagen origen. Es RGBA (8 bits sin signo por canal).
;   - width:   El ancho en píxeles de `dst`, `src` y `mask`.
;   - height:  El alto en píxeles de `dst`, `src` y `mask`.
;   - control: Una imagen que que regula el nivel de intensidad del filtro en
;              cada píxel. Es en escala de grises a 8 bits por canal.
global ej2c
ej2c:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = rgba_t*  dst
	; r/m64 = rgba_t*  src
	; r/m32 = uint32_t width
	; r/m32 = uint32_t height
	; r/m64 = uint8_t* control

	ret
