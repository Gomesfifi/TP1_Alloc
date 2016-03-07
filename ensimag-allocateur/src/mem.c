/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

/**Maccros*/
#define EST_DANS_ZONE_MEM(ptr) \
  (((uint64_t) ptr) >= ((uint64_t) zone_memoire)) \
    && ((uint64_t) ptr) < (uint64_t) ((uint64_t) zone_memoire + ALLOC_MEM_SIZE)

#define ADRESSE_BUDDY(ptr, index) \
  ((uint64_t) ptr)^((uint64_t) (1 << index)) 

/**Fonctions intermédiaires*/
uint8_t getIndex(unsigned long size);
uint8_t getFirstFree(uint8_t S);
void * diviser(uint8_t S, uint8_t Slibre);
void fusionner(uint64_t *ptr, uint8_t indice);
bool estLibre(uint64_t *buddy, uint8_t indice);
void supprimer(uint64_t *buddy, uint8_t indice);

/**Variables nécessaires*/
void *zone_memoire = 0;

// Table Zones Libres
// uint64_t pour ne pas avoir de problèmes entre machine 32 ou 64 bits
// et pour pouvoir déférencer (récupérer les adresses nécessaires )
// Initialisation à NULL
uint64_t* TZL[BUDDY_MAX_INDEX+1]={NULL};

// mem_init doit avoir été effectuée pour utiliser les fonctions mem_alloc, mem_free, mem_destroy
bool memEstInit = false;


/**Fonctions principales*/
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

    // la mémoire a été initialisé
    memEstInit = true;

    return 0;
}

void *
mem_alloc(unsigned long size)
{
    if (!memEstInit){
        perror("Mem_alloc : La mémoire doit être initialisé avec un mem_init");
        return (void*)0;
    }
    // Cas particulier de la taille 0
    if (size == 0)
        return (void*)0;

    // L'indice Si (notation énoncé)
    uint8_t S = getIndex(size);
    // Si débordement
    if (S > BUDDY_MAX_INDEX) {
        perror("mem_alloc : La taille demandée est plus grande que la mémoire disponible");
        return (void*)0;
    } //Sinon
    // Recherche du premier indice contenant un bloc libre
    uint8_t Slibre = getFirstFree(S);
    // Retourne une erreur si débordement du TZL : Pas de zone libre
    if (Slibre == (BUDDY_MAX_INDEX + 1)) {
        perror("Mem_alloc : Pas de zone libre à allouer");
        return (void *) 0;
    }
    // Fonction de découpage des blocs si nécessaire
    return diviser(S,Slibre);
}

int
mem_free(void *ptr, unsigned long size) {
    // Erreur: libération non autorisée
    if (!(EST_DANS_ZONE_MEM(ptr))) {
        perror("Mem_free: La zone à libérer n'appartient pas à la zone mémoire initialement allouée");
        return -1;
    }
    // Dépassement de la zone mémoire
    if (size > ALLOC_MEM_SIZE) {
        perror("Mem_free: La zone à libérer est plus grande que la zone mémoire initialement allouée");
        return -1;
    }

    // Taille minimale à respecter pour un mem_free
    if (size < sizeof(uint64_t *))
        size = sizeof(uint64_t *);

    //Taille maximale : Cas trivial
    if (size == ALLOC_MEM_SIZE) {
        TZL[BUDDY_MAX_INDEX] = (uint64_t *) ptr;
        return 0;
    }

    //Récupération de l'adresse du budy:
    uint64_t buddy = ADRESSE_BUDDY(ptr, getIndex(size));
    fusionner((uint64_t*) ptr, getIndex(size));

    return 0;
}


int
mem_destroy()
{
    // Désallocation
    free(zone_memoire);
    zone_memoire = 0;
    // La mémoire a été détruite
    memEstInit = false;
    return 0;
}

/**Implémentation fonctions intermédiares*/

/**
 * \fn uint8_t getIndex(unsigned long size)
 *
 * \details Trouvé sur internet
 * \details Calcul de la puissance de 2 supérieur ou égale
 * \details Log2 pour trouver cette puissance.
 */
uint8_t getIndex(unsigned long size){
    unsigned i;
    size--;
    for(i = 1; i < sizeof size * CHAR_BIT; i <<= 1)
        size |= size >> i;
    size++;
    return (uint8_t)(log(size)/log(2));
}

uint8_t getFirstFree(uint8_t S){
    uint8_t res = S;
    while (TZL[res] == (void*)0 && res < (BUDDY_MAX_INDEX + 1))
        res++;
    return res;
}

void * diviser(uint8_t S, uint8_t Slibre){
    // Cas fixe : Slibre est le plus petit indice plus grand que S contenant un bloc libre
    // Cas d'arret de la récursion : On est arrivé à la taille voulu
    if (S == Slibre){
        void* res = (void*)TZL[S];
        TZL[S] = (uint64_t*) *TZL[S];
        return res;
    }
    // Etape 1 : On place le bloc à l'indice précédent
    TZL[Slibre-1] = TZL[Slibre];
    // Etape 2 : On prend le bloc suivant dans l'indice actuel
    TZL[Slibre] = (u_int64_t *)*TZL[Slibre];
    // Etape 3 : Découpage en deux blocs de taille 2^{Slibre-1}
    // Etape 3.1) Le premier bloc pointe vers le second
    *TZL[Slibre-1] = (uint64_t )TZL[Slibre-1] + (uint64_t)pow(2,(Slibre-1));
    // Etape 3.2) Le second bloc ne pointe vers rien
    *((uint64_t *)*TZL[Slibre-1]) = 0;

    //Appel récursif
    return diviser(S,(uint8_t)(Slibre-1));

}


void fusionner(uint64_t *ptr, uint8_t indice) {
    // Etape 0 : Calcul du buddy
    uint64_t * buddy = (uint64_t *)(ADRESSE_BUDDY(ptr,indice));
    // Etape 1 : Le buddy est-il dans la TZL[indice]
    // Cas où il n'est pas dans la TZL
    if (!estLibre(buddy,indice)){
        *ptr = (uint64_t )TZL[indice];
        TZL[indice] = ptr;
        return;
    }
    // Etape 2 : Le buddy est dans la TZL
    // Etape 2.1) On le supprime de la liste
    supprimer(buddy,indice);
    // Etape 2.2) On prend l'adresse de debut de bloc
    uint64_t * adresse = ((uint64_t)ptr > (uint64_t)buddy) ? buddy : ptr;
    // Etape 2.3) On fusionne les blocs supérieurs si nécessaire
    fusionner(adresse,(uint8_t )(indice+1));
}

void supprimer(uint64_t *buddy, uint8_t indice) {
    uint64_t * cour = TZL[indice];
    // Cas où buddy est en tête
    if (cour == buddy)
        TZL[indice] = (uint64_t *)*buddy;
    // Sinon recherche du buddy
    while ((uint64_t *)*cour != buddy)
        cour = (uint64_t *)*cour;
    // cour est le prédécesseur de buddy
    // On supprime buddy en le sautant dans le chainage
    *cour = *buddy;
}

bool estLibre(uint64_t *buddy, uint8_t indice) {
    uint64_t * cour = TZL[indice];
    while (cour != (uint64_t*)0){
        if (cour == buddy)
            return true;
        cour = (uint64_t *)(*cour);
    }
    return false;
}
