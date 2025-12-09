#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - es_indice_ordenado
 */
bool EJERCICIO_1A_HECHO = false;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - contarCombustibleAsignado
 */
bool EJERCICIO_1B_HECHO = false;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - modificarUnidad
 */
bool EJERCICIO_1C_HECHO = false;

/**
 * OPCIONAL: implementar en C
 */
void optimizar(mapa_t mapa, attackunit_t* compartida, uint32_t (*fun_hash)(attackunit_t*)) {
    bool esCompartida;
    attackunit_t* hashCompartida = fun_hash(compartida);
    attackunit_t* hashActual;
    attackunit_t* unidadActual;

    for(int i=0; i < 255; i++){
        
        for(int j=0; i < 255;j++ ){
            unidadActual = mapa[i][j];
            hashActual = fun_hash(unidadActual);
            esCompartida = hashActual == hashCompartida;

            if(esCompartida){
                compartida->references ++;
                unidadActual->references--; 
                mapa[i][j] = compartida;
            }
            if(unidadActual->references == 0){
                 free(unidadActual);
            }
        }
    }
}

/**
 * OPCIONAL: implementar en C
 */

// NT: Esta funcion calcula el total de combustible ultilizado en todo el juego.


uint32_t contarCombustibleAsignado(mapa_t mapa, uint16_t (*fun_combustible)(char*)) {

    uint32_t combustible_total_asigando= 0;

    attackunit_t* unidadActual;
    char* claseUnidadActual;
    uint16_t combustibleActual;
    uint16_t combustibleBase;
    uint16_t combustible_utilizado;

    for (int i=0; i<255;i++){
        for(int j=0; j<255;j++){

            unidadActual = mapa[i][j];

            if(unidadActual == NULL){
                continue;
            }

            claseUnidadActual = unidadActual->clase;
            combustibleActual = unidadActual->combustible;
            combustibleBase = fun_combustible(claseUnidadActual);
            combustible_utilizado = combustibleBase - combustibleActual;

            combustible_total_asigando += combustible_utilizado;
        }
    }
    return combustible_total_asigando;
}

/**
 * OPCIONAL: implementar en C
 */
void modificarUnidad(mapa_t mapa, uint8_t x, uint8_t y, void (*fun_modificar)(attackunit_t*)) {

    attackunit_t* unidad_actual = mapa[x][y];

    if(unidad_actual == NULL){
        return;
    }

    bool estaOptimizada = unidad_actual ->references > 1;

    if(estaOptimizada){
        attackunit_t* nuevaUnidad = (attackunit_t*) malloc(sizeof(attackunit_t));
        unidad_actual->references --;

        // copia todos los campos a la nueva estructura
        // (solo para campos que no sean punteros, la copia es directa)
        *nuevaUnidad = *unidad_actual;

        nuevaUnidad->references = 1;
        mapa[x][y] = nuevaUnidad;

        unidad_actual = nuevaUnidad;

    }

    fun_modificar(unidad_actual );

}
