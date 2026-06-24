#ifndef COLA_H
#define COLA_H

typedef struct NodoCola {
    int valor;
    struct NodoCola *sig;
} NodoCola;

typedef struct {
    NodoCola *frente;
    NodoCola *fin;
    int cantidad;
} Cola;

void iniciarCola(Cola *c);
void encolar(Cola *c, int valor);
int desencolar(Cola *c);
int colaVacia(Cola *c);
void liberarCola(Cola *c);

#endif