#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "fase_Polifemo.h"

bool carregar_cenarios_polifemo(CenarioPolifemo* cenario) {
    // Carregar cenários base
    cenario->fundos[0] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo1.png");
    cenario->fundos[1] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo2.png");
    cenario->fundos[2] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo3.png");
    cenario->fundos[3] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo4.png");
    cenario->fundos[4] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5.png");
    cenario->fundos[5] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/fundo_Polifemo6.png");

    // Carregar sobreposições
    cenario->fundo4_arvore = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo4arvore.png");
    cenario->fundo5_pedra = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5pedra.png");
    cenario->fundo5_escuro = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5escuro.png");
    cenario->fundo6_rocha = al_load_bitmap("./imagensJogo/cenarios/Polifemo/fundo_Polifemo6rocha.png");

    // Verificar se todos carregaram
    for (int i = 0; i < 6; i++) {
        if (!cenario->fundos[i]) {
            fprintf(stderr, "Erro ao carregar fundo %d de Polifemo.\n", i + 1);
            return false;
        }
    }

    if (!cenario->fundo4_arvore || !cenario->fundo5_pedra ||
        !cenario->fundo5_escuro || !cenario->fundo6_rocha) {
        fprintf(stderr, "Erro ao carregar sobreposições de Polifemo.\n");
        return false;
    }

    // Inicializar controle de cenário
    cenario->cenario_atual = 0;
    cenario->total_cenarios = 6;

    return true;
}

void desenhar_cenario_polifemo(CenarioPolifemo* cenario, int largura_tela, int altura_tela) {
    // Desenha o fundo do cenário atual
    ALLEGRO_BITMAP* fundo_atual = cenario->fundos[cenario->cenario_atual];

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
    // Desenha sobreposições dependendo do cenário
    switch (cenario->cenario_atual) {
    case 3: // Cenário 4 (índice 3)
        al_draw_scaled_bitmap(
            cenario->fundo4_arvore,
            0, 0,
            al_get_bitmap_width(cenario->fundo4_arvore),
            al_get_bitmap_height(cenario->fundo4_arvore),
            0, 0,
            largura_tela, altura_tela,
            0
        );
        break;
    case 4: // Cenário 5 (índice 4)
        al_draw_scaled_bitmap(
            cenario->fundo5_pedra,
            0, 0,
            al_get_bitmap_width(cenario->fundo5_pedra),
            al_get_bitmap_height(cenario->fundo5_pedra),
            0, 0,
            largura_tela, altura_tela,
            0
        );
        break;
    case 5: // Cenário 6 (índice 5)
        al_draw_scaled_bitmap(
            cenario->fundo6_rocha,
            0, 0,
            al_get_bitmap_width(cenario->fundo6_rocha),
            al_get_bitmap_height(cenario->fundo6_rocha),
            0, 0,
            largura_tela, altura_tela,
            0
        );
        break;
    }
}

void atualizar_transicao_cenario(CenarioPolifemo* cenario, float* pos_x_personagem,
    float largura_personagem, int largura_tela) {
    int direcao_cenario = 0; // 0=nenhum, 1=esquerda->direita, 2=direita->esquerda

    if (*pos_x_personagem >= largura_tela)
        direcao_cenario = 1;
    else if (*pos_x_personagem + largura_personagem <= 0)
        direcao_cenario = 2;

    switch (direcao_cenario) {
    case 1: // esquerda para direita
        if (cenario->cenario_atual < cenario->total_cenarios - 1) {
            cenario->cenario_atual++;
            *pos_x_personagem = -largura_personagem + 10;
        }
        else {
            *pos_x_personagem = largura_tela - largura_personagem;
        }
        break;
    case 2: // direita para esquerda
        if (cenario->cenario_atual > 0) {
            cenario->cenario_atual--;
            *pos_x_personagem = largura_tela - 10;
        }
        else {
            *pos_x_personagem = 0;
        }
        break;
    }

    // Ajustes finos para limites do primeiro e último cenário
    switch (cenario->cenario_atual) {
    case 0:
        if (*pos_x_personagem <= -70)
            *pos_x_personagem = -70;
        break;
    case 5: // Último cenário (índice 5)
        if ((*pos_x_personagem + largura_personagem) > largura_tela + 70)
            *pos_x_personagem = largura_tela - largura_personagem + 70;
        break;
    }
}

void destruir_cenarios_polifemo(CenarioPolifemo* cenario) {
    // Destruir fundos
    for (int i = 0; i < cenario->total_cenarios; i++) {
        if (cenario->fundos[i]) 
        {
            al_destroy_bitmap(cenario->fundos[i]);
        }
    }

    // Destruir sobreposições
    if (cenario->fundo4_arvore) al_destroy_bitmap(cenario->fundo4_arvore);
    if (cenario->fundo5_pedra) al_destroy_bitmap(cenario->fundo5_pedra);
    if (cenario->fundo5_escuro) al_destroy_bitmap(cenario->fundo5_escuro);
    if (cenario->fundo6_rocha) al_destroy_bitmap(cenario->fundo6_rocha);
}