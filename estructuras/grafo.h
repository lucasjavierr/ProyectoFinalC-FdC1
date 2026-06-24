#ifndef GRAFO_H
#define GRAFO_H

#include "../mapa/mapa.h"

#define MAX_NODOS (FILAS * COLS)

typedef struct NodoAdj {
    int destino;
    struct NodoAdj *sig;
} NodoAdj;

typedef struct {
    NodoAdj *cabeza;
} Nodo;

typedef struct {
    Nodo nodos[MAX_NODOS];
    int totalNodos;
    int blinkyFila;
    int blinkyCol;
} Grafo;

int celda_a_id(int fila, int col);
void id_a_celda(int id, int *fila, int *col);

void construirGrafo(Grafo *g);
void liberarGrafo(Grafo *g);

int bfsProximoPaso(Grafo *g, int origen, int destino);

#include "../personajes/personajes.h"
void moverFantasmaInteligente(Fantasma *fantasma, Pacman *pacman, Grafo *g);

#endif