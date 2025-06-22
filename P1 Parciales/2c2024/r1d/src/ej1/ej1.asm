extern malloc
extern strcpy

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - copiarUnidad
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - comenzarBatalla
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1C como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - determinarBatalla
global EJERCICIO_1C_HECHO
EJERCICIO_1C_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

section .text

global copiarUnidad
copiarUnidad:
	; r/m64 = attackunit_t*		base
	ret

global comenzarBatalla
comenzarBatalla:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = mapa_t           mapa
	; r/m64 = uint8_t*         x
	; r/m64 = uint8_t*         y
	ret

global determinarBatalla
determinarBatalla:
	; r/m64 = attackunit_t*		jugadorDeTurno
	; r/m64 = attackunit_t*		oponente
	ret


global aplicarResultadoBatalla
aplicarResultadoBatalla:
	; r/m64 = attackunit_t*		jugadorDeTurno
	; r/m64 = attackunit_t*		oponente
	ret
