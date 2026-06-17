#include <stdlib.h>
#include <string.h>
#include "grafo.h"
#include "mapa.h"
#include "cola.h"

int celda_a_id(int fila, int col) {
    return fila * COLS + col;
}

void id_a_celda(int id, int *fila, int *col) {
    *fila = id / COLS;
    *col  = id % COLS;
}

static void agregarArista(Grafo *g, int desde, int hasta) {
    NodoAdj *nuevo = (NodoAdj *)malloc(sizeof(NodoAdj));
    nuevo->destino = hasta;
    nuevo->sig = g->nodos[desde].cabeza;
    g->nodos[desde].cabeza = nuevo;
}

void construirGrafo(Grafo *g) {
    g->totalNodos = MAX_NODOS;
    for (int i = 0; i < MAX_NODOS; i++)
        g->nodos[i].cabeza = NULL;

    int deltas[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };

    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLS; c++) {
            if (mapa[f][c] == PARED) continue;

            int idOrigen = celda_a_id(f, c);
            for (int d = 0; d < 4; d++) {
                int nf = f + deltas[d][0];
                int nc = c + deltas[d][1];
                if (nf < 0 || nf >= FILAS || nc < 0 || nc >= COLS) continue;
                if (mapa[nf][nc] == PARED) continue;

                agregarArista(g, idOrigen, celda_a_id(nf, nc));
            }
        }
    }
}

void moverFantasmaInteligente(Fantasma *fantasma, Pacman *pacman, Grafo *g) {
    int origen  = celda_a_id(fantasma->fila, fantasma->col);
    int destino = celda_a_id(pacman->fila, pacman->col);

    if (fantasma->asustado) {
        moverFantasma(fantasma);
        return;
    }

    int proximoId = bfsProximoPaso(g, origen, destino);
    if (proximoId == -1) {
        moverFantasma(fantasma);
        return;
    }

    id_a_celda(proximoId, &fantasma->fila, &fantasma->col);
}

void liberarGrafo(Grafo *g) {
    for (int i = 0; i < MAX_NODOS; i++) {
        NodoAdj *actual = g->nodos[i].cabeza;
        while (actual) {
            NodoAdj *tmp = actual;
            actual = actual->sig;
            free(tmp);
        }
        g->nodos[i].cabeza = NULL;
    }
}

int bfsProximoPaso(Grafo *g, int origen, int destino) {
    if (origen == destino) return -1;

    int visitado[MAX_NODOS] = {0};
    int padre[MAX_NODOS];
    for (int i = 0; i < MAX_NODOS; i++) padre[i] = -1;

    Cola cola;
    iniciarCola(&cola);

    encolar(&cola, origen);
    visitado[origen] = 1;

    int encontrado = 0;

    while (!colaVacia(&cola)) {
        int actual = desencolar(&cola);

        NodoAdj *vecino = g->nodos[actual].cabeza;
        while (vecino) {
            int v = vecino->destino;
            if (!visitado[v]) {
                visitado[v] = 1;
                padre[v] = actual;

                if (v == destino) {
                    encontrado = 1;
                    break;
                }
                encolar(&cola, v);
            }
            vecino = vecino->sig;
        }
        if (encontrado) break;
    }

    liberarCola(&cola);

    if (!encontrado) return -1;

    // Reconstruir camino para encontrar el primer paso
    int paso = destino;
    while (padre[paso] != origen)
        paso = padre[paso];

    return paso;
}