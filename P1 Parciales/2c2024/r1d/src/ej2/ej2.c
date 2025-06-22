#include "ej2.h"

/**
 * Marca el ejercicio 2A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - ej2a
 */
bool EJERCICIO_2A_HECHO = false;

/**
 * OPCIONAL: implementar en C
 *
 * Dada una imagen I le aplica el filtro sepia para obtener una imagen de salida O:
 * O[i,j].r = 0.5 * (I[i,j].r + I[i,j].g + I[i,j].b)
 * O[i,j].g = 0.3 * (I[i,j].r + I[i,j].g + I[i,j].b)
 * O[i,j].b = 0.2 * (I[i,j].r + I[i,j].g + I[i,j].b)
 * O[i,j].a = I[i,j].a
 * 
 * Parámetros:
 *   - dst:    La imagen destino O. Es RGBA (8 bits sin signo por canal).
 *   - src:    La imagen origen I. Es RGBA (8 bits sin signo por canal).
 *   - size:  El ancho y alto en píxeles de `dst` y `src`.
 */

void ej2a(rgba_t* dst, rgba_t* src, uint32_t size) {
}