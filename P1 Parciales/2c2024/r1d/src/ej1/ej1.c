#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - copiarUnidad
 */
bool EJERCICIO_1A_HECHO = false;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - comenzarBatalla
 */
bool EJERCICIO_1B_HECHO = false;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - aplicarResultadoBatalla
 */
bool EJERCICIO_1C_HECHO = false;


/**
 * OPCIONAL: implementar en C
 */
attackunit_t* copiarUnidad(attackunit_t* base) {
}

/**
 * OPCIONAL: implementar en C
 */
void comenzarBatalla(mapa_t mapa, uint8_t x, uint8_t y) {
}

/**
 * OPCIONAL:
 * Dadas dos unidades, indica si hay sobrevivientes
 */
int8_t determinarBatalla(attackunit_t* jugadorDeTurno, attackunit_t* oponente) {
}

/**
 * OPCIONAL: implementar en C
 */
void aplicarResultadoBatalla(mapa_t mapa, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
}
