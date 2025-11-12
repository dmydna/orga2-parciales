#include "ej1.h"

uint32_t* acumuladoPorCliente(uint8_t cantidadDePagos, pago_t* arr_pagos){
    uint32_t* pagosAprobadosPorCliente = (uint32_t*) malloc(sizeof(uint32_t)*10 );
    memset(pagosAprobadosPorCliente, 0, sizeof(uint32_t)*10 );

    for(int8_t i=0; i<cantidadDePagos;i++){
         uint8_t cliente = arr_pagos[i].cliente;
         pagosAprobadosPorCliente[cliente] += arr_pagos[i].aprobado;
    }

    return pagosAprobadosPorCliente;
}

uint8_t en_blacklist(char* comercio, char** lista_comercios, uint8_t n){
    for(uint8_t i=0; i<n;i++){
        char* comercioActual = lista_comercios[i];
        if(strcmp(comercio,comercioActual)==0){
            return 1;
        }
    }
    return 0;
}

pago_t** blacklistComercios(uint8_t cantidad_pagos, pago_t* arr_pagos, char** arr_comercios, uint8_t size_comercios){
    
    pago_t** pagosComercios = (pago_t**) malloc( sizeof(pago_t*) * size_comercios );

    for(uint8_t i=0; i< size_comercios;i++){
        char comercioActual = arr_comercios[i];

        pago_t* pagosDelComercio = (pago_t*) malloc(sizeof(pago_t));
        int cantidadPagosDelComercio = 1;

        for(uint8_t j=0;j<cantidad_pagos; j++){
            char* comercio = arr_pagos[i].comercio;
            if(comercioActual == comercio){
                cantidadPagosDelComercio ++;
                pagosDelComercio = (pago_t*)realloc(pagosDelComercio, sizeof(pago_t)*cantidadPagosDelComercio);
                pagosDelComercio[i] = arr_pagos[j];
            }
        }
    }
}


