# Recuperatorio primer parcial Diferido - Arquitectura y Organización de Computadoras
#### Segundo cuatrimestre 2024

## Normas generales y modalidad de entrega

- El parcial es **INDIVIDUAL**
- Una vez terminada la evaluación se deberá crear un issue con el *hash* del
  *commit* de entrega.
- Se recomienda leer todo el parcial antes de comenzar a resolverlo

## Régimen de Aprobación

- Para aprobar el examen es necesario obtener como mínimo **un ejercicio aprobado y un ejercicio regular**, demostrando conocimiento suficiente de todos los temas abarcados por el parcial.

## Actualización del fork individual

Es importante que, para esta instancia de parcial, **no creen un nuevo fork de este repositorio** si no que actualicen el mismo fork individual que estaban utilizando para el tp0.

Los pasos para actualizar el repositorio son los mismos que siguieron para el TP1-b y TP1-c, pero ante cualquier duda pueden consultarlos en el archivo [como_actualizar.md (link)](https://git.exactas.uba.ar/ayoc-doc/individual-2c2024/-/blob/master/p1/como_actualizar.md) de esta carpeta.

## Compilación y Testeo

Para compilar y ejecutar los tests cada ejercicio dispone de un archivo
`Makefile` con los siguientes *targets*:

| Comando             | Descripción                                                         |
| ------------------- | ------------------------------------------------------------------- |
| `make test_c`       | Genera el ejecutable usando la implementación en C del ejercicio.   |
| `make test_asm`     | Genera el ejecutable usando la implementación en ASM del ejercicio. |
| `make run_c`        | Corre los tests usando la implementación en C.                      |
| `make run_asm`      | Corre los tests usando la implementación en ASM.                    |
| `make valgrind_c`   | Corre los tests en valgrind usando la implementación en C.          |
| `make valgrind_asm` | Corre los tests en valgrind usando la implementación en ASM.        |
| `make clean`        | Borra todo archivo generado por el `Makefile`.                      |

El sistema de tests de este parcial **sólo correrá los tests que hayan marcado
como hechos**. Para esto deben modificar la variable `EJERCICIO_xx_HECHO`
correspondiente asignándole `true` (en C) ó `TRUE` (en ASM). `xx` es el inciso
en cuestión: `1A`, `1B`, `1C`, `2A`.

# Ejercicio 1

Luego del éxito de nuestro último juego "AyOC - la venganza de los punteros" hemos decidido incursionar al mundo de los juegos de estrategia por turnos para nuestro próximo juego, "Organized Wars".

En su turno, el jugador podrá colocar en un mapa de juego cuadriculado varias unidades de ataque de distintas clases. Cada clase tiene un valor inicial de combustible cargado, el cuál utilizarán en una etapa posterior para realizar acciones como moverse, disparar bombas, etc. Además del combustible precargado, el jugador cuenta con una reserva extra de combustible que puede repartir entre las unidades que desee, potenciando ciertas unidades puntuales.

Dado que es común que los jugadores reposicionen y modifiquen los niveles de combustible de las unidades constantemente durante su turno, el sistema de nuestro juego funciona del siguiente modo:

- Durante el transcurso del turno, cada unidad de ataque agregada se instancia independientemente. 
- Al momento de finalizar el turno, se revisa que el jugador no haya asignado más combustible extra del que tenía disponible en su reserva. De haber asignado combustible correctamente, se efectiviza el final del turno.
- Una vez finalizado el turno, se corre una optimización que reemplaza todas las instancias independientes de unidades equivalentes por una única instancia "compartida" (donde dos unidades son equivalentes si el resultado de aplicar una función de hash dada sobre cada una es el mismo).

Además, el jugador puede hacer que sus unidades batallen con las de otro jugador.
Para esto, es necesario que el jugador mueva su unidad a una adyacente de donde ya haya un enemigo.
En esta primera version, solo puede hacerse entre unidades de la misma clase. Caso contrario, no batallan.
En caso de que el jugador mueva una unidad a un lugar donde hay unidad enemiga, la batalla transcurre de la siguiente forma:
- Se resta el combustible de la unidad enemiga sobre la propia (de manera cruzada).
- Las unidades con 0 o menos combustible son destruidas.

Las batallas son lo último que suceden en el turno, por lo que la optimización se ejecuta justo antes.
Por esta razón, en caso de haber batallas, necesitamos asegurarnos que la optimización siga funcionando de manera correcta.

*Importante*: Para simplificar el cómputo, se decidió que no va a haber unidades en los bordes del mapa, es decir, en la columna/fila 0 ni en la última columna/fila. Por lo tanto, no es necesario validar esos casos bordes. 


![alt text](img/optimizacion.jpg)

---

**a)** Programar en lenguaje assembler una función que, dados un puntero a una unidad de ataque, crea una copia de ella. Las referencias de la copia se inicializarán en 1.

`attackunit_t* copiarUnidad(attackunit_t* base)`

---

**b)** Programar en lenguaje assembler la función

`void comenzarBatalla(mapa_t mapa, uint8_t x, uint8_t y)`

que se utilizará para determinar si, de haber una unidad en la posición dada, hay otra unidad adyacente (en cualquiera de las 8 direcciones) con la que pueda batallar. De haberla, se deberá concretar la batalla y actualizar el resultado en el mapa (hint: utilizar o tomar como dada la función del punto c).

De no haber una unidad en la posición especificada o no haber unidad adyacente, no se debe hacer nada.

---

**c)** Programar en lenguaje assembler una función
    
`void aplicarResultadoBatalla(mapa_t mapa, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)`

que dada la posición de la unidad del jugador de turno (x1,y1) y la posición de una unidad adyacente (x2,y2), aplique los cambios generados por la batalla, es decir, eliminar las unidades destruidas y reducir el combustible de las sobrevivientes. 

Dado que la destrucción por batalla tiene que ser consistente con la optimización, hay que tener cuidado en caso en que las unidades destruidas tengan más de una referencia. Si se fuera a modificar una unidad que está compartiendo instancia con otras por una optimización, se debe crear una nueva instancia individual para la unidad modificada en lugar de modificar la instancia compartida (lo cual resultaría en modificaciones indebidas en otras unidades).

---
**Observaciones:**

- :warning: La instancia compartida podría ser una **nueva instancia** o **alguna de las instancias individuales preexistentes**.
- Los mapas siempre tienen 256x256 (posiciones 0 a 255) de tamaño.
- En los tests solo se modifican e imprimen los primeros 5x5 lugares del mapa para simplificar la visualización, pero se evalua que se resuelva correctamente para el mapa completo.
- Para cada función se incluye un último test que sí trabaja sobre las 256x256 posiciones del mapa. Este test no correrá hasta que los anteriores pasen exitosamente.
- A fin de debuggear puede ser útil revisar la función de hash utilizada en los tests, la cual está definida al principio de `test.c`.

# Ejercicio 2 - SIMD

Este ejercicio consiste en implementar una función de procesamiento de imagenes utilizando el modelo de procesamiento SIMD.

Se **deben** procesar al menos **4 canales en simultáneo, en el mismo registro XMM**, aprovechando instrucciones de procesamiento paralelo. 

:warning: El uso innecesario de registros de propósito general será penalizado.

## La función `sepia`

`void sepia(rgba_t* O, rgba_t* I, uint32_t size);`

Queremos implementar el filtro de tono sepia para aplicarlo a imagenes de mapa de bits de tamaño size x size.

## Detalle del algoritmo

El algoritmo del filtro `sepia` aplica una función a cada pixel de la imagen:
```math
		\begin{array}{ccc}

\mathsf{O}_{i,j}^r & = & saturar(0.5 \cdot \mathsf{suma}_{i,j}) \\
\mathsf{O}_{i,j}^g & = & saturar(0.3 \cdot \mathsf{suma}_{i,j}) \\
\mathsf{O}_{i,j}^b & = & saturar(0.2 \cdot \mathsf{suma}_{i,j}) \\
\mathsf{O}_{i,j}^a & = &  \mathsf{I}_{i,j}^a \\
\end{array}
```

Donde 
```math
O_{i,j}^r, O_{i,j}^g, O_{i,j}^b, O_{i,j}^a
```
es el pixel de la imagen Output (Output para O, e Input para I) en la posición i,j y el canal r,g,b,a respectivamente, y donde
```math
\mathsf{suma}_{i,j} = \mathsf{I}_{i,j}^r + \mathsf{I}_{i,j}^g + \mathsf{I}_{i,j}^b.
```
y saturar se corresponde a aplicar saturación para el tamaño correspondiente.

## Ejercicio
Programar en `asm` utilizando SIMD, la función `sepia`. 

---
