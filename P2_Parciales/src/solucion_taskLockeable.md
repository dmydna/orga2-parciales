# Solucion

#### idea general

Cada tarea puede solicitar usar el area de LOCKABLE, si obtiene acceso a la misma continuara ejecutando. Si no lo obtiene, entonces debera esperar a que el AREA LOCKABLE sea liberada por alguna otra tarea que tiene accesso a la misma.
La tarea que queda esperando el accesso no puede retomar su ejecucion a modo usuario hasta que obtenga el acceso. Por lo que cada vez que esta tarea vuelva a ejecutarse lo hara desde la syscall que llamo para solicitar el acceso. De otro modo, se provocaria un page faul al intentar acceder a un area de memoria que no tiene acceso. 


## Ejercicio 1.


- En `idt.c` agremos dos nuevas entries para las syscall (nivel 3) que atenderan las interrupciones pedidas.<BR>
- En `isr.asm` se agregan las rutina `_isr` que atiende las nuevas entries 98 (para `LOCK`) y 99 (para `RELEASE`).<BR>
- En `task.c` se implementan las funciones que son llamadas desde las _isr_.<BR>


`idt.c`:
```asm

void idt_init(){
    //LOCK
    IDT_ENTRY3(98)
    //RELEASE
    IDT_ENTRY3(99)
}
```

`isr.asm`:

En ambas rutinas mandamos por parametro a la syscall la direccion virtual (en rango de `LOCKABLE`) usando el registro EAX.

##### En la _isr98  (`get_lock`)

Se introduce en un loop `.while`, el cual mantendra bloqueda la tarea hasta obtener el `lock`. De esta manera la tarea no retomara el nivel de usuario hasta que lo obtenga.

Luego de llamar a la syscall `get_lock` se chequea si la tarea consigio el lock:
- Si la tarea no obtuvo acceso al lock es inmediatamente desalojada. Y se pasa a la siguiente tarea.

Cuando la tarea vuelve a ser ejecutada (inmediatamente despues del jmp far) se salta a `.while` volviendo a repetir el proceso de adquision del `lock`

~~~asm
global _isr98
_isr98:
    pushad

    push eax
    .while:
       call get_lock 

       mov rdi, [current_task]
       cmp [CURRENT_TASK_LOCKABLE], rdi
       je .fin 

       call sched_next_task
       mov word [sched_task_selector], ax
       jmp far [sched_task_offset]

       jmp .while
    .fin:

    add esp, 4
    popad
    iret
~~~

##### En la **_isr98**  (`release`)

Luego de llamar a la syscall `release` se chequea si la tarea consigio liberar el lock: 
- Si la tarea ya no tiene accesso al `lock` entonces 
es inmediatamente desalojada. Y se pasa a la sig tarea <BR> (Este comportamiento no se aclara en el enunciado)
- En caso contrario se continua con ejecucion saltando `.fin`


~~~asm
global _isr99
_isr99:
    pushad

    push eax
    call release

    mov rdi, [FREE_LOCK]
    cmp [CURRENT_TASK_LOCKABLE], rdi
    jne .fin

    call sched_next_task
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    add esp, 4
    popad
    iret
~~~


`task.c`:

En `get_lock`:
Si la flag CURRENT_TASK_LOCKABLE es igual FREE_LOCK entonces
Vamos a modificar la bandera con la id de la tarea que esta solicitando el `lock` y mapear la pagina LOCKEABLE. 


En `release`:
Si la tarea no es dueña del lock, entonces no hara nada.
En caso contrario desmapeara y actualizara la Bandera correspondiente como Libre.


```c
TASK_LOCKABLE_PAGE_VIRT
TASK_LOCKABLE_PAGE_PHY

// podemos pensar que FREE_LOCK es un task_id invalida como MAX_TASK + 1
static uint8_t CURRENT_TASK_LOCKABLE = FREE_LOCK; 


uint32_t ObtenerCR3(uint8_t task_id){
    tss_t* tss_task = &tss_tasks[task_id];
    return tss_task->cr3
}


void get_lock(vaddr_t shared_page){

    if (shared_page >= TASK_LOCKABLE_PAGE_VIRT && shared_page <= TASK_LOCKABLE_PAGE_VIRT + PAGE_SIZE){
        if(CURRENT_TASK_ID_LOCKABLE == FREE_LOCK){
            mmu_map_page(rcr3(), TASK_LOCKABLE_PAGE_VIRT, TASK_LOCKABLE_PAGE_PHY,  MMU_P | MMU_W);
            CURRENT_TASK_ID_LOCKABLE = current_task;
        }
    }else{
        kassert(false, "No es direccion lockeable");
    }
}

void release(vaddr_t shared_page){
    if(CURRENT_TASK_LOCKABLE != current_task){
        return;
    }
    if(shared_page >= TASK_LOCKABLE_PAGE_VIRT && shared_page <=  TASK_LOCKABLE_PAGE_VIRT + PAGE_SIZE){
        mmu_unmap_page(rcr3(), TASK_LOCKABLE_PAGE_VIRT);
        CURRENT_TASK_LOCKABLE = FREE_LOCK;
    }else{
        assert(false, "No es direccion lockeable");
    }

}

```

## Ejercicio 2.

ejercicio a. 

Para que se pueda leer el area `LOCKABLE` sin tener el lock y sin tener que llamar a la syscall y si es que esta libre:

 Tenemos que modificar el handler del page fault en `mmu.c`. Vamos a verificar que el acceso ,osea virt, se de en rango `LOCKABLE` y si el lock esta libre, entonces mapeamos el area `LOCKABLE` como solo lectura a la pd de la tarea solicitante.

Notar que la tarea solo leera y no obtendra el `lock`.


`mmm.c`:

```c
bool page_fault_handler(vaddr_t virt) {

  if(virt >= TASK_LOCKABLE_PAGE_VIRT && virt <= TASK_LOCKABLE_PAGE_VIRT + PAGE_SIZE){
      uint32_t cr3 = rcr3();
      uint32_t attrs = MMU_P | MMU_U ;
      mmu_map_page(cr3, virt, ON_DEMAND_MEM_START_PHYSICAL,attrs);
      return true;
  }

  // si esta en rango On-demand  
  if (virt >= ON_DEMAND_MEM_START_VIRTUAL && virt <= ON_DEMAND_MEM_END_VIRTUAL) {
    //...
  }
  // Si el page fault no se pudo atender o está fuera del área on-demand
  return false;
}
```

ejercicio b. 

Para lograr que las tareas obtengan un accesso legitimo al lock sin la syscall get_lock: 

Debemos mover el codigo de la syscall (get_lock), al handler del page faul (en mmu.c), justo donde se checkea el acceso a area de LOCKEABLE. (ejercicio 2 a).<BR>
En caso que el area LOCKABLE no este libre, el handler de page_fault (en mmu.c) 
debera retornar un valor especifico (ej: `404`).

En la isr14 en isr.asm, debemos envolver el llamado al handler del page_faul en un loop, que chequee si el handler devolvio el valor especifico, de esta manera la tarea solicitante quedara bloqueada en la rutina del page fault hasta obtener el lock, cuando lo haga el handler del page fault (en mmu.c) devolvera un valor distinto a `404` (0 o 1), permitiendo salor del loop y retomar el modo usuario a la tarea solicitante.
`

Para Poder quitar el lock a una tarea despues de 5 desalojos:

En `sched.c` , hay que definir una contador_de_ticks por tarea, (un array cuyo indices son los task id de las tarea  e indican cuanta veces fueron desalojadas las mismas si tenian el lock)

El contador se actualizara cada vez que se ejecute `sched_task_next` para eso vamos a modificar el codigo de la misma `sched_task_next` para que:

1. Se incrementa el contador de tick de la current task si tiene el lock.
2. Se chequee que el contador sea mayor de 5:
   - Si es el caso: <BR> - Se libera el lock seteando el flag correspondiente. <BR> - Se resetea su contador a 0. <BR> 
3. Pasa a buscar la siguiente tarea. 


