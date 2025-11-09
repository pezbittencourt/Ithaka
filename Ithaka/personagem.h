#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include <stdbool.h>

typedef struct Personagem {
    float x, y, forca_disparo, tempo_ataque;
    int largura, altura, frame_atual, contador_animacao,vida;
    bool olhando_direita, olhando_esquerda, andando, desembainhando,
        sofrendo_dano, guardando_espada, atacando, acerto, tem_espada, disparando;
    int frame_contador;
    int velocidade_animacao;
    int num_frames;
    int cooldown_ataque;
	int cooldown_dano;
} Personagem;

#endif