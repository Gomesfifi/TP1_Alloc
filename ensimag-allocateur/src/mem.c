/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

// Table Zones Libres
// uint64_t pour ne pas avoir de problèmes entre machine 32 ou 64 bits
// et pour pouvoir déférencer (récupérer les adresses nécessaires )
// Initialisation à NULL
uint64_t* TZL[BUDDY_MAX_INDEX+1]={NULL};

// mem_init doit avoir été effectuée pour utiliser les fonctions mem_alloc, mem_free, mem_destroy
bool memEstInit = false;

int
mem_init()
{
    if (! zone_memoire)
        zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
    if (zone_memoire == 0)
    {
        perror("mem_init: la zone mémoire n'est pas disponible");
        return -1;
    }

    // Initialisation de TZL
    // La zone mémoire est entièrement contenu à la taille 2^BUDDY_MAX_INDEX
    TZL[BUDDY_MAX_INDEX] = (uint64_t *)zone_memoire;
    // Il n'y a pas de bloc suivant
    *TZL[BUDDY_MAX_INDEX] = 0;

    memEstInit = true;

    return 0;
}

void *
mem_alloc(unsigned long size)
{
    if (!memEstInit){
        perror("La mémoire doit être initialisé avant un mem_alloc");
        return NULL;
    }

    return 0;
}

int
mem_free(void *ptr, unsigned long size)
{
    /* ecrire votre code ici */
    return 0;
}


int
mem_destroy()
{
    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}

