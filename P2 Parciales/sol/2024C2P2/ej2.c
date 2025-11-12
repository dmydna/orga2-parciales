// DEFINO VARIBALES GLOBALES

bool* opendevices[MAX_TASK];
vaddr_t acceso_virt = 0xACCE50;
vaddr_t buffer_virt = OxBABAB000;

// Declaro la nueva rutina de interrupcion con nivel 0 porque es una interrupcion de harware

IDTENTRY0(40)  //interupcion del cartucho
IDTENTRY3(99)  //open devices 
IDTENTRY3(100) //close device

// isr.asm

; rutina de atencion del cartucho
_isr40:
	pushad
	
	call pic_finish1
	call device_ready
	
	popad
	iret


; syscall opendevice
_isr99:
	pushad
	
	call opendevice
	
	popad
	iret

; syscall closedevice
_isr100:
	pushad
	
	call closedevice
	
	popad
	iret




void opendevice(){
	opendevices[current_task] = true;
	sched_disable_task(current_task);	
}

void closedevice(){
	opendevices[current_task] = false;
}


void device_ready(){

	for (int i=0; i<MAX_TASK; i++){
		if(opendevices[i]){
			// Accede a la direccion virtual mapeada en cada tarea que indica el tipo de acceso al buffer	
			
			uint32 cr3_tarea = tss_task[i].cr3
				
			pd_entry_t* pd = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3)
			vaddr_t pd_index = VIRT_PAGE_DIR(acceso_virt);
			vaddr_t pt_index = VIRT_PAGE_TABLE(acceso_virt);
			pt_entry_t* pt = (pt_entry_t*)MMU_ENTRY_PADDR(pd[pd_index].pt);
      
			uint8_t acceso = *(( &pt[pt_index] & 0xFFFFFF000) | VIRT_PAGE_OFFSET(acceso_virt))

		if(acceso == 1){
			buffer_dma(pd)
		}else if(acceso  == 2){
		    vaddr_t virt = tss_task[i].esp + offser_ECX 
			buffer_copy(pd, next_free_user_page(), virt)
		}
		sched_enable_task[i]
		}
	}
}
