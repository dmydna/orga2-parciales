extern malloc

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

section .text
; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - es_indice_ordenado
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - indice_a_inventario
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

;; La funcion debe verificar si una vista del inventario está correctamente 
;; ordenada de acuerdo a un criterio (comparador)

;; bool es_indice_ordenado(item_t** inventario, uint16_t* indice, uint16_t tamanio, comparador_t comparador);

;; Dónde:
;; - `inventario`: Un array de punteros a ítems que representa el inventario a
;;   procesar.
;; - `indice`: El arreglo de índices en el inventario que representa la vista.
;; - `tamanio`: El tamaño del inventario (y de la vista).
;; - `comparador`: La función de comparación que a utilizar para verificar el
;;   orden.
;; 
;; Tenga en consideración:
;; - `tamanio` es un valor de 16 bits. La parte alta del registro en dónde viene
;;   como parámetro podría tener basura.
;; - `comparador` es una dirección de memoria a la que se debe saltar (vía `jmp` o
;;   `call`) para comenzar la ejecución de la subrutina en cuestión.
;; - Los tamaños de los arrays `inventario` e `indice` son ambos `tamanio`.
;; - `false` es el valor `0` y `true` es todo valor distinto de `0`.
;; - Importa que los ítems estén ordenados según el comparador. No hay necesidad
;;   de verificar que el orden sea estable.

global es_indice_ordenado
es_indice_ordenado:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = item_t**     inventario
	; r/m64 = uint16_t*    indice
	; r/m16 = uint16_t     tamanio
	; r/m64 = comparador_t comparador

	; rdi -> inventario **
	; rsi -> indice*
	; rdx -> tamanio 
	; rcx -> comparador

		push rbp ; alieno pila
		mov rbp, rsp
		sub rsp, 8 
		push r10
		push r11
		push r12 ;pila alineada

		mov r9, rdi ; guardo inventario
		mov r10,rsi ; guardo indices

		mov rax, 1; res = true
		mov r8, 1 


		; r8 -> i
		; r11 -> indice[i]
		; r12 -> indice[i-1]

		.while:
			cmp r8, rdx ; i < tamanio
			je .end
			; obtengo indice
			mov r11, [rsi+r8] ; indice[i]
			mov r12, [rsi + r8 -1 ] ; indice[i-1]
			; obtengo items
			mov rdi , [r9 + r11] ; inventario[indice[i]]
			mov rsi,  [r9 + r12] ; inventario[indice[i-1]]
			call rcx
			cmp rax, 0
			je .end
			inc r8	; i ++
			jmp .while
		.end:

		pop r12
		pop r11
		pop r10
		add rsp, 10
		mov rbp, rsp
		pop rsp
		ret

;; Dado un inventario y una vista, crear un nuevo inventario que mantenga el
;; orden descrito por la misma.

;; La memoria a solicitar para el nuevo inventario debe poder ser liberada
;; utilizando `free(ptr)`.

;; item_t** indice_a_inventario(item_t** inventario, uint16_t* indice, uint16_t tamanio);

;; Donde:
;; - `inventario` un array de punteros a ítems que representa el inventario a
;;   procesar.
;; - `indice` es el arreglo de índices en el inventario que representa la vista
;;   que vamos a usar para reorganizar el inventario.
;; - `tamanio` es el tamaño del inventario.
;; 
;; Tenga en consideración:
;; - Tanto los elementos de `inventario` como los del resultado son punteros a
;;   `ítems`. Se pide *copiar* estos punteros, **no se deben crear ni clonar
;;   ítems**

global indice_a_inventario
indice_a_inventario:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi /m64 = item_t**  inventario
	; rsi /m64 = uint16_t* indice
	; rdx /m16 = uint16_t  tamanio
		push rbp
		mov rbp, rsp

		push r11
		push r12
		push r13
		push rdi


		mov rdi, 24 ; largo de item_t
		mov rax, rdx
		mul r8

		call malloc ;-> rax

		pop rdi
		mov r11, 0 ; i 

		; r11 -> i
		; r12 -> indice[i]
		; r13 -> inventario[indice[i]]

		.while:
			cmp r11, rdx ; i == tamanio
			je .end
			; obtengo indices
			mov r12, [rsi+r11] ; indice[i]
			; copio punteros a rax
			mov r13, [rdi+r12] ; inventario[indice[i]]
			mov [rax], r13

			inc rax
			inc r11; i++
			jmp .while
		.end:
		pop r11
		pop r12
		pop r13
		mov rsp, rbp
		pop rbp
		ret
