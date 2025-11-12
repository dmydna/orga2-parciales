

uint32_t getMappings(uint32_t virtual, gdt_entry_t* gdt){

	// virtual = pd_index | pt_index | page_offset
	uint32_t contador = 0;

	for(int i=0; i<GDT_LENGTH; i++){

		// Itero sobre la gdt y busco descriptores de TSS
		if(gtd[i].type == DESC_TYPE_32BIT_TSS ){
		
		// Obtengo la TSS
		uint32_t base_tss = ((uint32_t)gtd[i].base_15_0) | 
                          ((uint32_t)gtd[i].base_23_16) << 16 |
                          ((uint32_t)gdt[i].base_31_24) << 24);

		tss_t* tss_actual = (tss_t*) base_tss ;
		pd_entry_t* pd = (pd_entry_t*)CR3_TO_PAGE_DIR(tss_actual->cr3);

		vaddr_t pd_index = VIRT_PAGE_DIR(virtual);
		vaddr_t pt_index = VIRT_PAGE_TABLE(virtual);
		// checkeo si la pt y la pagina estan presentes.
			if (pd[pd_index].attrs & MMU_P) {
				pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt);
				if(pt[pt_index].attrs & MMU_P){
					contador ++
				}
			}

		}
	}

	return contador;
};


