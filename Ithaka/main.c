#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include "menu/menu_inicial.h"
#include "mapa/mapa.h"
#include "informacoes_tela.h"
#include "flecha.h"
#include "fase.h"
#include "personagem.h"

InformacoesTela obter_resolucao_tela_atual() {
    InformacoesTela tela;
    ALLEGRO_MONITOR_INFO informacoes_monitor;
    al_get_monitor_info(0, &informacoes_monitor);
    tela.largura = informacoes_monitor.x2 - informacoes_monitor.x1;
    tela.altura = informacoes_monitor.y2 - informacoes_monitor.y1;
    return tela;
}

ALLEGRO_DISPLAY* criar_tela_cheia(InformacoesTela tela) {
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    ALLEGRO_DISPLAY* tela_jogo = al_create_display(tela.largura, tela.altura);
    if (tela_jogo) return tela_jogo;
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    return al_create_display(tela.largura, tela.altura);
}

static float limitar_valor(float valor, float minimo, float maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

bool verificar_colisao(float objeto1_x, float objeto1_y, float objeto1_largura, float objeto1_altura,
    float objeto2_x, float objeto2_y, float objeto2_largura, float objeto2_altura) {
    return (objeto1_x < objeto2_x + objeto2_largura &&
        objeto1_x + objeto1_largura > objeto2_x &&
        objeto1_y < objeto2_y + objeto2_altura &&
        objeto1_y + objeto1_altura > objeto2_y);
}
void adicionarFlecha(Flecha** array, int* count, int* capaciade, Flecha flecha) {
    if (*count == *capaciade) {
        *capaciade = (*capaciade == 0) ? 1 : (*capaciade * 2);
        Flecha* temp = realloc(*array, *capaciade * sizeof(Flecha));
        if (temp == NULL) {
            printf(stderr, "Realocação de memória falhou ao adicionar flecha!\n");
            exit(EXIT_FAILURE);
        }
        *array = temp;
    }

    (*array)[*count] = flecha;
    (*count)++;
}

void criarPontosParabola(float x, float y, float v, ALLEGRO_MOUSE_STATE estadoMouse, float pontos[8], float target_y, float g) {
    float angulo = atan2f(y - estadoMouse.y, x + estadoMouse.x);

    float vx = v * cosf(angulo);
    if (estadoMouse.x < x) vx = -vx;
    float vy = -v * sinf(angulo);

    float delta = vy * vy - 2.0f * g * (y - target_y);
    if (delta < 0) delta = 0;

    float t_hit = (-vy + sqrtf(delta)) / g;

    float x1 = x + vx * t_hit;
    float y1 = y + vy * t_hit + 0.5f * g * t_hit * t_hit;

    float m = vy / vx;
    float m1 = (vy + g * t_hit) / vx;

    float dx = x1 - x;

    float x3 = x + dx / 3;
    float y3 = y + dx / 3 * m;

    float x4 = x1 - dx / 3;
    float y4 = y1 - dx / 3 * m1;


    pontos[0] = x; pontos[1] = y;
    pontos[2] = x3;        pontos[3] = y3;
    pontos[4] = x4;       pontos[5] = y4;
    pontos[6] = x1;  pontos[7] = y1;
}

bool atualizar_flecha(Flecha* flecha, int y_chao, float g, Personagem circe) {
    if (flecha->y >= y_chao) {
        flecha->y = y_chao;
        flecha->tempo_de_vida++;
        return false;
    }
    float x1 = flecha->x + flecha->vx;
    float y1 = flecha->y - flecha->vy;
    flecha->angulo = -atan2f(flecha->vy, flecha->vx);
    flecha->x = x1;
    flecha->y = y1;

    flecha->vy -= g;
    return verificar_colisao(
        flecha->x, flecha->y, flecha->largura, flecha->altura,
        circe.x, circe.y, circe.altura, circe.largura
    );
}

void remover_flecha(Flecha** array, int* count, int indice) {
    if (indice < 0 || indice >= *count) {
        printf(stderr, "Índice inválido ao remover flecha!\n");
        return;
    }

    for (int i = indice; i < *count - 1; i++) {
        (*array)[i] = (*array)[i + 1];
    }

    (*count)--;
}

int main(void) {
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_install_mouse()) return -1;
    if (!al_init_image_addon()) return -1;
    if (!al_init_primitives_addon()) return -1;

    InformacoesTela tela = obter_resolucao_tela_atual();
    const int ALTURA_TELA_ORIGINAL = 1080;
    const int LARGURA_TELA_ORIGINAL = 1920;
    const int LARGURA_TELA = tela.largura;
    const float FORCA_DISPARO_MAX = 30.f;
    const int ALTURA_TELA = tela.altura;
    const int ALTURA_FLECHA = 20, LARGURA_FLECHA = 75;
    const float GRAVIDADE = 1.0f;
    const int ALTURA_SPRITE = 250, LARGURA_SPRITE = 250;
    int ALTURA_PERSONAGEM = deixarProporcional(ALTURA_SPRITE + 60, ALTURA_TELA, ALTURA_TELA_ORIGINAL);
    int LARGURA_PERSONAGEM = deixarProporcional(LARGURA_SPRITE + 25, LARGURA_TELA, LARGURA_TELA_ORIGINAL);

    ALLEGRO_DISPLAY* tela_jogo = criar_tela_cheia(tela);
    if (!tela_jogo) {
        printf("Falha ao criar display.\n");
        return -1;
    }

    if (!menu_inicial(tela_jogo)) {
        printf("Jogo encerrado.\n");
        al_destroy_display(tela_jogo);
        return 0; // Saída normal, não é erro
    }

    // Exibir mapa 
    int escolha_mapa = exibir_mapa_inicial(tela_jogo);
    if (!escolha_mapa) {
        printf("Usuário voltou do mapa.\n");
        al_destroy_display(tela_jogo);
        return 0; // Usuário pressionou ESC no mapa
    }

    Fase* fase = (Fase*)malloc(sizeof(Fase));
    if (!fase) {
        printf("Erro ao alocar memória para a fase.\n");
        return -1;
    }
    if (!carregar_cenario(fase, escolha_mapa)) {
        printf("Erro ao carregar cenários.\n");
        al_destroy_display(tela_jogo);
        return -1;

    }

    // Carregar sprites

    //Odisseu
    ALLEGRO_BITMAP* odisseuParado = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiParado.png");
    ALLEGRO_BITMAP* odisseuAndando = al_load_bitmap("./imagensJogo/personagens/Odisseu/andandoSemEspada.png");
    ALLEGRO_BITMAP* odisseuDesembainhar = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiDesembainhar.png");
    ALLEGRO_BITMAP* odisseuAtacando = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiAtacando.png");
    ALLEGRO_BITMAP* odisseuParadoEspada = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiParadoEspada.png");
    ALLEGRO_BITMAP* odisseuAndandoEspada = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiAndandoEspada.png");

    //Penelope
	ALLEGRO_BITMAP* Penelope = al_load_bitmap("./imagensJogo/personagens/Penelope/penelope.png");

    //Circe
    ALLEGRO_BITMAP* circeparada = al_load_bitmap("./imagensJogo/personagens/Circe/circeparada.png");
    ALLEGRO_BITMAP* circeDano = al_load_bitmap("./imagensJogo/personagens/Circe/circeDano.png");

    //Hermes
    ALLEGRO_BITMAP* hermesParado = al_load_bitmap("./imagensJogo/personagens/Hermes/hermesParado.png");
    ALLEGRO_BITMAP* hermesTiraElmo = al_load_bitmap("./imagensJogo/personagens/Hermes/tirandoElmo.png");



    //parados
    ALLEGRO_BITMAP* inimigoParado1 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo1parado.png");
    ALLEGRO_BITMAP* inimigoParado2 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo2parado.png");
    ALLEGRO_BITMAP* inimigoParado3 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo3parado.png");
    ALLEGRO_BITMAP* inimigoParado4 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo4parado.png");
    ALLEGRO_BITMAP* inimigoParado5 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo5parado.png");
    ALLEGRO_BITMAP* inimigoParado6 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo6parado.png");


    //tomando dano
    ALLEGRO_BITMAP* inimigo1_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo1dano.png");
    ALLEGRO_BITMAP* inimigo2_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo2dano.png");
    ALLEGRO_BITMAP* inimigo3_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo3dano.png");
    ALLEGRO_BITMAP* inimigo4_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo4dano.png");
    ALLEGRO_BITMAP* inimigo5_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo5dano.png");
    ALLEGRO_BITMAP* inimigo6_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo6dano.png");



    //Objetos
    ALLEGRO_BITMAP* sprite_flecha = al_load_bitmap("./imagensJogo/objetos/flecha.png");
    ALLEGRO_BITMAP* sprite_coracao = al_load_bitmap("./imagensJogo/objetos/coracao.png");


    if (!odisseuParado || !odisseuAndando || !odisseuDesembainhar ||
        !odisseuAtacando || !odisseuParadoEspada || !odisseuAndandoEspada ||
        !circeparada || !circeDano || !hermesParado || !hermesTiraElmo || !sprite_flecha ||
        !sprite_coracao) {
        printf("Erro ao carregar imagens dos personagens.\n");
        return -1;
    }


    // Configurações de frames

    //Odisseu configuração
    int total_frames_Odisseu_parado = 5;
    int largura_frame_Odisseu_parado = al_get_bitmap_width(odisseuParado) / total_frames_Odisseu_parado;
    int altura_frame_Odisseu_parado = al_get_bitmap_height(odisseuParado);

    int total_frames_Odisseu_parado_espada = 5;
    int largura_frame_Odisseu_parado_espada = al_get_bitmap_width(odisseuParadoEspada) / total_frames_Odisseu_parado_espada;
    int altura_frame_Odisseu_parado_espada = al_get_bitmap_height(odisseuParadoEspada);

    int total_frames_Odisseu_andando = 6;
    int largura_frame_Odisseu_andando = al_get_bitmap_width(odisseuAndando) / total_frames_Odisseu_andando;
    int altura_frame_Odisseu_andando = al_get_bitmap_height(odisseuAndando);

    int total_frames_Odisseu_andando_espada = 6;
    int largura_frame_Odisseu_andando_espada = al_get_bitmap_width(odisseuAndandoEspada) / total_frames_Odisseu_andando_espada;
    int altura_frame_Odisseu_andando_espada = al_get_bitmap_height(odisseuAndandoEspada);

    int total_frames_desembainhar = 7;
    int largura_frame_desembainhar = al_get_bitmap_width(odisseuDesembainhar) / total_frames_desembainhar;
    int altura_frame_desembainhar = al_get_bitmap_height(odisseuDesembainhar);//aqui

    const int VELOCIDADE_ANIMACAO_DESEMBAINHAR = 6;

    int total_frames_Odisseu_atacando = 6;
    int largura_frame_Odisseu_atacando = al_get_bitmap_width(odisseuAtacando) / total_frames_Odisseu_atacando;
    int altura_frame_Odisseu_atacando = al_get_bitmap_height(odisseuAtacando);


    //Penelope configuração
    int total_frames_penelope_parada = 5;
    int largura_frame_penelope_parada = al_get_bitmap_width(Penelope) / total_frames_penelope_parada;
    int altura_frame_penelope_parada = al_get_bitmap_height(Penelope);


    //Circe configuração
    int total_frames_circeparada = 5;
    int largura_frame_circeparada = al_get_bitmap_width(circeparada) / total_frames_circeparada;
    int altura_frame_circeparada = al_get_bitmap_height(circeparada);

    int total_frames_dano = 7;
    int largura_frame_dano = al_get_bitmap_width(circeDano) / total_frames_dano;
    int altura_frame_dano = al_get_bitmap_height(circeDano);

    //Hermes configuração
    int total_frames_hermesParado = 5;
    int largura_frame_hermesParado = al_get_bitmap_width(hermesParado) / total_frames_hermesParado;
    int altura_frame_hermesParado = al_get_bitmap_height(hermesParado);

    int total_frames_hermesTiraElmo = 18;
    int largura_frame_hermesTiraElmo = al_get_bitmap_width(hermesTiraElmo) / total_frames_hermesTiraElmo;
    int altura_frame_hermesTiraElmo = al_get_bitmap_height(hermesTiraElmo);


    //Config padrão dos inimigos
    int  total_frames_inimigos_parados = 4;
    int total_frames_inimigos_dano = 6;

    //inimigo 1
    int largura_frame_inimigo1_parado = al_get_bitmap_width(inimigoParado1) / total_frames_inimigos_parados;
    int altura_frame_inimigo1_parado = al_get_bitmap_height(inimigoParado1);

    int largura_frame_inimigo1_dano = al_get_bitmap_width(inimigo1_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo1_dano = al_get_bitmap_height(inimigo1_sofre_dano);

    //inimigo 2
    int largura_frame_inimigo2_parado = al_get_bitmap_width(inimigoParado2) / total_frames_inimigos_parados;
    int altura_frame_inimigo2_parado = al_get_bitmap_height(inimigoParado2);

    int largura_frame_inimigo2_dano = al_get_bitmap_width(inimigo2_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo2_dano = al_get_bitmap_height(inimigo2_sofre_dano);
    //inimigo 3
    int largura_frame_inimigo3_parado = al_get_bitmap_width(inimigoParado3) / total_frames_inimigos_parados;
    int altura_frame_inimigo3_parado = al_get_bitmap_height(inimigoParado3);

    int largura_frame_inimigo3_dano = al_get_bitmap_width(inimigo3_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo3_dano = al_get_bitmap_height(inimigo3_sofre_dano);
    //inimigo 4
    int largura_frame_inimigo4_parado = al_get_bitmap_width(inimigoParado4) / total_frames_inimigos_parados;
    int altura_frame_inimigo4_parado = al_get_bitmap_height(inimigoParado4);

    int largura_frame_inimigo4_dano = al_get_bitmap_width(inimigo4_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo4_dano = al_get_bitmap_height(inimigo4_sofre_dano);
    //inimigo 5
    int largura_frame_inimigo5_parado = al_get_bitmap_width(inimigoParado5) / total_frames_inimigos_parados;
    int altura_frame_inimigo5_parado = al_get_bitmap_height(inimigoParado5);

    int largura_frame_inimigo5_dano = al_get_bitmap_width(inimigo5_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo5_dano = al_get_bitmap_height(inimigo5_sofre_dano);
    //inimigo 6
    int largura_frame_inimigo6_parado = al_get_bitmap_width(inimigoParado6) / total_frames_inimigos_parados;
    int altura_frame_inimigo6_parado = al_get_bitmap_height(inimigoParado6);

    int largura_frame_inimigo6_dano = al_get_bitmap_width(inimigo6_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo6_dano = al_get_bitmap_height(inimigo6_sofre_dano);

    // Inicializar personagens
    Personagem odisseu = {
        .x = (escolha_mapa == 2) ? (LARGURA_TELA / 4.2) : (LARGURA_TELA / 30),
        .y = (escolha_mapa == 3) ? deixarProporcional(650, ALTURA_TELA, ALTURA_TELA_ORIGINAL)
        : deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
        .vida = 3,
        .largura = LARGURA_PERSONAGEM,
        .altura = ALTURA_PERSONAGEM,
        .olhando_direita = true,
        .olhando_esquerda = false,
        .andando = false,
        .desembainhando = false,
        .sofrendo_dano = false,
        .guardando_espada = false,
        .atacando = false,
        .tem_espada = false,
        .frame_atual = 0,
        .contador_animacao = 0,
        .disparando = false,
        .forca_disparo = 0.0f

    };
    const int y_chao = odisseu.y + ALTURA_PERSONAGEM;


    Personagem circe = {
        .x = LARGURA_TELA - (LARGURA_TELA / 3),
        .y = deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
        .vida = 3,
        .largura = LARGURA_PERSONAGEM,
        .altura = ALTURA_PERSONAGEM,
        .olhando_direita = true,
        .olhando_esquerda = false,
        .andando = false,
        .desembainhando = false,
        .sofrendo_dano = false,
        .guardando_espada = false,
        .atacando = false,
        .tem_espada = false,
        .frame_atual = 0,
        .contador_animacao = 0
    };

        Personagem penelope = {
        .x = LARGURA_TELA - (LARGURA_TELA / 2.2),
        .y = deixarProporcional(650, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
        .largura = LARGURA_PERSONAGEM,
        .altura = ALTURA_PERSONAGEM,
        .olhando_direita = true,
        .olhando_esquerda = false,
        .frame_atual = 0,
        .contador_animacao = 0
    };

    Personagem Hermes = {
        .x = LARGURA_TELA / 1.5,
        .y = deixarProporcional(710, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
        .largura = LARGURA_PERSONAGEM,
        .altura = ALTURA_PERSONAGEM,
        .olhando_direita = true,
        .olhando_esquerda = false,
        .andando = false,
        .desembainhando = false,
        .sofrendo_dano = false,
        .guardando_espada = false,
        .atacando = false,
        .tem_espada = false,
        .frame_atual = 0,
        .contador_animacao = 0,
        .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 5
    };

    Personagem inimigo1 = {
    .x = LARGURA_TELA / 3,
    .y = deixarProporcional(550, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
    .vida = 3,
    .largura = LARGURA_PERSONAGEM,
    .altura = ALTURA_PERSONAGEM,
    .olhando_direita = true,
    .olhando_esquerda = false,
    .sofrendo_dano = false,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };

    Personagem inimigo2 = {
    .x = LARGURA_TELA / 1.9,
    .y = deixarProporcional(550, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
    .vida = 3,
    .largura = LARGURA_PERSONAGEM,
    .altura = ALTURA_PERSONAGEM,
    .olhando_direita = true,
    .olhando_esquerda = false,
    .sofrendo_dano = false,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };

    Personagem inimigo3 = {
    .x = LARGURA_TELA / 1.45,
    .y = deixarProporcional(550, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
    .vida = 3,
    .largura = LARGURA_PERSONAGEM,
    .altura = ALTURA_PERSONAGEM,
    .olhando_direita = true,
    .olhando_esquerda = false,
    .sofrendo_dano = false,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };

    Personagem inimigo4 = {
    .x = LARGURA_TELA / 3,
    .y = deixarProporcional(550, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
    .vida = 3,
    .largura = LARGURA_PERSONAGEM,
    .altura = ALTURA_PERSONAGEM,
    .olhando_direita = true,
    .olhando_esquerda = false,
    .sofrendo_dano = false,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };

    Personagem inimigo5 = {
    .x = LARGURA_TELA / 1.8,
    .y = deixarProporcional(550, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
    .vida = 3,
    .largura = LARGURA_PERSONAGEM,
    .altura = ALTURA_PERSONAGEM,
    .olhando_direita = true,
    .olhando_esquerda = false,
    .sofrendo_dano = false,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };

    Personagem inimigo6 = {
    .x = LARGURA_TELA / 1.2,
    .y = deixarProporcional(550, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
    .vida = 3,
    .largura = LARGURA_PERSONAGEM,
    .altura = ALTURA_PERSONAGEM,
    .olhando_direita = true,
    .olhando_esquerda = false,
    .sofrendo_dano = false,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };


    static bool hermes_animacao_concluida = false;
    bool ataque_ativado = false;
    int duracao_ataque = 0;
    const int DURACAO_MAXIMA_ATAQUE = 15;
    float pontosArco[8];

    // Sistema de eventos
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    ALLEGRO_TIMER* temporizador = al_create_timer(1.0 / 60.0);
    al_register_event_source(fila_eventos, al_get_display_event_source(tela_jogo));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(temporizador));
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_start_timer(temporizador);

    bool jogo_rodando = true;
    bool redesenhar_tela = false;
    Flecha* listaFlechas = (Flecha*)malloc(sizeof(*listaFlechas));
    int count_flechas = 0, capacidade_flechas = 0;
    if (listaFlechas == NULL) {
        printf(stderr, "alocação de memória para as flechas falhou!\n");
        return -1;
    }


    while (jogo_rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jogo_rodando = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                jogo_rodando = false;
                return 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_E && !odisseu.andando &&
                !odisseu.desembainhando && !odisseu.atacando && !odisseu.tem_espada) {
                odisseu.desembainhando = true;
                odisseu.frame_atual = 0;
                odisseu.contador_animacao = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_E && !odisseu.andando &&
                !odisseu.desembainhando && !odisseu.atacando && odisseu.tem_espada) {
                odisseu.guardando_espada = true;
                odisseu.frame_atual = total_frames_desembainhar - 1;
                odisseu.contador_animacao = 0;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1 && odisseu.tem_espada &&
                !odisseu.andando && !odisseu.desembainhando && !odisseu.atacando) {
                odisseu.atacando = true;
                odisseu.frame_atual = 0;
                ataque_ativado = true;
                duracao_ataque = 0;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            ALLEGRO_KEYBOARD_STATE estado_teclado;
            ALLEGRO_MOUSE_STATE estado_mouse;
            al_get_keyboard_state(&estado_teclado);
            al_get_mouse_state(&estado_mouse);
            float odisseu_direcao_x = 0.0f;

            if (!odisseu.desembainhando && !odisseu.atacando && !odisseu.guardando_espada) {
                if (al_key_down(&estado_teclado, ALLEGRO_KEY_A) || al_key_down(&estado_teclado, ALLEGRO_KEY_LEFT)) {
                    odisseu_direcao_x -= 0.1f;
                    odisseu.olhando_direita = false;
                }
                if (al_key_down(&estado_teclado, ALLEGRO_KEY_D) || al_key_down(&estado_teclado, ALLEGRO_KEY_RIGHT)) {
                    odisseu_direcao_x += 0.1f;
                    odisseu.olhando_direita = true;
                }
            }

            if (odisseu_direcao_x != 0.0f) {
                float comprimento = sqrtf(odisseu_direcao_x * odisseu_direcao_x);
                odisseu_direcao_x /= comprimento;
            }
            if (!odisseu.tem_espada && al_mouse_button_down(&estado_mouse, ALLEGRO_MOUSE_BUTTON_LEFT)) {
                odisseu.disparando = true;
                if (odisseu.forca_disparo <= FORCA_DISPARO_MAX - 0.75f) {
                    odisseu.forca_disparo += 0.75f;
                }
                criarPontosParabola(odisseu.x + (LARGURA_PERSONAGEM / 2), odisseu.y + (ALTURA_PERSONAGEM / 2), odisseu.forca_disparo, estado_mouse, pontosArco, y_chao, GRAVIDADE);
            }
            else {
                if (odisseu.disparando) {
                    Flecha f = {
                        .x = odisseu.x + (LARGURA_PERSONAGEM / 2),
                        .y = odisseu.y + (ALTURA_PERSONAGEM / 2),
                        .altura = ALTURA_FLECHA,
                        .largura = LARGURA_FLECHA,
                        .tempo_de_vida = 0.0f
                    };
                    f.angulo = atan2f(f.y - estado_mouse.y, f.x + estado_mouse.x);
                    f.vx = odisseu.forca_disparo * cosf(f.angulo),
                        f.vy = odisseu.forca_disparo * sinf(f.angulo),
                        adicionarFlecha(&listaFlechas, &count_flechas, &capacidade_flechas, f);
                }
                odisseu.disparando = false;
                odisseu.forca_disparo = 0.0f;
            }


            const float velocidade_odisseu = 15.0f;
            odisseu.x += odisseu_direcao_x * velocidade_odisseu;
            odisseu.y = limitar_valor(odisseu.y, 0, ALTURA_TELA - odisseu.altura);
            odisseu.andando = (odisseu_direcao_x != 0.0f);

            // Atualizar transição de cenário
            atualizar_transicao_cenario(fase, &odisseu.x, odisseu.largura, LARGURA_TELA);


            //barreira no primeiro cenario de todos os mapas
            if (fase->cenario_atual == 0 && odisseu.x < LARGURA_TELA / -24) odisseu.x = LARGURA_TELA / -24;


            //barreira mapa 1 (Polifemo)
            if (fase->cenario_atual == 5 && escolha_mapa == 1 && odisseu.x > LARGURA_TELA - LARGURA_TELA / 10) odisseu.x = LARGURA_TELA - LARGURA_TELA / 10;


           


            //barreira mapa 5 (submundo)
            if (fase->cenario_atual == 0 && escolha_mapa == 5 && odisseu.x > LARGURA_TELA / 3.5) odisseu.x = LARGURA_TELA / 3.5;

           
            // Atualizar animação do Odisseu
            int estado_animacao = 0;
            if (odisseu.desembainhando) estado_animacao = 1;
            else if (odisseu.guardando_espada) estado_animacao = 2;
            else if (odisseu.atacando) estado_animacao = 3;

            switch (estado_animacao) {
            case 1: // Desembainhar
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= VELOCIDADE_ANIMACAO_DESEMBAINHAR) {
                    odisseu.frame_atual++;
                    if (odisseu.frame_atual >= total_frames_desembainhar) {
                        odisseu.desembainhando = false;
                        odisseu.frame_atual = 0;
                        odisseu.tem_espada = true;
                    }
                    odisseu.contador_animacao = 0;
                }
                break;
            case 2: // Guardar espada
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= VELOCIDADE_ANIMACAO_DESEMBAINHAR) {
                    odisseu.frame_atual--;
                    if (odisseu.frame_atual < 0) {
                        odisseu.guardando_espada = false;
                        odisseu.frame_atual = 0;
                        odisseu.tem_espada = false;
                    }
                    odisseu.contador_animacao = 0;
                }
                break;
            case 3: // Atacar
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= 10) {
                    odisseu.frame_atual++;
                    if (odisseu.frame_atual >= total_frames_Odisseu_atacando) {
                        odisseu.atacando = false;
                        odisseu.frame_atual = 0;
                        ataque_ativado = false;
                    }
                    odisseu.contador_animacao = 0;
                }
                break;
            case 0: // Andar ou parado
            default:
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= 10) {
                    odisseu.frame_atual++;
                    if (odisseu.andando)
                        odisseu.frame_atual %= (odisseu.tem_espada ? total_frames_Odisseu_andando_espada : total_frames_Odisseu_andando);
                    else
                        odisseu.frame_atual %= (odisseu.tem_espada ? total_frames_Odisseu_parado_espada : total_frames_Odisseu_parado);
                    odisseu.contador_animacao = 0;
                }
                break;
            }



            //================================MISSÃO CIRCE=============================================
			
            if (escolha_mapa == 2) {
               
           //barreiras para não serem ultrapassadas no mapa 2
           //cenário 0
                if (fase->cenario_atual == 0 && odisseu.x < (LARGURA_TELA / 4.2)) odisseu.x = (LARGURA_TELA / 4.2);
           //cenario 3
                if (fase->cenario_atual == 3 && odisseu.x > LARGURA_TELA - LARGURA_TELA / 10) odisseu.x = LARGURA_TELA - LARGURA_TELA / 10;


                // Atualizar Circe
                if (fase->cenario_atual == 3 && circe.vida > 0) {
                    circe.contador_animacao++;
                    int delay_animacao_circe = circe.sofrendo_dano ? 5 : 10;
                    if (circe.contador_animacao >= delay_animacao_circe) {
                        if (circe.sofrendo_dano) {
                            circe.frame_atual++;
                            if (circe.frame_atual >= total_frames_dano) {
                                circe.sofrendo_dano = false;
                                circe.frame_atual = 0;
                            }
                        }
                        else {
                            circe.frame_atual = (circe.frame_atual + 1) % total_frames_circeparada;
                        }
                        circe.contador_animacao = 0;
                    }
                }

                // configuração Hermes
                // Atualizar Hermes

                if (fase->cenario_atual == 1) {
                    // Atualizar direção de Hermes baseado na posição do Odisseu
                    if (odisseu.x > Hermes.x) {
                        Hermes.olhando_direita = true;
                    }
                    else {
                        Hermes.olhando_direita = false;
                    }

                    // Se a animação de tirar elmo não foi concluída
                    if (!hermes_animacao_concluida) {
                        Hermes.frame_contador++;
                        if (Hermes.frame_contador >= Hermes.velocidade_animacao) {
                            Hermes.frame_contador = 0;
                            Hermes.frame_atual++;

                            // Quando completar os 18 frames da animação de tirar elmo
                            if (Hermes.frame_atual >= 18) {
                                hermes_animacao_concluida = true;
                                Hermes.frame_atual = 0;
                                Hermes.num_frames = 5; // Volta para animação parada com 5 frames
                            }
                        }
                    }
                    // Após concluir a animação, continua com a animação parada normal
                    else {
                        Hermes.frame_contador++;
                        if (Hermes.frame_contador >= Hermes.velocidade_animacao) {
                            Hermes.frame_contador = 0;
                            Hermes.frame_atual++;

                            if (Hermes.frame_atual >= Hermes.num_frames) {
                                Hermes.frame_atual = 0;
                            }
                        }
                    }
                }



            }



           
            //=================================MISSÃO ITACA=============================================
            else if (escolha_mapa == 3) {

                if (fase->cenario_atual == 2 && odisseu.x > LARGURA_TELA - LARGURA_TELA / 9) odisseu.x = LARGURA_TELA - LARGURA_TELA / 9;

                if (fase->cenario_atual == 1){ 
                bool algum_inimigo_vivo = (inimigo4.vida > 0 || inimigo5.vida > 0 || inimigo6.vida > 0);
                float limite_barreira = LARGURA_TELA - LARGURA_TELA / 9;

                if (algum_inimigo_vivo && odisseu.x > limite_barreira) {
                    odisseu.x = limite_barreira;
                }
            }
                
                if (fase->cenario_atual == 0) {
                    bool algum_inimigo_vivo = (inimigo1.vida > 0 || inimigo2.vida > 0 || inimigo3.vida > 0);
                    float limite_barreira = LARGURA_TELA - LARGURA_TELA / 9;

                    if (algum_inimigo_vivo && odisseu.x > limite_barreira) {
                        odisseu.x = limite_barreira;
                    }
                }
                //config da escada
                if (fase->cenario_atual == 0 && odisseu.x < LARGURA_TELA / 1.38) odisseu.y = 550;
                if (fase->cenario_atual == 0 && (odisseu.x > LARGURA_TELA / 1.32)) odisseu.y = 520;
                if (fase->cenario_atual == 0 && (odisseu.x > LARGURA_TELA / 1.25)) odisseu.y = 490;
                if (fase->cenario_atual == 0 && (odisseu.x > LARGURA_TELA / 1.22)) odisseu.y = 460;
                if (fase->cenario_atual == 0 && (odisseu.x > LARGURA_TELA / 1.19)) odisseu.y = 430;
                if (fase->cenario_atual == 0 && (odisseu.x > LARGURA_TELA / 1.15)) odisseu.y = 380;
                if (fase->cenario_atual == 1 && inimigo4.vida > 0 && inimigo5.vida > 0 && inimigo6.vida > 0 && odisseu.x > LARGURA_TELA - LARGURA_TELA / 9) odisseu.x = LARGURA_TELA - LARGURA_TELA / 9;
				if (fase->cenario_atual == 1) odisseu.y = 550;
                if (fase->cenario_atual == 2) odisseu.y = 650;


                //===========================CARREGAMENTO DOS PERSONAGENS ITACA=================================
                
                // Atualizar animação dos inimigos
                if (fase->cenario_atual == 0) {
                    // Atualizar inimigo1
                    inimigo1.contador_animacao++;
                    if (inimigo1.contador_animacao >= inimigo1.velocidade_animacao) {
                        inimigo1.frame_atual = (inimigo1.frame_atual + 1) % inimigo1.num_frames;
                        inimigo1.contador_animacao = 0;
                    }
                    if (inimigo1.sofrendo_dano && inimigo1.vida > 0) {
                        inimigo1.contador_animacao++;
                        if (inimigo1.contador_animacao >= 5) {
                            inimigo1.frame_atual++;
                            if (inimigo1.frame_atual >= total_frames_inimigos_dano) {
                                inimigo1.sofrendo_dano = false;
                                inimigo1.frame_atual = 0;
                            }
                            inimigo1.contador_animacao = 0;
                        }
                    }

                    // Atualizar inimigo2
                    inimigo2.contador_animacao++;
                    if (inimigo2.contador_animacao >= inimigo2.velocidade_animacao) {
                        inimigo2.frame_atual = (inimigo2.frame_atual + 1) % inimigo2.num_frames;
                        inimigo2.contador_animacao = 0;
                    }
                    if (inimigo2.sofrendo_dano && inimigo2.vida > 0) {
                        inimigo2.contador_animacao++;
                        if (inimigo2.contador_animacao >= 5) {
                            inimigo2.frame_atual++;
                            if (inimigo2.frame_atual >= total_frames_inimigos_dano) {
                                inimigo2.sofrendo_dano = false;
                                inimigo2.frame_atual = 0;
                            }
                            inimigo2.contador_animacao = 0;
                        }
                    }

                    // Atualizar inimigo3
                    inimigo3.contador_animacao++;
                    if (inimigo3.contador_animacao >= inimigo3.velocidade_animacao) {
                        inimigo3.frame_atual = (inimigo3.frame_atual + 1) % inimigo3.num_frames;
                        inimigo3.contador_animacao = 0;
                    }
                    if (inimigo3.sofrendo_dano && inimigo3.vida > 0) {
                        inimigo3.contador_animacao++;
                        if (inimigo3.contador_animacao >=5) {
                            inimigo3.frame_atual++;
                            if (inimigo3.frame_atual >= total_frames_inimigos_dano) {
                                inimigo3.sofrendo_dano = false;
                                inimigo3.frame_atual = 0;
                            }
                            inimigo3.contador_animacao = 0;
                        }
                    }
                }

				if (fase->cenario_atual == 1 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0) {

                    // Atualizar inimigo4
                    inimigo4.contador_animacao++;
                    if (inimigo4.contador_animacao >= inimigo4.velocidade_animacao) {
                        inimigo4.frame_atual = (inimigo4.frame_atual + 1) % inimigo4.num_frames;
                        inimigo4.contador_animacao = 0;
                    }
                    if (inimigo4.sofrendo_dano && inimigo4.vida > 0) {
                        inimigo4.contador_animacao++;
                        if (inimigo4.contador_animacao >= 5) {
                            inimigo4.frame_atual++;
                            if (inimigo4.frame_atual >= total_frames_inimigos_dano) {
                                inimigo4.sofrendo_dano = false;
                                inimigo4.frame_atual = 0;
                            }
                            inimigo4.contador_animacao = 0;
                        }
                    }
                    // Atualizar inimigo5
                    inimigo5.contador_animacao++;
                    if (inimigo5.contador_animacao >= inimigo5.velocidade_animacao) {
                        inimigo5.frame_atual = (inimigo5.frame_atual + 1) % inimigo5.num_frames;
                        inimigo5.contador_animacao = 0;
                    }
                    if (inimigo5.sofrendo_dano && inimigo5.vida > 0) {
                        inimigo5.contador_animacao++;
                        if (inimigo5.contador_animacao >= 5) {
                            inimigo5.frame_atual++;
                            if (inimigo5.frame_atual >= total_frames_inimigos_dano) {
                                inimigo5.sofrendo_dano = false;
                                inimigo5.frame_atual = 0;
                            }
                            inimigo5.contador_animacao = 0;
                        }
                    }
                    // Atualizar inimigo6
                    inimigo6.contador_animacao++;
                    if (inimigo6.contador_animacao >= inimigo6.velocidade_animacao) {
                        inimigo6.frame_atual = (inimigo6.frame_atual + 1) % inimigo6.num_frames;
                        inimigo6.contador_animacao = 0;
                    }
                    if (inimigo6.sofrendo_dano && inimigo6.vida > 0) {
                        inimigo6.contador_animacao++;
                        if (inimigo6.contador_animacao >= 5) {
                            inimigo6.frame_atual++;
                            if (inimigo6.frame_atual >= total_frames_inimigos_dano) {
                                inimigo6.sofrendo_dano = false;
                                inimigo6.frame_atual = 0;
                            }
                            inimigo6.contador_animacao = 0;
                        }
                    }                  
                }
                if (fase->cenario_atual == 2) {
                    // Atualizar Penelope
                    penelope.contador_animacao++;
                    if (penelope.contador_animacao >= 10) {
                        penelope.frame_atual = (penelope.frame_atual + 1) % total_frames_penelope_parada;
                        penelope.contador_animacao = 0;
                    }
                }
            }


            // Verificar colisão
            if (ataque_ativado) {
                duracao_ataque++;
                float area_ataque_x = odisseu.olhando_direita ? odisseu.x + odisseu.largura * 0.5f : odisseu.x - odisseu.largura * 0.3f;
                float area_ataque_y = odisseu.y + odisseu.altura * 0.1f;
                float area_ataque_largura = odisseu.largura * 0.1f;
                float area_ataque_altura = odisseu.altura * 0.1f;

                if (verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                    circe.x, circe.y, circe.largura, circe.altura)) {

                    // Só aplicar dano se a Circe ainda não estiver no estado "sof rendo_dano"
                    if (!circe.sofrendo_dano) {
                        circe.sofrendo_dano = true;
                        circe.vida--;
                        circe.frame_atual = 0;
                    }
                }
                if (escolha_mapa == 3) {
                    // Inimigo 1
                    if (inimigo1.vida > 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                        inimigo1.x, inimigo1.y, inimigo1.largura, inimigo1.altura)) {
                        if (!inimigo1.sofrendo_dano) {
                            inimigo1.sofrendo_dano = true;
                            inimigo1.vida--;
                            inimigo1.frame_atual = 0;
                        }
                    }

                    // Inimigo 2
                    if (inimigo2.vida > 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                        inimigo2.x, inimigo2.y, inimigo2.largura, inimigo2.altura)) {
                        if (!inimigo2.sofrendo_dano) {
                            inimigo2.sofrendo_dano = true;
                            inimigo2.vida--;
                            inimigo2.frame_atual = 0;
                        }
                    }

                    // Inimigo 3
                    if (inimigo3.vida > 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                        inimigo3.x, inimigo3.y, inimigo3.largura, inimigo3.altura)) {
                        if (!inimigo3.sofrendo_dano) {
                            inimigo3.sofrendo_dano = true;
                            inimigo3.vida--;
                            inimigo3.frame_atual = 0;
                        }
                    }

                    // Inimigo 4
                    if (inimigo4.vida > 0 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0  && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                        inimigo4.x, inimigo4.y, inimigo4.largura, inimigo4.altura)) {
                        if (!inimigo4.sofrendo_dano) {
                            inimigo4.sofrendo_dano = true;
                            inimigo4.vida--;
                            inimigo4.frame_atual = 0;
                        }
                    }

                    // Inimigo 5
                    if (inimigo5.vida > 0 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                        inimigo5.x, inimigo5.y, inimigo5.largura, inimigo5.altura)) {
                        if (!inimigo5.sofrendo_dano) {
                            inimigo5.sofrendo_dano = true;
                            inimigo5.vida--;
                            inimigo5.frame_atual = 0;
                        }
                    }

                    // Inimigo 6
                    if (inimigo6.vida > 0 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                        inimigo6.x, inimigo6.y, inimigo6.largura, inimigo6.altura)) {
                        if (!inimigo6.sofrendo_dano) {
                            inimigo6.sofrendo_dano = true;
                            inimigo6.vida--;
                            inimigo6.frame_atual = 0;
                        }
                    }
                }

                if (duracao_ataque >= DURACAO_MAXIMA_ATAQUE) ataque_ativado = false;
            }

            //atualiza flechas
            int i = 0;
            while (i < count_flechas) {
                if (atualizar_flecha(&listaFlechas[i], y_chao, GRAVIDADE, circe)) {
                    circe.sofrendo_dano = true;
                    circe.vida--;
                    remover_flecha(&listaFlechas, &count_flechas, i);
                }
                else if (listaFlechas[i].tempo_de_vida > 120.0f) {
                    remover_flecha(&listaFlechas, &count_flechas, i);
                }
                else {
                    i++;
                }
            }


            redesenhar_tela = true;
        }

        if (redesenhar_tela && al_is_event_queue_empty(fila_eventos)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Desenhar cenário
            desenhar_cenario(fase, LARGURA_TELA, ALTURA_TELA);

            // Selecionar sprite do Odisseu
            ALLEGRO_BITMAP* sprite_atual_odisseu;
            int largura_frame_odisseu, altura_frame_odisseu;
            int estado_odisseu = 0;

            if (odisseu.desembainhando || odisseu.guardando_espada) estado_odisseu = 2;
            else if (odisseu.atacando) estado_odisseu = 3;
            else if (odisseu.andando) estado_odisseu = odisseu.tem_espada ? 5 : 1;
            else estado_odisseu = odisseu.tem_espada ? 4 : 0;

            switch (estado_odisseu) {
            case 2:
                sprite_atual_odisseu = odisseuDesembainhar;
                largura_frame_odisseu = largura_frame_desembainhar;
                altura_frame_odisseu = altura_frame_desembainhar;
                break;
            case 3:
                sprite_atual_odisseu = odisseuAtacando;
                largura_frame_odisseu = largura_frame_Odisseu_atacando;
                altura_frame_odisseu = altura_frame_Odisseu_atacando;
                break;
            case 1:
                sprite_atual_odisseu = odisseuAndando;
                largura_frame_odisseu = largura_frame_Odisseu_andando;
                altura_frame_odisseu = altura_frame_Odisseu_andando;
                break;
            case 5:
                sprite_atual_odisseu = odisseuAndandoEspada;
                largura_frame_odisseu = largura_frame_Odisseu_andando_espada;
                altura_frame_odisseu = altura_frame_Odisseu_andando_espada;
                break;
            case 4:
                sprite_atual_odisseu = odisseuParadoEspada;
                largura_frame_odisseu = largura_frame_Odisseu_parado_espada;
                altura_frame_odisseu = altura_frame_Odisseu_parado_espada;
                break;
            case 0:
            default:
                sprite_atual_odisseu = odisseuParado;
                largura_frame_odisseu = largura_frame_Odisseu_parado;
                altura_frame_odisseu = altura_frame_Odisseu_parado;
                break;
            }

            // Desenhar Odisseu
            int flagsOdisseu = odisseu.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
            al_draw_scaled_bitmap(
                sprite_atual_odisseu,
                odisseu.frame_atual * largura_frame_odisseu, 0,
                largura_frame_odisseu, altura_frame_odisseu,
                odisseu.x, odisseu.y,
                odisseu.largura, odisseu.altura,
                flagsOdisseu);

            //desenha linha de disparo das flechas
            if (odisseu.disparando) {
                al_draw_spline(pontosArco, al_map_rgb(0, 0, 255), 3.0f);
            }

            //desenhar flechas
            for (int j = 0; j < count_flechas; j++) {
                al_draw_scaled_rotated_bitmap(
                    sprite_flecha,
                    0 + LARGURA_FLECHA / 2, 0 + ALTURA_FLECHA / 2,
                    listaFlechas[j].x + LARGURA_FLECHA / 2, listaFlechas[j].y + ALTURA_FLECHA / 2,
                    1, 1,
                    listaFlechas[j].angulo,
                    0
                );
            }
            if (escolha_mapa == 3) {
                // DESENHO DOS INIMIGOS NO CENÁRIO 0 (Inimigos 1, 2 e 3)
                if (fase->cenario_atual == 0) {
                    // INIMIGO 1
                    if (inimigo1.vida > 0) {
                        ALLEGRO_BITMAP* sprite_inimigo1 = inimigo1.sofrendo_dano ? inimigo1_sofre_dano : inimigoParado1;
                        int largura_inimigo1 = inimigo1.sofrendo_dano ? largura_frame_inimigo1_dano : largura_frame_inimigo1_parado;
                        int altura_inimigo1 = inimigo1.sofrendo_dano ? altura_frame_inimigo1_dano : altura_frame_inimigo1_parado;
                        int frame_inimigo1 = inimigo1.sofrendo_dano ? inimigo1.frame_atual : inimigo1.frame_atual % total_frames_inimigos_parados;
                        int flags_inimigo1 = inimigo1.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(sprite_inimigo1, frame_inimigo1 * largura_inimigo1, 0,
                            largura_inimigo1, altura_inimigo1, inimigo1.x, inimigo1.y,
                            inimigo1.largura, inimigo1.altura, flags_inimigo1);
                    }

                    // INIMIGO 2
                    if (inimigo2.vida > 0) {
                        ALLEGRO_BITMAP* sprite_inimigo2 = inimigo2.sofrendo_dano ? inimigo2_sofre_dano : inimigoParado2;
                        int largura_inimigo2 = inimigo2.sofrendo_dano ? largura_frame_inimigo2_dano : largura_frame_inimigo2_parado;
                        int altura_inimigo2 = inimigo2.sofrendo_dano ? altura_frame_inimigo2_dano : altura_frame_inimigo2_parado;
                        int frame_inimigo2 = inimigo2.sofrendo_dano ? inimigo2.frame_atual : inimigo2.frame_atual % total_frames_inimigos_parados;
                        int flags_inimigo2 = inimigo2.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(sprite_inimigo2, frame_inimigo2 * largura_inimigo2, 0,
                            largura_inimigo2, altura_inimigo2, inimigo2.x, inimigo2.y,
                            inimigo2.largura, inimigo2.altura, flags_inimigo2);
                    }

                    // INIMIGO 3
                    if (inimigo3.vida > 0) {
                        ALLEGRO_BITMAP* sprite_inimigo3 = inimigo3.sofrendo_dano ? inimigo3_sofre_dano : inimigoParado3;
                        int largura_inimigo3 = inimigo3.sofrendo_dano ? largura_frame_inimigo3_dano : largura_frame_inimigo3_parado;
                        int altura_inimigo3 = inimigo3.sofrendo_dano ? altura_frame_inimigo3_dano : altura_frame_inimigo3_parado;
                        int frame_inimigo3 = inimigo3.sofrendo_dano ? inimigo3.frame_atual : inimigo3.frame_atual % total_frames_inimigos_parados;
                        int flags_inimigo3 = inimigo3.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(sprite_inimigo3, frame_inimigo3 * largura_inimigo3, 0,
                            largura_inimigo3, altura_inimigo3, inimigo3.x, inimigo3.y,
                            inimigo3.largura, inimigo3.altura, flags_inimigo3);
                    }
                }

                // DESENHO DOS INIMIGOS NO CENÁRIO 1 (Inimigos 4, 5 e 6)
                if (fase->cenario_atual == 1) {
                    // INIMIGO 4
                    if (inimigo4.vida > 0) {
                        ALLEGRO_BITMAP* sprite_inimigo4 = inimigo4.sofrendo_dano ? inimigo4_sofre_dano : inimigoParado4;
                        int largura_inimigo4 = inimigo4.sofrendo_dano ? largura_frame_inimigo4_dano : largura_frame_inimigo4_parado;
                        int altura_inimigo4 = inimigo4.sofrendo_dano ? altura_frame_inimigo4_dano : altura_frame_inimigo4_parado;
                        int frame_inimigo4 = inimigo4.sofrendo_dano ? inimigo4.frame_atual : inimigo4.frame_atual % total_frames_inimigos_parados;
                        int flags_inimigo4 = inimigo4.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(sprite_inimigo4, frame_inimigo4 * largura_inimigo4, 0,
                            largura_inimigo4, altura_inimigo4, inimigo4.x, inimigo4.y,
                            inimigo4.largura, inimigo4.altura, flags_inimigo4);
                    }

                    // INIMIGO 5
                    if (inimigo5.vida > 0) {
                        ALLEGRO_BITMAP* sprite_inimigo5 = inimigo5.sofrendo_dano ? inimigo5_sofre_dano : inimigoParado5;
                        int largura_inimigo5 = inimigo5.sofrendo_dano ? largura_frame_inimigo5_dano : largura_frame_inimigo5_parado;
                        int altura_inimigo5 = inimigo5.sofrendo_dano ? altura_frame_inimigo5_dano : altura_frame_inimigo5_parado;
                        int frame_inimigo5 = inimigo5.sofrendo_dano ? inimigo5.frame_atual : inimigo5.frame_atual % total_frames_inimigos_parados;
                        int flags_inimigo5 = inimigo5.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(sprite_inimigo5, frame_inimigo5 * largura_inimigo5, 0,
                            largura_inimigo5, altura_inimigo5, inimigo5.x, inimigo5.y,
                            inimigo5.largura, inimigo5.altura, flags_inimigo5);
                    }

                    // INIMIGO 6
                    if (inimigo6.vida > 0) {
                        ALLEGRO_BITMAP* sprite_inimigo6 = inimigo6.sofrendo_dano ? inimigo6_sofre_dano : inimigoParado6;
                        int largura_inimigo6 = inimigo6.sofrendo_dano ? largura_frame_inimigo6_dano : largura_frame_inimigo6_parado;
                        int altura_inimigo6 = inimigo6.sofrendo_dano ? altura_frame_inimigo6_dano : altura_frame_inimigo6_parado;
                        int frame_inimigo6 = inimigo6.sofrendo_dano ? inimigo6.frame_atual : inimigo6.frame_atual % total_frames_inimigos_parados;
                        int flags_inimigo6 = inimigo6.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                        al_draw_scaled_bitmap(sprite_inimigo6, frame_inimigo6 * largura_inimigo6, 0,
                            largura_inimigo6, altura_inimigo6, inimigo6.x, inimigo6.y,
                            inimigo6.largura, inimigo6.altura, flags_inimigo6);
                    }
                }
                if (fase->cenario_atual == 2) {
                    // Desenhar Penelope
                    int flagsPenelope = penelope.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                    al_draw_scaled_bitmap(
                        Penelope,
                        penelope.frame_atual * largura_frame_penelope_parada, 0,
                        largura_frame_penelope_parada, altura_frame_penelope_parada,
                        penelope.x, penelope.y,
                        penelope.largura, penelope.altura,
                        flagsPenelope);
				}
            }
            desenhar_sobreposicoes(fase, LARGURA_TELA, ALTURA_TELA);

            // Desenhar Hermes no cenário 7 (mesmo comportamento visual do Odisseu parado)
            if (fase->cenario_atual == 1 && escolha_mapa == 2) {
                ALLEGRO_BITMAP* sprite_hermes;
                int largura_frame_hermes;
                int altura_frame_hermes;
                int total_frames_hermes;

                // Escolher sprite baseado se a animação foi concluída
                if (!hermes_animacao_concluida) {
                    sprite_hermes = hermesTiraElmo;
                    largura_frame_hermes = largura_frame_hermesTiraElmo;
                    altura_frame_hermes = altura_frame_hermesTiraElmo;
                    total_frames_hermes = total_frames_hermesTiraElmo;
                }
                else {
                    sprite_hermes = hermesParado;
                    largura_frame_hermes = largura_frame_hermesParado;
                    altura_frame_hermes = altura_frame_hermesParado;
                    total_frames_hermes = total_frames_hermesParado;
                } 
                int frame_hermes = Hermes.frame_atual % total_frames_hermes;
                int flagsHermes = Hermes.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

                al_draw_scaled_bitmap(
                    sprite_hermes,
                    frame_hermes * largura_frame_hermes, 0,
                    largura_frame_hermes, altura_frame_hermes,
                    Hermes.x, Hermes.y,
                    Hermes.largura, Hermes.altura,
                    flagsHermes);
            }


            // Desenhar Circe no último cenário
            if (fase->cenario_atual == 3 && escolha_mapa == 2 && circe.vida > 0) {
                ALLEGRO_BITMAP* sprite_atual_circe = circe.sofrendo_dano ? circeDano : circeparada;
                int largura_frame_circe = circe.sofrendo_dano ? largura_frame_dano : largura_frame_circeparada;
                int altura_frame_circe = circe.sofrendo_dano ? altura_frame_dano : altura_frame_circeparada;
                int frame_circe = circe.sofrendo_dano ? circe.frame_atual : circe.frame_atual % total_frames_circeparada;

                int flagsCirce = circe.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                al_draw_scaled_bitmap(
                    sprite_atual_circe,
                    frame_circe * largura_frame_circe, 0,
                    largura_frame_circe, altura_frame_circe,
                    circe.x, circe.y,
                    circe.largura, circe.altura, flagsCirce);
            }

            //desenhar coracao de vida
            for (int i = 0; i < odisseu.vida; i++) {
                float largura_coracao = (al_get_bitmap_width(sprite_coracao) * 0.1) * (tela.largura / 1920.0f);
                float altura_coracao = (al_get_bitmap_height(sprite_coracao) * 0.1) * (tela.altura / 1080.0f);
                float offset_x = (tela.largura * 0.05) + (largura_coracao * (i + 1));
                float offset_y = tela.altura * 0.05;
                al_draw_scaled_bitmap(
                    sprite_coracao,
                    0, 0,
                    al_get_bitmap_width(sprite_coracao),
                    al_get_bitmap_height(sprite_coracao),
                    offset_x, offset_y,
                    largura_coracao, altura_coracao, 0
                );
            }

            al_flip_display();

            redesenhar_tela = false;
        }
    }


    // Limpeza de eventos e personagens

    //Odisseu
    al_destroy_bitmap(odisseuParado);
    al_destroy_bitmap(odisseuAndando);
    al_destroy_bitmap(odisseuDesembainhar);
    al_destroy_bitmap(odisseuAtacando);
    al_destroy_bitmap(odisseuParadoEspada);
    al_destroy_bitmap(odisseuAndandoEspada);

    //Circe
    al_destroy_bitmap(circeparada);
    al_destroy_bitmap(circeDano);

    //Hermes
    al_destroy_bitmap(hermesParado);
    al_destroy_bitmap(hermesTiraElmo);

    //Penelope
	al_destroy_bitmap(Penelope);


    //Destruição dos personagens Inimigos Itaca
   //parados
    al_destroy_bitmap(inimigoParado1);
    al_destroy_bitmap(inimigoParado2);
    al_destroy_bitmap(inimigoParado3);
    al_destroy_bitmap(inimigoParado4);
    al_destroy_bitmap(inimigoParado5);
    al_destroy_bitmap(inimigoParado6);

    //tomando dano
    al_destroy_bitmap(inimigo1_sofre_dano);
    al_destroy_bitmap(inimigo2_sofre_dano);
    al_destroy_bitmap(inimigo3_sofre_dano);
    al_destroy_bitmap(inimigo4_sofre_dano);
    al_destroy_bitmap(inimigo5_sofre_dano);
    al_destroy_bitmap(inimigo6_sofre_dano);


    //Todo o resto
    destruir_cenarios(fase);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(tela_jogo);
    free(listaFlechas);

   

    return 0;
}
