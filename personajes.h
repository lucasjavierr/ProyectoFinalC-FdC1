#ifndef PERSONAJES_H
#define PERSONAJES_H

#include <time.h>

#define SPRITE_PACMAN "C>"
#define SPRITE_BLINKY "🔴"
#define SPRITE_PINKY "🩷"
#define SPRITE_INKY "🔵"
#define SPRITE_CLYDE "🟠"
#define SPRITE_ASUSTADO "😱"

typedef enum {
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA,
    QUIETO
} Direccion;

typedef struct {
    int fila;
    int col;
    int vidas;
    int puntaje;
    Direccion dir;
    char sprite[10];
    time_t tiempoInicio;
} Pacman;

typedef enum {
    BLINKY,
    PINKY,
    INKY,
    CLYDE
} TipoFantasma;

typedef struct {
    int fila;
    int col;
    TipoFantasma tipo;
    int asustado;
    Direccion dir;
    char sprite[10];
    char spriteAsustado[10];
} Fantasma;

void iniciarPacman(Pacman *pacman, int fila, int col);
void moverPacman(Pacman *pacman, Direccion dir);
void actualizarSpritePacman(Pacman *pacman);

void iniciarFantasma(Fantasma *fantasma, TipoFantasma tipo, int fila, int col);
void moverFantasma(Fantasma *fantasma);
void asustarFantasma(Fantasma fantasmas[], int n);
void calmarFantasma(Fantasma fantasmas[], int n);

int colisionPacmanFantasma(Pacman *pacman, Fantasma *fantasma);

#endif