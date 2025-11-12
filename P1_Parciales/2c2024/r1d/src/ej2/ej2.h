#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/**
 * Un píxel RGBA.
 *
 * Campos:
 *   - r: La cantidad de rojo en el píxel. Va de 0 a 255.
 *   - g: La cantidad de verde en el píxel. Va de 0 a 255.
 *   - b: La cantidad de azul en el píxel. Va de 0 a 255.
 *   - a: La transparencia del píxel. Va de 0 a 255.
 */
typedef struct rgba_pixfmt {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} rgba_t;

/**
 * Marca el ejercicio 2A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - ej2a
 */
extern bool EJERCICIO_2A_HECHO;

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
void ej2a(
	rgba_t* dst,
	rgba_t* src,
	uint32_t size
);