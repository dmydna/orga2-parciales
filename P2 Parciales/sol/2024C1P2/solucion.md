
### Ejercicio 1 - (50 puntos)
En un sistema similar al que implementamos en los talleres del curso (modo protegido con paginación activada), se tienen varias tareas en ejecución. Se desea agregar al sistema una syscall que le permita
a la tarea que la llama espiar la memoria de las otras tareas en ejecución. En particular queremos
copiar 4 bytes de una dirección de la tarea a espiar en una dirección de la tarea llamadora (tarea espía). La syscall tendrá los siguientes parámetros:

  - El selector de la tarea a espiar.
  - La dirección virtual a leer de la tarea espiada.
  - La dirección virtual a escribir de la tarea espía.

Si la dirección a espiar no está mapeada en el espacio de direcciones de la tarea correspondiente,
la syscall deberá devolver `−1` en `eax`, por el contrario, si se pudo hacer correctamente la operación
deberá devolver `0` en `eax`.

__Se pide__:

  a. Definir o modificar las estructuras de sistema necesarias para que dicho servicio pueda ser
invocado.

  b. Implementar la syscall y especificar claramente la forma de pasarle los parámetros correspon-
dientes.

Se recomienda organizar la resolución del ejercicio realizando paso a paso los items mencionados
anteriormente y explicar las decisiones que toman.
Aclaraciones:
  - Se puede asumir que el selector de tarea pasado es válido.
  - Se puede asumir que la dirección de destino de la tarea llamadora se encuentra correctamente mapeada



```c
// idt.c 

// Interrupciones de Software [INT 48 HASTA INT 255]
// uso IDENTRY3 porque es una interrupcion de sofware (llamada desde nivel de usuario 3)

IDENTRY3(99);

```


~~~nasm
// isr.asm

; La syscall devuelve;
; 0 si la direccion no esta mapeada
; 1 si la direccion esta mapeada

_isr99:
    pushad

    push ESI ;direccion virtual a leer tarea espiada  
    push EDI ;direccion virtual a escribir tarea espia
    push EAX ;selector de tarea a espiar 

    call espiar
    add ESP, 12 ; acomodo pila

    mov [ESP + offset_EAX], EAX; para no pisar el eax

    popad
    iret
~~~

```c
// solucion.c


int espiar(uint16_t selector, uint32_t* direccion_a_espiar, uint32_t* direccion_a_espiar){

    uint32_t cr3_tarea_a_espiar = obtenerCR3(selector);
    uint32_t cr3_tarea_espia = rcr3()
    paddr_t direccion_fisica_a_espiar = paddr_t direccion_fisica_a_espiar = obtenerDireccionFisica(cr3_tarea_a_espiar, direccion_a_espiar);

    if (direccion_fisica_a_espiar == 0) return 1;
    // Falló si la dirección de memoria no era válida

    mmu_map_page(cr3_tarea_espia, SRC_VIRT_PAGE, direccion_fisica_a_espiar)

    uint32_t dato_a_copiar = *((SRC_VIRT_PAGE & 0xFFFFFF000) | VIRT_PAGE_OFFSET(direccion_a_espiar));

    mmu_unmap_page(cr3_tarea_espia, SRC_VIRT_PAGE, direccion_fisica_a_espiar);
    
    direccion_a_escribir[0] = dato_a_copiar;
    
    return 0;

}

// FUNCIONES AUXILIARES

uint32_t obtenerCR3(uint32_t selector){
    // SELECTOR = INDICE (0:13) | TI(13:14 )| 14:16 DPL
    uint32_t TSS_IDX = selector >> 3;
    tss_t* TSS_TAREA = gdt[TSS_IDX];
    uint32_t cr3_tarea = TSS_TAREA->cr3;
    return cr3_tarea;
}

uint32_t obtenerDireccionFisica(uint32_t cr3_tarea_a_espiar,uint32_t* direccion_a_espiar){

    pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3_tarea_a_espiar);
    vaddr_t pd_index = VIRT_PAGE_DIR(direccion_a_espiar);
    vaddr_t pt_index = VIRT_PAGE_TABLE(direccion_a_espiar);

    if (!(pd[pd_index].attrs & MMU_P)) {
        return 0;
    }

    pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt);

    if (!(pt[pt_index].attrs & MMU_P)) {
        return 0;
    }
    paddr_t phy = MMU_ENTRY_PADDR(pt[pti].page);

    return phy;    // Devuelvo la base de la pagina (sin el offset)
}
```



### Ejercicio 2 - (50 puntos)

Partiendo del sistema trabajado en los talleres, se pide modificar la política del scheduler. El nuevo
scheduler distingue tareas prioritarias de no prioritarias. Las tareas prioritarias son aquellas que, al
saltar la interrupción del reloj, tengan el valor `0x00FAFAFA` en `EDX`. Las tareas pausadas y/o no
ejecutables no pueden ser prioritarias.

La forma en la que el nuevo scheduler determina la siguiente tarea a ejecutar es la siguiente:
1. Si hay otra tarea prioritaria distinta se elige esa. En caso de haber más de una se hace de forma
round-robin (como en el scheduler de los talleres).
2. Si no, se elige la próxima tarea como en el scheduler de los talleres.
La solución propuesta debe poder responder las siguientes preguntas:
 - ¿Dónde se guarda el `EDX` de nivel de usuario de las tareas desalojadas por el scheduler?
 - ¿Cómo determina el scheduler que una tarea es prioritaria?

_Se recomienda organizar la resolución del ejercicio explicando paso a paso las decisiones que
toman._

```c

// sched.c


static sched_entry_t sched_tasks[MAX_TASKS] = {0};
    
int8_t i;
int8_t current_task = 0;
int8_t last_task_priority = 0;
int8_t last_task_no_priority = 0;


uint16_t sched_next_task(void) {


    // Buscamos la próxima tarea viva con prioridad
    for ( i = (last_task_priority + 1);(i % MAX_TASKS) != last_task_priority; i++) {
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE && es_prioritaria(i)) {
            break;
        }
    }
    
    // A la salida:
    // - i != last_task_priority: más de una tarea prioritaria
    // - i == last_task_priority == current_task, no quiero repetir
    // - i == last_task_priority != current_task,
    // última no fue prioritaria
    // Ajustamos i para que esté entre 0 y MAX_TASKS-1
    
    i = i % MAX_TASKS;
    if (i != current_task && es_prioritaria(i)) {
        // Hay más de una tarea prioritaria + viva
        // o la última tarea ejecutada fue sin prioridad
        last_task_priority = i;
        current_task = i;
        return sched_tasks[i].selector;
    }

    // Si llegué acá es porque
    // - La última tarea ejecutada fue
    // con prioridad (y hay solo una con prioridad)
    // - o no hay tareas con prioridad
    // - o no hay más tareas vivas
    
    for ( i = (last_task_no_priority + 1);(i % MAX_TASKS) != last_task_no_priority; i++) {
        // Si esta tarea está disponible la ejecutamos
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
            break;
        }
    }

    // Si la tarea que encontramos es ejecutable ,
    // entonces la corremos.
    if (sched_tasks[i].state == TASK_RUNNABLE) {
        // Si llegamos acá, la tarea que encontramos
        // no es prioritaria
        last_task_no_priority = i;
        current_task = i;
        return sched_tasks[i].selector;
    }
    // En el peor de los casos no hay ninguna tarea viva.
    // Usemos la idle como selector.
    
    return GDT_IDX_TASK_IDLE << 3;
}


// auxiliares

uint8_t es_prioritaria(uint8_t idx) {
	tss_t* tss_task = obtener_TSS(sched_tasks[i].selector);
	uint32_t* pila = tss_task->esp;
	uint32_t edx = pila[5];
	return edx == 0x00FAFAFA;
}
```