#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h> 
#include <time.h>
#include "mapa.h"
#include "grafo.h"
#include "personajes.h"
#include "menu.h"

#define NUM_FANTASMAS 4
#define DURACION_ASUSTADO 15 
#define VELOCIDAD_FANTASMAS 2

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

void mostrarHighscore() {
    system("cls");
    printf("\n\n");
    printf("  ╔══════════════════════════════╗\n");
    printf("  ║        MEJORES PUNTAJES      ║\n");
    printf("  ╠══════════════════════════════╣\n");

    FILE *f = fopen("highscore.txt", "r");
    if (f == NULL) {
        printf("  ║   Todavia no hay puntajes.   ║\n");
    } else {
        char nombre[32];
        int puntaje;
        int posicion = 1;
        while (fscanf(f, "%31s %d", nombre, &puntaje) == 2 && posicion <= 5) {
            printf("  ║  %d. %-15s  %6d pts  ║\n", posicion, nombre, puntaje);
            posicion++;
        }
        fclose(f);
    }

    printf("  ╚══════════════════════════════╝\n");
    printf("\n  Presiona cualquier tecla para volver...\n");
    _getch();
}

void mostrarInstrucciones() {
    system("cls");
    printf("\n\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║             INSTRUCCIONES            ║\n");
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║                                      ║\n");
    printf("  ║  W / A / S / D  →  Mover a Pac-Man  ║\n");
    printf("  ║  Q              →  Salir del juego   ║\n");
    printf("  ║                                      ║\n");
    printf("  ║  Come los puntos (·) para puntaje.   ║\n");
    printf("  ║  Come los power-ups (●) para asustar ║\n");
    printf("  ║  a los fantasmas y comerlos.          ║\n");
    printf("  ║                                      ║\n");
    printf("  ║  Tenes 3 vidas. ¡No te atrapen!      ║\n");
    printf("  ║                                      ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("\n  Presiona cualquier tecla para volver...\n");
    _getch();
}

void mostrarPartidasGuardadas() {
    system("cls");
    printf("\n\n");
    printf("  ╔══════════════════════════════════╗\n");
    printf("  ║        PARTIDAS GUARDADAS        ║\n");
    printf("  ╠══════════════════════════════════╣\n");
    printf("  ║                                  ║\n");
    printf("  ║  Funcion no implementada aun.    ║\n");
    printf("  ║  Proximamente podras guardar     ║\n");
    printf("  ║  y continuar tus partidas.       ║\n");
    printf("  ║                                  ║\n");
    printf("  ╚══════════════════════════════════╝\n");
    printf("\n  Presiona cualquier tecla para volver...\n");
    _getch();
}

void guardarPuntaje(int puntaje) {
    FILE *f = fopen("highscore.txt", "a");
    if (f != NULL) {
        fprintf(f, "Jugador %d\n", puntaje);
        fclose(f);
    }
}

void jugar() {
    srand(time(NULL));
    SetConsoleOutputCP(65001);
    system("cls"); 
    ocultarCursor();

    Pacman pacman;
    iniciarPacman(&pacman, 10, 8);

    Fantasma fantasmas[NUM_FANTASMAS];
    iniciarFantasma(&fantasmas[0], BLINKY, 6, 9);
    iniciarFantasma(&fantasmas[1], PINKY, 8, 8);
    iniciarFantasma(&fantasmas[2], INKY, 8, 9);
    iniciarFantasma(&fantasmas[3], CLYDE, 8, 10);

    Grafo grafo;
    construirGrafo(&grafo);

    int juegoCorriendo = 1;
    int tiempoAsustado = 0;
    int turno = 0;

    while (juegoCorriendo && pacman.vidas > 0) {
        int pacmanFilaAnterior = pacman.fila;
        int pacmanColAnterior = pacman.col;
        
        int fantasmasFilaAnterior[NUM_FANTASMAS];
        int fantasmasColAnterior[NUM_FANTASMAS];
        for (int i = 0; i < NUM_FANTASMAS; i++) {
            fantasmasFilaAnterior[i] = fantasmas[i].fila;
            fantasmasColAnterior[i] = fantasmas[i].col;
        }

        if (_kbhit()) {
            char tecla = _getch();
            switch (tecla) {
                case 'w': case 'W': moverPacman(&pacman, ARRIBA); break;
                case 's': case 'S': moverPacman(&pacman, ABAJO); break;
                case 'a': case 'A': moverPacman(&pacman, IZQUIERDA); break;
                case 'd': case 'D': moverPacman(&pacman, DERECHA); break;
                case 'q': case 'Q': juegoCorriendo = 0; break; 
            }
        } else {
            if (pacman.dir != QUIETO) {
                moverPacman(&pacman, pacman.dir);
            }
        }

        if (mapa[pacman.fila][pacman.col] == POWER) { 
            mapa[pacman.fila][pacman.col] = CAMINO;
            pacman.puntaje += 50;
            tiempoAsustado = DURACION_ASUSTADO;
            asustarFantasma(fantasmas, NUM_FANTASMAS);
        }

        if (tiempoAsustado > 0) {
            tiempoAsustado--;
            if (tiempoAsustado == 0) {
                calmarFantasma(fantasmas, NUM_FANTASMAS);
            }
        }

        int colisionDetectada = 0;
        for (int i = 0; i < NUM_FANTASMAS; i++) {
            if (colisionPacmanFantasma(&pacman, &fantasmas[i]) || 
                (pacman.fila == fantasmasFilaAnterior[i] && pacman.col == fantasmasColAnterior[i] &&
                fantasmas[i].fila == pacmanFilaAnterior && fantasmas[i].col == pacmanColAnterior)) {
                
                if (fantasmas[i].asustado) {
                    pacman.puntaje += 200; 
                    if (i == 0) { fantasmas[i].fila = 6; fantasmas[i].col = 9; }
                    else if (i == 1) { fantasmas[i].fila = 8; fantasmas[i].col = 8; }
                    else if (i == 2) { fantasmas[i].fila = 8; fantasmas[i].col = 9; }
                    else if (i == 3) { fantasmas[i].fila = 8; fantasmas[i].col = 10; }
                
                    fantasmas[i].asustado = 0;
                    calmarFantasma(&fantasmas[i], 1);
                } else {
                    colisionDetectada = 1;
                    break;
                }
            }
        }

        if (colisionDetectada) {
            pacman.vidas--;
            pacman.fila = 10; 
            pacman.col = 8;
            pacman.dir = QUIETO;
            actualizarSpritePacman(&pacman);
            tiempoAsustado = 0;
            calmarFantasma(fantasmas, NUM_FANTASMAS);
            continue; 
        }

        turno++;
        if (turno % VELOCIDAD_FANTASMAS == 0) {
            for (int i = 0; i < NUM_FANTASMAS; i++) {
                moverFantasmaInteligente(&fantasmas[i], &pacman, &grafo);
            }
        }

        for (int i = 0; i < NUM_FANTASMAS; i++) {
            if (colisionPacmanFantasma(&pacman, &fantasmas[i]) || 
                (pacman.fila == fantasmasFilaAnterior[i] && pacman.col == fantasmasColAnterior[i] &&
                fantasmas[i].fila == pacmanFilaAnterior && fantasmas[i].col == pacmanColAnterior)) {
                
                if (fantasmas[i].asustado) {
                    pacman.puntaje += 200;
                    if (i == 0) { fantasmas[i].fila = 6; fantasmas[i].col = 9; }
                    else if (i == 1) { fantasmas[i].fila = 8; fantasmas[i].col = 8; }
                    else if (i == 2) { fantasmas[i].fila = 8; fantasmas[i].col = 9; }
                    else if (i == 3) { fantasmas[i].fila = 8; fantasmas[i].col = 10; }
                    fantasmas[i].asustado = 0;
                    calmarFantasma(&fantasmas[i], 1);
                } else {
                    colisionDetectada = 1;
                    break;
                }
            }
        }

        if (colisionDetectada) {
            pacman.vidas--;
            pacman.fila = 14; pacman.col = 9; pacman.dir = QUIETO;
            actualizarSpritePacman(&pacman);
            tiempoAsustado = 0;
            calmarFantasma(fantasmas, NUM_FANTASMAS);
            continue;
        }

        limpiarPantalla();
        printf("╔══════════════════════════════════════╗\n");
        (tiempoAsustado > 0) 
        ? printf("║   ¡FANTASMAS ASUSTADOS! Corre (%02d)   ║\n", tiempoAsustado)
        : printf("║       PAC-MAN — Moviendo...          ║\n");
        printf("╚══════════════════════════════════════╝\n\n");

        imprimirMapa(&pacman, fantasmas, NUM_FANTASMAS);

        printf("\nVidas: %d  |  Puntaje: %d\n", pacman.vidas, pacman.puntaje);
        printf("Controles: [W/A/S/D] para mover | [Q] para salir\n");

        Sleep(500);
    }

    system("cls");
    if (pacman.vidas <= 0) {
        printf("\n\n   ¡GAME OVER! Te has quedado sin vidas.\n");
    } else {
        printf("\n\n   Juego finalizado correctamente.\n");
    }
    printf("   Puntaje Final: %d\n\n", pacman.puntaje);
    liberarGrafo(&grafo);
}

int main() {
    srand(time(NULL));

    int enEjecucion = 1;
    while (enEjecucion) {
        OpcionMenu opcion = mostrarMenu();

        switch (opcion) {
            case OPCION_JUGAR:
                jugar();
                break;
            case OPCION_PARTIDAS_GUARDADAS:
                mostrarPartidasGuardadas();
                break;
            case OPCION_HIGHSCORE:
                mostrarHighscore();
                break;
            case OPCION_INSTRUCCIONES:
                mostrarInstrucciones();
                break;
            case OPCION_SALIR:
                enEjecucion = 0;
                break;
            default:
                break;
        }
    }
    system("cls");
    printf("\n  ¡Hasta luego!\n\n");
    return 0;
}