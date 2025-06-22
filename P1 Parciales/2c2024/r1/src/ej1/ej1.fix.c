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

            if(!esCompartida){
                continue;
            }
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