
paddr_t VIDEO_BUFF = 0xF151C000;
vaddr_t VIDEO_VIR_ADDR = 0xBABAB000 

void buffer_dma(pd_entry_t* pd){
	
	uint32_t cr3 = (uint32_t) &pd;
	uint32_t attrs = MMU_P | MMU_W 

	mmu_map_page(cr3, VIDEO_VIR_ADDR, VIDEO_BUFF, attrs)

}


/*
Me piden copiar el buffer de video en la direccion fisica 0xF151C000  a la direccion de memoria fisica 'phys' y mapear la direccion 'virt'

1. Mapeo la direccion phy a la direccion virt.
2. luego copia VIDEO_BUFF a phys 

nota: mmu_map_page checkea si fue mapeada.
*/


void buffer_copy(pd_entry_t* pd, paddr_t phys, vaddr_t virt){

	uint32_t cr3 = (uint32_t) &pd;

	copy_page(phys, VIDEO_BUFF)
	mmu_map_page(cr3, virt, phys)

}
