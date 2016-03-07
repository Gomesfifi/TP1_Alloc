#include "mem.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
int main(){
    /**mem_alloc sans initialisation*/
    printf("\n===TEST===\n");
    printf("\n 1) mem_alloc sans mem_init() : \n");
    assert(mem_alloc(2)==(void*)0);
    printf("CHECK");
    /**Initialisation*/
    mem_init();
    printf("\n 2) mem_alloc(0) retourne null : ");
    assert(mem_alloc(0) == (void*)0);
    printf("CHECK");
    printf("\n 3) mem_alloc(Taille trop grande) : \n");
    assert(mem_alloc((unsigned long)pow(2,21))==(void*)0);
    printf("CHECK");
    printf("\n 4) mem_alloc(7) : ");
    void* ptr = mem_alloc(7);
    assert(ptr != (void*)0);
    printf("CHECK");

    printf("\n\n ==== FIN DU TEST ==== \n\n");

    return 0;
}
