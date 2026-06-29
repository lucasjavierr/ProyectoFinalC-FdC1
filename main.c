#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h> 
#include <time.h>
#include "mapa/mapa.h"
#include "estructuras/grafo.h"
#include "personajes/personajes.h"
#include "menu/menu.h"

#define NUM_FANTASMAS 4
#define DURACION_ASUSTADO 15

typedef struct {
    int sleepMs;
    int velocidadFantasmas;
    float multiplicadorBonus;
    const char *nombre;
} ConfigNivel;

static const ConfigNivel NIVELES[3] = {
    { 500, 3, 1.0f,  "FACIL"   },
    { 450, 2, 1.5f,  "NORMAL"  },
    { 350, 1, 2.0f,  "DIFICIL" },
};

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
            printf("  ║  %d. %-10s  %6d pts   ║\n", posicion, nombre, puntaje);
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
    printf("  ║  W / A / S / D  →  Mover a Pac-Man   ║\n");
    printf("  ║  Q              →  Salir del juego   ║\n");
    printf("  ║                                      ║\n");
    printf("  ║  Come los puntos (·) para puntaje.   ║\n");
    printf("  ║  Come los power-ups (●) para asustar ║\n");
    printf("  ║  a los fantasmas y comerlos.         ║\n");
    printf("  ║                                      ║\n");
    printf("  ║  Tenes 3 vidas. ¡Que no te atrapen!  ║\n");
    printf("  ║                                      ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("\n  Presiona cualquier tecla para volver...\n");
    _getch();
}

int contarMonedas() {
    int total = 0;
    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLS; j++)
            if (mapa[i][j] == PUNTO || mapa[i][j] == POWER)
                total++;
    return total;
}

void mostrarVictoria(int puntaje, int segundos, const char *nivel) {
    system("cls");
    printf("\n\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║          ¡¡ GANASTE !!               ║\n");
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║                                      ║\n");
    printf("  ║  Dificultad: %-8s               ║\n", nivel);
    printf("  ║  Tiempo:     %3d segundos            ║\n", segundos);
    printf("  ║  Puntaje:    %6d pts             ║\n", puntaje);
    printf("  ║                                      ║\n");
    printf("  ╚══════════════════════════════════════╝\n\n");

    char nombre[21];
    while (_kbhit()) _getch();
    do {
        printf("  Ingresa tu nombre (max 20 caracteres): ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';
        if (nombre[0] == '\0')
            printf("  El nombre no puede estar vacio. Intenta de nuevo.\n");
    } while (nombre[0] == '\0');

    FILE *f = fopen("highscore.txt", "a");
    if (f != NULL) {
        fprintf(f, "%-20s %d\n", nombre, puntaje);
        fclose(f);
        printf("\n  ¡Puntaje guardado! Bien jugado, %s.\n\n", nombre);
    } else {
        printf("\n  No se pudo guardar el puntaje.\n\n");
    }

    printf("  Presiona cualquier tecla para continuar...\n");
    _getch();
}

int calcularBonusTiempo(int segundos) {
    int base = 180;
    if (segundos >= base) return 0;
    return (base - segundos) * 10;
}

void jugar(int nivel) {
    if (nivel < 1) nivel = 1;
    if (nivel > 3) nivel = 3;
    ConfigNivel cfg = NIVELES[nivel - 1];

    srand(time(NULL));
    SetConsoleOutputCP(65001);
    system("cls"); 
    ocultarCursor();
    reiniciarMapa();

    Pacman pacman;
    iniciarPacman(&pacman, 10, 9);
    pacman.tiempoInicio = time(NULL);

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
                case ' ': {
                    time_t inicioPausa = time(NULL);
                    limpiarPantalla();
                    printf("╔══════════════════════════════════════╗\n");
                    printf("║           ⏸  PAUSA                   ║\n");
                    printf("╚══════════════════════════════════════╝\n\n");
                    imprimirMapa(&pacman, fantasmas, NUM_FANTASMAS);
                    printf("\n  Presiona ESPACIO para continuar...\n");

                    char t;
                    do { t = _getch(); } while (t != ' ');

                    pacman.tiempoInicio += (time_t)(time(NULL) - inicioPausa);
                    break;
                }
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
            pacman.col = 9;
            pacman.dir = QUIETO;
            actualizarSpritePacman(&pacman);
            tiempoAsustado = 0;
            calmarFantasma(fantasmas, NUM_FANTASMAS);
            continue; 
        }

        turno++;
        if (turno % cfg.velocidadFantasmas == 0) {
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
            pacman.fila = 10;
            pacman.col = 9; 
            pacman.dir = QUIETO;
            actualizarSpritePacman(&pacman);
            tiempoAsustado = 0;
            calmarFantasma(fantasmas, NUM_FANTASMAS);
            continue;
        }

        if (contarMonedas() == 0) {
            int segundos = (int)(time(NULL) - pacman.tiempoInicio);
            int bonus = (int)(calcularBonusTiempo(segundos) * cfg.multiplicadorBonus);
            pacman.puntaje += bonus;
            liberarGrafo(&grafo);
            mostrarVictoria(pacman.puntaje, segundos, cfg.nombre);
            return;
        }

        int segundosTranscurridos = (int)(time(NULL) - pacman.tiempoInicio);

        limpiarPantalla();
        printf("╔══════════════════════════════════════╗\n");
        (tiempoAsustado > 0) 
        ? printf("║   ¡FANTASMAS ASUSTADOS! Corre (%02d)   ║\n", tiempoAsustado)
        : printf("║  [%-7s]  PAC-MAN — Moviendo...   ║\n", cfg.nombre);
        printf("╚══════════════════════════════════════╝\n\n");

        imprimirMapa(&pacman, fantasmas, NUM_FANTASMAS);

        printf("\nVidas: %d  |  Puntaje: %d  |  Tiempo: %ds  |  Nivel: %s\n",
                pacman.vidas, pacman.puntaje, segundosTranscurridos, cfg.nombre);
        printf("Controles: [W/A/S/D] para mover | [ESPACIO] pausa | [Q] para salir\n");

        Sleep(cfg.sleepMs);
    }

    liberarGrafo(&grafo);
    system("cls");

    if (pacman.vidas <= 0) {
        printf("\n\n");
        printf("  ╔══════════════════════════════════════╗\n");
        printf("  ║            GAME OVER                 ║\n");
        printf("  ╠══════════════════════════════════════╣\n");
        printf("  ║                                      ║\n");
        printf("  ║  Te quedaste sin vidas.              ║\n");
        printf("  ║  Nivel:    %-8s                 ║\n", cfg.nombre);
        printf("  ║  Puntaje:  %6d pts               ║\n", pacman.puntaje);
        printf("  ║                                      ║\n");
        printf("  ╚══════════════════════════════════════╝\n\n");
    } else {
        printf("\n\n");
        printf("  ╔══════════════════════════════════════╗\n");
        printf("  ║         JUEGO FINALIZADO             ║\n");
        printf("  ╠══════════════════════════════════════╣\n");
        printf("  ║                                      ║\n");
        printf("  ║  Puntaje:  %6d pts               ║\n", pacman.puntaje);
        printf("  ║                                      ║\n");
        printf("  ╚══════════════════════════════════════╝\n\n");
    }

    printf("  Presiona cualquier tecla para volver al menu...\n");
    while (_kbhit()) _getch();
    _getch();
}

int main() {
    srand(time(NULL));

    int enEjecucion = 1;
    while (enEjecucion) {
        OpcionMenu opcion = mostrarMenu();

        switch (opcion) {
            case OPCION_JUGAR: {
                int nivel = mostrarMenuNivel();
                jugar(nivel);
                break;
            }

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