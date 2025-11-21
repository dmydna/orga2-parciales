## Solucion

### Observacion General


- A cada tarea del sistema se le asigna y le corresponde una única estructura: `reserva_por_tarea`. <br>
 Esta estructura contiene un array de tipo `reserva_t`, llamado `array_reservas`, que se utiliza para registrar las   reservas de memoria física de __la tarea__.

- Cada tarea tiene un límite estricto de 4 MiB (Megabytes) de memoria que puede reservar en total. <BR> 
 Esto significa que la suma total de las páginas reservadas (campo `tamanio`) por todas las entradas (`reserva_t`) dentro del `array_reservas` no debe exceder los 1024.

- El __array de reservas__ de una tarea:
  1. Supone que todos sus entries (`reserva_t`) ya estan inicializados con `estado == 0`.
  2. No se agregan nuevas entries , solo se modifican las que tengan `estado == 0`.
  3. Tambien supone que el restos de los campos de una `reserva_t` no importan si su `estado == 0`.

---

#### Detalles de Implementacion

##### Malloco y Chau

- `Malloco` va a pedir paginas fisica del area del kernel.
- `Chau` no va a desmapear las paginas, solo va a marcar para liberar.

##### Garbage collector

- Se va a encargar desmapear las paginas que fueron marcada para liberar por chau, 
- Solo se va a liberar memoria cada 100 tick del clock en otro caso no hace nada.
- El contador de ticks es un contador global, no cuenta los ticks de cada tarea.
 
---

#### Ejercicio 1: 

Para integrar malloco y chau para poder pedir memoria en nivel de usuario:

1. En `idt.c`: se agregan nuevas entries (nivel 3) en la IDT, para atender las syscall malloco y chau, 
2. En `isr.asm`: Implementamos las syscall de malloco y chau
3. En `mmu.c`: Hay que modificar el `page_fault_hander` para ir reservando las paginas pedidas por malloco (cuando son accedidas)



`idt.c`:
```c
void idt_init() {
  // Malloco
  IDT_ENTRY3(99); 
  // Chau
  IDT_ENTRY3(100); 
}
```

`isr.asm`:

- Declaro las las interrupciones de malloco y chau
- Modifico la interrupcion del page fault para que mate a la tarea actual y cambie a la proxima tarea si corresponde.

~~~asm
global _isr99
_isr99:
    pushad
    push eax
    call malloco
    mov [esp + offset_EAX 28], eax
    add esp, 4
    popad
    iret

global _isr100
_isr99:
    pushad
    push eax
    call chau
    mov [esp + offset_EAX 28], eax
    add esp, 4
    popad
    iret

_isr14:
	; Estamos en un page fault.
	pushad
    ; COMPLETAR: llamar rutina de atención de page fault, pasandole la dirección que se intentó acceder
    mov eax, cr2
    push eax
    call page_fault_handler
    add esp, 4
    cmp al, 1
    je .fin
    cmp al, 0
    je .ring_exception
    cmp al, 3
    je .malloc_exception

    malloc_exception:
    ; mata tarea actual
    mov eax, [current_task]
    push eax
    call sched_killed_task
    ; salta a la prox tarea de forma inmediata
    call sched_next_task
    mov word [sched_task_selector], ax
    mov esp, 4
    jmp far [sched_task_offset]

    .ring0_exception:
	; Si llegamos hasta aca es que cometimos un page fault fuera del area compartida.
    call kernel_exception
    jmp $

    .fin:
	popad
	add esp, 4 ; Pisar error code
	iret
~~~


## Ejercicio 2:

Para integrar el garbage_collector:

0. En `mmu.c`: creamos la PD para la tarea en el espacio del kernel.
1. En `tss.c`: creamos la tss para la tarea.
2. En `task.c`: Hay que agrega el selector de la tarea en la gdt.
3. En `sched.c`: definimos un contador de tick. 
3. En `isr.asm`: Hay que modificar la rutina de interrupcion del clock `isr32` <BR> para que llame a la tarea del garbage_collector cada 100 ticks del clock 


### Garbage_collector 

`isr.asm`:

Modifico la interrupcion del clock para que <BR>
llame a la tarea `garbage_collector`  despues de 100 ticks del clock

~~~asm
extern inc_tick_counter

global _isr32
_isr32:
 pushad
 call pic_finish1

 call inc_tick_counter
 cmp eax, 100
 je (GDT_IDX_TASK_GARBAGE << 3):0   

 call next_clock
 call sched_next_task
 ; ...
 iret
~~~


`mmu.c`:

- Creo la PD Para la tarea en el espacio del kernel.

```c
paddr_t create_cr3_for_kernel_task() {
 // Inicializamos el directorio de paginas
 paddr_t task_page_dir = mmu_next_free_kernel_page();
 zero_page(task_page_dir);
 // Realizamos el identity mapping
 for (uint32_t i = 0; i < identity_mapping_end; i += PAGE_SIZE) {
    mmu_map_page(task_page_dir,i, i, MMU_W);
 }
 return task_page_dir;
}
```

`tss.c`:

- Creo la tss de la tarea 

```c

tss_t tss_create_kernel_task(paddr_t code_start) {
  vaddr_t stack = mmu_next_free_kernel_page();
  return (tss_t) {
    .cr3 = create_cr3_for_kernel_task(),
    .esp = stack + PAGE_SIZE,
    .ebp = stack + PAGE_SIZE,
    .eip = (vaddr_t)code_start,
    .cs = GDT_CODE_0_SEL,
    .ds = GDT_DATA_0_SEL,
    .es = GDT_DATA_0_SEL,
    .fs = GDT_DATA_0_SEL,
    .gs = GDT_DATA_0_SEL,
    .ss = GDT_DATA_0_SEL,
    .ss0 = GDT_DATA_0_SEL,
    .esp0 = stack + PAGE_SIZE,
    .eflags = EFLAGS_IF,
  };
}

```



`task.c`:

- Agrego el descriptor de la tarea a la GDT
- Defino la implementacion de la tarea

```c
static int8_t create_garbage_task() {
  size_t gdt_id;
  for (gdt_id = GDT_TSS_START; gdt_id < GDT_COUNT; gdt_id++) {
    if (gdt[gdt_id].p == 0) {
      break;
    }
  }
  kassert(gdt_id < GDT_COUNT, "No hay entradas disponibles en la GDT");

  int8_t task_id = sched_add_task(gdt_id << 3);
  tss_tasks[task_id] = tss_create_kernel_task(&garbage_collector);;
  gdt[gdt_id] = tss_gdt_entry_for_task(&tss_tasks[task_id]);
  return task_id;

}


extern tss_tasks

void garbage_collector(void) {

   while(true){
    if (ticks_counter <= 100) continue;
  
    for (int task_id = 0; task_id < MAX_TASKS; task_id++) {
     if (task_id == current_task) continue;
     
     tss_curent_task = tss_tasks[task_id];
     reserva_por_tarea* reserva_tarea_actual = dameReservas(task_id);
    
     for(int i=0; i< reserva_tarea_actual->tamanio; i++ ){
      reserva_t reserva_actual = reserva_tarea_actual->array_reservas[i]
      if(reserva_actual->estado == 3){
        mmu_unmap_page(tss_current_task->cr3, reserva_actual->virt) 
      }
     }
   }
 }
}

```

`sched.c`

- Agrego contador de ticks

```c
static uint8_t contador_de_ticks = 0;


void sched_killed_task(int8_t task_id) {
  sched_tasks[task_id].state =  TASK_SLOT_FREE,;
}

uint8_t inc_tick_counter() {
 uint8_t count = contador_de_ticks ++;
 return count;
}
```

## Ejercicio 3

#### Malloco 

`mmu.c`:

```c
void* malloco(size_t size){
  if(size > PAGE_SIZE * 1024){
    return null;
  }
  reservas_por_tarea* malloco_reservas = dameReservas(int task_id);
  uint32_t reservas_tarea       = malloco_reservas->array_reservas;
  uint32_t reservas_tarea_size  = malloco_reservas->reservas_size;
  
  uint32_t cantMemoriaReservada = 0;

  // checkeamos si hay memoria disponible
  for(uint32_t i=0; i < reservas_tarea_size; i++){
    if( reservas_tarea[i].estado == 1 ){
      cantMemoriaReservada += reserva_tarea[i].tamanio;
    }
  }

  if(cantMemoriaReservada >= 1024 
  || cantMemoriaReservada + size > 1024){
    return NULL;
  }

  for(uint32_t i=0; i < reservas_tarea_size; i++){
    // busco una entry disponible 
    if( reservas_tarea[i].estado; == 0 )
    { 
      reservas_tarea[i].virt    = malloco_virt_start + cantMemoriaReservada * PAGE_SIZE;
      reservas_tarea[i].tamanio = size;
      reservas_tarea[i].estado  = 1;
    }
  }

}

```

#### Chau

`mmu.c`

```c
void chau(virtaddr_t virt){

  reservas_por_tarea* malloco_reservas = dameReservas(int task_id);
  uint32_t reservas_tarea       = malloco_reservas->array_reservas;
  uint32_t reservas_tarea_size  = malloco_reservas->reservas_size;
  
  for(uint32_t i=0; i < reservas_tarea_size; i++){
    uint32_t  reserva_virt_start = reservas_tarea[i].virt;
    uint8_t   reserva_estado     = reservas_tarea[i].estado;
    if(reserva_estado == 1 && virt == reserva_virt_start){ 
    // solo marca para liberar, garbage_collector va a liberar (desmapear)
     reserva_actual.estado = 3;
     return true;
    }
  }
}

```


#### Pagefaul (modificacion )

`mmu.c`

Modifica el pagefaul handler para manejar el acceso a direcciones en el rango de malloco.

```c

uint8_t page_fault_handler(vaddr_t virt) {
  print("Atendiendo page fault...", 0, 0, C_FG_WHITE | C_BG_BLACK);

  // Checkea que este en rango de malloco
  if(virt >= malloco_virt_start && virt <= malloco_virt_end){
    if(esMemoriaReservada(virt)){
      uint32_t attrs = MMU_P | MMU_U | MMU_W;
      uint32_t cr3 = rcr3();
      paddr_t phy = mmu_next_free_kernel_page();
      mmu_map_page(cr3, virt, phy, attrs);
    }else{
      // Intenta acceder a memoria de malloco no reservada

      // marca toda la memoria reservada para liberar
      reserva_t* reservasTareaActual = dameReservas(current_task_id)->array_reservas;
      uint32_t tamanioReservas = dameReservas(current_task_id)->tamanio;

      for(int i = 0; i< tamanioReservas; i++){
        if(reservasTareaActual[i].estado == 1){
          for(int i = 0; i < reservasTareaActual[i].tamanio; i++)  
            // marca para liberar todas las paginas reservadas por el bloque
            chau(reservasTareaActual[i].virt + i * PAGESIZE);
        }
      }
      return 3; // malloco exception
    }
  }

  // Chequeemos si el acceso fue dentro del area on-demand
  // En caso de que si, mapear la pagina
  if (virt >= ON_DEMAND_MEM_START_VIRTUAL && virt <= ON_DEMAND_MEM_END_VIRTUAL) {    
    // si esta en rango On-demand  
    // ....
    return 1;
  }
  // Si el page fault no se pudo atender o está fuera del área on-demand
  return 0;
}


```


---



### Extra:

Codigo de los aux cuya implementacion no son proporcionadas (y tampoco se pide hacerlo) por el enunciado. 


`mmu.c`: 
```c
// supone que existe la direccion fisica KERNEL_MALLOCO_ARRAY
static reservas_por_tarea* MALLOCO_RESERVAS = (rerseva_por_tarea*) KERNEL_MALLOCO_ARRAY; 

static vaddr malloco_virt_start = 0xA10C0000 /*ENUNCIADO*/
static vaddr malloco_virt_end   = 0xA10C0000 + PAGESIZE * 1024 /*4MiB*/

/*ENUNCIADO*/
typedef struct {
  uint32_t task_id;
  reserva_t* array_reservas;
  uint32_t reservas_size;
} reservas_por_tarea; 

/*ENUNCIADO*/
typedef struct {
  uint32_t virt; //direccion virtual donde comienza el bloque reservado
  uint32_t tamanio; //tamaño del bloque en bytes
  uint8_t estado; //0 si la casilla está libre, 1 si la reserva está activa, 2 si la reserva está marcada para liberar, 3 si la reserva ya fue liberada
} reserva_t; 


/*IMPLEMENTACION DE AUX*/
reservas_por_tarea* dameReservas(int task_id){
  return MALLOCO_RESERVAS[task_id];
}

/*IMPLEMENTACION DE AUX*/
uint8_t esMemoriaReservada(virtaddr_t virt){

  // Checkea que este en rango de malloco
  if(!(virt >= malloco_virt_start && virt <= malloco_virt_end)){
    return false
  }

  reservas_por_tarea* malloco_reservas = dameReservas(int task_id);

  uint32_t reservas_tarea_size = malloco_reservas->reservas_size;
  uint32_t reservas_tarea = malloco_reservas->array_reservas;

  for(uint32_t i=0; i < reservas_tarea_size; i++){

    reserva_t reserva_actual     = reservas_tarea[i] 
    uint32_t  reserva_virt_start = reserva_actual.virt;
    uint32_t  reserva_tamanio    = reserva_actual.tamanio;
    uint8_t   reserva_estado     = reserva_actual.estado;
    uint32_t  reserva_virt_end   = virt_start + PAGESIZE * tamanio;

    // Checkea que este dentro del rango de una reserva valida
    if(reserva_estado == 1 && virt >= reserva_virt_start && 
     virt  <= reserva_virt_end){ 
     return true;
    }
  }

  return false;
}
```
