# Solucion 

## Idea General

...

---
### Parte 1 :  implementación de las syscalls

__Ejercicio 1.1__

### `crear_Pareja()`

```c
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
}
```
### `juntarse_con(id_tarea)`


```c
int juntarse_con(int id_tarea){
    if(!pareja_de_actual()) return 1;
    if(!es_lider(id_tarea)) return 1;
    conformar_pareja(id_tarea);
    return 0
}
```

### `abandonar_pareja()`

Cuando una tarea abandona la `pareja_t` entonces tendra un puntero `NULL`.
La otra parte aun conservara el puntero a la `pareja_t`.

```c
void abandonar_pareja(){
    if(!pareja_de_actual()) return;
    if(!es_lider(current_task)) romper_pareja();
    sched_disable_task(current_task)
}
```

__Ejercicio 1.2__

##### `conformar_pareja(task_id tarea)`

Cuando se conforma una pareja ambas tareas comparten el mismo puntero a una `pareja_t`


```c
void conformar_pareja(uint8_t task_id){
    pareja_t* pareja =  sched_task[task_id].pareja
    sched_task[current_task].pareja = pareja;

    if(es_lider(task_id)){
        pareja->compañera = current_task
    }else{
        pareja->compañera = task_id
    }
    pareja->estado = EMPAREJADA;

}
```

##### `romper_pareja()`

Si una tarea no tiene pareja y no es lider, tiene puntero a NULL.

```c
void romper_pareja(){
    if(pareja_de_actual() == 0) return;
    pareja_t* pareja = sched_task[pareja_actual()].pareja;
    pareja->estado = ROTA;  // rompe de pareja
    sched_task[current_task].pareja = NULL;  // sale de la pareja
}
```

__Ejercicio 1.3__



##### `pareja_de_actual()` 

```c
task_id pareja_actual(){
    pareja_t* pareja = sched_task[current_task_id].pareja;
    // Tiene pareja y no esta rota 
    if(!pareja || 
       pareja->estado == BUSCANDO ||
       pareja->estado = ROTA)
       return (task_id)0;

    if( pareja->compañera == current_task_id ){
        return (task_id)pareja->lider;
    }else{
        return (task_id)pareja->compañera;
    }
}
```

##### `es_lider(task_id tarea)` 

```c
bool es_lider(uint8_t task_id){
    pareja_t* pareja = sched_task[task_id].pareja;
    if(pareja && pareja->lider == task_id){
        return true;
    }
    return false;
}
```

##### `aceptando_pareja(task_id tarea)`



---


```c 

typedef enum  {
  BUSCANDO,
  ENPAREJADA,
  ROTA
} pareja_state_t;




static paddr_t  SHARED_PAIR  0xC0C00000 /*ENUNCIADO*/
static paddr_t  PAIR_MEM_PHY ; /*ESPACIO EN EL KERNEL PARA MAPEAR PAGINAS PEDIDAS POR LAS PAREJAS*/


typedef struct {
  int16_t selector;
  task_state_t state;
  pareja_t* pareja;
} sched_entry_t;



typedef struct {
  uint8_t  estado;     // 0 buscando, 2 en pareja, 3 pareja rota
  uint8_t  compañera;  // id de la otra tarea
  uint8_t  lider;      // id de la tarea lider
  uint32_t pt_compañera; // guardar una copia de la pt del area de parejas
} pareja_t; 

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


`MMUC`:

Modifico para que cada tarea tenga 4MiB exclusivos para usar en pareja (osea un pde).
La idea es que la parte lider acceda a su propio espacio de 4MiB
y la parte compañera mapea esta misma area en su tabla de paginacion, (antes guardando un copia para no perder los 4MiB propios).

ej: `pd_lider[index_pareja_area] = pd_compañera[index_pareja_area]`

```C
paddr_t mmu_init_task_dir(paddr_t phy_start) {

  paddr_t cr3 = mmu_next_free_kernel_page();
  paddr_t stack = mmu_next_free_user_page();
  paddt_t pair = mmu_next_free_user_page();

  zero_page(cr3);
  
  for (uint32_t i = 0; i < 1024; i++) {
    mmu_map_page(cr3, i<<12, i<<12, MMU_P | MMU_W);
  }

  // Mappeo código, stack y shared
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 0), phy_start , MMU_P | MMU_U);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 1), phy_start + PAGE_SIZE , MMU_P | MMU_U);
  mmu_map_page(cr3, TASK_CODE_VIRTUAL + (PAGE_SIZE * 2), stack, MMU_P | MMU_U | MMU_W);
  mmu_map_page(cr3, TASK_SHARED_PAGE, SHARED, MMU_P | MMU_U);


  // Reservo 4MiB para cada TAREA (area PAREJA)
  for (uint32_t i = 0; i < 1024; i++) {
     paddr_t p_next = mmu_next_free_user_page();
      mmu_map_page(cr3, 0xC0C00000 + (PAGE_SIZE * i), p_next, MMU_P | MMU_U | MMU_W);
  }

  return (paddr_t)cr3;
}
```