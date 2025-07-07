/*

IDEA.

0. Creo una VARIABLE GLOBAL.

1. Creo una syscall que setee la  VARIABLE GLOBAL.

2. La tarea podra llamar o no a la syscall

3. Si la tarea llamo  a la syscall (es decir se seteo la VARIABLE GLOBAL) 
  entonces ejecutar la modificacion durante la rutina de atencion del clock
 (donde se produce el CAMBIO DE TAREA). 
 
4. Creo una funcion que modifique el bit de acceso de las paginas de una tarea.

*/


// Máscara del bit Accessed (bit 5)
MMU_A 1 << 5

// Variable global que indica si hay que limpiar el bit de acceso
uint8_t DISABLE_BIT_ACCESS_PAGE = 0


// Activación por syscall
void disable_access_page(){
	DISABLE_BIT_ACCESS_PAGE = 1;
	return;
}

// Función para limpiar el bit de acceso de todas las páginas de usuario de una tareas
void clear_page_accessed_bit(uint32_t selector) {

	if(!DISABLE_BIT_ACCESS_PAGE){
		return
	}

	uint32_t cr3_prox_tarea = obtenerCR3(selector);
	pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);

	for(int pd_index=0; pd_index<1024; pd_index++){
	// BUSCO TABLAS DE USUARIO
		if (pd[pd_index].attrs & MMU_P| MMU_U) {
	// Seteo tabla como no accedida
			pd[pd_index].attrs &=  ~MMU_A;
			pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt);
	// Seteo paginas como no accedida
			for(int pt_index = 0; pt_index < 1024; i++){
				pt[pt_index].attrs &=  ~MMU_A;
			}
		}
	}
	// restablece variable antes de salir
	DISABLE_BIT_ACCESS_PAGE = 0
	return
}



// IDT.C

// Agrego una nueva IDTENTRY 

IDTENTRY0(99)


//ISR.ASM

// CREO UNA SYSCALL 
// que solicita la desactivacion del bit de acceso de la siguiente tarea
global _isr99
 _isr99:
 pushad

 ; setea la variable global
 call disable_bit_access_page

 popad
 iret

// MODIFICAR LA RUTINA DE ATENCION DEL CLOCK 
// donde se produce el cambio de tareas
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

	;importante: mando selector de prox tarea 
	push bx; 
	call clear_page_accessed_bit
	mov esp, 4

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    call tasks_tick
    call tasks_screen_update
    popad
    iret


