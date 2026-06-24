#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "menu.h"

#define COLOR_RESET      7
#define COLOR_AMARILLO  14
#define COLOR_ROJO       4
#define COLOR_MAGENTA   13
#define COLOR_CIAN      11
#define COLOR_NARANJA    6
#define COLOR_AZUL_OSC   9
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

static void animarLogo(int xOff, int yOff) {
    for (int f = 0; f < LOGO_FILAS; f++) {
        gotoxy(xOff, yOff + f);
        setColor(LOGO_COLORES[f]);
        const char *linea = LOGO[f];
        for (int c = 0; linea[c] != '\0'; c++) {
            putchar(linea[c]);
            fflush(stdout);
            Sleep(8);
        }
    }
    setColor(COLOR_RESET);
}

typedef struct {
    float x;
    int color;
    const char *sprite;
} FantasmaAnim;

#define N_FANTASMAS_ANIM 4
#define ANCHO_PANTALLA   60
#define FILA_FANTASMAS   8

static FantasmaAnim banda[N_FANTASMAS_ANIM] = {
    { 0.0f,  COLOR_ROJO,     "\xDB\xDB" },
    { 8.0f,  COLOR_MAGENTA,  "\xDB\xDB" },
    {16.0f,  COLOR_CIAN,     "\xDB\xDB" },
    {24.0f,  COLOR_NARANJA,  "\xDB\xDB" },
};

static void dibujarFantasmaAnim(int x, int fila, int color) {
    if (x < 0 || x >= ANCHO_PANTALLA - 1) return;
    gotoxy(x, fila);
    setColor(color);
    printf("\xDB\xDB");
    setColor(COLOR_RESET);
}

static void borrarFantasmaAnim(int x, int fila) {
    if (x < 0 || x >= ANCHO_PANTALLA - 1) return;
    gotoxy(x, fila);
    printf("  ");
}

static const char *OPCIONES_TEXTO[TOTAL_OPCIONES] = {
    "  [ JUGAR ]                 ",
    "  [ VER HIGHSCORE ]         ",
    "  [ INSTRUCCIONES ]         ",
    "  [ SALIR ]                 ",
};

static const int COLORES_OPCION[TOTAL_OPCIONES] = {
    COLOR_AMARILLO,
    COLOR_NARANJA,
    COLOR_MAGENTA,
    COLOR_ROJO,
};

#define FILA_MENU_BASE 11
#define COL_MENU 8

static void dibujarMenu(int seleccion) {
    for (int i = 0; i < TOTAL_OPCIONES; i++) {
        gotoxy(COL_MENU, FILA_MENU_BASE + i * 2);
        if (i == seleccion) {
            setColor(COLOR_BLANCO);
            printf(">");
            setColor(COLORES_OPCION[i] | BACKGROUND_INTENSITY);
            SetConsoleTextAttribute(hConsola, COLORES_OPCION[i] | (BACKGROUND_BLUE >> 4));
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
    gotoxy(xOff + 1, yOff + LOGO_FILAS + 1);
    setColor(COLOR_GRIS);
    printf("~ Desarrollado por Lucas Javier. Todos los derechos son reservados. ~\n");
    setColor(COLOR_RESET);
}

OpcionMenu mostrarMenu() {
    hConsola = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(437);
    system("cls");
    ocultarCursor();

    int xOff = 7, yOff = 1;
    animarLogo(xOff, yOff);
    dibujarSubtitulo(xOff, yOff);

    int seleccion = 0;
    dibujarMenu(seleccion);
    dibujarAyudaNavegacion();

    for (int i = 0; i < N_FANTASMAS_ANIM; i++)
        banda[i].x = (float)(i * 10);

    int posAnt[N_FANTASMAS_ANIM];
    for (int i = 0; i < N_FANTASMAS_ANIM; i++)
        posAnt[i] = (int)banda[i].x;

    int corriendo = 1;
    int frameCounter = 0;

    while (corriendo) {
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

        if (_kbhit()) {
            char tecla = _getch();

            if (tecla == 0 || tecla == (char)224) {
                tecla = _getch();
                if      (tecla == 72) tecla = 'w';
                else if (tecla == 80) tecla = 's';
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

        Sleep(50);
    }

    system("cls");
    SetConsoleOutputCP(65001);
    return (OpcionMenu)seleccion;
}

int mostrarMenuNivel() {
    system("cls");
    SetConsoleOutputCP(437);
    ocultarCursor();

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD c = { 8, 2 };
    SetConsoleCursorPosition(h, c);
    SetConsoleTextAttribute(h, COLOR_AMARILLO);
    printf("SELECCIONA LA DIFICULTAD");
    SetConsoleTextAttribute(h, COLOR_RESET);

    static const char *niveles[3] = {
        "  [ FACIL   ]  - Ritmo tranquilo, ideal para aprender   ",
        "  [ NORMAL  ]  - Velocidad estandar, el desafio clasico ",
        "  [ DIFICIL ]  - Todo mas rapido, para los valientes     ",
    };
    static const int coloresNivel[3] = {
        COLOR_CIAN,
        COLOR_AMARILLO,
        COLOR_ROJO,
    };

    int sel = 0;
    int corriendo = 1;

    while (corriendo) {
        for (int i = 0; i < 3; i++) {
            COORD pos = { (SHORT)6, (SHORT)(5 + i * 2) };
            SetConsoleCursorPosition(h, pos);
            if (i == sel) {
                SetConsoleTextAttribute(h, COLOR_BLANCO);
                printf(">");
                SetConsoleTextAttribute(h, coloresNivel[i]);
            } else {
                SetConsoleTextAttribute(h, COLOR_GRIS);
                printf(" ");
            }
            printf("%s", niveles[i]);
            SetConsoleTextAttribute(h, COLOR_RESET);
        }

        COORD ayuda = { 6, 12 };
        SetConsoleCursorPosition(h, ayuda);
        SetConsoleTextAttribute(h, COLOR_GRIS);
        printf("  W / S   para navegar     Enter para confirmar  ");
        SetConsoleTextAttribute(h, COLOR_RESET);

        if (_kbhit()) {
            char tecla = _getch();
            if (tecla == 0 || tecla == (char)224) {
                tecla = _getch();
                if (tecla == 72) tecla = 'w';
                else if (tecla == 80) tecla = 's';
            }
            switch (tecla) {
                case 'w': case 'W':
                    sel = (sel - 1 + 3) % 3;
                    break;
                case 's': case 'S':
                    sel = (sel + 1) % 3;
                    break;
                case '\r': case '\n':
                    corriendo = 0;
                    break;
            }
        }
        Sleep(50);
    }

    system("cls");
    SetConsoleOutputCP(65001);
    return sel + 1;
}