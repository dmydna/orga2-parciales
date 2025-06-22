// Declaro la nueva rutina de interrupcion con nivel 0 porque es una interrupcion de harware

IDTENTRY0(40)

//open devices
IDTENTRY3(99) 
//close device
IDTENTRY3(100)


// Defino la rutina en ISR.ASM


Global _isr40

_isr40:
	pushad
	call pic_finish1
	call device_ready
	popad
	iret

void device_ready(){
	
	for (int i=0; i<opendevices.size; i++){
		if(opendevices[i]){
			
			cr3_tarea = tss_task[i].cr3
			pd_entry_t* pd = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3)

		if(accesos[i] ==1){
			buffer_dma()
		}else if(accesos[i] == 2){
			pila = tss_task[i].esp
			virt = pila_offset_ECX
			buffer_copy(pd, next_free_user_page(), virt
		}
		enable_task[i]
		}
	}

}


global _isr99

_isr99:
	pushad
	call guardarAcceso
	call actualizar_open_devices
	call deshabilitar_tarea_activa

	call sched_next_task
	
	mov word [sched_task_selector],ax
	jmp far [sched_task_offset]
	
	popad
	iret

	




