#include "circe.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <math.h>

const float POSITION_TOLERANCE = 0.1f;

static void clamp_to_screen(float* value, InformacoesTela tela) {
    if (*value > tela.largura - 100) {
        *value = tela.largura - 100;
    }
    else if (*value < 100) {
        *value = 100;
    }
}

static void move_to_target(Personagem* circe, bool angulo, float progresso_end)
{
    float dx = circe->target_x - circe->x;
    float dy = circe->target_y - circe->y;

    float distance = sqrtf(dx * dx + dy * dy);
    float speed = 6.0f;
    float newx = circe->x + (dx / distance) * speed;
    float newy = circe->y + (dy / distance) * speed;

    if (fabs(circe->x - circe->target_x) > POSITION_TOLERANCE) {
        circe->x = newx;

        if (fabs(circe->x - circe->target_x) < speed) {
            circe->x = circe->target_x;
        }
    }
    if (fabs(circe->y - circe->target_y) > POSITION_TOLERANCE) {
        circe->y = newy;

        if (fabs(circe->y - circe->target_y) < speed) {
            circe->y = circe->target_y;
        }
    }
    if (angulo) {
        circe->angulo = calc_angulo(circe->x, circe->y, newx, newy);
    }
    if ((fabs(circe->x - circe->target_x) < POSITION_TOLERANCE &&
        fabs(circe->y - circe->target_y) < POSITION_TOLERANCE)) {
        circe->estado_progresso = progresso_end;
    }
}


static void movimento_ataque_corvo(Personagem* circe, InformacoesTela tela) {
    if (circe->estado_progresso == 0 &&
        (fabs(circe->x - circe->target_x) > POSITION_TOLERANCE ||
            fabs(circe->y - circe->target_y) > POSITION_TOLERANCE)) {

        float dx = circe->target_x - circe->x;
        float dy = circe->target_y - circe->y;

        float distance = sqrtf(dx * dx + dy * dy);

        float speed = 6.0f;
        float newx = circe->x + (dx / distance) * speed;
        float newy = circe->y + (dy / distance) * speed;

        circe->x = newx;
        circe->y = newy;

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

    float newx = center_x + offset_x;
    float newy = center_y + offset_y;

    circe->angulo = calc_angulo(circe->x, circe->y, newx, newy);

    circe->x = newx;
    circe->y = newy;

    circe->sprite_ativo = CIRCE_SPRITE_CORVO_ATACA;
    circe->estado_progresso = circe->estado_progresso + (100.0f / (5.0f * 60.0f));
}

static void movimento_ataque_tigre(Personagem* circe, Personagem odisseu, InformacoesTela tela, int* circe_stall) {
    int repeticao = (int)((circe->estado_progresso + 0.001f) / 33.3f) + 1;
    float progresso_na_repeticao = circe->estado_progresso - ((repeticao - 1) * 33.3f);
    int etapa = (int)((progresso_na_repeticao + 0.001f) / 11.1f) + 1;
    switch (etapa) {
    case 1: {
        float target_x = odisseu.x + (odisseu.x < circe->x ? circe->largura : -circe->largura);
        float target_y = circe->y;
        circe->olhando_direita = odisseu.x > circe->x;
        circe->sprite_ativo = CIRCE_SPRITE_TIGRE_ANDANDO;
        circe->target_x = target_x;
        circe->target_y = target_y;
        move_to_target(circe, false, ((repeticao - 1) * 33.3f) + 11.1f);
        break;
    }
    case 2:
        circe->atacando = true;
        circe->sprite_ativo = CIRCE_SPRITE_TIGRE_ATAQUE;
        circe->frame_atual = 0;
        circe->estado_progresso = ((repeticao - 1) * 33.3f) + 22.2f;
        *circe_stall = 30;
        break;
    case 3: {

        //deixa a animação do ataque terminar antes de recuar
        if (circe->atacando) {
            return;
        }

        float target_x = odisseu.x + (odisseu.x < circe->x ? circe->largura * 3.0f : -circe->largura * 3.0f);
        clamp_to_screen(&target_x, tela);
        float target_y = circe->y;
        circe->target_x = target_x;
        circe->target_y = target_y;
        circe->olhando_direita = odisseu.x > circe->x;
        circe->sprite_ativo = CIRCE_SPRITE_TIGRE_ANDANDO;
        move_to_target(circe, false, ((repeticao - 1) * 33.3f) + 33.3f);
        break;
    }
    default:
        printf("etapa fora de indice: %d\n", etapa);
        break;
    }
}

static float calc_angulo(float x, float y, float x1, float y1) {
    float distx = x1 - x;
    float disty = y1 - y;
    float angulo = atan2f(-disty, distx);

    //retorna invertido pq o allegro eh estranho cm angulo
    return -angulo;
}

bool carregar_cenarios_circe(Fase* fase) {
    if (!fase) return false;
    init_fase(fase, 4);
 
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
        circe->angulo = 0;
        circe->vulneravel = false;
        circe->estado = (rand() % 2) + 1 == 0  ? 4 : 4;
        circe->sprite_ativo = circe->estado == 1 ? CIRCE_SPRITE_CIRCE_CORVO : CIRCE_SPRITE_CIRCE_TIGRE;
        circe->frame_atual = 0;
        circe->estado_progresso = 0;
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
            circe->estado = 7;
            circe->angulo = 0;
            circe->frame_atual = 0;
            circe->sprite_ativo = CIRCE_SPRITE_CORVO_CIRCE;
            *circe_stall = 70;
        }
        break;
    case 4:
        if (circe->estado_progresso >= 99.8f) {
            circe->estado = 5;
            circe->target_x = tela.largura - (tela.largura / 3);
            circe->target_y = (750 * tela.altura) / 1080;
            circe->estado_progresso = 0;
        }
        break;
    case 5:
        if (circe->estado_progresso >= 100) {
            circe->estado = 7;
            circe->frame_atual = 0;
            circe->sprite_ativo = CIRCE_SPRITE_TIGRE_CIRCE;
            *circe_stall = 70;
        }
        break;
    case 7:
        circe->vulneravel = true;
        circe->estado = 0;
        *circe_stall = 300;
        circe->frame_atual = 0;
        circe->sprite_ativo = CIRCE_SPRITE_BATALHA;
        break;
    default:
        *circe_stall = 0;
        break;
    }
}

void atualizar_circe(Personagem* circe, Personagem odisseu, InformacoesTela tela,int *circe_stall)
{
    if (*circe_stall != 0) {
        return;
    }
    switch (circe->estado)
    {
    case 2:
        movimento_ataque_corvo(circe, tela);
        break;
    case 4:
        movimento_ataque_tigre(circe, odisseu, tela,circe_stall);
        break;
    case 5:
        move_to_target(circe, false,100.f);
        break;
    case 3:
        move_to_target(circe,true,100.0f);
        break;
    default:
        break;
    }
}