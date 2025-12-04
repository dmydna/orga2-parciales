#include "ej2.h"

#include <string.h>

// OPCIONAL: implementar en C
void invocar_habilidad(void* carta_generica, char* habilidad) {
	card_t* carta = carta_generica;

	void (*card_ability_fun)(void*);
	char* card_ability_name;

	while (carta){
		
		directory_entry_t** directory_entry_list = carta->__dir;
		int directory_len = carta->__dir_entries;

		for(int i=0; i < directory_len ;i++){
		// actualizo segun Carta actual
			directory_len = carta->__dir_entries;
			card_ability_name = directory_entry_list[i]->ability_name;
			card_ability_fun = directory_entry_list[i]->ability_ptr;

			if( strcmp(card_ability_name, habilidad) == 0 ){
				card_ability_fun(carta);
				return;
			}
		}

	carta = carta->__archetype;
	}
	
	return;
}
