// Ejercicio 1

/*
Pasos de la resolución:
1. Obtener el id de la tarea que llama a la syscall
2. Desactivar la tarea actual
3. Buscar la siguiente tarea a ejecutar
a. Obtener el id
b. Obtener el selector
4. Antes de realizar el cambio de contexto, escribir el id obtenido en el paso 1 en el EAX destino.
Para eso:
a. Obtenemos la TSS de la nueva tarea
b. Buscamos el ESP de la nueva tarea
c. Nos movemos en la pila hasta donde se ubica el EAX (pusheado por pushad antes de
cambiar de contexto)
d. Lo pisamos con el id
5. Seguimos con el cambio de contexto normal

*/


// idt.c

IDTENTRY3(99); ;syscall exit

// isr.asm


global _isr99


_isr99: ;syscall exit
	pushad
	push DWORD [current_task]
	call sched_disable_task
	call sched_next_task_id
	push eax
	call pass_exit_id_to_next_task
	call sched_next_task
	mov word [sched_task_selector], ax
	jmp far [sched_task_offset]
	.fin:
	mov esp, 8
	call tasks_tick
	call tasks_screen_update
	popad
	iret


_isr32: ;rutina de atención del reloj
	pushad
	call pic_finish1
	call next_clock

	push DWORD [current_task]
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
	call tasks_screen_update
	popad
	iret




// en sched.c


uint8_t sched_next_task_id(void){
	// Buscamos la proxima tarea viva (comenzando en la actual)
	 int8_t i;
	for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
 // Si esta tarea está disponible la ejecutamos
	if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE)
		break;
 }
// Ajustamos i para que esté entre 0 y MAX_TASKS-1
	i = i % MAX_TASKS;
	return i
}


uint16_t sched_next_task(uint8_t task_id){
 // Si la tarea que encontramos es ejecutable entonces vamos a correrla.
	if (sched_tasks[i].state == TASK_RUNNABLE){
		current_task = i;
	return sched_tasks[i].selector;
	}
// En el peor de los casos no hay ninguna tarea viva. Usemos la idle como selector.
	return GDT_IDX_TASK_IDLE << 3;
}



// en tasks.c


void pass_exit_id_to_next_task(uint8_t exit_task_id, uint8_t new_task_id){
	tss_t new_task_id = tss_tasks[new_task_id];
	uint32_t* new_task_esp = (uint32_t*) new_task_tss.esp; 
// como es nivel 0, esta mapeado con identity mapping
	*(new_task_esp + eax_offset) = exit_task_id;
	return;
}








