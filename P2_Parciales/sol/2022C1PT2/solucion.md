# Solucion

### Ejercicio 1.

`idt.c`:

Definimos una entrie para la IDT, nivel 3 porque es una syscall.

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

    call sched_next_task
    push eax
    call clearAccessPages

    mov esp, 4
    popad
    iret
~~~


`solucion.c`:
```c

MMU_A 1 << 5

void clearAccessPages(uint16_t selector){
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



