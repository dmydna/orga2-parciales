# Solucion

### Ejercicio 1.

- En `idt.c`: Definimos una entrie para la IDT, nivel 3 porque es una syscall.
- En `isr.asm`: Definimos las syscall para las nuevas entries de la idt.
   - Usamos `sched_next_task_id` para obtener la task_id de prox tarea 
   - Llamamos a una nueva funcion ` sched_task_clear_access` para marcar la prox tarea a limpiar sus paginas accedidas.
- En `sched.c`: para limpiar el bit de acceso de la tarea:
   - Vamos a agregar un nuevo estado a las tareas `TASK_CLEAR_ACCESS`. 
   - Vamos a modicar el `sched_next_task` para que ejecute la rutina correspondiente segun el nuevo estado.
   - Vamos separar la logica para encontrar la id de la siguiente tarea (en `sched_next_task`). Para eso definimos sched_next_task_id que devolvera la id de la prox tarea (distinta a la `current_task`).

- En `solucion.c` vamos a implementar la funcion que limpie el bit de accesso de una tarea, usando su selector pasado por parametro.

`idt.c`:
```c
void idt_init() {
  IDT_ENTRY3(98);
}
```

`isr.asm`:

~~~asm
global _isr98
_isr98:
    pushad

    ; busca prox task_id
    call sched_next_task_id
    push eax
    call sched_clear_access_request
    mov esp, 4
    popad
    iret
~~~

`sched.c`

```c
typedef enum {
  TASK_SLOT_FREE,
  TASK_RUNNABLE,
  TASK_PAUSED
  TASK_CLEAR_ACCESS
} task_state_t;



void sched_clear_access_request(int8_t task_id) {
  sched_tasks[task_id].state = TASK_CLEAR_ACCESS;
}



uint16_t sched_next_task_id(void){
  for (int i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
   // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE  
        || sched_tasks[i % MAX_TASKS].state = TASK_CLEAR_ACCESS) 
    {
      return i;
    }
}


}

uint16_t sched_next_task(void) {

  // Buscamos la próxima tarea viva y diferente a la actual (comenzando en la actual)  
  uint8_t i = sched_next_task_id();

  // Limpia bit de accesso (para la siguiente tarea)
  if(sched_tasks[i].state == TASK_CLEAR_ACCESS){
     task_clear_bit_access(sched_tasks[i].selector)
     sched_tasks[i].state = RUNNABLE
  }

  // Si la tarea que encontramos es ejecutable entonces vamos a correrla.
  if (sched_tasks[i].state == TASK_RUNNABLE) {
    current_task = i;
    return sched_tasks[i].selector;
  }

  // En el peor de los casos no hay ninguna tarea viva. Usemos la idle como
  // selector.
  return GDT_IDX_TASK_IDLE << 3;
}


```



`solucion.c`:
```c

MMU_A 1 << 5

void task_clear_bit_access (uint32_t selector){
    uint32_t cr3_proxima_tarea = obtenerCR3(selector);
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
}


uint32_t obtenerCR3(uint32_t selector){
    // SELECTOR = INDICE (0:13) | TI(13:14 )| 14:16 DPL
    uint32_t TSS_IDX = selector >> 3;
    tss_t* TSS_TAREA = gdt[TSS_IDX];
    uint32_t cr3_tarea = TSS_TAREA->cr3;
    return cr3_tarea;
}

```

## Ejercicio 2

 Vamos a iterar sobre la GDT en busca de los descriptores de tss para encontrar la bases de la PD de las tareas. En cada pd, usamos los indices de la direccion virtual para buscar la pagina y contamos si esta presente.


```c
uint32_t getMappings(uint32_t virtual, gdt_entry_t* gdt){
	// virtual = pd_index | pt_index | page_offset
	uint32_t contador = 0;

    // Itera sobre toda la GDT! 
	for(int i=0; i<GDT_LENGTH; i++){
		// busca descriptores de TSS
		if(gtd[i].type == DESC_TYPE_32BIT_TSS){
		// Obtengo la TSS
		uint32_t base_tss = ((uint32_t)gtd[i].base_15_0) | 
                            ((uint32_t)gtd[i].base_23_16) << 16 |
                            ((uint32_t)gdt[i].base_31_24) << 24);

		tss_t* tss_actual = (tss_t*) base_tss ;
		pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(tss_actual->cr3);

		vaddr_t pd_index = VIRT_PAGE_DIR(virtual);
		vaddr_t pt_index = VIRT_PAGE_TABLE(virtual);
		// Checkea si la pt y la pagina estan presentes.
			if (pd[pd_index].attrs & MMU_P) {
				pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt);
				if(pt[pt_index].attrs & MMU_P){
					contador ++
				}
			}
		}
	}
	return contador;
};

```




