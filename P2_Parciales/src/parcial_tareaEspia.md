# Solucion

# Idea General

---

## Ejercicio 1

__Ejercicio a.__ 


1. En `idt.c`vamos a  agregar nuevas entries, nivel 3 porque atenderas syscall que seran llamadas desde el nivel de usuario.
2. En `isr.asm` vamos a definir las syscall de las nuevas entries.


`idt.c`:

```c
idt_init(){
    IDT_ENTRY3(98); spy
}
```

`isr.asm`:

~~~asm

global _isr98
_isr98:
    pushad

    push edi; selector tarea a espiar
    push esi; dirección virtual a leer de la tarea espiada
    push ecx; dirección virtual a escribir de la tarea espía
    call syscall_espiar

    add esp, 4*3
    popad
    iret
~~~


```c

int8_t syscall_espiar(uint16_t selector, uint32_t SRC_VIRT_PAGE, uint32_t DST_VIRT_PAGE){

    uint16_t cr3_espiada = selector_to_CR3(selector);
    pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);
    pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt)

    vaddr_t pd_index = VIRT_PAGE_DIR(src_virt);
    vaddr_t pt_index = VIRT_PAGE_TABLE(src_virt);

    if ((pd[pd_index].attrs & MMU_P) && (pt[pt_index].attrs & MMU_P)) {

       // MAPEA LA SRC DE LA TAREA ESPIADA A UNA DIRECCION VIRTUAL DE LA TAREA ACTUAL.
        mmu_map_page(rcr3(), SRC_VIRT_PAGE, Obtener_direccion_fisica(SRC_VIRT_PAGE), MMU_P);
       
        uint8_t* dst = (uint8_t*) DST_VIRT_PAGE; // ya esta mapeado
        uint8_t* src = (uint8_t*) SRC_VIRT_PAGE;

        for (int i = 0; i < 4000; i++) {
          dst[i] = src[i];
        }
  
        mmu_unmap_page(cr3, SRC_VIRT_PAGE);
        mmu_unmap_page(cr3, DST_VIRT_PAGE);

       return 0
    }
    
    


    return -1
}



```
