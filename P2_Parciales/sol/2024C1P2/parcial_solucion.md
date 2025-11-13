# Solucion
#### Ejercicio 1


`idt.c`:

Definimos nuevas entradas nivel 0 para la idt, que llamara la syscall de la tarea a espiar 




```c
void idt_init() {

  IDT_ENTRY0(99); TAREA ESPIA
  IDT_ENTRY0(100)

}
```


`isr.asm`:
~~~asm

global _isr33
_isr33:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    ; 2. Leemos la tecla desde el teclado y la procesamos
    in al, 0x60
    push eax
    call tasks_input_process
    add esp, 4
    popad
    iret



~~~






```c

void tarea_espia(uint32_t selector, vaddr_t virt, vaddr_t spy){


    return

}



```