#ifndef CIRCE_H
#define CIRCE_H

#include <allegro5/allegro.h>
#include <stdbool.h>

// Estrutura para gerenciar os cenários
typedef struct {
    ALLEGRO_BITMAP* fundos[4];
    ALLEGRO_BITMAP* frenteCirce1;
    ALLEGRO_BITMAP* frenteCirce3;
    ALLEGRO_BITMAP* efeitoFrenteCirce3;
    int cenario_atual;
    int total_cenarios;
} cenarioCirce;

// Funções principais
bool carregar_cenarios_circe(cenarioCirce* cenario);
void desenhar_cenario_circe(cenarioCirce* cenario, int largura_tela, int altura_tela);
void desenhar_sobreposicoes_circe(cenarioCirce* cenario, int largura_tela, int altura_tela);
void atualizar_transicao_cenario_circe(cenarioCirce* cenario, float* pos_x_personagem,
    float largura_personagem, int largura_tela);
void destruir_cenarios_circe(cenarioCirce* cenario);

#endif