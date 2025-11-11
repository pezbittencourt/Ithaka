#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include <stdbool.h>

typedef struct Personagem {
    float x, y,target_x, target_y, forca_disparo, tempo_ataque, estado_progresso, angulo;
    int largura, altura, frame_atual, contador_animacao, vida, estado, sprite_ativo;
    bool olhando_direita, olhando_esquerda, andando, desembainhando,
        sofrendo_dano, guardando_espada, atacando, acerto, tem_espada, disparando;
    int frame_contador;
    int velocidade_animacao;
    int num_frames;
    int cooldown_ataque;
    int cooldown_dano;
} Personagem;

#endif