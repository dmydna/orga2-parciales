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
;   - unwrap
global EJERCICIO_2A_HECHO
EJERCICIO_2A_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

global modps
modps:
	;n-floor(n/m)*m
	push rbp
	mov rbp, rsp
	
	movdqu xmm2, xmm0
	divps xmm2, xmm1
	roundps xmm2, xmm2, 1
	mulps xmm2, xmm1
	subps xmm0, xmm2
	
	pop rbp
	ret

global unwrap
unwrap:
	ret

