# Solucion


- En `task.c` Tenemos que agregarle información a la tarea sobre la dirección virtual que va a usar para mapear el
buffer. En particular, podemos __agregar un estado a una tarea__ la cual sea BLOCKED, la cual da un
indicio de que esta bloqueada esperando a tener acceso al device.


- En `idt.c` Tenemos que agregar una interrupción por hardware y dos syscalls.
  - Como la interrupción es de hardware, entonces el kernel es el único que puede atender a la
misma.
  - Como las syscalls son servicios que provee el SO al usuario, entonces estas deben ser
capaces de ser llamadas por un usuario, y el código que ejecutan es de nivel 0.

- En `sched.c`: vamos a agregar la informacion sobre el modo de acceso que va a tener la tarea, y la direccion
virtual en donde va a poder encontrar el buffer


`task.c`:

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  // Agregado para resolver
  // el mecanismo propuesto
  uint32_t copyDir;
  uint8_t mode;
} sched_entry_t;

typedef enum {
  NO_ACCESS,
  ACCESS_DMA,
  ACCESS_COPIA
} task_access_mode_t;

typedef enum {
  TASK_SLOT_FREE,
  TASK_RUNNABLE,
  TASK_PAUSED,
  // Nuevo estado de tarea
  TASK_BLOCKED
  // Estado para usar después
  TASK_KILLED
} task_state_t;

```

`idt.c`:

```c
void idt_init() {
  // ...
  IDT_ENTRY0(40);
  // ...
  IDT_ENTRY3(90);
  IDT_ENTRY3(91);
  // ...
}
```

`isr-asm`:
~~~asm

;---------------------------------
;; OPENDEVICE

extern opendevice
global isr_90

isr_90:  
  pushad
  
  push ecx
  call opendevice
  add esp, 4
  call sched_next_task

  mov word [sched_task_selector], ax
  jmp far [sched_task_offset]
  
  popad
  iret

;---------------------------------
;; DEVICEREADY
  
global isr_40
extern deviceready
isr_40: 
  pushad
  call pic_finish
  call deviceready
  popad
  iret

;---------------------------------
;; CLOSDEVICE

extern closedevice
global isr_91
isr_91: 
  pushad
  call closedevice
  popad
  iret
~~~



### Device Ready

Cuando se ejecuta esta funcion, entonces el kernel toma posession de la tarea que estaba ejecutandom y empieza a mapear a todas las tareas qye hayan solicitado acceso al area del buffer.

Para ello, la funcion debe:

- iterar sobre todas las tareas definidas en el scheduler
- comprobar si esta esperando para acceder o si ya tiene acceso al buffer
- actualizar las estructuras de paginacion segun corresponda:
   - La tarea esta solicitando accesso:<BR> 
     - Si es por DMA entonces tenemos que mapear la direccion virtual 0xBABA000 a la fisica 0xF151C000 con permisos de usuario y Read-Only.
     - Si es por copia, mapeamos la direccion virtual pasada por parametro a una __nueva direccion fisica (En caso de primer mapeo)__ y hacemos la copia de datos.

La tarea ya tiene accesso:
- Si es por DMA no tenemos que hacer nada.
- Si es por copia hay que actualizar la copia.

`idt.c`:


```c
void deviceready(void){
  for(int i = 0; i < MAX_TASKS; i++){
    sched_entry_t* tarea = &sched_tasks[i];
    if(tarea->mode == NO_ACCESS) // No solicita acceso al buffer
       continue;
    if(tarea->status == BLOCKED){
        if(tarea->mode == ACCESS_DMA) // Solicita acceso en modo DMA
            buffer_dma(CR3_TO_PAGE_DIR(task_selector_to_CR3(tarea->selector)));
        if(tarea->mode == ACCESS_COPY) // Solicita acceso en modo por copia
            buffer_copy(CR3_TO_PAGE_DIR(task_selector_to_CR3(tarea->selector)), mmu_next_user_page(), tarea->copyDir);

       tarea->status = TASK_RUNNABLE; // Dejamos la tarea lista para correr en una próxima ejecución
    } else {
       if(tarea->mode == ACCESS_COPY){ // Actualizar copia
          paddr_t destino = virt_to_phy (task_selector_to_CR3    (tarea->selector), tarea->copyDir);
          copy_page((paddr_t)0xF151C000, 
          destino);
       }
    }
  }
}

```

### Opendevice

Syscall que permite a las tareas solicitar acceso al buffer según el tipo configurado. En el
caso de acceso por copia, la dirección virtual donde realizar la copia estará dada por el valor
del registro __ECX__ al momento de llamarla.

El sistema __no debe retornar la ejecución__ de las tareas que llaman a la syscall hasta que
se detecte que el buffer está listo y se haya realizado el mapeo DMA o la copia
correspondiente


### Closedevice

`idt.c`:

```C
void closedevice(void){
    // En el caso DMA, la dir virtual de la pagina es siempre la misma
    if(sched_task[current_task].mode == ACCESS_DMA)
        mmu_unmap_page(rcr3(), (vaddr_t)0xBABAB000);
 // En el caso por copia, la dir virtual la tenemos en el struct del scheduler
    if(sched_tasks[current_task].mode == ACCESS_COPY)
        mmu_unmap_page(rcr3(), sched_tasks[current_task].copyDir);
        
    sched_tasks[current_task].mode = NO_ACCESS;
}
```
### Buffer (DMA/Copy)

`mmu.c`:
```c
void buffer_dma(pd_entry_t pd){
   mmu_map_page( (uint32_t)pdm (vaddr_t)0xBABAB000, 
                 (paddr_t)0xF151C000, MMU_U | MMU_P);
}

void buffer_copy(pd_entry_t pd, paddr_t phyDir, vaddr_t copyDir){
    mmu_map_page((uint32_t)pd, copyDir, phyDir, MMU_U | MMU_W | MMU_P);
    copy_page(phyDir, (paddr_t)0xF151C00);
}

```
---
##### funciones auxialiares

`tasks.c`:
```c

uint32_t task_selector_to_CR3(uint16_t selector){
   uint16_t index = selector >> 3; // Sacamos los atributos
   gdt_entry_t* taskDescriptor = &gdt[index];
   tss_t* tss = (tss_t*)((taskDescriptor->base_15_0) | 
                         (taskDescriptor->base_23_16 << 16) | 
                         (taskDescriptor->base_31_24 << 24));
   return tss->cr3;
}

paddr_t virt_to_phy(uint32_t cr3, vaddr_t virt) {

  uint32_t* cr3 = task_selector_to_cr3(task_id);
  uint32_t pd_index = VIRT_PAGE_DIR(virtual_address);
  uint32_t pt_index = VIRT_PAGE_TABLE(virtual_address);
  pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);

  pt_entry_t* pt = pd[pd_index].pt << 12;
  return (paddr_t) (pt[pt_index].page << 12);
}

```







