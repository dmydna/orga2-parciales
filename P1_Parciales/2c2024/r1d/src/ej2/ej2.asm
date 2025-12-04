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

; Dada una imagen I le aplica el filtro sepia para obtener una imagen de salida O:
; O[i,j].r = 0.5 * (I[i,j].r + I[i,j].g + I[i,j].b)
; O[i,j].g = 0.3 * (I[i,j].r + I[i,j].g + I[i,j].b)
; O[i,j].b = 0.2 * (I[i,j].r + I[i,j].g + I[i,j].b)
; O[i,j].a = I[i,j].a
;
; Parámetros:
;   - dst:    La imagen destino O. Es RGBA (8 bits sin signo por canal).
;   - src:    La imagen origen I. Es RGBA (8 bits sin signo por canal).
;   - size:  El ancho y alto en píxeles de `dst` y `src`.
global ej2a
ej2a:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = rgba_t*  dst
	; r/m64 = rgba_t*  src
	; r/m32 = uint32_t size

	ret