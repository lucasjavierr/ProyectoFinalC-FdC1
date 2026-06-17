#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "personajes.h"
#include "mapa.h"

#define NUM_FANTASMAS 4

void limpiarPantalla() {
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ocultarCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

int main() {
    srand(time(NULL));
    SetConsoleOutputCP(65001);
    ocultarCursor();

    Pacman pacman;
    iniciarPacman(&pacman, 10, 8);

    Fantasma fantasmas[NUM_FANTASMAS];
    iniciarFantasma(&fantasmas[0], BLINKY, 6, 9);
    iniciarFantasma(&fantasmas[1], PINKY, 8, 8);
    iniciarFantasma(&fantasmas[2], INKY, 8, 9);
    iniciarFantasma(&fantasmas[3], CLYDE, 8, 10);

    int juegoCorriendo = 1;
    while (juegoCorriendo && pacman.vidas > 0) {
        if (_kbhit()) {
            char tecla = _getch();
            switch (tecla) {
                case 'w': case 'W': moverPacman(&pacman, ARRIBA); break;
                case 's': case 'S': moverPacman(&pacman, ABAJO); break;
                case 'a': case 'A': moverPacman(&pacman, IZQUIERDA); break;
                case 'd': case 'D': moverPacman(&pacman, DERECHA); break;
                case 'q': case 'Q': juegoCorriendo = 0; break; // Salir del juego
            }
        } else {
            if (pacman.dir != QUIETO) {
                moverPacman(&pacman, pacman.dir);
            }
        }

        for (int i = 0; i < NUM_FANTASMAS; i++) {
            moverFantasma(&fantasmas[i]);
        }

        for (int i = 0; i < NUM_FANTASMAS; i++) {
            if (colisionPacmanFantasma(&pacman, &fantasmas[i])) {
                pacman.vidas--;
                pacman.fila = 10;
                pacman.col = 8;
                pacman.dir = QUIETO;
                actualizarSpritePacman(&pacman);
                break;
            }
        }
        limpiarPantalla();
        printf("╔══════════════════════════════════════╗\n");
        printf("║       PAC-MAN — Moviendo...          ║\n");
        printf("╚══════════════════════════════════════╝\n\n");

        imprimirMapa(&pacman, fantasmas, NUM_FANTASMAS);

        printf("\nVidas: %d  |  Puntaje: %d\n", pacman.vidas, pacman.puntaje);
        printf("Controles: [W/A/S/D] para mover | [Q] para salir\n");

        Sleep(900);
    }

    system("cls");
    if (pacman.vidas <= 0) {
        printf("\n\n   ¡GAME OVER! Te has quedado sin vidas.\n");
    } else {
        printf("\n\n   Juego finalizado correctamente.\n");
    }
    printf("   Puntaje Final: %d\n\n", pacman.puntaje);
    return 0;
}