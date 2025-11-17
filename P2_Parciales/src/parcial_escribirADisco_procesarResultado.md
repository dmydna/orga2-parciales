### Arquitectura y Organización de Computadoras - DC - UBA 
# Segundo Parcial - Primer Cuatrimestre de 2023

## Enunciado

### Ejercicio 1 - (70 puntos)

En un sistema similar al que implementamos en los talleres del curso (modo protegido con paginacion activada), se tiene 5 tareas en ejecucion y una sexta que procesa los resiñtados enviados por las otras. Cualquiera de estas 5 tareas puede en algun momento realizar una cuenta y enviar el resultado de la misma a la sexta tarea para que lo utilice de manera inmediata. Para ellos la tarea que relaizo la cuenta guardara el resultado de la misma EAX. A continuacion, la tarea que hizo la cuenta cedera el tiempo de ejecucion que le queda a la tarea que va procesar el resultado (lo recibira en EAX). Tener en cuenta que la tarea que hizo la cuenta no volvera a ser ejecutada hasta que la otra tarea no hay terminado de utilizar el resultado de operacion realizada.

Se desea agregar al sistena una syscall para que la tareas despues de realizar la cuenta en cuestion puedan cederle el tiempo de ejecucion a la tarea que procesara el resultado.

**Se pide:**

- a. Definir o modificar las estructuras de sistema necesarias para que dicho servicio pueda ser invocado.
- b. Implementar la syscall que llamaran las tarea. 
- c. Dar el pseudo-codigo de la tarea que procesa resultados (no importa como lo procese).
- d. Mostrar un psedo-codigo de la funcion ``sched_nex_task` para que funcione de acuerdo a las necesidades de este sistema. Responder: ¿Que problemas podrian surgir dadas las modificaciones al sistema?¿Como lo solucionarias?

_Se recomienda organizar la resolucion de ejercicio realizando paso a paso los itrmas mencionados anteriormente y explicar las decisiones que toman._

**Detalles de implementacion:**

- Las 5 tareas originales corren en nivel 3.
- La sexta tarea tendra nivel de privilegio 0.
 

### Ejercicio 2 - (30 puntos)

Se desea implementar una modificacion sobre un kernel como el de los talleres: en el momento de desalojar una pagina de memoria que fue modificada esta suele escribir a disco, sin embargo se desea modificar el sistema para que no sea escrita a disco si la pagina fue modificada por una tarea especifica.
Se les pide que hagan una funcion que, dado el CR3 de la tarea mencionada y la direccion fisica de la pagina a desalojar, diga si dicha pagina deber ser escrita a disco o no.
La funcion a implementar es:

```c
uint8_t Escribir_a_Disco(int32_t cr3, paddr_t phy);
```
 **Detalles de implementacion**

- Si necesitan, pueden asumir que el sistema tine segmentacion flat
- NO DEBEN modificar las estructuras del kernel para llamar funcion que estan creando.<BR>Solamente deben programar la funcion que se pide.
 
**A tener en cuenta para la entrega (par todos los ejercicios)**

- Esta permitido utilizar las funciones desarrolladas en los talleres.
- Es necesario que se incluya una explicacion con sus palabras de la idea general de las soluciones.
- Es necesario escribir todas las asunciones que haga sobre el sistema.
- Es necesario la entrega de codigo o pseudocodigo que implemente las soluciones.
