#include "circe.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>

bool carregar_cenarios_circe(cenarioCirce* cenario) {
    if (!cenario) return false;

    // === Carregamento dos fundos ===
    cenario->fundos[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce1.png");
    cenario->fundos[1] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce2.png");
    cenario->fundos[2] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce3.png");
    cenario->fundos[3] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce4.png");

	// === Carregamento das sobreposições ===
	cenario->frenteCirce1 = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce1.png");
	cenario->frenteCirce3 = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce3.png");
	cenario->efeitoFrenteCirce3 = al_load_bitmap("./imagensJogo/cenarios/Circe/efeitoFrenteCirce3.png");

    // === Verificação de carregamento ===
    for (int i = 0; i < 4; i++) {
        if (!cenario->fundos[i]) {
            printf(" Erro ao carregar fundo %d do cenário.\n", i + 1);
            return false;
        }
    }

    if (!cenario->frenteCirce1 ||!cenario->frenteCirce3 || !cenario->efeitoFrenteCirce3) {
        printf(" Erro ao carregar uma das sobreposições dos cenários.\n");
        return false;
    }

    // Inicialização
    cenario->cenario_atual = 0;
    cenario->total_cenarios = 12;

    return true;
}

void desenhar_cenario_circe(cenarioCirce* cenario, int largura_tela, int altura_tela) {
    ALLEGRO_BITMAP* fundo_atual = cenario->fundos[cenario->cenario_atual];
    if (!fundo_atual) return;

    al_draw_scaled_bitmap(
        fundo_atual,
        0, 0,
        al_get_bitmap_width(fundo_atual),
        al_get_bitmap_height(fundo_atual),
        0, 0,
        largura_tela, altura_tela,
        0
    );
}

void desenhar_sobreposicoes_circe(cenarioCirce* cenario, int largura_tela, int altura_tela) {
    switch (cenario->cenario_atual) {
    case 1: // Fundo 1
        al_draw_scaled_bitmap(cenario->frenteCirce1, 0, 0,
            al_get_bitmap_width(cenario->frenteCirce1),
            al_get_bitmap_height(cenario->frenteCirce1),
            0, 0, largura_tela, altura_tela, 0);
        al_draw_scaled_bitmap(cenario->frenteCirce1, 0, 0,
            al_get_bitmap_width(cenario->frenteCirce1),
            al_get_bitmap_height(cenario->frenteCirce1),
            0, 0, largura_tela, altura_tela, 0);
        break;


    case 3: // Fundo Circe3 – aplicar efeito e frente
        al_draw_scaled_bitmap(cenario->efeitoFrenteCirce3, 0, 0,
            al_get_bitmap_width(cenario->efeitoFrenteCirce3),
            al_get_bitmap_height(cenario->efeitoFrenteCirce3),
            0, 0, largura_tela, altura_tela, 0);
        al_draw_scaled_bitmap(cenario->frenteCirce3, 0, 0,
            al_get_bitmap_width(cenario->frenteCirce3),
            al_get_bitmap_height(cenario->frenteCirce3),
            0, 0, largura_tela, altura_tela, 0);
        break;
    }
}

void atualizar_transicao_cenario_circe(cenarioCirce* cenario, float* pos_x_personagem,
    float largura_personagem, int largura_tela) {
    int direcao_cenario = 0;

    if (*pos_x_personagem >= largura_tela)
        direcao_cenario = 1;
    else if (*pos_x_personagem + largura_personagem <= 0)
        direcao_cenario = 2;

    if (direcao_cenario == 1) {
        if (cenario->cenario_atual < cenario->total_cenarios - 1) {
            cenario->cenario_atual++;
            *pos_x_personagem = -largura_personagem + 10;
        }
        else {
            *pos_x_personagem = largura_tela - largura_personagem;
        }
    }
    else if (direcao_cenario == 2) {
        if (cenario->cenario_atual > 0) {
            cenario->cenario_atual--;
            *pos_x_personagem = largura_tela - 10;
        }
        else {
            *pos_x_personagem = 0;
        }
    }
}

void destruir_cenarios_circe(cenarioCirce* cenario) {
    for (int i = 0; i < 4; i++) {
        if (cenario->fundos[i]) {
            al_destroy_bitmap(cenario->fundos[i]);
            cenario->fundos[i] = NULL;
        }

        if (cenario->frenteCirce1) { al_destroy_bitmap(cenario->frenteCirce1); cenario->frenteCirce1 = NULL; }
        if (cenario->frenteCirce3) { al_destroy_bitmap(cenario->frenteCirce3); cenario->frenteCirce3 = NULL; }
        if (cenario->efeitoFrenteCirce3) { al_destroy_bitmap(cenario->efeitoFrenteCirce3); cenario->efeitoFrenteCirce3 = NULL; }
    }
}
