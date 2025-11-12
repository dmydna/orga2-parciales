



// idt.c 
// Interrupciones de Software [INT 48 HASTA INT 255]
// uso IDENTRY3 porque es una interrupcion de sofware (llamada desde nivel de usuario 3)

IDENTRY3(99);


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



