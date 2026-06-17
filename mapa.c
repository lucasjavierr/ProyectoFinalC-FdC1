#include <stdio.h>
#include "mapa.h"
#include "personajes.h"

int mapa[FILAS][COLS] = {
    {0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0},
    {2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2},
    {2, 0, 3, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 3, 0, 2},
    {2, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 2},
    {2, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 0, 2},
    {2, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 2},
    {2, 2, 2, 0, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 0, 2, 2, 2},
    {0, 0, 2, 0, 2, 0, 2, 0, 0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 0},
    {2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2},
    {0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0},
    {2, 2, 2, 0, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 0, 2, 2, 2},
    {2, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 2},
    {2, 0, 2, 0, 2, 2, 2, 0, 2, 0, 2, 0, 2, 2, 2, 0, 2, 0, 2},
    {2, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 2},
    {2, 0, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 2},
    {2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2},
    {2, 2, 2, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 0, 2, 2, 2},
    {0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
};

void imprimirMapa(Pacman *pacman, Fantasma fantasmas[], int n) {
    char mapaVisual[FILAS][COLS][8];

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLS; j++) {
            switch(mapa[i][j]) {
                case PARED: snprintf(mapaVisual[i][j], 10, "██"); break;
                case CAMINO: snprintf(mapaVisual[i][j], 10, "  "); break;
                case PUNTO: snprintf(mapaVisual[i][j], 10, " ·"); break;
                case POWER: snprintf(mapaVisual[i][j], 10, " ●"); break;
                default: snprintf(mapaVisual[i][j], 10, "  "); break;
            }
        }
    }

    for (int k = 0; k < n; k++) {
        int f = fantasmas[k].fila;
        int c = fantasmas[k].col;
        if (f >= 0 && f < FILAS && c >= 0 && c < COLS) {
            snprintf(mapaVisual[f][c], 10, "%s", fantasmas[k].sprite);
        }
    }

    if (pacman->fila >= 0 && pacman->fila < FILAS && pacman->col >= 0 && pacman->col < COLS) {
        snprintf(mapaVisual[pacman->fila][pacman->col], 10, "%s", pacman->sprite);
    }

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%s", mapaVisual[i][j]);
        }
        printf("\n");
    }
}

int esCamino(int fila, int col) {
    if (fila < 0 || fila >= FILAS || col < 0 || col >= COLS)
        return 0;
    return mapa[fila][col] != PARED;
}