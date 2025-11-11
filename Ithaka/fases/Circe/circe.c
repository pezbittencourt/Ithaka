#include "circe.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <math.h>

const float POSITION_TOLERANCE = 0.1f;


bool carregar_cenarios_circe(Fase* fase) {
    if (!fase) return false;
    // Initialize the fase with 6 scenarios
    init_fase(fase, 4);

    // Verify allocation was successful
    if (!fase->cenarios || fase->total_cenarios < 4) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[0], 1, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce1.png");
    fase->cenarios[0].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce1.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposicoes do cenário 1\n");
        return false;
    }

    printf("Inicializando cenário 2...\n");
    init_cenario(&fase->cenarios[1], 0, 0);
    fase->cenarios[1].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce2.png");
    if (!fase->cenarios[1].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 2\n");
        return false;
    }

    printf("Inicializando cenário 3...\n");
    init_cenario(&fase->cenarios[2], 2, 0);
    fase->cenarios[2].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce3.png");
    fase->cenarios[2].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce3.png");
    fase->cenarios[2].sobreposicoes[1] = al_load_bitmap("./imagensJogo/cenarios/Circe/efeitoFrenteCirce3.png");
    if (!fase->cenarios[2].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposicoes do cenário 3\n");
        return false;
    }

    printf("Inicializando cenário 4...\n");
    init_cenario(&fase->cenarios[3], 0, 0);
    fase->cenarios[3].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce4.png");
    if (!fase->cenarios[3].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 4\n");
        return false;
    }

    return true;
}

void processar_acao_circe(Personagem* odisseu, Personagem* circe, int* circe_stall, InformacoesTela tela) {
    switch (circe->estado)
    {
    case 0:
        circe->estado = 1;
        circe->sprite_ativo = CIRCE_SPRITE_CIRCE_CORVO;
        circe->frame_atual = 0;
        *circe_stall = 80;
        break;
    case 1:
        circe->estado = 2;
        circe->estado_progresso = 0;
        circe->target_x = tela.largura / 2 - (circe->largura / 2);
        circe->target_y = tela.altura / 1.8 - (circe->altura / 2);
        break;
    case 2:
        if (circe->estado_progresso >= 100) {
            circe->estado = 3;
            circe->target_x = tela.largura - (tela.largura / 3);
            circe->target_y = (750* tela.altura) / 1080;
            circe->estado_progresso = 0;
        }
        break;
    case 3:
        if (circe->estado_progresso >= 100) {
            circe->estado = 4;
            circe->sprite_ativo = CIRCE_SPRITE_CORVO_CIRCE;
            *circe_stall = 80;
        }
        break;
    case 4:
        circe->estado = 0;
        *circe_stall = 600;
        circe->sprite_ativo = CIRCE_SPRITE_PARADA;
        break;
    default:
        *circe_stall = 0;
        break;
    }
}

void atualizar_circe(Personagem* circe, Personagem odisseu, InformacoesTela tela)
{
    switch (circe->estado)
    {
    case 2:
        movimento_ataque_corvo(circe, tela);
        break;
    case 3:
        move_to_target(circe);

    default:
        break;
    }
}

void movimento_ataque_corvo(Personagem* circe, InformacoesTela tela) {
    if (circe->estado_progresso == 0 &&
        (fabs(circe->x - circe->target_x) > POSITION_TOLERANCE ||
            fabs(circe->y - circe->target_y) > POSITION_TOLERANCE)) {

        float dx = circe->target_x - circe->x;
        float dy = circe->target_y - circe->y;

        float distance = sqrtf(dx * dx + dy * dy);

        float speed = 6.0f;
        circe->x += (dx / distance) * speed;
        circe->y += (dy / distance) * speed;

        if (fabs(circe->x - circe->target_x) < speed && fabs(circe->y - circe->target_y) < speed) {
            circe->x = circe->target_x;
            circe->y = circe->target_y;
        }
        circe->sprite_ativo = CIRCE_SPRITE_CORVO_PARADO;
        return;
    }

    float center_x = (tela.largura / 2) - (circe->largura / 2);
    float center_y = (tela.altura / 1.8) - (circe->altura / 2);
    float scale_x = tela.largura * 0.4f;
    float scale_y = tela.altura * 0.7f;

    float angular_progress = circe->estado_progresso * (6.0f * ALLEGRO_PI / 100.0f);
    float t = angular_progress + (ALLEGRO_PI / 2);

    float denominator = 1 + sin(t) * sin(t);

    float offset_x = scale_x * cos(t) / denominator;
    float offset_y = scale_y * sin(t) * cos(t) / denominator;

    circe->x = center_x + offset_x;
    circe->y = center_y + offset_y;
    circe->sprite_ativo = CIRCE_SPRITE_CORVO_ATACA;
    circe->estado_progresso = circe->estado_progresso + (100.0f / (20.0f * 60.0f));
}

void move_to_target(Personagem* circe)
{
    if (circe->estado_progresso == 0 &&
        (fabs(circe->x - circe->target_x) > POSITION_TOLERANCE ||
            fabs(circe->y - circe->target_y) > POSITION_TOLERANCE)) {

        float dx = circe->target_x - circe->x;
        float dy = circe->target_y - circe->y;

        float distance = sqrtf(dx * dx + dy * dy);

        float speed = 6.0f;
        circe->x += (dx / distance) * speed;
        circe->y += (dy / distance) * speed;

        if (fabs(circe->x - circe->target_x) < speed && fabs(circe->y - circe->target_y) < speed) {
            circe->x = circe->target_x;
            circe->y = circe->target_y;
        }
    }
    else {
        circe->estado_progresso = 100;
    }
}

