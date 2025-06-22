#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - init_fantastruco_dir
 */
bool EJERCICIO_1A_HECHO = true;

// OPCIONAL: implementar en C
void init_fantastruco_dir(fantastruco_t* card) {

    directory_entry_t** directory_entry_list = (directory_entry_t*)malloc(sizeof(directory_entry_t*)*2);

    directory_entry_list[0] = create_dir_entry("sleep", sleep) ;
    directory_entry_list[1] = create_dir_entry("wakeup", wakeup) ; 

    card->__dir = directory_entry_list;
    card->__dir_entries = 2;
}

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - summon_fantastruco
 */
bool EJERCICIO_1B_HECHO = true;

// OPCIONAL: implementar en C
fantastruco_t* summon_fantastruco() {

    fantastruco_t* new_Card = (fantastruco_t*)malloc(sizeof(fantastruco_t));

    init_fantastruco_dir(new_Card);

    new_Card->face_up = 1;
    new_Card->__archetype = NULL;
    
    return new_Card;

}
