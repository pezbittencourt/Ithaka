#include "circe.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <math.h>

// Tolerância para verificação de posição (evita comparações exatas com float)
const float POSITION_TOLERANCE = 0.1f;

// Limita um valor dentro dos limites da tela (com margem de 100px)
static void clamp_to_screen(float* value, InformacoesTela tela) {
    if (*value > tela.largura - 100) {
        *value = tela.largura - 100;
    }
    else if (*value < 100) {
        *value = 100;
    }
}

// Move o personagem suavemente em direção ao alvo
static void move_to_target(Personagem* circe, bool angulo, float progresso_end)
{
    // Calcula a direção para o alvo
    float dx = circe->target_x - circe->x;
    float dy = circe->target_y - circe->y;

    // Calcula a distância total usando teorema de Pitágoras
    float distance = sqrtf(dx * dx + dy * dy);
    float speed = 3.0f; // Velocidade de movimento

    // Calcula nova posição com movimento normalizado
    float newx = circe->x + (dx / distance) * speed;
    float newy = circe->y + (dy / distance) * speed;

    // Atualiza posição X se não estiver suficientemente perto do alvo
    if (fabs(circe->x - circe->target_x) > POSITION_TOLERANCE) {
        circe->x = newx;

        // Snap para posição exata se estiver muito próximo
        if (fabs(circe->x - circe->target_x) < speed) {
            circe->x = circe->target_x;
        }
    }

    // Atualiza posição Y se não estiver suficientemente perto do alvo
    if (fabs(circe->y - circe->target_y) > POSITION_TOLERANCE) {
        circe->y = newy;

        // Snap para posição exata se estiver muito próximo
        if (fabs(circe->y - circe->target_y) < speed) {
            circe->y = circe->target_y;
        }
    }

    // Atualiza ângulo de rotação se solicitado
    if (angulo) {
        circe->angulo = calc_angulo(circe->x, circe->y, newx, newy);
    }

    // Verifica se chegou ao destino e atualiza progresso
    if ((fabs(circe->x - circe->target_x) < POSITION_TOLERANCE &&
        fabs(circe->y - circe->target_y) < POSITION_TOLERANCE)) {
        circe->estado_progresso = progresso_end;
    }
}

// Controla o movimento do ataque em forma de corvo
static void movimento_ataque_corvo(Personagem* circe, InformacoesTela tela) {
    // Fase 1: Movimento linear até o ponto inicial do ataque
    if (circe->estado_progresso == 0 &&
        (fabs(circe->x - circe->target_x) > POSITION_TOLERANCE ||
            fabs(circe->y - circe->target_y) > POSITION_TOLERANCE)) {

        // Calcula direção e distância
        float dx = circe->target_x - circe->x;
        float dy = circe->target_y - circe->y;
        float distance = sqrtf(dx * dx + dy * dy);

        // Move em direção ao alvo
        float speed = 3.0f;
        float newx = circe->x + (dx / distance) * speed;
        float newy = circe->y + (dy / distance) * speed;

        circe->x = newx;
        circe->y = newy;

        // Snap para posição exata se estiver muito próximo
        if (fabs(circe->x - circe->target_x) < speed && fabs(circe->y - circe->target_y) < speed) {
            circe->x = circe->target_x;
            circe->y = circe->target_y;
        }

        // Define sprite parado durante aproximação
        circe->sprite_ativo = CIRCE_SPRITE_CORVO_PARADO;
        return;
    }

    // Fase 2: Movimento em padrão especial (lemniscata/figura-8)
    // Calcula centro da trajetória
    float center_x = (tela.largura / 2) - (circe->largura / 2);
    float center_y = (tela.altura / 1.8) - (circe->altura / 2);

    // Escalas para o padrão de movimento
    float scale_x = tela.largura * 0.4f;
    float scale_y = tela.altura * 0.7f;

    // Progresso angular baseado no progresso do estado
    float angular_progress = circe->estado_progresso * (3.0f * ALLEGRO_PI / 100.0f);
    float t = angular_progress + (ALLEGRO_PI / 2); // Deslocamento de fase

    // Fórmula para padrão lemniscata (figura-8)
    float denominator = 1 + sin(t) * sin(t);
    float offset_x = scale_x * cos(t) / denominator;
    float offset_y = scale_y * sin(t) * cos(t) / denominator;

    // Calcula nova posição
    float newx = center_x + offset_x;
    float newy = center_y + offset_y;

    // Atualiza ângulo baseado na direção do movimento
    circe->angulo = calc_angulo(circe->x, circe->y, newx, newy);

    // Aplica nova posição
    circe->x = newx;
    circe->y = newy;

    // Define sprite de ataque e incrementa progresso
    circe->sprite_ativo = CIRCE_SPRITE_CORVO_ATACA;
    circe->estado_progresso = circe->estado_progresso + (100.0f / (5.0f * 60.0f)); // 5 segundos para completar
}

// Controla o movimento do ataque em forma de tigre
static void movimento_ataque_tigre(Personagem* circe, Personagem odisseu, InformacoesTela tela, int* circe_stall) {
    // Divide o ataque em 3 repetições com 3 etapas cada
    int repeticao = (int)((circe->estado_progresso + 0.001f) / 33.3f) + 1;
    float progresso_na_repeticao = circe->estado_progresso - ((repeticao - 1) * 33.3f);
    int etapa = (int)((progresso_na_repeticao + 0.001f) / 11.1f) + 1;

    switch (etapa) {
    case 1: { // Etapa 1: Aproximação do alvo
        // Calcula posição alvo (ao lado do Odisseu)
        float target_x = odisseu.x + (odisseu.x < circe->x ? circe->largura : -circe->largura);
        float target_y = circe->y;

        // Define direção do sprite
        circe->olhando_direita = odisseu.x > circe->x;
        circe->sprite_ativo = CIRCE_SPRITE_TIGRE_ANDANDO;

        // Move em direção ao alvo
        circe->target_x = target_x;
        circe->target_y = target_y;
        move_to_target(circe, false, ((repeticao - 1) * 33.3f) + 11.1f);
        break;
    }
    case 2: // Etapa 2: Execução do ataque
        circe->atacando = true;
        circe->sprite_ativo = CIRCE_SPRITE_TIGRE_ATAQUE;
        circe->frame_atual = 0; // Reinicia animação
        circe->estado_progresso = ((repeticao - 1) * 33.3f) + 22.2f;
        *circe_stall = 30; // Pausa para animação do ataque
        break;
    case 3: { // Etapa 3: Recuo após ataque

        // Aguarda término da animação do ataque
        if (circe->atacando) {
            return;
        }

        // Calcula posição de recuo (mais distante)
        float target_x = odisseu.x + (odisseu.x < circe->x ? circe->largura * 3.0f : -circe->largura * 3.0f);
        clamp_to_screen(&target_x, tela); // Garante que não saia da tela
        float target_y = circe->y;

        // Move para posição de recuo
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

// Calcula ângulo entre dois pontos
static float calc_angulo(float x, float y, float x1, float y1) {
    float distx = x1 - x;
    float disty = y1 - y;
    float angulo = atan2f(-disty, distx); // Inverte Y porque o Allegro usa coordenadas invertidas

    return -angulo; // Retorna invertido devido ao sistema de coordenadas do Allegro
}

// Carrega os cenários da fase da Círce
bool carregar_cenarios_circe(Fase* fase) {
    if (!fase) return false;
    init_fase(fase, 4); // Inicializa fase com 4 cenários

    // Verifica se a inicialização foi bem sucedida
    if (!fase->cenarios || fase->total_cenarios < 4) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    // Cenário 1: Com sobreposição frontal
    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[0], 1, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce1.png");
    fase->cenarios[0].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce1.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "Erro ao carregar fundo ou sobreposicoes do cenário 1\n");
        return false;
    }

    // Cenário 2: Apenas fundo
    printf("Inicializando cenário 2...\n");
    init_cenario(&fase->cenarios[1], 0, 0);
    fase->cenarios[1].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce2.png");
    if (!fase->cenarios[1].fundo) {
        fprintf(stderr, "Erro ao carregar fundo do cenário 2\n");
        return false;
    }

    // Cenário 3: Com duas sobreposições (frente e efeito)
    printf("Inicializando cenário 3...\n");
    init_cenario(&fase->cenarios[2], 2, 0);
    fase->cenarios[2].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce3.png");
    fase->cenarios[2].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce3.png");
    fase->cenarios[2].sobreposicoes[1] = al_load_bitmap("./imagensJogo/cenarios/Circe/efeitoFrenteCirce3.png");
    if (!fase->cenarios[2].fundo) {
        fprintf(stderr, "Erro ao carregar fundo ou sobreposicoes do cenário 3\n");
        return false;
    }

    // Cenário 4: Apenas fundo
    printf("Inicializando cenário 4...\n");
    init_cenario(&fase->cenarios[3], 0, 0);
    fase->cenarios[3].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce4.png");
    if (!fase->cenarios[3].fundo) {
        fprintf(stderr, "Erro ao carregar fundo do cenário 4\n");
        return false;
    }

    return true;
}

// Processa a máquina de estados da Círce
void processar_acao_circe(Personagem* odisseu, Personagem* circe, int* circe_stall, InformacoesTela tela) {
    switch (circe->estado)
    {
    case 0: // Estado inicial: escolhe próximo ataque
        circe->angulo = 0;
        circe->vulneravel = false;

        // SEMPRE escolhe estado 4 (tigre) - código comentado mostra que era aleatório
        circe->estado = (rand() % 2) + 1 == 0 ? 4 : 4;
        circe->estado = (rand() % 2) + 1 == 0 ? 4 : 4;

        // Define sprite baseado no estado escolhido
        circe->sprite_ativo = circe->estado == 1 ? CIRCE_SPRITE_CIRCE_CORVO : CIRCE_SPRITE_CIRCE_TIGRE;
        circe->frame_atual = 0;
        circe->estado_progresso = 0;
        *circe_stall = 80; // Tempo de espera antes do próximo estado
        break;

    case 1: // Transição para ataque corvo
        circe->estado = 2;
        circe->estado_progresso = 0;
        // Define posição alvo no centro da tela
        circe->target_x = tela.largura / 2 - (circe->largura / 2);
        circe->target_y = tela.altura / 1.8 - (circe->altura / 2);
        break;

    case 2: // Ataque corvo em andamento
        if (circe->estado_progresso >= 100) {
            circe->estado = 3;
            // Define nova posição alvo
            circe->target_x = tela.largura - (tela.largura / 3);
            circe->target_y = (750 * tela.altura) / 1080;
            circe->estado_progresso = 0;
        }
        break;

    case 3: // Movimento após ataque corvo
        if (circe->estado_progresso >= 100) {
            circe->estado = 7;
            circe->angulo = 0;
            circe->frame_atual = 0;
            circe->sprite_ativo = CIRCE_SPRITE_CORVO_CIRCE; // Transformação de volta
            *circe_stall = 70; // Tempo de espera para transformação
        }
        break;

    case 4: // Ataque tigre em andamento
        if (circe->estado_progresso >= 99.8f) {
            circe->estado = 5;
            // Define posição alvo
            circe->target_x = tela.largura - (tela.largura / 3);
            circe->target_y = (750 * tela.altura) / 1080;
            circe->estado_progresso = 0;
        }
        break;

    case 5: // Movimento após ataque tigre
        if (circe->estado_progresso >= 100) {
            circe->estado = 7;
            circe->frame_atual = 0;
            circe->sprite_ativo = CIRCE_SPRITE_TIGRE_CIRCE; // Transformação de volta
            *circe_stall = 70; // Tempo de espera para transformação
        }
        break;

    case 7: // Estado final: volta à forma normal
        circe->vulneravel = true; // Torna-se vulnerável a ataques
        circe->estado = 0; // Reinicia ciclo
        *circe_stall = 300; // Longo tempo de espera antes do próximo ataque
        circe->frame_atual = 0;
        circe->sprite_ativo = CIRCE_SPRITE_BATALHA; // Sprite normal de batalha
        break;

    default:
        *circe_stall = 0;
        break;
    }
}

// Atualiza a Círce baseado no estado atual
void atualizar_circe(Personagem* circe, Personagem odisseu, InformacoesTela tela, int* circe_stall)
{
    // Verifica se está em stall (espera)
    if (*circe_stall != 0) {
        return;
    }

    // Executa ação baseada no estado atual
    switch (circe->estado)
    {
    case 2: // Ataque corvo
        movimento_ataque_corvo(circe, tela);
        break;
    case 4: // Ataque tigre
        movimento_ataque_tigre(circe, odisseu, tela, circe_stall);
        break;
    case 5: // Movimento pós-ataque tigre
        move_to_target(circe, false, 100.f);
        break;
    case 3: // Movimento pós-ataque corvo
        move_to_target(circe, true, 100.0f);
        break;
    default:
        break;
    }
}