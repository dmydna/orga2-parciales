#pragma once
#include <stdbool.h>
#include <stdint.h>

/**
 * Una unidad de ataque (tanques, barcos de batalla, etc.) en nuestro videojuego.
 *
 * Campos:
 *   - clase:          El nombre de la clase.
 *   - combustible:    La cantidad de combustible disponible de la unidad.
 *   - references:     La cantidad de referencias a la unidad en el mapa.
 *   - jugador:		   Numero de jugador
 */
typedef struct {
	char clase[11];
	uint16_t combustible;
	uint8_t references;
	bool jugador;
} attackunit_t;

typedef attackunit_t *mapa_t[255][255];

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - optimizar
 */
extern bool EJERCICIO_1A_HECHO;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - contarCombustibleAsignado
 */
extern bool EJERCICIO_1B_HECHO;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - modificarUnidad
 */
extern bool EJERCICIO_1C_HECHO;

/**
 * OPCIONAL: implementar en C
 */
attackunit_t* copiarUnidad(attackunit_t* base);

/**
 * OPCIONAL: implementar en C
 */
void comenzarBatalla(mapa_t mapa, uint8_t x, uint8_t y);

/**
 * OPCIONAL: implementar en C
 */
void aplicarResultadoBatalla(mapa_t mapa, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);