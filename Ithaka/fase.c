#include "fase.h"
#include <stdio.h>
#include "mapa/mapa.h"
#include "fases/Polifemus/fase_Polifemo.h"
#include "fases/Circe/circe.h"
#include "fases/Itaca/Itaca.h"
#include "fases/Poseidon/poseidon.h"
#include "fases/Olimpo/olimpo.h"
#include "fases/Submundo/submundo.h"

void init_fase(Fase* fase, int total_cenarios)
{
    if (!fase) return;

    // Allocate array of Cenario structs
    fase->cenarios = (Cenario*)malloc(total_cenarios * sizeof(Cenario));
    if (!fase->cenarios) {
        fprintf(stderr, "Erro: não foi possível alocar memória para cenários\n");
        fase->total_cenarios = 0;
        return;
    }

    // Initialize all Cenario structs to zero
    for (int i = 0; i < total_cenarios; i++) {
        fase->cenarios[i].fundo = NULL;
        fase->cenarios[i].sobreposicoes = NULL;
        fase->cenarios[i].personagens = NULL;
        fase->cenarios[i].total_sobreposicoes = 0;
        fase->cenarios[i].total_personagens = 0;
    }

    fase->total_cenarios = total_cenarios;
    fase->cenario_atual = 0;
}

void init_cenario(Cenario* cenario, int total_sobreposicoes, int total_personagens)
{
    if (!cenario) {
        fprintf(stderr, "Erro: cenario é NULL\n");
        return;
    }

    // Initialize overlays array
    if (total_sobreposicoes > 0) {
        cenario->sobreposicoes = (ALLEGRO_BITMAP**)malloc(total_sobreposicoes * sizeof(ALLEGRO_BITMAP*));
        if (cenario->sobreposicoes) {
            cenario->total_sobreposicoes = total_sobreposicoes;
            // Initialize all overlay pointers to NULL
            for (int i = 0; i < total_sobreposicoes; i++) {
                cenario->sobreposicoes[i] = NULL;
            }
        }
        else {
            fprintf(stderr, "Erro: não foi possível alocar memória para sobreposições\n");
            cenario->total_sobreposicoes = 0;
        }
    }

    // Initialize characters array
    if (total_personagens > 0) {
        cenario->personagens = (Personagem*)malloc(total_personagens * sizeof(Personagem));
        if (cenario->personagens) {
            cenario->total_personagens = total_personagens;
        }
        else {
            fprintf(stderr, "Erro: não foi possível alocar memória para personagens\n");
            cenario->total_personagens = 0;
        }
    }
}

void free_cenario(Cenario* cenario)
{
    if (!cenario) return;

    if (cenario->sobreposicoes) {
        free(cenario->sobreposicoes);
        cenario->sobreposicoes = NULL;
    }
    if (cenario->personagens) {
        free(cenario->personagens);
        cenario->personagens = NULL;
    }
    cenario->total_sobreposicoes = 0;
    cenario->total_personagens = 0;
}

void free_fase(Fase* fase)
{
    if (!fase) return;

    if (fase->cenarios) {
        // First free all individual cenarios
        for (int i = 0; i < fase->total_cenarios; i++) {
            free_cenario(&fase->cenarios[i]);
        }
        // Then free the cenarios array
        free(fase->cenarios);
        fase->cenarios = NULL;
    }
    fase->total_cenarios = 0;
    fase->cenario_atual = 0;
}

bool carregar_cenario(Fase* fase, int escolha_fase) {
    switch (escolha_fase)
    {
    case MAPA_FASE_POLIFEMO:
        carregar_cenarios_polifemo(fase);
        break;
    case MAPA_FASE_CIRCE:
        carregar_cenarios_circe(fase);
        break;
    case MAPA_FASE_ITACA:
        carregar_cenarios_itaca(fase);
        break;
    case MAPA_FASE_OLIMPO:
        carregar_cenarios_olimpo(fase);
        break;
    case MAPA_FASE_POSEIDON:
        carregar_cenarios_poseidon(fase);
        break;
    case MAPA_FASE_SUBMUNDO:
        carregar_cenarios_submundo(fase);
        break;
    default:
        return false;
    }
}

void desenhar_cenario(Fase* fase, int largura_tela, int altura_tela) {
    Cenario* cenario = &fase->cenarios[fase->cenario_atual];
    ALLEGRO_BITMAP* fundo_atual = cenario->fundo;
    if (!fundo_atual) return;

    for (int i = 0; i < cenario->total_sobreposicoes; i++) {
        ALLEGRO_BITMAP* sobreposicao_atual = cenario->sobreposicoes[i];
        al_draw_scaled_bitmap(
            sobreposicao_atual,
            0, 0,
            al_get_bitmap_width(sobreposicao_atual),
            al_get_bitmap_height(sobreposicao_atual),
            0, 0,
            largura_tela, altura_tela,
            0
        );
    }

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

void desenhar_sobreposicoes(Fase* fase, int largura_tela, int altura_tela) {
    Cenario* cenario = &fase->cenarios[fase->cenario_atual];
    ALLEGRO_BITMAP* fundo_atual = cenario->fundo;
    if (!fundo_atual) return;

    for (int i = 0; i < cenario->total_sobreposicoes; i++) {
        ALLEGRO_BITMAP* sobreposicao_atual = cenario->sobreposicoes[i];
        al_draw_scaled_bitmap(
            sobreposicao_atual,
            0, 0,
            al_get_bitmap_width(sobreposicao_atual),
            al_get_bitmap_height(sobreposicao_atual),
            0, 0,
            largura_tela, altura_tela,
            0
        );
    }
}

void atualizar_transicao_cenario(Fase* fase, float* pos_x_personagem,
    float largura_personagem, int largura_tela) {
    int direcao_cenario = 0;

    if (*pos_x_personagem >= largura_tela)
        direcao_cenario = 1;
    else if (*pos_x_personagem + largura_personagem <= 0)
        direcao_cenario = 2;

    if (direcao_cenario == 1) {
        if (fase->cenario_atual < fase->total_cenarios - 1) {
            fase->cenario_atual++;
            *pos_x_personagem = -largura_personagem + 10;
        }
        else {
            *pos_x_personagem = largura_tela - largura_personagem;
        }
    }
    else if (direcao_cenario == 2) {
        if (fase->cenario_atual > 0) {
            fase->cenario_atual--;
            *pos_x_personagem = largura_tela - 10;
        }
        else {
            *pos_x_personagem = 0;
        }
    }
}

void destruir_cenarios(Fase* fase) {
    for (int i = 0; i < fase->total_cenarios; i++) {
        Cenario* cenario = &fase->cenarios[i];
        if (cenario->fundo) {
            al_destroy_bitmap(cenario->fundo);
            cenario->fundo = NULL;
        }
        for (int j = 0; j < cenario->total_sobreposicoes; j++) {
            if (cenario->sobreposicoes[j]) {
                al_destroy_bitmap(cenario->sobreposicoes[j]);
                cenario->sobreposicoes[j] = NULL;
            }
        }
        free_cenario(cenario);
    }

    free_fase(fase);
}