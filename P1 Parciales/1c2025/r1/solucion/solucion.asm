; Definiciones comunes
; Definiciones comunes
TRUE  EQU 1
FALSE EQU 0

; Identificador del jugador rojo
JUGADOR_ROJO EQU 1
; Identificador del jugador azul
JUGADOR_AZUL EQU 2

; Ancho y alto del tablero de juego
tablero.ANCHO EQU 10
tablero.ALTO  EQU 5

; Marca un OFFSET o SIZE como no completado
; Esto no lo chequea el ABI enforcer, sirve para saber a simple vista qué cosas
; quedaron sin completar :)
NO_COMPLETADO EQU -1

extern strcmp

;########### ESTOS SON LOS OFFSETS Y TAMAÑO DE LOS STRUCTS
; Completar las definiciones (serán revisadas por ABI enforcer):
carta.en_juego EQU 0
carta.nombre   EQU 1    ; Warning!: Nombre es array[12], no es un puntero, para leerlo uso 'lea' no 'mov'
carta.vida     EQU 14
carta.jugador  EQU 16
carta.SIZE     EQU 18

tablero.mano_jugador_rojo EQU 0
tablero.mano_jugador_azul EQU 8
tablero.campo             EQU 16
tablero.SIZE              EQU (16 + tablero.ANCHO*tablero.ALTO*8) ; Warning!: la matriz no es un puntero si un array hard-codeado.

accion.invocar   EQU 0
accion.destino   EQU 8
accion.siguiente EQU 16
accion.SIZE      EQU 24

; Variables globales de sólo lectura
section .rodata

; Marca el ejercicio 1 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - hay_accion_que_toque
global EJERCICIO_1_HECHO
EJERCICIO_1_HECHO: db TRUE

; Marca el ejercicio 2 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - invocar_acciones
global EJERCICIO_2_HECHO
EJERCICIO_2_HECHO: db TRUE

; Marca el ejercicio 3 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - contar_cartas
global EJERCICIO_3_HECHO
EJERCICIO_3_HECHO: db TRUE

section .text

; Dada una secuencia de acciones determinar si hay alguna cuya carta tenga un
; nombre idéntico (mismos contenidos, no mismo puntero) al pasado por
; parámetro.
;
; El resultado es un valor booleano, la representación de los booleanos de C es
; la siguiente:
;   - El valor `0` es `false`
;   - Cualquier otro valor es `true`
;
; ```c
; bool hay_accion_que_toque(accion_t* accion, char* nombre);
; ```
global hay_accion_que_toque
hay_accion_que_toque:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi/m64 = accion_t*  accion
	; rsi/m64 = char*      nombre

	push rbp
	mov rbp, rsp

	push r12
	push r13
	push r14
	push r15

	mov r14, rdi ; accion
	mov r15, rsi ; nombre

	.while:
	cmp r14, 0
	je .end

	; R14 -> Accion
	; R13 -> Carta

	mov  r13, [r14 + accion.destino]

	lea  rdi, [r13 + carta.nombre]
	mov  rsi, r15
	call strcmp
	cmp rax, 0
	jne .continue

	mov rax, 1
	jmp .exit

	.continue:
	mov r14, [r14 + accion.siguiente]
	jmp .while
	
	.end:
	xor rax, rax

	.exit:
	pop r15
	pop r14
	pop r13
	pop r12

	pop rbp

	ret

; Invoca las acciones que fueron encoladas en la secuencia proporcionada en el
; primer parámetro.
;
; A la hora de procesar una acción esta sólo se invoca si la carta destino
; sigue en juego.
;
; Luego de invocar una acción, si la carta destino tiene cero puntos de vida,
; se debe marcar ésta como fuera de juego.
;
; Las funciones que implementan acciones de juego tienen la siguiente firma:
; ```c
; void mi_accion(tablero_t* tablero, carta_t* carta);
; ```
; - El tablero a utilizar es el pasado como parámetro
; - La carta a utilizar es la carta destino de la acción (`accion->destino`)
;
; Las acciones se deben invocar en el orden natural de la secuencia (primero la
; primera acción, segundo la segunda acción, etc). Las acciones asumen este
; orden de ejecución.
;
; ```c
; void invocar_acciones(accion_t* accion, tablero_t* tablero);
; ```
global invocar_acciones
invocar_acciones:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = accion_t*  accion
	; r/m64 = tablero_t* tablero
	push rbp
	mov rbp, rsp


	push r12
	push r13
	push r14
	push r15


	mov r14, rdi ; accion
	mov r15, rsi ; tablero



	.while:

	; R14 -> ACCION
	; R13 -> CARTA
	; R15 -> TABLERO

	cmp r14, 0
	je .end

	mov r13, [r14 +  accion.destino]
	cmp [r13 +  carta.en_juego], byte 1
	jne .continue

	mov rcx, [r14 + accion.invocar]

	mov rdi, r15
	mov rsi, r13

	call rcx

	cmp [r13 + carta.vida], word 0
	jne .continue
	
	mov [r13 + carta.en_juego], word 0

	.continue:
	mov r14, [r14 + accion.siguiente]
	jmp .while
	
	.end:
	pop r15
	pop r14
	pop r13
	pop r12

	pop rbp

	ret
; Cuenta la cantidad de cartas rojas y azules en el tablero.
;
; Dado un tablero revisa el campo de juego y cuenta la cantidad de cartas
; correspondientes al jugador rojo y al jugador azul. Este conteo incluye tanto
; a las cartas en juego cómo a las fuera de juego (siempre que estén visibles
; en el campo).
;
; Se debe considerar el caso de que el campo contenga cartas que no pertenecen
; a ninguno de los dos jugadores.
;
; Las posiciones libres del campo tienen punteros nulos en lugar de apuntar a
; una carta.
;
; El resultado debe ser escrito en las posiciones de memoria proporcionadas
; como parámetro.
;
; ```c
; void contar_cartas(tablero_t* tablero, uint32_t* cant_rojas, uint32_t* cant_azules);
; ```
global contar_cartas
contar_cartas:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi/m64 = tablero_t* tablero
	; rsi/m64 = uint32_t*  cant_rojas
	; rdx/m64 = uint32_t*  cant_azules

	push rbp
	mov rbp, rsp

	mov dword [rsi], 0
	mov dword [rdx], 0

	.isNull:
	cmp rdi, 0
	je .end

	xor rbx, rbx
	lea r10, [rdi + tablero.campo]

	; RDI -> TABLERO
	; R10 -> *(TABLERO->CAMPO)
	; R11 -> CARTA
	; RBX -> indice

	.while:

	mov r11, [r10 + rbx * 8]
	cmp r11, 0
	je .continue

	mov r8b, byte [r11 + carta.jugador]
	cmp r8b, JUGADOR_ROJO
	jne .otro_color

	add dword [rsi], 1
	jmp .continue

	.otro_color:
	cmp r8b, JUGADOR_AZUL
	jne .continue

	add dword [rdx], 1

	.continue:
	inc rbx
	cmp rbx, tablero.ANCHO*tablero.ALTO
	jne .while

	.end:

	pop rbp
	ret
