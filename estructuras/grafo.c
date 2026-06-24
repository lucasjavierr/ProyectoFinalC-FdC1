#include <stdlib.h>
#include <string.h>
#include "grafo.h"
#include "../mapa/mapa.h"
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
    g->blinkyFila = 6;
    g->blinkyCol  = 9;
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

static int distancia(int f1, int c1, int f2, int c2) {
    int df = f1 - f2; if (df < 0) df = -df;
    int dc = c1 - c2; if (dc < 0) dc = -dc;
    return df + dc;
}

static int clampFila(int f) { return f < 0 ? 0 : (f >= FILAS ? FILAS-1 : f); }
static int clampCol(int c)  { return c < 0 ? 0 : (c >= COLS  ? COLS -1 : c); }

static int celdaObjetivoCercana(int tf, int tc) {
    tf = clampFila(tf);
    tc = clampCol(tc);
    if (mapa[tf][tc] != PARED) return celda_a_id(tf, tc);

    int deltas[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };
    for (int r = 1; r <= 3; r++) {
        for (int d = 0; d < 4; d++) {
            int nf = clampFila(tf + deltas[d][0] * r);
            int nc = clampCol(tc + deltas[d][1] * r);
            if (mapa[nf][nc] != PARED) return celda_a_id(nf, nc);
        }
    }
    return celda_a_id(tf, tc);
}

void moverFantasmaInteligente(Fantasma *fantasma, Pacman *pacman, Grafo *g) {
    if (fantasma->asustado) {
        moverFantasma(fantasma);
        return;
    }

    int origen = celda_a_id(fantasma->fila, fantasma->col);
    int destino;

    switch (fantasma->tipo) {
        case BLINKY: {
            destino = celda_a_id(pacman->fila, pacman->col);
            break;
        }

        case PINKY: {
            int tf = pacman->fila;
            int tc = pacman->col;
            switch (pacman->dir) {
                case ARRIBA:    tf -= 4; break;
                case ABAJO:     tf += 4; break;
                case IZQUIERDA: tc -= 4; break;
                case DERECHA:   tc += 4; break;
                default: break;
            }
            destino = celdaObjetivoCercana(tf, tc);
            break;
        }

        case INKY: {
            int refF = pacman->fila;
            int refC = pacman->col;
            switch (pacman->dir) {
                case ARRIBA:    refF -= 2; break;
                case ABAJO:     refF += 2; break;
                case IZQUIERDA: refC -= 2; break;
                case DERECHA:   refC += 2; break;
                default: break;
            }

            int tf = refF + (refF - g->blinkyFila);
            int tc = refC + (refC - g->blinkyCol);
            destino = celdaObjetivoCercana(tf, tc);
            break;
        }

        case CLYDE: {
            int dist = distancia(fantasma->fila, fantasma->col, pacman->fila, pacman->col);
            if (dist > 8) {
                destino = celda_a_id(pacman->fila, pacman->col);
            } else {
                destino = celdaObjetivoCercana(FILAS - 1, 0);
            }
            break;
        }

        default:
            destino = celda_a_id(pacman->fila, pacman->col);
            break;
    }

    if (fantasma->tipo == BLINKY) {
        g->blinkyFila = fantasma->fila;
        g->blinkyCol  = fantasma->col;
    }

    int proximoId = bfsProximoPaso(g, origen, destino);
    if (proximoId == -1) {
        moverFantasma(fantasma);
        return;
    }
    id_a_celda(proximoId, &fantasma->fila, &fantasma->col);

    if (fantasma->tipo == BLINKY) {
        g->blinkyFila = fantasma->fila;
        g->blinkyCol  = fantasma->col;
    }
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

    int paso = destino;
    while (padre[paso] != origen)
        paso = padre[paso];

    return paso;
}