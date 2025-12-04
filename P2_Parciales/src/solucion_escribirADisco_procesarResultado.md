# Solucion

#### Idea General

La tarea 6 corre en nivel 0, esta tarea va a procesar un resultado que las demas tareas le van a mandar por una syscall.
Por limitaciones del sistema (porque el enunciado no lo aclara), la tarea 6 va procesar de a un resultado a la vez. <BR>
Entonces, tenemos que agregar un nuevo estado a la tareas, para marcar que la tarea que mando un resultado y que se va a quedar esperando a la tarea 6 termine de procesar el resultado.

---

## Ejercicio 1


**ejecicio 1a**

- En `idt.c` agrego la nueva entrie para la idt, nivel 3 porque es una syscall. 
- En `isr.asm` vamos a definir la syscall.
- En `sched.c` vamos a agregar un estado para la tarea. 

`idt.c`:

```c
idt_init(){
    // syscall_procesar_eax
    IDT_ENTRY3(98);
}
```


`isr.asm`:


Cuando una tarea llame a la `_isr98`, esta tarea sera **desalojada** y se cambia a la tarea 6 de forma inmediata . 
Es decir no espera a la interrupcion del clock (para realizar el cambio de tarea), de esta forma estamos "cediendo el tiempo de ejecucion restante a la tarea 6".<BR>
Notar que la proxima tarea en ejecutarse al producirse la **siguiente interrupcion del clock** puede ser cualquiera.


~~~asm
global _isr98
_isr98:
   pushad

   mov word [task_selector_procesar], di
   push edi; PARAMETRO: (task selector de la tarea 6)
   push eax; PARAMETRO: (EAX enviado por la tarea llamante)
   call syscall_procesar_eax

   mov word [task_selector_procesar], ax
   jmp far [sched_task_offset]; se reanuda en la _isr32 despues del jmp far.

   mov esp, 8
   popad
   ret
~~~


`sched.c`

Agrego un nuevo estado para las tareas. (`TASK_AWAITING`) el cual va reflejar, que la tarea:
1. Mando un resultado a procesar.
2. Que fue desalojada.
3. Que esta esperando que la tarea 6 termine de procesar el resultado para volver a ejecutarse.

```c

typedef enum {
  TASK_SLOT_FREE,
  TASK_RUNNABLE,
  TASK_PAUSED,
  TASK_AWAITING
} task_state_t;

sched_awaiting_task(uint8_t task_id){
    sched_tasks[task_id].state = TASK_AWAITING;
}


```
**ejecicio 1b**

`solucion.c`:

Para que la syscall, pueda mandar el registro EAX a la tarea 6, tengo que meterme en la pila de nivel del kernel (de la tarea 6) y modificar el valor EAX guardado por el `pushad` cuando se produjo la interrupcion.
Al hacer esto, (ya en el contexto de la tarea) cuando se termine ejecutar el codigo de la `isr` el valor de EAX sera pisado por el nuevo valor cuando se ejecute el `popad`.

Porque no usar `tss_task->eax`

Cuando una tarea es desalojada su ejecucion termina en el `jmp far`
.En este momento se guarda su estado en su tss y se cambiara de contexto. <BR>
Cuando la misma tarea retoma la ejecucion lo hace desde el ultimo punto donde fue interrumpida, es decir despues del `jmp far` en la `isr` correspondiente. <BR>
Al hacer `jmp far` se restauran los registro desde su tss ( se restauran al momento justo antes del jmp far, incluido el EAX). 
Luego sabemos que se ejecutara el `popad` al salir de la `isr`, esto pisara el valor de EAX (y los demas registros de proposito general) que fueron restaurados por la TSS.


```c
void syscall_procesar_eax(uint16_t selector, uint32_t eax){
    // La tarea que llama no vuelve hacer  
    // ejecutada hasta terminar de procesar.
    sched_awaiting_task(current_task);
    tss_t* tss_task = tss_tasks[selector >> 3];
    uint32_t* stack0 = tss_task->esp0;
	stack0[7] = eax;
}

// Esto tiene sentido cuando el sistema procesa de a un pedido a la vez
uint8_t termina_procesar(){
    for (int8_t i = 0; i < MAX_TASKS; i++) {
      if (sched_tasks[i].state == TASK_AWAITING) {
          sched_tasks[task_id].state = TASK_RUNNABLE;
          break;
      }
      sched_disable_task[current_task]
    }
}
```




**ejecicio 1c**

- Implementacion del codigo de la tarea 6 (nivel 0)

~~~asm
    global _task_procesar
    _task_procesar:

    push eax
	call procesar_valor_EAX
    call termina_procesar
    mov esp, 4
	jmp $
~~~


**ejecicio 1d**

Como se menciono en la idea general. El sistema procesa un resultado a la vez. Ya que no hay forma de evitarlo actualmente, cuando otra tarea llame a la tara 6 para procesar un resultado, habra un problema si todavia no se termino de procesar un pedido anterior (previamente pedido por otra tarea).

Para arreglarlo, se tendria que agregar un sistema de cola de prioridad para que la tarea 6 pueda administrar los distintos pedidos para procesar varios resultados o en su defecto decirles a las tareas que esta ocupada actualmente y no puede antender nuevos pedidos hasta terminar.

### Ejercicio 2

Si la tarea cuyo `CR3` es proporsionado modifico la pagina fisica `phy` entonces la tarea tuvo que haber mapeado dicha pagina fisica (direccion fisica) en su `PD` y tuvo que haber accedido (bit A prendido) y si hizo modificaciones se predio el bit d.
Entonces tengo que recorrer toda su `PD` y buscar entre las pagina que fueron accedidas (bit a) y que fueron modificadas (bit d), la pagina que mapea la direccion `phy`


```c
#define MMU_A       (1 << 5) 
#define MMU_D       (1 << 6)

#define MMU_PHY_TO_VIRT(PD_INDEX, PT_INDEX, PHY) \
    ((PD_INDEX << 22) | (PT_INDEX << 12) | (PHY & 0x00000FFF))

void Escribir_a_Disco(uint32_t CR3, paddr_t phy){
    pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(CR3);
    pt_entry_t* pt;
    for(int=0; pd_index<1024; pd_index++){
            if (!(pd[pd_index].attrs & MMU_P)){
               continue;
            }
            pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt)
            for(int pt_index = 0; pt_indec < 1024; pt_indec++){
               if ((pt[pt_index].attrs & MMU_P) && (pt[pt_index].attrs & (MMU_D | MMU_A))){
                    if(pt[pt_index].page == phy >> 12){
                       mmu_unmap_page(CR3, MMU_PHY_TO_VIRT(pt_index, pt_index, phy))
                       return
                    }
                }
        }
    }
}


```
