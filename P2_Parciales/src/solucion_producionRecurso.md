# Solucion

<details>
 <summary>Nota al margen</summary>
 
Pensar que hay un `array_pedidos[MAX_TASK]` universal para todas las tareas.

 
 ```c
 task_id_t hay_consumidora_esperando(recurso_t recurso)
 ```
 indica cual es la siguiente consumidora activa. (Probablemente obtienen las tarea que elevaron un pedido al systema en `array_pedidos`

</details>

Idea General


---

Ejercicio 1.

- En `idt.c` agrego las nuevas entries para la idt, nivel 3 porque son syscall que seran llamadas por usuario. 
- En `isr.asm` vamos a definir las syscall.
- En `sched.c` vamos a agregar un estado para las tarea. <br> 


`idt.c`:
```c
idt_init(){
    //solicitar recurso
    IDT_ENTRY3(98);
    //recurso_listo
    IDT_ENTRY3(99)
    //iniciar produccion
    IDT_ENTRY0(40)
}
```

`ìsr.asm`:

```asm
global _isr98



; La idea es que la tarea cuando se vuelva a  ejecutar lo hara despues del jmp far
; la misma quedara atrapada hasta conseguir prioridad para ser la siguiente tarea que
; obtendra el recurso.

; 1. La tarea no volvera a modo de usuario hasta obtener el recurso.
; 2. La tarea cada vez que se ejecute checkeara si tiene productora libre.
; 3. La tarea en espera buscara obtener la prioridad en cada ejecucion.

; de esta manera se consigue que cualquier productora  libre atienda cualquier pedido valido con prioridad

_isr98:
    pushad
    push eax; mando parametro
    .while:
        call sched_solicitar_recurso
        cmp eax, 0
        jne .fin

        call sched_next_task
        mov word [sched_task_selector], ax
        jmp far [sched_task_offset]

        jmp .while
        .fin:
        call tasks_tick; ...isr32
        call tasks_screen_update; ...isr32
    add esp, 4
    popad
    iret

global _isr99
_isr99:
    pushad

    push eax; mando parametro
    call sched_recurso_listo

    ; desalojo tarea 
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    add esp, 4
    popad
    iret
```


`sched.c`:

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  produccion_t produccion;
} sched_entry_t;


typedef struct {
    produccion_state_t state;
    pedido_t pedido;
} produccion_t;

typedef struct{
    uint8_t esPedidoValido;  // indica si el pedido esta activo
    uint8_t tienePrioridad;  // prioridad
    recurso_t* recurso;
} pedido_t


typedef enum {
    OCUPADA, // productora
    LIBRE,   // productora
    ESPERANDO_RECURSO // consumidora
    RECURSO_LISTO     // consumidora
} produccion_state_t;


uint8_t sched_solicitar_recurso(recurso_t recurso){

    produccion_t * produccion = &sched_task[current_task].produccion;
    pedido_t* pedido = &produccion->pedido

    if(produccion->state == RECURSO_LISTO){
        pedido_consumidora->esPedidoValido = false;
        return true;
    }

    if(!pedido->esPedidoValido){
        // incializa pedido la primera vez.
        pedido->esPedidoValido = true
        pedido->tienePrioridad = false
        pedido->recurso = &recurso
        produccion->state = ESPERA_RECURSO
    }

    // la tarea que llegue primero cuando una productora este libre
    // obtendra prioridad y sera la proxima en ser atendida por la productora.
    if(produccion->state == ESPERA_RECURSO &&
      !pedido->tienePrioridad){
        if(hay_tarea_disponible_para_recurso(recurso) != 0){
            pedido->tienePrioridad = true;
        }
    }
    return false
}


void sched_recurso_listo(recurso_t recurso){

    copiar_recurso(TASK_VIRT_CONSUMIDORA, TASK_VIRT_PRODUCTORA);
    
    task_id_t consumidora_id = para_quien_produce(task_id_t id_tarea);
    produccion_t* produccion_consumidora = &sched_tasks[consumidora_id].produccion;
    pedido_t* pedido_consumidora = &produccion->pedido;

    produccion_consumidora->state = RECURSO_LISTO;

    if(hay_consumidora_esperando(recurso) != 0){
        produccion_t* produccion_productora = &sched_tasks[current_task].produccion;
        produccion_productora->state = LIBRE;
        restaurar_tarea(current_task);
    }
}



vaddr_t TASK_VIRT_CONSUMIDORA = 0x0BBBB000;
vaddr_t TASK_VIRT_PRODUCTORA = 0x0AAAA000;


paddr_t mmu_init_task_dir(paddr_t phy_start) {

  paddr_t cr3 = mmu_next_free_kernel_page();
  paddr_t stack = mmu_next_free_user_page();

  paddr_t TASK_PHY_CONSUMIDORA = mmu_next_free_user_page();
  paddr_t TASK_PHY_PRODUCTORA  = mmu_next_free_user_page();

  zero_page(cr3);

  for (uint32_t i = 0; i < 1024; i++) {
    mmu_map_page(cr3, i<<12, i<<12, MMU_P | MMU_W);
  }
  // Mappeo código, stack y shared
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 0), phy_start , MMU_P | MMU_U);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 1), phy_start + PAGE_SIZE , MMU_P | MMU_U);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 2), stack, MMU_P | MMU_U | MMU_W);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 4), SHARED, MMU_P | MMU_U);
  // Mapeo paginas consumidora y productoras
  mmu_map_page(cr3, TASK_VIRT_CONSUMIDORA, TASK_PHY_CONSUMIDORA, MMUP | MMU_U);
  mmu_map_page(cr3, TASK_VIRT_PRODUCTORA , TASK_PHY_PRODUCTORA , MMUP | MMU_U);

  return (paddr_t)cr3;
}
```

`auxiliares`:

```c
int  copiar_recurso(uint32_t* direccion_solicitante, uint32_t* direccion_productora){
    task_id_t tarea_solicitante = para_quien_produce(task_id_t id_tarea)
    uint32_t cr3_solicitante = obtenerCR3(sched_task[tarea_solicitante].selector);
    uint32_t cr3_productora = rcr3()
    paddr_t direccion_fisica_de_solicitante = obtenerDireccionFisica(cr3_tarea_solicitante, direccion_solicitante);
    void mmu_map_page(uint32_t cr3_productora, vaddr_t direccion_productora, (paddr_t) 0xFAFAFA, uint32_t MMU_P) 
    copy_page(paddr_t direccion_fisica_solicitante, direccion_productora)
}




uint32_t obtenerDireccionFisica(uint32_t cr3, uint32_t* virt){

    pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);
    vaddr_t pd_index = VIRT_PAGE_DIR(virt);
    vaddr_t pt_index = VIRT_PAGE_TABLE(virt);

    if (!(pd[pd_index].attrs & MMU_P)) {
        return 0;
    }

    pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt);

    if (!(pt[pt_index].attrs & MMU_P)) {
        return 0;
    }
    paddr_t phy = MMU_ENTRY_PADDR(pt[pti].page);

    return phy;   
}




task_id_t obtenerCR3(uint16_t selector){
    tss_t* TSS_TAREA = gdt[selector>>3];
    uint32_t cr3_tarea = TSS_TAREA->cr3;
    return cr3_tarea;
}
```


**Nota:** 
La estructura `produccion_t` almacena el recurso, el pedido y el estado de las operaciones de las consumidoras y productoras:
- `para_quien_produce` usara las tareas que estan en espera de ese recurso y que tengan prioridad (si hay mas una con prioridad toma la primera que encuentra).
- `hay_consumidora_esperando` vera todas las tareas en espera por el recurso
- `hay_tarea_disponible_para_recurso` usara el estado LIBRE o OCUPADO de la tarea productora.





