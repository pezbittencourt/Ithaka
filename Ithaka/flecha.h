#ifndef FLECHA_H
#define FLECHA_H

#include "personagem.h"

typedef struct Flecha {
    float x, y, angulo, anguloInicial, vx, vy, tempo_de_vida;
    int largura, altura;
} Flecha;

void adicionarFlecha(Flecha** array, int* count, int* capacidade, Flecha flecha);
bool atualizar_flecha(Flecha* flecha, int y_chao, float g, Personagem circe);
void remover_flecha(Flecha** array, int* count, int indice);

#endif