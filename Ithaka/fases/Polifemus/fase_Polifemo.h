#ifndef FASE_POLIFEMO_H
#define FASE_POLIFEMO_H

#include <allegro5/allegro.h>
#include <stdbool.h>

// Estrutura para gerenciar os cenários
typedef struct {
    ALLEGRO_BITMAP* fundos[16];
    ALLEGRO_BITMAP* fundo4_arvore;
    ALLEGRO_BITMAP* fundo5_pedra;
    ALLEGRO_BITMAP* fundo5_escuro;
    ALLEGRO_BITMAP* fundo6_rocha;
    ALLEGRO_BITMAP* frenteCirce1;
    ALLEGRO_BITMAP* frenteCirce3;
    ALLEGRO_BITMAP* efeitoFrenteCirce3;
    int cenario_atual;
    int total_cenarios;
} CenarioPolifemo;

// Funções principais
bool carregar_cenarios_polifemo(CenarioPolifemo* cenario);
void desenhar_cenario_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela);
void desenhar_sobreposicoes_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela);
void atualizar_transicao_cenario(CenarioPolifemo* cenario, float* pos_x_personagem,
    float largura_personagem, int largura_tela);
void destruir_cenarios_polifemo(CenarioPolifemo* cenario);

#endif
