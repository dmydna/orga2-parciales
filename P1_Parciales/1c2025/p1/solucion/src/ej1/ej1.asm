extern malloc
extern sleep
extern wakeup
extern create_dir_entry

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio
sleep_name: DB "sleep", 0
wakeup_name: DB "wakeup", 0

section .text
; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1


; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - init_fantastruco_dir
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.
; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - summon_fantastruco
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

;########### ESTOS SON LOS OFFSETS Y TAMAÑO DE LOS STRUCTS
; Completar las definiciones (serán revisadas por ABI enforcer):
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

; void init_fantastruco_dir(fantastruco_t* card);
global init_fantastruco_dir
init_fantastruco_dir:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = fantastruco_t*     card

	push rbp
	mov rbp, rsp
	; rdi -> *Card 


	push r12
	push r13
	push r14
	push rdi

	; DIRECTORY_ENTRY_LIST
	mov rdi, 2*PTR_SIZE
	;rdi --> *DIRENTRY_ENTRY_T * 2

	call malloc
	mov r12, rax ;
	;r12 --> DIRECTORY_ENTRY_LIST

	mov rdi, sleep_name
	mov rsi, sleep
	call create_dir_entry
	mov r13, rax ; 
	;r13 --> DIR_ENTRY_SLEEP

	mov rdi, wakeup_name
	mov rsi, wakeup
	call create_dir_entry
	mov r14, rax 
	; r14 --> DIR_ENTRY_WAKEUP

	;DIRECTORY_ENTRY_LIST[0] = DIR_ENTRY_SLEEP
	mov [r12 + PTR_SIZE * 0], r13; 
	;DIRECTORY_ENTRY_LIST[1] = DIR_ENTRY_WAKEUP
	mov [r12 + PTR_SIZE * 1], r14  

	pop rdi
	; CARD->__DIR = DIRECTORY_ENTRY_LIST 
	mov [rdi + FANTASTRUCO_DIR_OFFSET], r12
	; CARD->__DIR_ENTRIES = 2
	mov word [rdi + FANTASTRUCO_ENTRIES_OFFSET], 2

	pop r14
	pop r13
	pop r12

	pop rbp
	ret ;No te olvides el ret!

; fantastruco_t* summon_fantastruco();
global summon_fantastruco
summon_fantastruco:
	; Esta función no recibe parámetros
	push rbp
	mov rbp, rsp
	
	push r12
	push r13

	mov rdi, FANTASTRUCO_SIZE
	call malloc
	mov r12,rax
	; r12 -> NEW_CARD

	mov rdi, rax
	; INIT_FANTASTRUCO_DIR(NEW_CARD)
	call init_fantastruco_dir


	; NEW_CARD->__ARCHETYPE = NULL
	mov qword [r12 + FANTASTRUCO_ARCHETYPE_OFFSET], NULL
	; NEW_CARD->FACE_UP = 1
	mov byte  [r12 + FANTASTRUCO_FACEUP_OFFSET], 1	

	mov rax, r12
	pop r13
	pop r12

	pop rbp
	ret ;No te olvides el ret!
