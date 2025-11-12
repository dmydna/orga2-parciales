/*

syscalls:

1. lock (solicita shared memory)
2. release (libre shared memory)


tarea -> solicita lock :
	Casos:
	1. Obtiene lock
	2. lock ocupadado -> se pasa a la sig tarea

tarea -> tiene lock:
	Casos:
	1. libera lock
	2. no libera lock

*/

// idt.c

// Declarar las interrupciones de software

IDENTRY0(99) // lock
IDENTRY0(98) // release


// isr.asm

// Definir las rutinas de interrupciones

_isr99:
	pushad

	call lock

	cmp ax, CURRENT_TASK_ID
	jz .fin

	call _isr32
	fin:

	popad
	iret

_isr99:
	pushad

	call release

	popad
	iret


_isr32:
    pushad
    call pic_finish1
    call next_clock
    call sched_next_task
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    call tasks_tick
    popad
    iret



// Defino varible global : 
// La misma, almacena la task_id_actual o es null



uint32_t LOCK_ACCESS = 0
bool REQUEST_LOCK[CANT_TASK] 


// IDEA: lock devuelve la current_task_id si obtuvo existo, si no devolvera una task_id de otra tarea

uint32_t lock(){
// si el lock esta disponible se setea con la task_id solicitante
	if(LOCK_ACCESS == null){
		LOCK_ACCESS = current_task_id
		return LOCK_ACCESS
	}
// si lock no esta disponible
// guarda la solicitud
// pausa la tarea hasta que se libere lock 
	REQUEST_LOCK[current_task_id] = true
	sched_disable_task[current_task_id]
	return LOCK_ACCESS
}


// IDEA: si una tarea necesita lock pero no la tiene 
// entonces se pausa su ejecución para correr la siguiente tarea.
// si la siguiente tarea tambien necesitara lock pero no la tiene se pasara a la siguiente.

uint32_t release(){
// Solo la current_task_id que tiene lock puede liberarla.
	if(current_task_id == LOCK_ACCESS){

		// libera lock
		LOCK_ACCESS = 0

		REQUEST_LOCK[current_task_id] = false
	
		// libera todas las tareas que necesitan lock
		for(int i=0; i<MAX_TASKS;i++){
			if(REQUEST_LOCK[i] == true){
				sched_enable_task_task[i]
			}
		}
	}
}















