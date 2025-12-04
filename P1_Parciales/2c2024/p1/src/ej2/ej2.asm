section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el filtro
ALIGN 16

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
EJERCICIO_2A_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

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
	; r/m64 = rgba_t*  dst
	; r/m64 = rgba_t*  src
	; r/m32 = uint32_t width
	; r/m32 = uint32_t height
	; r/m8  = uint8_t  amount

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
