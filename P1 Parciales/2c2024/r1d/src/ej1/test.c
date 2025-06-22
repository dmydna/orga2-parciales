#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Cuenta cuántos tests corrieron exitosamente.
 */
uint64_t successful_tests = 0;
/**
 * Cuenta cuántos tests test fallaron.
 */
uint64_t failed_tests = 0;

/**
 * El mensaje [DONE] escrito en verde.
 */
#define DONE "[\033[32;1mDONE\033[0m] "

/**
 * El mensaje [FAIL] escrito en rojo.
 */
#define FAIL "[\033[31;1mFAIL\033[0m] "

/**
 * El mensaje [INFO] escrito en amarillo.
 */
#define INFO "[\033[33;1mINFO\033[0m] "

/**
 * El mensaje [SKIP] escrito en magenta.
 */
#define SKIP "[\033[95;1mSKIP\033[0m] "

uint32_t hash(attackunit_t* p) {
    uint32_t res = p->combustible;
    if (strcmp(p->clase, "anti-air") == 0) {
        res += 1000;
    } else if (strcmp(p->clase, "artillery") == 0) {
        res += 2000;
    } else if (strcmp(p->clase, "battleship") == 0) {
        res += 3000;
    } else if (strcmp(p->clase, "mech") == 0) {
        res += 4000;
    }
    return res;
}

uint16_t get_combustible(char* clase) {
	// Referencia: https://awbw.fandom.com/wiki/Units
    uint16_t res;
    if (strcmp(clase, "anti-air") == 0) {
        res = 60;
    } else if (strcmp(clase, "artillery") == 0) {
        res = 50;
    } else if (strcmp(clase, "battleship") == 0) {
        res = 99;
    } else if (strcmp(clase, "mech") == 0) {
        res = 70;
    } else {
		// No debería caer acá pero por las dudas
		res = 20;
	}
    return res;
}

void fun_modificar(attackunit_t* unidad) {
	unidad->combustible -= 10;
	if (strcmp(unidad->clase, "mech") == 0)
		unidad->combustible -= 10;
}

#define P(valor_clase, valor_combustible, valor_references, valor_jugador) (&(attackunit_t) { \
	.clase = #valor_clase,                                          \
	.combustible = (valor_combustible),                             \
	.references = (valor_references),                               \
	.jugador = (valor_jugador),                              		\
})

void print_map(mapa_t mapa) {
    // Print headers
    printf("%-30s %-30s\n", "Hashes", "Punteros (5 últimos dígitos)");
    printf("%-30s %-30s\n", "-----------------------------", "-------------------------------");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Print hash or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5u ", hash(mapa[i][j]));
            } else {
                printf("%5d ", 0);
            }
        }

        // Align second column for pointers
        printf("   ");

        for (int j = 0; j < 5; j++) {
            // Print last 5 digits of pointer or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5lu ", (unsigned long)mapa[i][j] % 100000);
            } else {
                printf("%5d ", 0);
            }
        }

        // Move to the next row
        printf("\n");
    }
    printf("\n");
}

void print_map_combustible(mapa_t mapa) {
    // Print headers
    printf("%-30s %-30s\n", "Combustible base", "Combustible");
    printf("%-30s %-30s\n", "-----------------------------", "-------------------------------");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Print hash or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5u ", get_combustible((char*) mapa[i][j]->clase));
            } else {
                printf("%5d ", 0);
            }
        }

        // Align second column for pointers
        printf("   ");

        for (int j = 0; j < 5; j++) {
            // Print last 5 digits of pointer or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5u ", mapa[i][j]->combustible);
            } else {
                printf("%5d ", 0);
            }
        }

        // Move to the next row
        printf("\n");
    }
    printf("\n");
}

void comparar_1a(attackunit_t* res, attackunit_t* expected, char* testName) {
if (res->combustible == expected->combustible &&
	res->jugador == expected->jugador &&
	res->references == 1 &&
	strcmp(res->clase, expected->clase) == 0) {
		successful_tests++;
		printf(DONE "%s: la copia y el original coinciden.\n",testName);
	} else {
		failed_tests++;
		printf("Se esperaba una copia con otras características\n");
	}
}

bool comparar(attackunit_t* res, attackunit_t* expected, char* testName) {
	int clases_iguales = strcmp(res->clase, expected->clase);
	if (res->combustible == expected->combustible &&
		res->jugador == expected->jugador &&
		res->references == expected->references &&
		clases_iguales == 0) {
			successful_tests++;
			printf(DONE "%s: la copia y el original coinciden.\n",testName);
			return true;
		} else {
			failed_tests++;
			printf("Se esperaba una copia con otras características:\n");
			return false;
		}
	return false;
}

void test_1a_varios_copias() {
	mapa_t mapa = {0};
	mapa[1][2] = malloc(sizeof(attackunit_t));
	*mapa[1][2] = *P(anti-air, 60, 1, 0);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(anti-air, 60, 1, 0);
	mapa[2][3] = malloc(sizeof(attackunit_t));
	*mapa[2][3] = *P(artillery, 50, 1, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(anti-air, 60, 1, 1);
	mapa[3][3] = malloc(sizeof(attackunit_t));
	*mapa[3][3] = *P(battleship, 99, 1, 1);


	attackunit_t* nuevaUnidad0 = copiarUnidad(mapa[1][2]);
	attackunit_t* nuevaUnidad1 = copiarUnidad(mapa[1][1]);
	attackunit_t* nuevaUnidad2 = copiarUnidad(mapa[2][3]);
	attackunit_t* nuevaUnidad3 = copiarUnidad(mapa[3][4]);
	attackunit_t* nuevaUnidad4 = copiarUnidad(mapa[3][3]);

	comparar_1a(nuevaUnidad0, mapa[1][2], "copiarUnidad");
	comparar_1a(nuevaUnidad1, mapa[1][1], "copiarUnidad");
	comparar_1a(nuevaUnidad2, mapa[2][3], "copiarUnidad");
	comparar_1a(nuevaUnidad3, mapa[3][4], "copiarUnidad");
	comparar_1a(nuevaUnidad4, mapa[3][3], "copiarUnidad");

	free(mapa[1][2]);
	free(mapa[1][1]);
	free(mapa[2][3]);
	free(mapa[3][4]);
	free(mapa[3][3]);
	free(nuevaUnidad0);
	free(nuevaUnidad1);
	free(nuevaUnidad2);
	free(nuevaUnidad3);
	free(nuevaUnidad4);
}

void test_1a_instancia_compartida_nueva() {
	attackunit_t nuevaUnidad = *P(anti-air, 60, 2, 0);

	attackunit_t* nuevaUnidad0 = copiarUnidad(&nuevaUnidad);

	comparar_1a(nuevaUnidad0, nuevaUnidad0, "copiarUnidad");
	free(nuevaUnidad0);
}

/**
 * Evalúa los tests del ejercicio 1A. Este ejercicio requiere implementar
 * `es_indice_ordenado`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1A_HECHO`.
 */
void test_ej1a(void) {
	uint64_t successful_at_start = successful_tests;
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1A_HECHO) {
		printf(SKIP "El ejercicio 1A no está hecho aún.\n");
		return;
	}

	test_1a_varios_copias();
	test_1a_instancia_compartida_nueva();

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1A tuvo tests que fallaron.\n");
	}
}


void testResults1B(mapa_t esperado, mapa_t resultado, char* testName, size_t map_size) {
	bool iguales = true;
	for (size_t i = 0; i < map_size; i++)
	{
		for (size_t j = 0; j < map_size; j++)
		{
			if (resultado[i][j] != 0 && esperado[i][j] != 0) {
				if (!comparar(esperado[i][j], resultado[i][j], testName)) { 
					iguales = false;
					break;
				}
			}
		}
		if (!iguales) {
			break;
		}
		
	}
	if (iguales) {
		successful_tests++;
		printf(DONE "%s: los mapas coinciden.\n",testName);
	} else {
		failed_tests++;		
		printf(FAIL "%s: Hubo una inconsistencia entre los mapas. Esperado:\n", testName);
		print_map_combustible(esperado);
		printf("Resultado:\n");
		print_map_combustible(resultado);
	}
}

void test_1b_sin_cambios() {
	mapa_t mapa = {0};
	mapa[1][2] = malloc(sizeof(attackunit_t));
	*mapa[1][2] = *P(anti-air, 60, 1, 0);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(mech, 70, 1, 0);
	mapa[3][2] = malloc(sizeof(attackunit_t));
	*mapa[3][2] = *P(artillery, 20, 1, 0);
	mapa[3][3] = malloc(sizeof(attackunit_t));
	*mapa[3][3] = *P(battleship, 99, 1, 0);

	mapa_t esperado = {0};
	esperado[1][2] = mapa[1][2];
	esperado[1][1] = mapa[1][1];
	esperado[3][2] = mapa[3][2];
	esperado[3][3] = mapa[3][3];

	comenzarBatalla(mapa, 1, 2);
	comenzarBatalla(mapa, 1, 1);
	comenzarBatalla(mapa, 3, 3);
	comenzarBatalla(mapa, 3, 2);
	comenzarBatalla(mapa, 4, 4);
	testResults1B(esperado, mapa, "test_1b_sin_cambios", 5);

	free(mapa[1][2]);
	free(mapa[1][1]);
	free(mapa[3][2]);
	free(mapa[3][3]);
}

void test_1b_mapa_completo(uint64_t successful_at_start) {
	if (successful_tests - successful_at_start < 4) {
		printf(FAIL "El test sobre el mapa completo se ejecutará cuando pasen los tests anteriores. y el 1C este marcado como completo\n");
		return;
	}

	attackunit_t* anti_aerea_0 = malloc(sizeof(attackunit_t));
	attackunit_t* anti_aerea_0_bis = malloc(sizeof(attackunit_t));
	attackunit_t* anti_aerea_1 = malloc(sizeof(attackunit_t));
	attackunit_t* anti_aerea_1_bis = malloc(sizeof(attackunit_t));
	attackunit_t* artilleria_0 = malloc(sizeof(attackunit_t));
	attackunit_t* artilleria_1 = malloc(sizeof(attackunit_t));
	attackunit_t* artilleria_bis = malloc(sizeof(attackunit_t));
	*anti_aerea_0 = *P(anti-air, 40, 2, 0);
	*anti_aerea_0_bis = *P(anti-air, 40, 1, 0);
	*anti_aerea_1 = *P(anti-air, 60, 1, 1);
	*anti_aerea_1_bis = *P(anti-air, 20, 1, 1);
	*artilleria_0 = *P(artillery, 50, 1, 0);
	*artilleria_1 = *P(artillery, 50, 1, 1);
	*artilleria_bis = *P(artillery, 7, 1, 0);
	
	mapa_t mapa = {0};
	mapa[1][1] = anti_aerea_0;
	mapa[43][89] = anti_aerea_0;
	mapa[43][90] = anti_aerea_1;
	mapa[241][240] = artilleria_0;
	mapa[242][241] = artilleria_1;
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(artillery, 57, 1, 0);
	mapa[253][253] = malloc(sizeof(attackunit_t));
	*mapa[253][253] = *P(battleship, 99, 1, 0);

	mapa_t esperado = {0};
	esperado[1][1] = anti_aerea_0_bis;
	esperado[43][89] = 0;
	esperado[43][90] = anti_aerea_1_bis;
	esperado[241][240] = 0;
	esperado[242][241] = 0;
	esperado[4][3] = 0;
	esperado[3][4] = artilleria_bis;
	esperado[253][253] = mapa[254][254];

	for (size_t i = 0; i < 255; i++)
	{
		for (size_t j = 0; j < 255; j++)
		{
			comenzarBatalla(mapa, i, j);
		}
		
	}
	
	testResults1B(esperado, mapa, "test_1b_mapa_completo", 255);

	free(mapa[0][0]);
	free(mapa[43][89]);
	free(mapa[3][4]);
	free(anti_aerea_0_bis);
	free(anti_aerea_1_bis);
	free(artilleria_bis);
	free(mapa[253][253]);
	free(anti_aerea_0);
	free(anti_aerea_1);
}


/**
 * Evalúa los tests del ejercicio 1B.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1B_HECHO`.
 */
void test_ej1b(void) {
	uint64_t successful_at_start = successful_tests;
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1B_HECHO) {
		printf(SKIP "El ejercicio 1B no está hecho aún.\n");
		return;
	}

	test_1b_sin_cambios();
	if (EJERCICIO_1B_HECHO) {
		test_1b_mapa_completo(successful_at_start);
	}

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1B tuvo tests que fallaron.\n");
	}
}


void test_1c_pierde_0() {
	mapa_t mapa = {0};
	mapa[2][3] = malloc(sizeof(attackunit_t));
	*mapa[2][3] = *P(artillery, 52, 1, 0);
	mapa[3][3] = malloc(sizeof(attackunit_t));
	*mapa[3][3] = *P(anti-air, 60, 1, 1);

	aplicarResultadoBatalla(mapa, 2, 3, 3, 3);
	
	attackunit_t *expected = P(anti-air, 8, 1, 1);
	comparar(expected, mapa[3][3], "test_1c");
	free(mapa[3][3]);
}

void test_1c_pierde_1() {
	mapa_t mapa = {0};
	mapa[3][3] = malloc(sizeof(attackunit_t));
	*mapa[3][3] = *P(artillery, 52, 1, 1);
	mapa[3][2] = malloc(sizeof(attackunit_t));
	*mapa[3][2] = *P(anti-air, 60, 1, 0);

	attackunit_t* twoRefs = malloc(sizeof(attackunit_t));
	*twoRefs = *P(artillery, 52, 2, 1);
	mapa[2][1] = twoRefs;
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(anti-air, 60, 1, 0);

	aplicarResultadoBatalla(mapa, 3, 3, 3, 2);
	aplicarResultadoBatalla(mapa, 2, 1, 1, 1);
	
	bool todo_bien = true;
	attackunit_t *expected_3_4 = P(anti-air, 8, 1, 0);
	comparar(expected_3_4, mapa[3][2], "test_1c");
	if (mapa[3][3] != 0) {todo_bien = false;}
	attackunit_t *expected_1_1 = P(anti-air, 8, 1, 0);
	comparar(expected_1_1, mapa[1][1], "test2_1c");
	if (mapa[2][1] != 0) {todo_bien = false;}
	free(mapa[3][2]);
	free(mapa[1][1]);
	free(twoRefs);
	if (todo_bien) {
		successful_tests++;
		printf(DONE "%s: la copia y el original coinciden.\n", "pierde 1");
	} else {
		failed_tests++;
		printf("Se esperaba una copia con otras características:\n");
	}
}

void test_1c_pierden_ambos() {
	mapa_t mapa = {0};
	mapa[2][3] = malloc(sizeof(attackunit_t));
	*mapa[2][3] = *P(artillery, 52, 1, 1);
	mapa[3][3] = malloc(sizeof(attackunit_t));
	*mapa[3][3] = *P(anti-air, 52, 1, 0);

	attackunit_t* unidad21 = malloc(sizeof(attackunit_t));
	*unidad21 = *P(artillery, 52, 2, 1);
	attackunit_t* unidad12 = malloc(sizeof(attackunit_t));
	*unidad12 = *P(anti-air, 52, 2, 0);

	mapa[2][1] = unidad21;
	mapa[1][2] = unidad12;

	aplicarResultadoBatalla(mapa, 2, 3, 3, 3);
	aplicarResultadoBatalla(mapa, 2, 1, 1, 2);

	free(unidad21);
	free(unidad12);
	
	if (mapa[2][3] == 0 && mapa[3][3] == 0) {
		successful_tests++;
		printf(DONE "%s: el resultado y el esperado coinciden.\n","1c_pierden_ambos");
	} else {
		failed_tests++;
		printf("Esperado != obtenido \n");
	}
}


/**
 * Evalúa los tests del ejercicio 1B. Este ejercicio requiere implementar
 * `es_indice_ordenado`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1C_HECHO`.
 */
void test_ej1c(void) {
	uint64_t successful_at_start = successful_tests;
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1C_HECHO) {
		printf(SKIP "El ejercicio 1C no está hecho aún.\n");
		return;
	}

	test_1c_pierde_0();
	test_1c_pierde_1();
	test_1c_pierden_ambos();

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1C tuvo tests que fallaron.\n");
	}
}

/**
 * Corre los tests de este ejercicio.
 *
 * Las variables `EJERCICIO_1A_HECHO`, `EJERCICIO_1B_HECHO` y `EJERCICIO_1C_HECHO` controlan qué
 * testsuites se van a correr. Ponerlas como `false` indica que el ejercicio no
 * está implementado y por lo tanto no querés que se corran los tests asociados
 * a él.
 *
 * Recordá que los dos ejercicios pueden implementarse independientemente uno
 * del otro.
 *
 * Si algún test falla el programa va a terminar con un código de error.
 */
int main(int argc, char* argv[]) {
	// 1A
	test_ej1a();
	// 1B
	test_ej1b();
	// 1C
	test_ej1c();

	printf(
		"\nSe corrieron %ld tests. %ld corrieron exitosamente. %ld fallaron.\n",
		failed_tests + successful_tests, successful_tests, failed_tests
	);

	if (failed_tests) {
		return 1;
	} else {
		return 0;
	}
}
