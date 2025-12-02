#ifndef FASE_H
#define FASE_H

#include <allegro5/allegro.h>
#include "personagem.h"

typedef struct {
    ALLEGRO_BITMAP* fundo;
    ALLEGRO_BITMAP** sobreposicoes;
    Personagem* personagens;
    int total_personagens;
    int total_sobreposicoes;
} Cenario;

typedef struct {
    Cenario* cenarios;
    ALLEGRO_BITMAP** sprites;
    int total_cenarios, cenario_atual, total_sprites;
} Fase;

void init_fase(Fase* fase, int total_cenarios);
void init_fase_sprites(Fase* fase, int total_cenarios, int total_sprites);
void free_fase(Fase* fase);
void init_cenario(Cenario* cenario, int total_sobreposicoes, int total_personagens);
void free_cenario(Cenario* cenario);
void atualizar_transicao_cenario(Fase* fase, float* pos_x_personagem,
    float largura_personagem, int largura_tela);
bool carregar_cenario(Fase* fase, int escolha_fase);
void destruir_cenarios(Fase* fase);
void destruir_sprites(Fase* fase);
void desenhar_cenario(Fase* fase, int largura_tela, int altura_tela);
void desenhar_sobreposicoes(Fase* fase, int largura_tela, int altura_tela);
#endif