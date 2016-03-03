#ifndef SRC_LISTBLOC_H
#define SRC_LISTBLOC_H

#include <stdbool.h>

typedef struct bloc bloc;
struct bloc{
    int taille;
    bool estLibre;
    void* debutMemoire;
    bloc* suiv;
};

typedef struct listbloc listbloc;
struct listbloc{
    int nombreBloc;
    bloc* premier;
    bloc* dernier;
};

#endif //SRC_LISTBLOC_H
