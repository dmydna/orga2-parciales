### Ejercicio 1

Se tienen __5 tareas en ejecucion__ y una __sexta que procesa los resultados__ enviados por las otras. Cualquiera de estas 5 tareas puede en algun momento realizar una cuenta y enviar el __resultado__ (en EAX) a la sexta tarea para que lo utilice de manera inmediata. 

A continuacion, la tarea que realizo la cuenta le __cedera el tiempo de ejecucion__ que le queda a la sexta tarea, la cual va a procesar el resultado (lo recibira en EAX). Tener en cuenta que la tarea que hizo la cuenta __no volvera a ser ejecutada__ hasta que la sexta tarea no haya terminado de utilizar el resultado de la operacion realizada.

Se desa agregar al sistema una `syscall` para que las tareas despues de realizar la cuenta en cuestion, puedan __cederle el tiempo de ejecucion__ a la tarea que procesara el resultado. 



~~~nasm
; isr.asm

_isr47:
   pushad
   push eax
   call habilitar_tarea_6
   pop eax
   jmp far [task_6_offset]:0
   popad
   iret
~~~

```c

//sched.c

uint8_t habilitar_tarea_6 (uint32_t resultado){
   // uso current_task como variable global y task_6_id
   sched_disable_task(current_task);
   sched_enable_task(task_6_id)
   // ahora acceda a la TSSde la tarea 6
   // el resultado de la tarea A.
   // busco el selector de la TSS de la tarea 6 en sched_entry_t, que tiene los descriptores y todas las tareas
   uint16_t index_t6_tss = sched_entry_t[task_6_id] >> 3;
   vaddr_t tss_t6_addr = (gdt[task_t6_id].base)
   tss_t* tss_t6 = (tss_t*)tss_t6_addr;
   tss_t6 -> eax = resultado;
   // en todo el proceso anterior se accede al descriptorde TSS (que esta en la GDT) para luego acceder a la TSS de la tarea 6 y cambiarle el valor de su registro eax    
   tarea_desalojada = current_task // es una variable global
   return current_task;              
}

task_6_offset dw 0
task_6_selector dw 0



TAREA 6
   while(true){
   // incializar sus variables
   // procesar el dato
   // Hablitar la tarea A
   sched_enable_task (tarea_desalojada);
   sched_disable_task(task_6_id);  
   cambiar_tarea();
   }   
}

```

~~~nasm
global cambiar_tarea

cambiar_tarea:
   call sched_next_task
   mov word[sched_task_selector], ax
   jmp far [sched_task_offset]:0
   popad
   ret
~~~

### Ejercicio 2

Se pide implementar una modificacion sobre el kernel, en el momento de desalojar una pagina de memoria que fue modificada esta se suele escribir a disco, sin embargo se desa modificar el sistema para que no sea escrita a disco si la fue modificada por una tarea especifica.

- Implementar una funcion que dado el CR3 de la tarea mencionada y la direccion fisica de la pagina a desalojar, diga si dicha pagina deder sser escrita a disco o no.
  
 `uint8_t Escribir_a_Disco(int32_t cr4, paddr_t phy)` 


```c

#define MMU_D (1 << 5)

uint8_t escribir_a_disco(uint32_t cr3, paddr_t phy){
   uint32_t pd_addr = (cr3 & 0xFFFF000);
   pd_entry_t * pd =  (pd_entry_t*)pad_addr
   uint8_t res = 0;
   for(int i = 0; i < 1024; i++){
      pt_entry_t* pt = (pt_entry_t*) pd[i]
      if (pt->attr)& MMU_P) == 1){
         res |= checkep_pt(pt, phy)      
      }
   
   }   
   return res

}




uint8_t checkep_pt(pt_addr_t* pt, paddr_t phy){
   uint8_t res = 0;
   for(int j = 0; j < 1024; j++ ){
     if (pt[j]->attr & MMU_P){
         if(pt[i]->page) == phy & 0xFFFFF000{
             if(pt[j]->attr & MMU_D){
                res = 1
             }
          }
     }   
      
   }
   return res

}


```
