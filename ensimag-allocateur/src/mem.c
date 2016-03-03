/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "listBloc.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

// Table Zones Libres
listbloc TZL[BUDDY_MAX_INDEX+1];

int
mem_init()
{
    if (! zone_memoire)
        zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
    if (zone_memoire == 0)
    {
        perror("mem_init:");
        return -1;
    }

    // Initialisation de TZL
    for (int i = 0; i < BUDDY_MAX_INDEX ; i++){
        TZL[i].nombreBloc = 0;
        TZL[i].premier = 0;
        TZL[i].dernier = 0;
    }
    TZL[BUDDY_MAX_INDEX].nombreBloc = 1;
    TZL[BUDDY_MAX_INDEX].premier = malloc(sizeof(bloc));
    TZL[BUDDY_MAX_INDEX].premier->debutMemoire = &zone_memoire[0];
    TZL[BUDDY_MAX_INDEX].premier->estLibre = true;
    TZL[BUDDY_MAX_INDEX].premier->suiv = 0;
    // On ne depasse pas la taille maximale (2^20)
    TZL[BUDDY_MAX_INDEX].premier->taille = 1 << BUDDY_MAX_INDEX;
    TZL[BUDDY_MAX_INDEX].dernier = TZL[BUDDY_MAX_INDEX].premier;

    return 0;
}

void *
mem_alloc(unsigned long size)
{
    /*  ecrire votre code ici */
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
    /* ecrire votre code ici */

    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}

