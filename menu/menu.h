#ifndef MENU_H
#define MENU_H

typedef enum {
    OPCION_JUGAR = 0,
    OPCION_HIGHSCORE,
    OPCION_INSTRUCCIONES,
    OPCION_SALIR,
    TOTAL_OPCIONES
} OpcionMenu;

OpcionMenu mostrarMenu();
int mostrarMenuNivel();

#endif