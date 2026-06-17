#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "personajes.h"
#include "mapa.h"
#include "grafo.h"

void iniciarPacman(Pacman *pacman, int fila, int col) {
    pacman->fila = fila;
    pacman->col = col;
    pacman->vidas = 3;
    pacman->puntaje = 0;
    pacman->dir = QUIETO;
    snprintf(pacman->sprite, sizeof(pacman->sprite), "%s", SPRITE_PACMAN);
    pacman->sprite[sizeof(pacman->sprite) - 1] = '\0';
}

void actualizarSpritePacman(Pacman *pacman) {
    switch (pacman->dir) {
        case ARRIBA: snprintf(pacman->sprite, sizeof(pacman->sprite), "C^"); break;
        case ABAJO: snprintf(pacman->sprite, sizeof(pacman->sprite), "Cv"); break;
        case IZQUIERDA: snprintf(pacman->sprite, sizeof(pacman->sprite), "<C"); break;
        case DERECHA: snprintf(pacman->sprite, sizeof(pacman->sprite), "C>"); break;
        default: snprintf(pacman->sprite, sizeof(pacman->sprite), "%s", SPRITE_PACMAN); break;
    }
    pacman->sprite[sizeof(pacman->sprite) - 1] = '\0';
}

void moverPacman(Pacman *pacman, Direccion dir) {
    int nuevaFila = pacman->fila;
    int nuevaCol = pacman->col;

    switch (dir) {
        case ARRIBA: nuevaFila--; break;
        case ABAJO: nuevaFila++; break;
        case IZQUIERDA: nuevaCol--; break;
        case DERECHA: nuevaCol++; break;
        default: break;
    }

    if (!esCamino(nuevaFila, nuevaCol)) return;

    pacman->dir = dir;
    pacman->fila = nuevaFila;
    pacman->col = nuevaCol;
    actualizarSpritePacman(pacman);

    if (mapa[nuevaFila][nuevaCol] == PUNTO) {
        mapa[nuevaFila][nuevaCol] = CAMINO;
        pacman->puntaje += 10;
    }
}

static const char* spriteFantasmas[] = {
    SPRITE_BLINKY,
    SPRITE_PINKY,
    SPRITE_INKY,
    SPRITE_CLYDE
};

void iniciarFantasma(Fantasma *fantasma, TipoFantasma tipo, int fila, int col) {
    fantasma->fila = fila;
    fantasma->col = col;
    fantasma->tipo = tipo;
    fantasma->asustado = 0;
    fantasma->dir = ARRIBA;

    snprintf(fantasma->sprite, sizeof(fantasma->sprite), "%s", spriteFantasmas[tipo]);
    snprintf(fantasma->spriteAsustado, sizeof(fantasma->spriteAsustado), "%s", SPRITE_ASUSTADO);
}

void moverFantasma(Fantasma *fantasma) {
    int deltas[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    int direccionesValidas[4];
    int direccionesProbadas = 0;

    for (int i = 0; i < 4; i++) {
        int nuevaFila = fantasma->fila + deltas[i][0];
        int nuevaCol = fantasma->col + deltas[i][1];

        if (esCamino(nuevaFila, nuevaCol)) {
            if ((i == ARRIBA && fantasma->dir == ABAJO) ||
                (i == ABAJO && fantasma->dir == ARRIBA) ||
                (i == IZQUIERDA && fantasma->dir == DERECHA) ||
                (i == DERECHA && fantasma->dir == IZQUIERDA)) {
                continue; 
            }
            direccionesValidas[direccionesProbadas] = i;
            direccionesProbadas++;
        }
    }

    if (direccionesProbadas == 0) {
        for (int i = 0; i < 4; i++) {
            int nuevaFila = fantasma->fila + deltas[i][0];
            int nuevaCol = fantasma->col + deltas[i][1];
            if (esCamino(nuevaFila, nuevaCol)) {
                direccionesValidas[direccionesProbadas] = i;
                direccionesProbadas++;
            }
        }
    }

    if (direccionesProbadas > 0) {
        int indiceElegido = rand() % direccionesProbadas;
        int dirElegida = direccionesValidas[indiceElegido];

        fantasma->fila += deltas[dirElegida][0];
        fantasma->col += deltas[dirElegida][1];
        fantasma->dir = (Direccion)dirElegida;
    }
}

void asustarFantasma(Fantasma fantasmas[], int n) {
    for (int i = 0; i < n; i++) {
        fantasmas[i].asustado = 1;
        snprintf(fantasmas[i].sprite, sizeof(fantasmas[i].sprite), "%s", fantasmas[i].spriteAsustado);
    }
}

void calmarFantasma(Fantasma fantasmas[], int n) {
    for (int i = 0; i < n; i++) {
        fantasmas[i].asustado = 0;
        snprintf(fantasmas[i].sprite, sizeof(fantasmas[i].sprite), "%s", spriteFantasmas[fantasmas[i].tipo]);
    }
}

int colisionPacmanFantasma(Pacman *pacman, Fantasma *fantasma) {
    return pacman->fila == fantasma->fila && pacman->col == fantasma->col;
}