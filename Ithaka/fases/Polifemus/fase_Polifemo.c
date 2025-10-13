#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "fase_Polifemo.h"

bool carregar_cenarios_polifemo(CenarioPolifemo* cenario) {
    if (!cenario) return false;

    // === Carregamento dos fundos ===
    cenario->fundos[0] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo1.png");
    cenario->fundos[1] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo2.png");
    cenario->fundos[2] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo3.png");
    cenario->fundos[3] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo4.png");
    cenario->fundos[4] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5.png");
    cenario->fundos[5] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/fundo_Polifemo6.png");
    cenario->fundos[6] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce1.png");
    cenario->fundos[7] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce2.png");
    cenario->fundos[8] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce3.png");
    cenario->fundos[9] = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce4.png");
    cenario->fundos[10] = al_load_bitmap("./imagensJogo/cenarios/Submundo/submundoProfeta.png");
    cenario->fundos[11] = al_load_bitmap("./imagensJogo/cenarios/Poseidon/fundoPoseidon.png");
    cenario->fundos[12] = al_load_bitmap("./imagensJogo/cenarios/Olimpo/fundoOlimpo.png");

    // === Carregamento das sobreposições ===
    cenario->fundo4_arvore = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo4arvore.png");
    cenario->fundo5_pedra = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5pedra.png");
    cenario->fundo5_escuro = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5escuro.png");
    cenario->fundo6_rocha = al_load_bitmap("./imagensJogo/cenarios/Polifemo/fundo_Polifemo6rocha.png");
    cenario->frenteCirce1 = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce1.png");
    cenario->frenteCirce3 = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce3.png");
    cenario->efeitoFrenteCirce3 = al_load_bitmap("./imagensJogo/cenarios/Circe/efeitoFrenteCirce3.png");

    // === Verificação de carregamento ===
    for (int i = 0; i < 13; i++) {
        if (!cenario->fundos[i]) {
            fprintf(stderr, "❌ Erro ao carregar fundo %d do cenário.\n", i + 1);
            return false;
        }
    }

    if (!cenario->fundo4_arvore || !cenario->fundo5_pedra || !cenario->fundo5_escuro ||
        !cenario->fundo6_rocha || !cenario->frenteCirce1 ||
        !cenario->frenteCirce3 || !cenario->efeitoFrenteCirce3) {
        fprintf(stderr, "❌ Erro ao carregar uma das sobreposições dos cenários.\n");
        return false;
    }

    // Inicialização
    cenario->cenario_atual = 0;
    cenario->total_cenarios = 12;

    return true;
}

void desenhar_cenario_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela) {
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

void desenhar_sobreposicoes_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela) {
    switch (cenario->cenario_atual) {
    case 3: // Fundo 4
        al_draw_scaled_bitmap(cenario->fundo4_arvore, 0, 0,
            al_get_bitmap_width(cenario->fundo4_arvore),
            al_get_bitmap_height(cenario->fundo4_arvore),
            0, 0, largura_tela, altura_tela, 0);
        break;
    case 4: // Fundo 5
        al_draw_scaled_bitmap(cenario->fundo5_pedra, 0, 0,
            al_get_bitmap_width(cenario->fundo5_pedra),
            al_get_bitmap_height(cenario->fundo5_pedra),
            0, 0, largura_tela, altura_tela, 0);
        break;
    case 5: // Fundo 6
        al_draw_scaled_bitmap(cenario->fundo6_rocha, 0, 0,
            al_get_bitmap_width(cenario->fundo6_rocha),
            al_get_bitmap_height(cenario->fundo6_rocha),
            0, 0, largura_tela, altura_tela, 0);
        break;
    case 8: // Fundo Circe3 – aplicar efeito e frente
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

void atualizar_transicao_cenario(CenarioPolifemo* cenario, float* pos_x_personagem,
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

void destruir_cenarios_polifemo(CenarioPolifemo* cenario) {
    for (int i = 0; i < 10; i++) {
        if (cenario->fundos[i]) {
            al_destroy_bitmap(cenario->fundos[i]);
            cenario->fundos[i] = NULL;
        }
    }

    if (cenario->fundo4_arvore) { al_destroy_bitmap(cenario->fundo4_arvore); cenario->fundo4_arvore = NULL; }
    if (cenario->fundo5_pedra) { al_destroy_bitmap(cenario->fundo5_pedra); cenario->fundo5_pedra = NULL; }
    if (cenario->fundo5_escuro) { al_destroy_bitmap(cenario->fundo5_escuro); cenario->fundo5_escuro = NULL; }
    if (cenario->fundo6_rocha) { al_destroy_bitmap(cenario->fundo6_rocha); cenario->fundo6_rocha = NULL; }
    if (cenario->frenteCirce1) { al_destroy_bitmap(cenario->frenteCirce1); cenario->frenteCirce1 = NULL; }
    if (cenario->frenteCirce3) { al_destroy_bitmap(cenario->frenteCirce3); cenario->frenteCirce3 = NULL; }
    if (cenario->efeitoFrenteCirce3) { al_destroy_bitmap(cenario->efeitoFrenteCirce3); cenario->efeitoFrenteCirce3 = NULL; }
}
