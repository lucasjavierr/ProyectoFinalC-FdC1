#ifndef MAPA_H
#define MAPA_H

#include "personajes.h"

#define FILAS 19
#define COLS 19

#define PARED 0
#define CAMINO 1
#define PUNTO 2
#define FANTASMA 3

extern int mapa[FILAS][COLS];

// 3. Prototipos de funciones
void imprimirMapa(Pacman *pacman, Fantasma fantasmas[], int n);
int esCamino(int fila, int col);

#endif