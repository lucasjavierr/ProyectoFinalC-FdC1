#include <stdlib.h>
#include "cola.h"

void iniciarCola(Cola *c) {
    c->frente = NULL;
    c->fin = NULL;
    c->cantidad = 0;
}

void encolar(Cola *c, int valor) {
    NodoCola *nuevo = (NodoCola *)malloc(sizeof(NodoCola));
    nuevo->valor = valor;
    nuevo->sig = NULL;

    if (c->fin == NULL) {
        c->frente = nuevo;
        c->fin = nuevo;
    } else {
        c->fin->sig = nuevo;
        c->fin = nuevo;
    }
    c->cantidad++;
}

int desencolar(Cola *c) {
    if (colaVacia(c)) return -1;

    NodoCola *tmp = c->frente;
    int valor = tmp->valor;
    c->frente = c->frente->sig;

    if (c->frente == NULL)
        c->fin = NULL;

    free(tmp);
    c->cantidad--;
    return valor;
}

int colaVacia(Cola *c) {
    return c->frente == NULL;
}

void liberarCola(Cola *c) {
    while (!colaVacia(c))
        desencolar(c);
}