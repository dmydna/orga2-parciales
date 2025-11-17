# Solucion 

## Idea General

...

---
### Parte 1 :  implementación de las syscalls


```c 
static paddr_t  SHARED_PAIR  0xC0C00000 /*ENUNCIADO*/
static paddr_t  PAIR_MEM_PHY ; /*ESPACIO EN EL KERNEL PARA MAPEAR PAGINAS PEDIDAS POR LAS PAREJAS*/
static pareja_entrie_t* PAREJA_TAREAS[CANT_TAREAS] = (pareja_entrie_t*) PAREJA_ARRAY_FHY /*SUPONE QUE EXISTE EN EL KERNEL*/


typedef struct {
  uint8_t  estado; // 0 buscando, 2 en pareja, 3 pareja rota
  uint8_t  otra;   // id de la otra tarea
  uint8_t  lider;  // id de la tarea lider
} pareja_t; 



bool es_lider(uint8_t task_id){
    pareja_t* pareja = PAREJA_TAREAS[task_id];
    if(pareja->lider == task_id){
        return true;
    }
    return false;
}

task_id pareja_actual(){
    pareja_t* pareja = PAREJA_TAREAS[current_task_id];
    task_id   res = 0;
    // Tiene pareja y no esta rota 
    if(pareja != NULL && pareja.estado != 3){
        if( pareja->otra == current_task_id ){
            res = pareja->lider
        }else{
            res = pareja->lider
        }
    } 
    return res
}

void crear_pareja();{
    if(!pareja_de_actual()){
       PAREJA_TAREAS[current_task_id] = 
       &(pareja_t){
           .estado = 0;
           .otra = 0
           .lider = current_task_id
       }
       sched_disable_task(current_task_id)
    }
    mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs) 
} // Syscall


int juntarse_con(int id_tarea){
    if(!pareja_de_actual()) return 1;
    if(!es_lider(id_tarea)) return 1;
    conformar_pareja(id_tarea);
    return 0
}

void abandonar_pareja(){
    if(!pareja_de_actual()){
        return 
    }

    if(!es_lider(current_task_id)){
        romper_pareja()
    }
    sched_disable_task(current_task_id)

}


void conforma_pareja(uint8_t task_id){
    pareja_t* pareja =  PAREJA_TAREAS[id_tarea]
    // recuerdo que lider se setea al crear pareja_t
    if(es_lider(task_id)){
        pareja.otra = current_task_id
    }else{
        pareja.otra = task_id
    }
    pareja.estado = 2;
    PAREJA_TAREAS[current_task_id] = pareja;
}


void romper_pareja(){
    // Si una tarea tiene pareja_t* :
    //   0. si estado = 0 esta buscando pareja 
    //   2. si estado = 2 esta emparejada
    //   3. si estado = 3 quiere decir que esta sola
    // Si una tarea no tiene pareja y no es lider, tiene puntero a NULL.
    if(pareja_de_actual() == 0){
        return
    }
    pareja_t* pareja = PAREJA_TAREAS[pareja_actual()]
    // rompe de pareja
    pareja->estado = 3
    // sale de la pareja
    PAREJA_TAREAS[current_task_id] = NULL
}


// PAIR_MEM_PHY SUPONE QUE EXISTE EL ESPACIO FISICO RESERVADO

bool page_fault_handler(vaddr_t virt) {
  //....

  // si esta en rango de tareas emperejadas
  if (virt >=  0xC0C00000 && virt <= 0xC0C00000 + 1024*PAGESIZE) {

    // no es lider solitario
    if(!pareja_de_actual() && !es_lider(current_task_id)){
        return
    }
    // si es lider solitario o tiene pareja
    uint32_t attrs = MMU_P;
    if(es_lider(current_task_id) ){
         attrs |= MMU_U | MMU_W;
    }else{
         attrs |= MMU_U;
    }
    uint32_t cr3 = rcr3();
    // mmu_map_page checkea si esta mapeada y si no lo esta lo mapea.
    mmu_map_page(cr3, virt, PAIR_MEM_PHY, attrs);
    return true;
  }
  // ...
}


paddr_t mmu_init_task_dir(paddr_t phy_start) {

  paddr_t cr3 = mmu_next_free_kernel_page();
  paddr_t stack = mmu_next_free_user_page();

  zero_page(cr3);
  
  for (uint32_t i = 0; i < 1024; i++) {
    mmu_map_page(cr3, i<<12, i<<12, MMU_P | MMU_W);
  }

  // Mappeo código, stack y shared
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 0), phy_start , MMU_P | MMU_U);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 1), phy_start + PAGE_SIZE , MMU_P | MMU_U);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 2), stack, MMU_P | MMU_U | MMU_W);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 4), SHARED, MMU_P | MMU_U);

  return (paddr_t)cr3;
}




```