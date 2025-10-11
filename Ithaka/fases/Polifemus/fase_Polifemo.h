#ifndef FASE_POLIFEMO_H
#define FASE_POLIFEMO_H

#include <allegro5/allegro.h>
#include <stdbool.h>

// Estrutura para gerenciar os cenários
typedef struct {
    ALLEGRO_BITMAP* fundos[6];
    ALLEGRO_BITMAP* fundo4_arvore;
    ALLEGRO_BITMAP* fundo5_pedra;
    ALLEGRO_BITMAP* fundo5_escuro;
    ALLEGRO_BITMAP* fundo6_rocha;
    int cenario_atual;
    int total_cenarios;
} CenarioPolifemo;

// Declaração de TODAS as funções
bool carregar_cenarios_polifemo(CenarioPolifemo* cenario);
void desenhar_cenario_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela);
void desenhar_sobreposicoes_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela);  // ← FALTAVA ESTA
void atualizar_transicao_cenario(CenarioPolifemo* cenario, float* pos_x_personagem,
    float largura_personagem, int largura_tela);
void destruir_cenarios_polifemo(CenarioPolifemo* cenario);
void desenhar_sobreposicoes_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela);
#endif