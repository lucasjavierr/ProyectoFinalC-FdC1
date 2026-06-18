#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "menu.h"

/* ─── Colores de consola Windows ─────────────────────────────── */
#define COLOR_RESET      7
#define COLOR_AMARILLO  14   /* Pac-Man */
#define COLOR_ROJO       4   /* Blinky  */
#define COLOR_MAGENTA   13   /* Pinky   */
#define COLOR_CIAN      11   /* Inky    */
#define COLOR_NARANJA    6   /* Clyde   */
#define COLOR_AZUL_OSC   9   /* fantasma asustado */
#define COLOR_BLANCO    15
#define COLOR_GRIS       8

static HANDLE hConsola;

static void setColor(int color) {
    SetConsoleTextAttribute(hConsola, color);
}

static void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsola, c);
}

static void ocultarCursor() {
    CONSOLE_CURSOR_INFO info = { 100, FALSE };
    SetConsoleCursorInfo(hConsola, &info);
}

/* ─── Logo ASCII ─────────────────────────────────────────────── */
/*  Cada línea del logo tiene su propio color para efecto arcade  */
static const char *LOGO[] = {
    "  ____   _    ____       __  __    _    _   _ ",
    " |  _ \\ / \\  / ___|     |  \\/  |  / \\  | \\ | |",
    " | |_) / _ \\| |   _____ | |\\/| | / _ \\ |  \\| |",
    " |  __/ ___ \\ |__|_____|| |  | |/ ___ \\| |\\  |",
    " |_| /_/   \\_\\____|     |_|  |_/_/   \\_\\_| \\_|",
};
#define LOGO_FILAS 5
#define LOGO_COLS  46

static const int LOGO_COLORES[LOGO_FILAS] = {
    COLOR_AMARILLO,
    COLOR_AMARILLO,
    COLOR_AMARILLO,
    COLOR_AMARILLO,
    COLOR_AMARILLO,
};

/* ─── Animación del logo (letra por letra) ───────────────────── */
static void animarLogo(int xOff, int yOff) {
    for (int f = 0; f < LOGO_FILAS; f++) {
        gotoxy(xOff, yOff + f);
        setColor(LOGO_COLORES[f]);
        const char *linea = LOGO[f];
        for (int c = 0; linea[c] != '\0'; c++) {
            putchar(linea[c]);
            fflush(stdout);
            Sleep(8);   /* velocidad de escritura */
        }
    }
    setColor(COLOR_RESET);
}

/* ─── Banda de fantasmas animada ─────────────────────────────── */
/* Los fantasmas "corren" de izquierda a derecha en loop          */
typedef struct {
    float x;
    int color;
    const char *sprite;
} FantasmaAnim;

#define N_FANTASMAS_ANIM 4
#define ANCHO_PANTALLA   60
#define FILA_FANTASMAS   8   /* fila de consola donde se muestran */

static FantasmaAnim banda[N_FANTASMAS_ANIM] = {
    { 0.0f,  COLOR_ROJO,     "\xDB\xDB" },   /* Blinky  (bloques ASCII)  */
    { 8.0f,  COLOR_MAGENTA,  "\xDB\xDB" },   /* Pinky                    */
    {16.0f,  COLOR_CIAN,     "\xDB\xDB" },   /* Inky                     */
    {24.0f,  COLOR_NARANJA,  "\xDB\xDB" },   /* Clyde                    */
};

/* Dibuja un "fantasma" hecho de 2 bloques de color */
static void dibujarFantasmaAnim(int x, int fila, int color) {
    if (x < 0 || x >= ANCHO_PANTALLA - 1) return;
    gotoxy(x, fila);
    setColor(color);
    printf("\xDB\xDB");
    setColor(COLOR_RESET);
}

/* Borra la posición anterior */
static void borrarFantasmaAnim(int x, int fila) {
    if (x < 0 || x >= ANCHO_PANTALLA - 1) return;
    gotoxy(x, fila);
    printf("  ");
}

/* ─── Etiquetas del menú ─────────────────────────────────────── */
static const char *OPCIONES_TEXTO[TOTAL_OPCIONES] = {
    "  [ JUGAR ]                 ",
    "  [ PARTIDAS GUARDADAS ]    ",
    "  [ VER HIGHSCORE ]         ",
    "  [ INSTRUCCIONES ]         ",
    "  [ SALIR ]                 ",
};

static const int COLORES_OPCION[TOTAL_OPCIONES] = {
    COLOR_AMARILLO,
    COLOR_CIAN,
    COLOR_NARANJA,
    COLOR_MAGENTA,
    COLOR_ROJO,
};

#define FILA_MENU_BASE  11   /* fila de consola donde empieza el menú */
#define COL_MENU        8

static void dibujarMenu(int seleccion) {
    for (int i = 0; i < TOTAL_OPCIONES; i++) {
        gotoxy(COL_MENU, FILA_MENU_BASE + i * 2);
        if (i == seleccion) {
            setColor(COLOR_BLANCO);
            printf(">");
            setColor(COLORES_OPCION[i] | BACKGROUND_INTENSITY);
            /* resaltar con fondo usando atributo de intensidad */
            SetConsoleTextAttribute(hConsola,
                COLORES_OPCION[i] | (BACKGROUND_BLUE >> 4));
            printf("%s", OPCIONES_TEXTO[i]);
        } else {
            setColor(COLOR_GRIS);
            printf(" %s", OPCIONES_TEXTO[i]);
        }
        setColor(COLOR_RESET);
    }
}

static void dibujarAyudaNavegacion() {
    int fila = FILA_MENU_BASE + TOTAL_OPCIONES * 2 + 1;
    gotoxy(COL_MENU, fila);
    setColor(COLOR_GRIS);
    printf("  W / S   para navegar     Enter para confirmar");
    setColor(COLOR_RESET);
}

static void dibujarSubtitulo(int xOff, int yOff) {
    gotoxy(xOff + 8, yOff + LOGO_FILAS + 1);
    setColor(COLOR_GRIS);
    printf("~ Desarrollado por Lucas Javier. Todos los derechos son reservados. ~\n");
    setColor(COLOR_RESET);
}

/* ─── Función principal del menú ─────────────────────────────── */
OpcionMenu mostrarMenu() {
    hConsola = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(437);   /* Página de códigos con bloques ASCII */
    system("cls");
    ocultarCursor();

    /* --- 1. Animación inicial del logo --- */
    int xOff = 7, yOff = 1;
    animarLogo(xOff, yOff);
    dibujarSubtitulo(xOff, yOff);

    /* --- 2. Dibujar menú en su estado inicial --- */
    int seleccion = 0;
    dibujarMenu(seleccion);
    dibujarAyudaNavegacion();

    /* --- 3. Inicializar posiciones de fantasmas --- */
    for (int i = 0; i < N_FANTASMAS_ANIM; i++)
        banda[i].x = (float)(i * 10);

    int posAnt[N_FANTASMAS_ANIM];
    for (int i = 0; i < N_FANTASMAS_ANIM; i++)
        posAnt[i] = (int)banda[i].x;

    /* --- 4. Loop principal del menú --- */
    int corriendo = 1;
    int frameCounter = 0;

    while (corriendo) {
        /* Mover y redibujar fantasmas cada ciertos frames */
        if (frameCounter % 2 == 0) {
            for (int i = 0; i < N_FANTASMAS_ANIM; i++) {
                borrarFantasmaAnim(posAnt[i], FILA_FANTASMAS);
                banda[i].x += 1.2f;
                if (banda[i].x >= ANCHO_PANTALLA)
                    banda[i].x = -4.0f;
                posAnt[i] = (int)banda[i].x;
                dibujarFantasmaAnim(posAnt[i], FILA_FANTASMAS, banda[i].color);
            }
        }
        frameCounter++;

        /* Leer tecla sin bloquear */
        if (_kbhit()) {
            char tecla = _getch();

            /* Soporte para teclas de flecha (devuelven 0 o 224 + código) */
            if (tecla == 0 || tecla == (char)224) {
                tecla = _getch();
                if      (tecla == 72) tecla = 'w';   /* flecha arriba  */
                else if (tecla == 80) tecla = 's';   /* flecha abajo   */
            }

            switch (tecla) {
                case 'w': case 'W':
                    seleccion = (seleccion - 1 + TOTAL_OPCIONES) % TOTAL_OPCIONES;
                    dibujarMenu(seleccion);
                    break;
                case 's': case 'S':
                    seleccion = (seleccion + 1) % TOTAL_OPCIONES;
                    dibujarMenu(seleccion);
                    break;
                case '\r': case '\n':
                    corriendo = 0;
                    break;
            }
        }

        Sleep(50);   /* ~20 fps */
    }

    system("cls");
    SetConsoleOutputCP(65001);   /* Restaurar UTF-8 para el juego */
    return (OpcionMenu)seleccion;
}