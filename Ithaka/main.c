#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_video.h>
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
#include "Olimpo/olimpo.h"
#include "Circe/circe.h"
#include "Poseidon/poseidon.h"
#include "util.h"
#include "dialogo/dialogo.h"

typedef enum {
    TELA_MENU,
    TELA_MAPA,
    TELA_JOGO,
    TELA_SAIR
} TelaSistema;

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
    if (!al_init_video_addon()) return -1;
    al_install_audio(); 
    al_init_acodec_addon(); 
    al_reserve_samples(16);

    InformacoesTela tela = obter_resolucao_tela_atual();
    const int LARGURA_TELA = tela.largura;
    const float FORCA_DISPARO_MAX = 30.f;
    const int ALTURA_TELA = tela.altura;
    const int ALTURA_FLECHA = 20, LARGURA_FLECHA = 75;
    const float GRAVIDADE = 1.0f;
    const int ALTURA_SPRITE = 250, LARGURA_SPRITE = 250;
    int ALTURA_PERSONAGEM = deixarProporcional(ALTURA_SPRITE + 60, ALTURA_TELA, ALTURA_TELA_ORIGINAL);
    int LARGURA_PERSONAGEM = deixarProporcional(LARGURA_SPRITE + 25, LARGURA_TELA, LARGURA_TELA_ORIGINAL);
    int mapas_disponiveis = 2;
    int sleep_turno = 0;

    ALLEGRO_DISPLAY* tela_jogo = criar_tela_cheia(tela);
    if (!tela_jogo) {
        printf("Falha ao criar display.\n");
        return -1;
    }

    if (!menu_inicial(tela_jogo, false)) {
        printf("Jogo encerrado.\n");
        al_destroy_display(tela_jogo);
        return 0; // Saída normal, não é erro
    }

    // Exibir mapa 
    int escolha_mapa = exibir_mapa_inicial(tela_jogo, mapas_disponiveis);
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
    ALLEGRO_BITMAP* odisseuMirando = al_load_bitmap("./imagensJogo/personagens/Odisseu/odisseuMirando.png");
    ALLEGRO_BITMAP* odisseuPuxandoArco = al_load_bitmap("./imagensJogo/personagens/Odisseu/odisseuPuxaArco.png");

    //Penelope
	ALLEGRO_BITMAP* Penelope = al_load_bitmap("./imagensJogo/personagens/Penelope/penelope.png");

    //Circe
    ALLEGRO_BITMAP* sprites_circe[CIRCE_NUMERO_SPRITES];
    int contagem_frames_circe[CIRCE_NUMERO_SPRITES];
    contagem_frames_circe[CIRCE_SPRITE_PARADA] = 5;
    sprites_circe[CIRCE_SPRITE_PARADA] = al_load_bitmap("./imagensJogo/personagens/Circe/circeparada.png");
    contagem_frames_circe[CIRCE_SPRITE_DANO] = 7;
    sprites_circe[CIRCE_SPRITE_DANO] = al_load_bitmap("./imagensJogo/personagens/Circe/circeDano.png");
    contagem_frames_circe[CIRCE_SPRITE_CORVO_PARADO] = 8;
    sprites_circe[CIRCE_SPRITE_CORVO_PARADO] = al_load_bitmap("./imagensJogo/personagens/Circe/corvo.png");
    contagem_frames_circe[CIRCE_SPRITE_CIRCE_CORVO] = 8;
    sprites_circe[CIRCE_SPRITE_CIRCE_CORVO] = al_load_bitmap("./imagensJogo/personagens/Circe/circe_corvo.png");
    contagem_frames_circe[CIRCE_SPRITE_CORVO_CIRCE] = 8;
    sprites_circe[CIRCE_SPRITE_CORVO_CIRCE] = al_load_bitmap("./imagensJogo/personagens/Circe/corvo_circe.png");
    contagem_frames_circe[CIRCE_SPRITE_CORVO_ATACA] = 3;
    sprites_circe[CIRCE_SPRITE_CORVO_ATACA] = al_load_bitmap("./imagensJogo/personagens/Circe/corvo_ataca.png");
    contagem_frames_circe[CIRCE_SPRITE_BATALHA] = 5;
    sprites_circe[CIRCE_SPRITE_BATALHA] = al_load_bitmap("./imagensJogo/personagens/Circe/circe_batalha.png");
    contagem_frames_circe[CIRCE_SPRITE_BATALHA_HIT] = 7;
    sprites_circe[CIRCE_SPRITE_BATALHA_HIT] = al_load_bitmap("./imagensJogo/personagens/Circe/circe_batalha_hit.png");
    contagem_frames_circe[CIRCE_SPRITE_CIRCE_TIGRE] = 10;
    sprites_circe[CIRCE_SPRITE_CIRCE_TIGRE] = al_load_bitmap("./imagensJogo/personagens/Circe/circe_tigre.png");
    contagem_frames_circe[CIRCE_SPRITE_TIGRE_CIRCE] = 10;
    sprites_circe[CIRCE_SPRITE_TIGRE_CIRCE] = al_load_bitmap("./imagensJogo/personagens/Circe/tigre_circe.png");
    contagem_frames_circe[CIRCE_SPRITE_TIGRE_PARADO] = 6;
    sprites_circe[CIRCE_SPRITE_TIGRE_PARADO] = al_load_bitmap("./imagensJogo/personagens/Circe/tigre_parado.png");
    contagem_frames_circe[CIRCE_SPRITE_TIGRE_ANDANDO] = 6;
    sprites_circe[CIRCE_SPRITE_TIGRE_ANDANDO] = al_load_bitmap("./imagensJogo/personagens/Circe/tigre_andando.png");
    contagem_frames_circe[CIRCE_SPRITE_TIGRE_ATAQUE] = 5;
    sprites_circe[CIRCE_SPRITE_TIGRE_ATAQUE] = al_load_bitmap("./imagensJogo/personagens/Circe/tigre_ataque.png");


    //Porcos

    ALLEGRO_BITMAP* porcoParado1 = al_load_bitmap("./imagensJogo/cenarios/Circe/porco1.png");
    ALLEGRO_BITMAP* porcoParado2 = al_load_bitmap("./imagensJogo/cenarios/Circe/porco2.png");
    ALLEGRO_BITMAP* porcoParado3 = al_load_bitmap("./imagensJogo/cenarios/Circe/porco3.png");

    //Hermes
    ALLEGRO_BITMAP* hermesParado = al_load_bitmap("./imagensJogo/personagens/Hermes/hermesParado.png");
    ALLEGRO_BITMAP* hermesTiraElmo = al_load_bitmap("./imagensJogo/personagens/Hermes/tirandoElmo.png");



    //parados
    ALLEGRO_BITMAP* inimigoParado1 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo1/inimigo1parado.png");
    ALLEGRO_BITMAP* inimigoParado2 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo2/inimigo2parado.png");
    ALLEGRO_BITMAP* inimigoParado3 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo3/inimigo3parado.png");
    ALLEGRO_BITMAP* inimigoParado4 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo4/inimigo4parado.png");
    ALLEGRO_BITMAP* inimigoParado5 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo5/inimigo5parado.png");
    ALLEGRO_BITMAP* inimigoParado6 = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo6/inimigo6parado.png");


    //tomando dano
    ALLEGRO_BITMAP* inimigo1_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo1/inimigo1dano.png");
    ALLEGRO_BITMAP* inimigo2_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo2/inimigo2dano.png");
    ALLEGRO_BITMAP* inimigo3_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo3/inimigo3dano.png");
    ALLEGRO_BITMAP* inimigo4_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo4/inimigo4dano.png");
    ALLEGRO_BITMAP* inimigo5_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo5/inimigo5dano.png");
    ALLEGRO_BITMAP* inimigo6_sofre_dano = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo6/inimigo6dano.png");

    //Batendo
    ALLEGRO_BITMAP* inimigo1_atacando = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo1/inimigo1golpe.png");
    ALLEGRO_BITMAP* inimigo2_atacando = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo2/inimigo2golpe.png");
    ALLEGRO_BITMAP* inimigo3_atacando = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo3/inimigo3golpe.png");
    ALLEGRO_BITMAP* inimigo4_atacando = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo4/inimigo4golpe.png");
    ALLEGRO_BITMAP* inimigo5_atacando = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo5/inimigo5golpe.png");
    ALLEGRO_BITMAP* inimigo6_atacando = al_load_bitmap("./imagensJogo/personagens/pretendentes/inimigo6/inimigo6golpe.png");



    //Objetos
    ALLEGRO_BITMAP* sprite_flecha = al_load_bitmap("./imagensJogo/objetos/flecha.png");
    ALLEGRO_BITMAP* sprite_coracao = al_load_bitmap("./imagensJogo/objetos/coracao.png");


    //Carregando fontes 
    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_FONT* fonte_quiz = al_load_ttf_font("./fontes/arial.ttf", 24, 0);
    ALLEGRO_FONT* font_titulo = al_load_ttf_font("./fontes/arial.ttf", 48, 0);

  



    if (!odisseuParado || !odisseuAndando || !odisseuDesembainhar ||
        !odisseuAtacando || !odisseuParadoEspada || !odisseuAndandoEspada ||
        !sprites_circe[CIRCE_SPRITE_PARADA] || !sprites_circe[CIRCE_SPRITE_DANO] || !hermesParado || !hermesTiraElmo || !sprite_flecha ||
        !sprite_coracao || !inimigoParado1 || !inimigoParado2 || !inimigoParado3 || !inimigoParado4 || !inimigoParado5 || !inimigoParado6 || 
        !inimigo1_sofre_dano || !inimigo2_sofre_dano || !inimigo3_sofre_dano || !inimigo4_sofre_dano || !inimigo5_sofre_dano || !inimigo6_sofre_dano 
        || !inimigo1_atacando || !inimigo2_atacando || !inimigo3_atacando || !inimigo4_atacando || !inimigo5_atacando || !inimigo6_atacando) {
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
    int altura_frame_desembainhar = al_get_bitmap_height(odisseuDesembainhar);

    const int VELOCIDADE_ANIMACAO_DESEMBAINHAR = 6;

    int total_frames_Odisseu_atacando = 6;
    int largura_frame_Odisseu_atacando = al_get_bitmap_width(odisseuAtacando) / total_frames_Odisseu_atacando;
    int altura_frame_Odisseu_atacando = al_get_bitmap_height(odisseuAtacando);

    int total_frames_Odisseu_mirando = 3;
    int largura_frame_Odisseu_mirando = al_get_bitmap_width(odisseuMirando) / total_frames_Odisseu_mirando;
    int altura_frame_Odisseu_mirando = al_get_bitmap_height(odisseuMirando);

    int total_frames_odisseu_puxa_arco = 7;
    int largura_frame_Odisseu_puxa_arco = al_get_bitmap_width(odisseuPuxandoArco) / total_frames_odisseu_puxa_arco;
    int altura_frame_Odisseu_puxa_arco = al_get_bitmap_height(odisseuPuxandoArco);


    //Penelope configuração
    int total_frames_penelope_parada = 5;
    int largura_frame_penelope_parada = al_get_bitmap_width(Penelope) / total_frames_penelope_parada;
    int altura_frame_penelope_parada = al_get_bitmap_height(Penelope);

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
    int total_frames_inimigos_atacando = 6;
    //=============inimigo 1================
	//parado
    int largura_frame_inimigo1_parado = al_get_bitmap_width(inimigoParado1) / total_frames_inimigos_parados;
    int altura_frame_inimigo1_parado = al_get_bitmap_height(inimigoParado1);


	//tomando dano
    int largura_frame_inimigo1_dano = al_get_bitmap_width(inimigo1_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo1_dano = al_get_bitmap_height(inimigo1_sofre_dano);

	//batendo
    int total_frames_inimigo1_atacando = 6;
    int largura_frame_inimigo1_atacando = al_get_bitmap_width(inimigo1_atacando) / total_frames_inimigo1_atacando;
    int altura_frame_inimigo1_atacando = al_get_bitmap_height(inimigo1_atacando);


    //==============inimigo 2==============
	//parado
    int largura_frame_inimigo2_parado = al_get_bitmap_width(inimigoParado2) / total_frames_inimigos_parados;
    int altura_frame_inimigo2_parado = al_get_bitmap_height(inimigoParado2);

	//tomando dano
    int largura_frame_inimigo2_dano = al_get_bitmap_width(inimigo2_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo2_dano = al_get_bitmap_height(inimigo2_sofre_dano);

    //batendo
    int total_frames_inimigo2_atacando = 6;
    int largura_frame_inimigo2_atacando = al_get_bitmap_width(inimigo2_atacando) / total_frames_inimigo2_atacando;
    int altura_frame_inimigo2_atacando = al_get_bitmap_height(inimigo2_atacando);
   
    //==============inimigo 3==============
	//parado
    int largura_frame_inimigo3_parado = al_get_bitmap_width(inimigoParado3) / total_frames_inimigos_parados;
    int altura_frame_inimigo3_parado = al_get_bitmap_height(inimigoParado3);

	//tomando dano
    int largura_frame_inimigo3_dano = al_get_bitmap_width(inimigo3_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo3_dano = al_get_bitmap_height(inimigo3_sofre_dano);

    //batendo
    int total_frames_inimigo3_atacando = 6;
    int largura_frame_inimigo3_atacando = al_get_bitmap_width(inimigo3_atacando) / total_frames_inimigo3_atacando;
    int altura_frame_inimigo3_atacando = al_get_bitmap_height(inimigo3_atacando);


    //==============inimigo 4==============
    //parado
    int largura_frame_inimigo4_parado = al_get_bitmap_width(inimigoParado4) / total_frames_inimigos_parados;
    int altura_frame_inimigo4_parado = al_get_bitmap_height(inimigoParado4);

    //tomando dano
    int largura_frame_inimigo4_dano = al_get_bitmap_width(inimigo4_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo4_dano = al_get_bitmap_height(inimigo4_sofre_dano);

    //batendo
    int total_frames_inimigo4_atacando = 6;
    int largura_frame_inimigo4_atacando = al_get_bitmap_width(inimigo4_atacando) / total_frames_inimigo4_atacando;
    int altura_frame_inimigo4_atacando = al_get_bitmap_height(inimigo4_atacando);

    //==============inimigo 5==============
    //parado
    int largura_frame_inimigo5_parado = al_get_bitmap_width(inimigoParado5) / total_frames_inimigos_parados;
    int altura_frame_inimigo5_parado = al_get_bitmap_height(inimigoParado5);

    //tomando dano
    int largura_frame_inimigo5_dano = al_get_bitmap_width(inimigo5_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo5_dano = al_get_bitmap_height(inimigo5_sofre_dano);
    
    //batendo
    int total_frames_inimigo5_atacando = 6;
    int largura_frame_inimigo5_atacando = al_get_bitmap_width(inimigo5_atacando) / total_frames_inimigo5_atacando;
    int altura_frame_inimigo5_atacando = al_get_bitmap_height(inimigo5_atacando);

    //==============inimigo 6==============
    //parado
    int largura_frame_inimigo6_parado = al_get_bitmap_width(inimigoParado6) / total_frames_inimigos_parados;
    int altura_frame_inimigo6_parado = al_get_bitmap_height(inimigoParado6);

    //tomando dano
    int largura_frame_inimigo6_dano = al_get_bitmap_width(inimigo6_sofre_dano) / total_frames_inimigos_dano;
    int altura_frame_inimigo6_dano = al_get_bitmap_height(inimigo6_sofre_dano);

    //batendo
    int total_frames_inimigo6_atacando = 6;
    int largura_frame_inimigo6_atacando = al_get_bitmap_width(inimigo6_atacando) / total_frames_inimigo6_atacando;
    int altura_frame_inimigo6_atacando = al_get_bitmap_height(inimigo6_atacando);


    //config porco
    int  total_frames_porcos_parados = 4;
    int largura_frame_Porco1_parado = al_get_bitmap_width(porcoParado1) / total_frames_porcos_parados;
    int altura_frame_Porco1_parado = al_get_bitmap_height(porcoParado1);

    //porco 2
    int largura_frame_Porco2_parado = al_get_bitmap_width(porcoParado2) / total_frames_porcos_parados;
    int altura_frame_Porco2_parado = al_get_bitmap_height(porcoParado2);
    //porco 3
    int largura_frame_Porco3_parado = al_get_bitmap_width(porcoParado3) / total_frames_porcos_parados;
    int altura_frame_Porco3_parado = al_get_bitmap_height(porcoParado3);

    //===========================INICIALIZAÇÃO DOS PERSONAGENS===========================

    Personagem odisseu = {
        .x = (escolha_mapa == MAPA_FASE_CIRCE) ? (LARGURA_TELA / 4.2) : (LARGURA_TELA / 30),
        .y = (escolha_mapa == MAPA_FASE_ITACA) ? deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL)
        : deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
        .vida = 5,
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
        .forca_disparo = 0.0f,
        .cooldown_dano = 0,
        .tem_arco = false,
        .puxando_arco = false,
        .guardando_arco = false
    };

    const int y_chao = odisseu.y + ALTURA_PERSONAGEM;


    Personagem circe = {
        .x = LARGURA_TELA - (LARGURA_TELA / 3),
        .y = deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
        .vida = 8,
        .largura = LARGURA_PERSONAGEM,
        .altura = ALTURA_PERSONAGEM,
        .olhando_direita = false,
        .olhando_esquerda = true,
        .andando = false,
        .desembainhando = false,
        .sofrendo_dano = false,
        .guardando_espada = false,
        .atacando = false,
        .tem_espada = false,
        .frame_atual = 0,
        .contador_animacao = 0,
        .estado = 0,
        .estado_progresso = 0,
        .angulo = 0,
        .sprite_ativo = CIRCE_SPRITE_BATALHA
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
        .y = deixarProporcional(730, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
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
    .atacando = false,
    .acerto = false,
    .cooldown_ataque = 0,
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
    .atacando = false,
    .acerto = false,
    .cooldown_ataque = 0,
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
    .atacando = false,
    .acerto = false,
    .cooldown_ataque = 0,
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
    .atacando = false,
    .acerto = false,
    .cooldown_ataque = 0,
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
    .atacando = false,
    .acerto = false,
    .cooldown_ataque = 0,
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
    .atacando = false,
    .acerto = false,
    .cooldown_ataque = 0,
    .frame_atual = 0,
    .contador_animacao = 0,
    .frame_contador = 0,
    .velocidade_animacao = 10,
    .num_frames = 4
    };

    Personagem porco1 = {
   .x = LARGURA_TELA / 1.45,
   .y = deixarProporcional(820, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
   .vida = 3,
   .largura = deixarProporcional(LARGURA_SPRITE + 15, LARGURA_TELA, LARGURA_TELA_ORIGINAL),
   .altura = deixarProporcional(ALTURA_SPRITE + 5, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
   .olhando_direita = true,
   .olhando_esquerda = false,
   .sofrendo_dano = false,
   .atacando = false,
   .acerto = false,
   .cooldown_ataque = 0,
   .frame_atual = 0,
   .contador_animacao = 0,
   .frame_contador = 0,
   .velocidade_animacao = 10,
   .num_frames = 4
    };

    Personagem porco2 = {
   .x = LARGURA_TELA / 1.9,
   .y = deixarProporcional(820, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
   .vida = 3,
   .largura = deixarProporcional(LARGURA_SPRITE + 15, LARGURA_TELA, LARGURA_TELA_ORIGINAL),
   .altura = deixarProporcional(ALTURA_SPRITE + 5, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
   .olhando_direita = true,
   .olhando_esquerda = false,
   .frame_atual = 0,
   .contador_animacao = 0,
   .frame_contador = 0,
   .velocidade_animacao = 10,
   .num_frames = 4
     };

     Personagem porco3 = {
   .x = LARGURA_TELA / 3,
   .y = deixarProporcional(820, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
   .vida = 3,
    .largura = deixarProporcional(LARGURA_SPRITE + 15, LARGURA_TELA, LARGURA_TELA_ORIGINAL),
   .altura = deixarProporcional(ALTURA_SPRITE + 5, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
   .olhando_direita = true,
   .olhando_esquerda = false,
   .sofrendo_dano = false,
   .atacando = false,
   .acerto = false,
   .cooldown_ataque = 0,
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

    // ---------------------------------------- INICIALIZAR QUIZ --------------------------------------------

    pergunta perguntas[12];
    fazendo_as_perguntas(perguntas);

    //Variáveis declaradas em olimpo.h

    quiz estado = {
    .perguntaAtual = 0,
    .numPerguntas = 12,
    .respostaSelecionada = -1,
    .erros = 0,
    .respondida = false,
    .perdeu = false,
    .timer_feedback = 0
    };


    //-----------------------------------------------------------------------------------------------------------------


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
    int circe_stall = 120;
    Flecha* listaFlechas = (Flecha*)malloc(sizeof(*listaFlechas));
    int count_flechas = 0, capacidade_flechas = 0;
    if (listaFlechas == NULL) {
        printf(stderr, "alocação de memória para as flechas falhou!\n");
        return -1;
    }
    //config inicial para fase do poseidon
    int tabuleiro_jogador[10][10] = { 0 };
    int tabuleiro_com[10][10] = { 0 };

    bool setup = true;
    bool turno_jogador = true;

    bool vertical = false;

    bool player_ganha = false;
    bool com_ganha = false;

    Estado_Computador com = { 0 };

    Navio navios[5] = {
        {5, false},
        {4, false},
        {3, false},
        {3, false},
        {2, false}
    };
    int contagem_navios = 5;

    com_config_inicial(tabuleiro_com, navios, contagem_navios);

    for (int i = 0; i < contagem_navios; i++) navios[i].placed = false;

    while (jogo_rodando) {
        ALLEGRO_EVENT evento;
        ALLEGRO_KEYBOARD_STATE estado_teclado;
        ALLEGRO_MOUSE_STATE estado_mouse;
        bool mouse_left = false;
        bool mouse_right = false;
        al_wait_for_event(fila_eventos, &evento);
        if (escolha_mapa == MAPA_VOLTAR_MENU) {
            destruir_cenarios(fase);
            destruir_sprites(fase);
            free_fase(fase);
            escolha_mapa = exibir_mapa_inicial(tela_jogo, mapas_disponiveis);
            if (escolha_mapa != MAPA_SAIR) {
                if (!carregar_cenario(fase, escolha_mapa)) {
                    printf("Erro ao carregar cenários.\n");
                    al_destroy_display(tela_jogo);
                    return -1;
                }
            }
        }
        else if (escolha_mapa == MAPA_SAIR) {
            jogo_rodando = menu_inicial(tela_jogo, false);
            escolha_mapa = MAPA_VOLTAR_MENU;
        }

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jogo_rodando = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                // Verificar em qual "tela" estamos

                // Se ainda não carregou o mapa (teoricamente impossível chegar aqui)
                // Se está jogando alguma fase
                if (escolha_mapa > 0) {
                    printf("[JOGO] ESC pressionado - Voltando ao mapa...\n");
                    destruir_cenarios(fase);
                    destruir_sprites(fase);
                    free_fase(fase);

                    escolha_mapa = exibir_mapa_inicial(tela_jogo, mapas_disponiveis);
                    if (escolha_mapa != MAPA_SAIR) {
                        if (!carregar_cenario(fase, escolha_mapa)) {
                            printf("Erro ao carregar cenários.\n");
                            al_destroy_display(tela_jogo);
                            return -1;

                        }
                    }
                    else {
                        jogo_rodando = menu_inicial(tela_jogo, false);
                        escolha_mapa = MAPA_VOLTAR_MENU;
                    }
                    // Flag para indicar que deve voltar ao mapa
                } else {
                    jogo_rodando = menu_inicial(tela_jogo, false);
                    escolha_mapa = MAPA_VOLTAR_MENU;
                }
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_E && !odisseu.andando &&
                !odisseu.desembainhando && !odisseu.atacando && !odisseu.tem_espada && !odisseu.tem_arco) {
                odisseu.desembainhando = true;
                odisseu.frame_atual = 0;
                odisseu.contador_animacao = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_E && !odisseu.andando &&
                !odisseu.desembainhando && !odisseu.atacando && odisseu.tem_espada && !odisseu.tem_arco) {
                odisseu.guardando_espada = true;
                odisseu.frame_atual = total_frames_desembainhar - 1;
                odisseu.contador_animacao = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_F && !odisseu.andando && !odisseu.tem_espada
                && !odisseu.desembainhando && !odisseu.disparando && !odisseu.tem_arco) {
                odisseu.puxando_arco = true;
                odisseu.frame_atual = 0;
                odisseu.contador_animacao = 0;
            }
            if (evento.keyboard.keycode == ALLEGRO_KEY_F && !odisseu.andando && !odisseu.tem_espada
                && !odisseu.desembainhando && !odisseu.disparando && odisseu.tem_arco) {
                odisseu.guardando_arco = true;
                odisseu.frame_atual = total_frames_odisseu_puxa_arco - 1;
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

            //VERIFICA CLICK DO MOUSE DO QUIZ
      
            if (escolha_mapa == MAPA_FASE_CALYPSO && !estado.respondida) {
                int opcao_clicada = verificar_clique_opcao(
                    evento.mouse.x,
                    evento.mouse.y,
                    LARGURA_TELA,
                    ALTURA_TELA
                );

                if (opcao_clicada != -1) {
                    printf("[QUIZ] Opção %d clicada!\n", opcao_clicada + 1);
                    processa_resposta(perguntas, &estado, opcao_clicada);
                }

            }

        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            mouse_left = evento.mouse.button == 1;
            mouse_right = evento.mouse.button == 2;

            if (escolha_mapa == MAPA_FASE_POSEIDON && turno_jogador) {
                al_get_mouse_state(&estado_mouse);
                update_battleship(
                    estado_mouse,
                    mouse_left,
                    mouse_right,
                    tabuleiro_jogador,
                    tabuleiro_com,
                    &turno_jogador,
                    &setup,
                    navios,
                    contagem_navios,
                    &vertical,
                    &com,
                    &player_ganha,
                    &com_ganha,
                    tela
                );
                sleep_turno = 60;
            }
        }
        
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            al_get_keyboard_state(&estado_teclado);
            al_get_mouse_state(&estado_mouse);
            float odisseu_direcao_x = 0.0f;

            //ATUALIZAÇÃO DE TIMER DO QUIZ-----

            if (escolha_mapa == MAPA_FASE_CALYPSO && estado.respondida) {
                atualizar_timer_quiz(&estado);

                if (estado.timer_feedback == 0) {
                    proximaPergunta(&estado);
                }
            }

            if (fase->cenarios != NULL) {
                Cenario cenario = fase->cenarios[fase->cenario_atual];
                if (cenario.dialogo_caminho != NULL && !cenario.dialogo_completo) {
                    ArrayTextBox textos = { .tamanho = 0 };
                    carregar_dialogo(cenario.dialogo_caminho, &textos);
                    desenhar_dialogo(tela_jogo, cenario, textos);
                    fase->cenarios[fase->cenario_atual].dialogo_completo = true;
                }
                if (escolha_mapa == MAPA_FASE_SUBMUNDO || escolha_mapa == MAPA_FASE_ITACA) {
                    escolha_mapa = MAPA_VOLTAR_MENU;
                    mapas_disponiveis++;
                }
            }
            //-----------------------------------

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
            if (!odisseu.tem_espada && odisseu.tem_arco && al_mouse_button_down(&estado_mouse, ALLEGRO_MOUSE_BUTTON_LEFT)) {
                if (!odisseu.disparando) {
                    odisseu.frame_atual = 0;
                }
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
                    f.vx = odisseu.forca_disparo * cosf(f.angulo);
                    if (estado_mouse.x < f.x) {
                        f.vx = -f.vx;
                    }
                    f.vy = odisseu.forca_disparo * sinf(f.angulo);
                    adicionarFlecha(&listaFlechas, &count_flechas, &capacidade_flechas, f);
                }
                odisseu.disparando = false;
                odisseu.forca_disparo = 0.0f;
            }

            //-------------------------------------------EVENTOS QUIZ (Mudar/Estudar, claude) -------------------------------------------

            


            const float velocidade_odisseu = 15.0f;
            odisseu.x += odisseu_direcao_x * velocidade_odisseu;
            odisseu.y = limitar_valor(odisseu.y, 0, ALTURA_TELA - odisseu.altura);
            odisseu.andando = (odisseu_direcao_x != 0.0f);


			// ===============ATUALIZAÇÃO DE CENÁRIOS====================
            atualizar_transicao_cenario(fase, &odisseu.x, odisseu.largura, LARGURA_TELA);


            //================BARREIRA DOS MAPAS=========================
			//barreira inicial de todos os mapas
            if (fase->cenario_atual == 0 && odisseu.x < LARGURA_TELA / -24) odisseu.x = LARGURA_TELA / -24;

            //BARREIRA MAPA 5 - SUBMUNDO
            if (fase->cenario_atual == 0 && escolha_mapa == MAPA_FASE_SUBMUNDO && odisseu.x > LARGURA_TELA / 3.5) odisseu.x = LARGURA_TELA / 3.5;

           

            // Atualizar animação do Odisseu
            int estado_animacao = 0;
            if (odisseu.desembainhando) estado_animacao = 1;
            else if (odisseu.guardando_espada) estado_animacao = 2;
            else if (odisseu.atacando) estado_animacao = 3;
            else if (odisseu.disparando) estado_animacao = 4;
            else if (odisseu.puxando_arco) estado_animacao = 5;
            else if (odisseu.guardando_arco) estado_animacao = 6;

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
            case 4:
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= 10) {
                    odisseu.frame_atual++;
                    if (odisseu.frame_atual >= total_frames_Odisseu_mirando) {
                        odisseu.frame_atual = 0;
                    }
                    odisseu.contador_animacao = 0;
                }
                break;
               
            case 5: // puxa arco
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= VELOCIDADE_ANIMACAO_DESEMBAINHAR) {
                    odisseu.frame_atual++;
                    if (odisseu.frame_atual >= total_frames_odisseu_puxa_arco) {
                        odisseu.puxando_arco = false;
                        odisseu.frame_atual = 0;
                        odisseu.tem_arco = true;
                    }
                    odisseu.contador_animacao = 0;
                }
                break;
            case 6:
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= VELOCIDADE_ANIMACAO_DESEMBAINHAR) {
                    odisseu.frame_atual--;
                    if (odisseu.frame_atual < 0) {
                        odisseu.guardando_arco = false;
                        odisseu.frame_atual = 0;
                        odisseu.tem_arco = false;
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
                        if (odisseu.frame_atual >= (odisseu.tem_espada ? total_frames_Odisseu_andando_espada : total_frames_Odisseu_andando)) {
                            odisseu.frame_atual = 0;
                        }
                    else
                        if (odisseu.frame_atual >= (odisseu.tem_espada ? total_frames_Odisseu_parado_espada : total_frames_Odisseu_parado)) {
                            odisseu.frame_atual = 0;
                        }                   
                    odisseu.contador_animacao = 0;
                }
                break;
            }
            // Decrementar frames de imunidade
            if (odisseu.cooldown_dano > 0) {
                odisseu.cooldown_dano--;
            }

            // Atualizar animação de dano do Odisseu
            if (odisseu.sofrendo_dano) {
                odisseu.contador_animacao++;
                if (odisseu.contador_animacao >= 5) {
                    odisseu.frame_atual++;
                    // Assumindo que você não tem sprite de dano, apenas resetamos após alguns frames
                    if (odisseu.frame_atual >= 6) {
                        odisseu.sofrendo_dano = false;
                        odisseu.frame_atual = 0;
                    }
                    odisseu.contador_animacao = 0;
                }
            }
            //================================MISSÃO CIRCE=============================================
			
            if (escolha_mapa == MAPA_FASE_CIRCE) {
               
           //barreiras para não serem ultrapassadas no mapa 2
           
                if (fase->cenario_atual == 2) {

                    // Direção dos porquinho
                    porco1.olhando_direita = (odisseu.x < porco1.x);
                    porco2.olhando_direita = (odisseu.x < porco2.x);
                    porco3.olhando_direita = (odisseu.x < porco3.x);

                    if (porco1.vida > 0 && !porco1.sofrendo_dano) {
                        porco1.contador_animacao++;
                        if (porco1.contador_animacao >= porco1.velocidade_animacao) {
                            porco1.frame_atual = (porco1.frame_atual + 1) % porco1.num_frames;
                            porco1.contador_animacao = 0;
                        }
                    }

                    if (porco2.vida > 0 && !porco2.sofrendo_dano) {
                        porco2.contador_animacao++;
                        if (porco2.contador_animacao >= porco2.velocidade_animacao) {
                            porco2.frame_atual = (porco2.frame_atual + 1) % porco2.num_frames;
                            porco2.contador_animacao = 0;
                        }

                    }
                    if (porco3.vida > 0 && !porco3.sofrendo_dano) {
                        porco3.contador_animacao++;
                        if (porco3.contador_animacao >= porco3.velocidade_animacao) {
                            porco3.frame_atual = (porco3.frame_atual + 1) % porco3.num_frames;
                            porco3.contador_animacao = 0;
                        }
                    }
                }

           //cenário 0
                if (fase->cenario_atual == 0 && odisseu.x < (LARGURA_TELA / 4.2)) odisseu.x = (LARGURA_TELA / 4.2);
           //cenario 3
                if (fase->cenario_atual == 3) {
                    if (circe.vida > 0 && odisseu.x < LARGURA_TELA / -24) odisseu.x = LARGURA_TELA / -24;
                    float old_x = circe.x;
                    if (circe_stall == 0) {
                        processar_acao_circe(&odisseu, &circe, &circe_stall, tela);
                        atualizar_circe(&circe,odisseu,tela, &circe_stall);
                    }
                    if (circe_stall > 0) {
                        circe_stall--;
                    }
                    //aqui
                    //=============CONFIGURAÇÃO DA CIRCE==================
                    if (circe.vida > 0) {
                        circe.contador_animacao++;
                        int delay_animacao_circe = circe.sofrendo_dano ? 5 : 10;
                        if (circe.contador_animacao >= delay_animacao_circe) {
                            if (circe.sofrendo_dano) {
                                circe.sprite_ativo = CIRCE_SPRITE_BATALHA_HIT;
                                circe.frame_atual++;
                                if (circe.frame_atual >= contagem_frames_circe[CIRCE_SPRITE_BATALHA_HIT]) {
                                    circe.sprite_ativo = CIRCE_SPRITE_BATALHA;
                                    circe.sofrendo_dano = false;
                                    circe.frame_atual = 0;
                                }
                            }
                            else if (circe.estado == 1 || (circe.estado == 4 && circe.sprite_ativo == CIRCE_SPRITE_CIRCE_TIGRE)|| circe.estado == 7) {
                                circe.frame_atual++;

                                if (circe.estado == 4 && circe.frame_atual >= 4) {
                                    atualizar_tamanho_circe(&circe, tela, true);
                                }
                                else if (circe.estado == 7 && circe.frame_atual >= 6) {
                                    atualizar_tamanho_circe(&circe, tela, false);
                                }

                                if (circe.frame_atual >= contagem_frames_circe[circe.sprite_ativo]) {
                                    circe.frame_atual = contagem_frames_circe[circe.sprite_ativo];
                                }
                            }
                            else if (circe.sprite_ativo == CIRCE_SPRITE_TIGRE_ANDANDO) {
                                float old_dx = fabsf(odisseu.x - old_x);
                                float new_dx = fabsf(odisseu.x - circe.x);
                                
                                if (old_dx < new_dx) {
                                    circe.frame_atual--;
                                    if (circe.frame_atual < 0) {
                                        circe.frame_atual = contagem_frames_circe[circe.sprite_ativo];
                                    }
                                }else{
                                    circe.frame_atual = (circe.frame_atual + 1) % contagem_frames_circe[circe.sprite_ativo];
                                }
                            }
                            else if (circe.atacando && circe.estado == 4) {
                                circe.frame_atual++;
                                if (circe.frame_atual >= contagem_frames_circe[CIRCE_SPRITE_TIGRE_ATAQUE]) {
                                    circe.sprite_ativo = CIRCE_SPRITE_TIGRE_ANDANDO;
                                    circe.atacando = false;
                                    circe.frame_atual = 0;
                                }
                            }
                            else {
                                circe.frame_atual = (circe.frame_atual + 1) % contagem_frames_circe[CIRCE_SPRITE_BATALHA];
                            }
                            
                            circe.contador_animacao = 0;
                        }
                    }
                    else {
                        Cenario cenario = fase->cenarios[fase->cenario_atual];
                        ArrayTextBox textos = { .tamanho = 0 };
                        carregar_dialogo("./dialogo/source/dialogoCirceOdisseuPosLutaFundoCirce4.txt", &textos);
                        desenhar_dialogo(tela_jogo, cenario, textos);
                        escolha_mapa = MAPA_VOLTAR_MENU;
                        mapas_disponiveis++;
                    }
                }

				//=============CONFIGURAÇÃO DOS ESTADOS DE HERMES==================

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

            //================================== MISSÃO CALYPSO ============================================
            if (escolha_mapa == MAPA_FASE_CALYPSO && estado.perguntaAtual < estado.numPerguntas && !estado.perdeu) {

                // Detectar cliques do mouse
                if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && !estado.respondida) {
                    int mx = evento.mouse.x;
                    int my = evento.mouse.y;

                    // Usar função melhorada de detecção de cliques
                    int opcao_clicada = verificar_clique_opcao(mx, my, LARGURA_TELA, ALTURA_TELA);

                    if (opcao_clicada != -1) {
                        processa_resposta(perguntas, &estado, opcao_clicada);
                    }
                }

                // Avançar para próxima pergunta automaticamente após timer
                if (estado.respondida) {
                    atualizar_timer_quiz(&estado);

                    // Se o timer zerou, avança automaticamente
                    if (estado.timer_feedback == 0) {
                        proximaPergunta(&estado);
                    }
                }
            }

            //===================================== MISSÃO ÍTACA =====================================//
            else if (escolha_mapa == MAPA_FASE_ITACA) {

                //========================== RESTRIÇÕES DE MOVIMENTO ==========================//
                
				//==========================CENARIO 0==========================//
                if (fase->cenario_atual == 0) {
                    bool inimigos_vivos = (inimigo1.vida > 0 || inimigo2.vida > 0 || inimigo3.vida > 0);
                    float limite = LARGURA_TELA - LARGURA_TELA / 9;

                    if (inimigos_vivos && odisseu.x > limite)
                        odisseu.x = limite;
                    //========================== CONFIGURAÇÃO DA ESCADA ==========================//
                   
                    if (odisseu.x < LARGURA_TELA / 1.38) odisseu.y = 550;
                    if (odisseu.x > LARGURA_TELA / 1.32) odisseu.y = 520;
                    if (odisseu.x > LARGURA_TELA / 1.25) odisseu.y = 490;
                    if (odisseu.x > LARGURA_TELA / 1.22) odisseu.y = 460;
                    if (odisseu.x > LARGURA_TELA / 1.19) odisseu.y = 430;
                    if (odisseu.x > LARGURA_TELA / 1.15) odisseu.y = 380;

                }
                if (fase->cenario_atual == 1) {
                    bool inimigos_vivos = (inimigo4.vida > 0 || inimigo5.vida > 0 || inimigo6.vida > 0);
                    float limite = LARGURA_TELA - LARGURA_TELA / 9;

					odisseu.y = 550;
                    if (inimigos_vivos && odisseu.x > limite)
                        odisseu.x = limite;
                        
                }
                if (fase->cenario_atual == 2) {
                    float limite = LARGURA_TELA - LARGURA_TELA / 9;
                    if (odisseu.x > limite)
                    odisseu.x =limite;
                        odisseu.y = 650;
                }
                //===================== ATUALIZAÇÃO DE PERSONAGENS =====================//



                //------------------ CENÁRIO 0 ------------------//
                if (fase->cenario_atual == 0) {

                    //========== INIMIGO 1 ==========
                    // Direção
                    inimigo1.olhando_direita = (odisseu.x < inimigo1.x);

                    // Cooldown
                    if (inimigo1.cooldown_ataque > 0)
                        inimigo1.cooldown_ataque--;

                    // Ataque
                    if (!inimigo1.atacando && inimigo1.vida > 0 && !inimigo1.sofrendo_dano &&
                        inimigo1.cooldown_ataque == 0 && abs(odisseu.x - inimigo1.x) < 180) {
                        inimigo1.atacando = true;
                        inimigo1.frame_atual = 0;
                        inimigo1.contador_animacao = 0;
                    }

                    // Animação de ataque
                    if (inimigo1.atacando && inimigo1.vida > 0) {
                        inimigo1.contador_animacao++;
                        if (inimigo1.contador_animacao >= 10) {
                            inimigo1.frame_atual++;

                            if (inimigo1.frame_atual == 3)
                                inimigo1.acerto = true; // Frame de acerto

                            if (inimigo1.frame_atual >= total_frames_inimigos_atacando) {
                                inimigo1.atacando = false;
                                inimigo1.frame_atual = 0;
                                inimigo1.cooldown_ataque = 150; // 1s de cooldown (60 fps)
                            }
                            inimigo1.contador_animacao = 0;
                        }
                    }

                    // Parado
                    else if (inimigo1.vida > 0 && !inimigo1.sofrendo_dano) {
                        inimigo1.contador_animacao++;
                        if (inimigo1.contador_animacao >= inimigo1.velocidade_animacao) {
                            inimigo1.frame_atual = (inimigo1.frame_atual + 1) % inimigo1.num_frames;
                            inimigo1.contador_animacao = 0;
                        }
                    }

                    // Sofrendo dano
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

                    //========== INIMIGO 2 ==========
                   
                    // Direção
                    inimigo2.olhando_direita = (odisseu.x < inimigo2.x);

                    // Cooldown
                    if (inimigo2.cooldown_ataque > 0)
                        inimigo2.cooldown_ataque--;

                    // Ataque
                    if (!inimigo2.atacando && inimigo2.vida > 0 && !inimigo2.sofrendo_dano &&
                        inimigo2.cooldown_ataque == 0 && abs(odisseu.x - inimigo2.x) < 180) {
                        inimigo2.atacando = true;
                        inimigo2.frame_atual = 0;
                        inimigo2.contador_animacao = 0;
                    }

                    // Animação de ataque
                    if (inimigo2.atacando && inimigo2.vida > 0) {
                        inimigo2.contador_animacao++;
                        if (inimigo2.contador_animacao >= 10) {
                            inimigo2.frame_atual++;

                            if (inimigo2.frame_atual == 3)
                                inimigo2.acerto = true; // Frame de acerto

                            if (inimigo2.frame_atual >= total_frames_inimigos_atacando) {
                                inimigo2.atacando = false;
                                inimigo2.frame_atual = 0;
                                inimigo2.cooldown_ataque = 150; // 1s de cooldown (60 fps)
                            }
                            inimigo2.contador_animacao = 0;
                        }
                    }

                    // Parado
                    else if (inimigo2.vida > 0 && !inimigo2.sofrendo_dano) {
                        inimigo2.contador_animacao++;
                        if (inimigo2.contador_animacao >= inimigo2.velocidade_animacao) {
                            inimigo2.frame_atual = (inimigo2.frame_atual + 1) % inimigo2.num_frames;
                            inimigo2.contador_animacao = 0;
                        }
                    }

                    // Sofrendo dano
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

                    //========== INIMIGO 3 ==========
                    // Direção
                    inimigo3.olhando_direita = (odisseu.x < inimigo3.x);

                    // Cooldown
                    if (inimigo3.cooldown_ataque > 0)
                        inimigo3.cooldown_ataque--;

                    // Ataque
                    if (!inimigo3.atacando && inimigo3.vida > 0 && !inimigo3.sofrendo_dano &&
                        inimigo3.cooldown_ataque == 0 && abs(odisseu.x - inimigo3.x) < 180) {
                        inimigo3.atacando = true;
                        inimigo3.frame_atual = 0;
                        inimigo3.contador_animacao = 0;
                    }

                    // Animação de ataque
                    if (inimigo3.atacando && inimigo3.vida > 0) {
                        inimigo3.contador_animacao++;
                        if (inimigo3.contador_animacao >= 10) {
                            inimigo3.frame_atual++;

                            if (inimigo3.frame_atual == 3)
                                inimigo3.acerto = true; // Frame de acerto

                            if (inimigo3.frame_atual >= total_frames_inimigos_atacando) {
                                inimigo3.atacando = false;
                                inimigo3.frame_atual = 0;
                                inimigo3.cooldown_ataque = 150; // 1s de cooldown (60 fps)
                            }
                            inimigo3.contador_animacao = 0;
                        }
                    }

                    // Parado
                    else if (inimigo3.vida > 0 && !inimigo3.sofrendo_dano) {
                        inimigo3.contador_animacao++;
                        if (inimigo3.contador_animacao >= inimigo3.velocidade_animacao) {
                            inimigo3.frame_atual = (inimigo3.frame_atual + 1) % inimigo3.num_frames;
                            inimigo3.contador_animacao = 0;
                        }
                    }

                    // Sofrendo dano
                    if (inimigo3.sofrendo_dano && inimigo3.vida > 0) {
                        inimigo3.contador_animacao++;
                        if (inimigo3.contador_animacao >= 5) {
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
                    //------------------ CENÁRIO 1 ------------------//
                     //========== INIMIGO 4 ==========
                        // Direção
                    inimigo4.olhando_direita = (odisseu.x < inimigo4.x);

                    // Cooldown
                    if (inimigo4.cooldown_ataque > 0)
                        inimigo4.cooldown_ataque--;

                    // Ataque
                    if (!inimigo4.atacando && inimigo4.vida > 0 && !inimigo4.sofrendo_dano &&
                        inimigo4.cooldown_ataque == 0 && abs(odisseu.x - inimigo4.x) < 180) {
                        inimigo4.atacando = true;
                        inimigo4.frame_atual = 0;
                        inimigo4.contador_animacao = 0;
                    }

                    // Animação de ataque
                    if (inimigo4.atacando && inimigo4.vida > 0) {
                        inimigo4.contador_animacao++;
                        if (inimigo4.contador_animacao >= 10) {
                            inimigo4.frame_atual++;

                            if (inimigo4.frame_atual == 3)
                                inimigo4.acerto = true; // Frame de acerto

                            if (inimigo4.frame_atual >= total_frames_inimigos_atacando) {
                                inimigo4.atacando = false;
                                inimigo4.frame_atual = 0;
                                inimigo4.cooldown_ataque = 150; // 1s de cooldown (60 fps)
                            }
                            inimigo4.contador_animacao = 0;
                        }
                    }

                    // Parado
                    else if (inimigo4.vida > 0 && !inimigo4.sofrendo_dano) {
                        inimigo4.contador_animacao++;
                        if (inimigo4.contador_animacao >= inimigo4.velocidade_animacao) {
                            inimigo4.frame_atual = (inimigo4.frame_atual + 1) % inimigo4.num_frames;
                            inimigo4.contador_animacao = 0;
                        }
                    }

                    // Sofrendo dano
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
                    //========== INIMIGO 5 ==========
                      // Direção
                    inimigo5.olhando_direita = (odisseu.x < inimigo5.x);

                    // Cooldown
                    if (inimigo5.cooldown_ataque > 0)
                        inimigo5.cooldown_ataque--;

                    // Ataque
                    if (!inimigo5.atacando && inimigo5.vida > 0 && !inimigo5.sofrendo_dano &&
                        inimigo5.cooldown_ataque == 0 && abs(odisseu.x - inimigo5.x) < 180) {
                        inimigo5.atacando = true;
                        inimigo5.frame_atual = 0;
                        inimigo5.contador_animacao = 0;
                    }

                    // Animação de ataque
                    if (inimigo5.atacando && inimigo5.vida > 0) {
                        inimigo5.contador_animacao++;
                        if (inimigo5.contador_animacao >= 10) {
                            inimigo5.frame_atual++;

                            if (inimigo5.frame_atual == 3)
                                inimigo5.acerto = true; // Frame de acerto

                            if (inimigo5.frame_atual >= total_frames_inimigos_atacando) {
                                inimigo5.atacando = false;
                                inimigo5.frame_atual = 0;
                                inimigo5.cooldown_ataque = 150; // 1s de cooldown (60 fps)
                            }
                            inimigo5.contador_animacao = 0;
                        }
                    }

                    // Parado
                    else if (inimigo5.vida > 0 && !inimigo5.sofrendo_dano) {
                        inimigo5.contador_animacao++;
                        if (inimigo5.contador_animacao >= inimigo5.velocidade_animacao) {
                            inimigo5.frame_atual = (inimigo5.frame_atual + 1) % inimigo5.num_frames;
                            inimigo5.contador_animacao = 0;
                        }
                    }

                    // Sofrendo dano
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
                    //========== INIMIGO 6 ==========
                      // Direção
                    inimigo6.olhando_direita = (odisseu.x < inimigo6.x);

                    // Cooldown
                    if (inimigo6.cooldown_ataque > 0)
                        inimigo6.cooldown_ataque--;

                    // Ataque
                    if (!inimigo6.atacando && inimigo6.vida > 0 && !inimigo6.sofrendo_dano &&
                        inimigo6.cooldown_ataque == 0 && abs(odisseu.x - inimigo6.x) < 180) {
                        inimigo6.atacando = true;
                        inimigo6.frame_atual = 0;
                        inimigo6.contador_animacao = 0;
                    }

                    // Animação de ataque
                    if (inimigo6.atacando && inimigo6.vida > 0) {
                        inimigo6.contador_animacao++;
                        if (inimigo6.contador_animacao >= 10) {
                            inimigo6.frame_atual++;

                            if (inimigo6.frame_atual == 3)
                                inimigo6.acerto = true; // Frame de acerto

                            if (inimigo6.frame_atual >= total_frames_inimigos_atacando) {
                                inimigo6.atacando = false;
                                inimigo6.frame_atual = 0;
                                inimigo6.cooldown_ataque = 150; // 1s de cooldown (60 fps)
                            }
                            inimigo6.contador_animacao = 0;
                        }
                    }

                    // Parado
                    else if (inimigo6.vida > 0 && !inimigo6.sofrendo_dano) {
                        inimigo6.contador_animacao++;
                        if (inimigo6.contador_animacao >= inimigo6.velocidade_animacao) {
                            inimigo6.frame_atual = (inimigo6.frame_atual + 1) % inimigo6.num_frames;
                            inimigo6.contador_animacao = 0;
                        }
                    }

                    // Sofrendo dano
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
                //------------------ CENÁRIO 2 ------------------//
                if (fase->cenario_atual == 2) {
                    penelope.contador_animacao++;
                    if (penelope.contador_animacao >= 10) {
                        penelope.frame_atual = (penelope.frame_atual + 1) % total_frames_penelope_parada;
                        penelope.contador_animacao = 0;
                    }
                }
            }

            //Poseidon
            
           
            
           // Verificar colisão
            if (ataque_ativado) {
                duracao_ataque++;
                float area_ataque_x = odisseu.olhando_direita ? odisseu.x + odisseu.largura * 0.5f : odisseu.x - odisseu.largura * 0.3f;
                float area_ataque_y = odisseu.y + odisseu.altura * 0.1f;
                float area_ataque_largura = odisseu.largura * 0.1f;
                float area_ataque_altura = odisseu.altura * 0.1f;

                if (verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                    circe.x, circe.y, circe.largura, circe.altura)) {

                    // Só aplicar dano se a Circe ainda não estiver no estado "sofrendo_dano"
                    if (!circe.sofrendo_dano && circe.vulneravel) {
                        circe.sofrendo_dano = true;
                        circe.vida--;
                        circe.frame_atual = 0;
                    }
                }
                if (escolha_mapa == MAPA_FASE_ITACA) {

                    if (fase->cenario_atual == 0) {
                        // Inimigo 1 RECEBE dano
                        if (inimigo1.vida > 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                            inimigo1.x, inimigo1.y, inimigo1.largura, inimigo1.altura)) {
                            if (!inimigo1.sofrendo_dano) {
                                inimigo1.sofrendo_dano = true;
                                inimigo1.vida--;
                                inimigo1.frame_atual = 0;
                            }
                        }

                        // Inimigo 2 RECEBE dano
                        if (inimigo2.vida > 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                            inimigo2.x, inimigo2.y, inimigo2.largura, inimigo2.altura)) {
                            if (!inimigo2.sofrendo_dano) {
                                inimigo2.sofrendo_dano = true;
                                inimigo2.vida--;
                                inimigo2.frame_atual = 0;
                            }
                        }

                        // Inimigo 3 RECEBE dano
                        if (inimigo3.vida > 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                            inimigo3.x, inimigo3.y, inimigo3.largura, inimigo3.altura)) {
                            if (!inimigo3.sofrendo_dano) {
                                inimigo3.sofrendo_dano = true;
                                inimigo3.vida--;
                                inimigo3.frame_atual = 0;
                            }
                        }
                    }

					//CONFIGURAÇÃO A VERIFICAÇÃO DE COLISAO DOS INIMIGOS DO SEGUNDO CENÁRIO
                    if (fase->cenario_atual == 1 && inimigo1.vida ==0 && inimigo2.vida == 0 && inimigo3.vida == 0) {

                        // Inimigo 4 RECEBE dano
                        if (inimigo4.vida > 0 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                            inimigo4.x, inimigo4.y, inimigo4.largura, inimigo4.altura)) {
                            if (!inimigo4.sofrendo_dano) {
                                inimigo4.sofrendo_dano = true;
                                inimigo4.vida--;
                                inimigo4.frame_atual = 0;
                            }
                        }

                        // Inimigo 5 RECEBE dano
                        if (inimigo5.vida > 0 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                            inimigo5.x, inimigo5.y, inimigo5.largura, inimigo5.altura)) {
                            if (!inimigo5.sofrendo_dano) {
                                inimigo5.sofrendo_dano = true;
                                inimigo5.vida--;
                                inimigo5.frame_atual = 0;
                            }
                        }

                        // Inimigo 6 RECEBE dano
                        if (inimigo6.vida > 0 && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0 && verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                            inimigo6.x, inimigo6.y, inimigo6.largura, inimigo6.altura)) {
                            if (!inimigo6.sofrendo_dano) {
                                inimigo6.sofrendo_dano = true;
                                inimigo6.vida--;
                                inimigo6.frame_atual = 0;
                            }
                        }
                    }
                }
                if (duracao_ataque >= DURACAO_MAXIMA_ATAQUE) ataque_ativado = false;
              }

            //ataques circe
            if (escolha_mapa == MAPA_FASE_CIRCE) {

                //ataque corvo e colisao tigre
                if ((circe.estado == 2 || circe.estado == 4) && verificar_colisao(odisseu.x, odisseu.y, odisseu.largura, odisseu.altura,
                    circe.x, circe.y, circe.largura, circe.altura)) {
                     
                    if (odisseu.cooldown_dano <= 0) {
                        odisseu.sofrendo_dano = true;
                        odisseu.vida--;
                        odisseu.frame_atual = 0;
                        odisseu.cooldown_dano = 120;
                        if (odisseu.vida <= 0) {
                            printf("ODISSEU FOI DERROTADO!\n");
                            jogo_rodando = false;
                        }
                    }
                }

                //ataque tigre
                if (circe.estado == 4 && circe.atacando && circe_stall == 0 && circe.frame_atual >= 4) {
                    float area_ataque_circe_x = circe.olhando_direita ?
                        circe.x + circe.largura * 0.3f : circe.x - circe.largura * 0.3f;
                    float area_ataque_circe_y = circe.y + circe.altura * 0.1f;
                    float area_ataque_circe_largura = circe.largura;
                    float area_ataque_circe_altura = circe.altura * 0.5f;

                    if (verificar_colisao(area_ataque_circe_x, area_ataque_circe_y,
                        area_ataque_circe_largura, area_ataque_circe_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {

                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120;

                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                }
            }

            // ATAQUES DOS INIMIGOS
            if (escolha_mapa == MAPA_FASE_ITACA && fase->cenario_atual == 0) {
               // Verificar ataque do inimigo1
                if (inimigo1.acerto && inimigo1.vida > 0) {
                    float area_ataque_inimigo1_x = inimigo1.olhando_direita ?
                        inimigo1.x - inimigo1.largura * 0.3f : inimigo1.x + inimigo1.largura * 0.5f;
                    float area_ataque_inimigo1_y = inimigo1.y + inimigo1.altura * 0.1f;
                    float area_ataque_inimigo1_largura = inimigo1.largura * 0.3f;
                    float area_ataque_inimigo1_altura = inimigo1.altura * 0.5f;

                    if (verificar_colisao(area_ataque_inimigo1_x, area_ataque_inimigo1_y,
                        area_ataque_inimigo1_largura, area_ataque_inimigo1_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {
                        
                        //Verifica imunidade em vez de sofrendo_dano
                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120; // 2 segundos de munidade (120 frames)
                            
                            printf("===================================\n");
                            printf("ODISSEU SOFREU DANO DO INIMIGO 1!\n");
                            printf("Vida restante: %d\n", odisseu.vida);
                            printf("===================================\n");
                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                    inimigo1.acerto = false;
                }

                if (inimigo2.acerto && inimigo2.vida > 0) {
                    float area_ataque_inimigo2_x = inimigo2.olhando_direita ?
                        inimigo2.x - inimigo2.largura * 0.3f : inimigo2.x + inimigo2.largura * 0.5f;
                    float area_ataque_inimigo2_y = inimigo2.y + inimigo2.altura * 0.1f;
                    float area_ataque_inimigo2_largura = inimigo2.largura * 0.3f;
                    float area_ataque_inimigo2_altura = inimigo2.altura * 0.5f;

                    if (verificar_colisao(area_ataque_inimigo2_x, area_ataque_inimigo2_y,
                        area_ataque_inimigo2_largura, area_ataque_inimigo2_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {

                        //Verifica imunidade em vez de sofrendo_dano
                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120; // 2 segundos de munidade (120 frames)

                            printf("===================================\n");
                            printf("ODISSEU SOFREU DANO DO INIMIGO 2!\n");
                            printf("Vida restante: %d\n", odisseu.vida);
                            printf("===================================\n");
                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                    inimigo2.acerto = false;
                }

                if (inimigo3.acerto && inimigo3.vida > 0) {
                    float area_ataque_inimigo3_x = inimigo3.olhando_direita ?
                        inimigo3.x - inimigo3.largura * 0.3f : inimigo3.x + inimigo3.largura * 0.5f;
                    float area_ataque_inimigo3_y = inimigo3.y + inimigo3.altura * 0.1f;
                    float area_ataque_inimigo3_largura = inimigo3.largura * 0.3f;
                    float area_ataque_inimigo3_altura = inimigo3.altura * 0.5f;

                    if (verificar_colisao(area_ataque_inimigo3_x, area_ataque_inimigo3_y,
                        area_ataque_inimigo3_largura, area_ataque_inimigo3_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {

                        // Verifica imunidade
                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120; // 2 segundos de imunidade

                            printf("===================================\n");
                            printf("ODISSEU SOFREU DANO DO INIMIGO 3!\n");
                            printf("Vida restante: %d\n", odisseu.vida);
                            printf("===================================\n");
                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                    inimigo3.acerto = false;
                }
            }
            if (fase->cenario_atual == 1 && escolha_mapa == MAPA_FASE_ITACA && inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0) {
                
                if (inimigo4.acerto && inimigo4.vida > 0) {
                    float area_ataque_inimigo4_x = inimigo4.olhando_direita ?
                        inimigo4.x - inimigo4.largura * 0.3f : inimigo4.x + inimigo4.largura * 0.5f;
                    float area_ataque_inimigo4_y = inimigo4.y + inimigo4.altura * 0.1f;
                    float area_ataque_inimigo4_largura = inimigo4.largura * 0.3f;
                    float area_ataque_inimigo4_altura = inimigo4.altura * 0.5f;

                    if (verificar_colisao(area_ataque_inimigo4_x, area_ataque_inimigo4_y,
                        area_ataque_inimigo4_largura, area_ataque_inimigo4_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {

                        // Verifica imunidade
                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120; // 2 segundos de imunidade

                            printf("===================================\n");
                            printf("ODISSEU SOFREU DANO DO INIMIGO 4!\n");
                            printf("Vida restante: %d\n", odisseu.vida);
                            printf("===================================\n");
                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                    inimigo4.acerto = false;
                }

                if (inimigo5.acerto && inimigo5.vida > 0) {
                    float area_ataque_inimigo5_x = inimigo5.olhando_direita ?
                        inimigo5.x - inimigo5.largura * 0.3f : inimigo5.x + inimigo5.largura * 0.5f;
                    float area_ataque_inimigo5_y = inimigo5.y + inimigo5.altura * 0.1f;
                    float area_ataque_inimigo5_largura = inimigo5.largura * 0.3f;
                    float area_ataque_inimigo5_altura = inimigo5.altura * 0.5f;

                    if (verificar_colisao(area_ataque_inimigo5_x, area_ataque_inimigo5_y,
                        area_ataque_inimigo5_largura, area_ataque_inimigo5_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {

                        // Verifica imunidade
                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120; // 2 segundos de imunidade

                            printf("===================================\n");
                            printf("ODISSEU SOFREU DANO DO INIMIGO 5!\n");
                            printf("Vida restante: %d\n", odisseu.vida);
                            printf("===================================\n");
                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                    inimigo5.acerto = false;
                }

                if (inimigo6.acerto && inimigo6.vida > 0) {
                    float area_ataque_inimigo6_x = inimigo6.olhando_direita ?
                        inimigo6.x - inimigo6.largura * 0.3f : inimigo6.x + inimigo6.largura * 0.5f;
                    float area_ataque_inimigo6_y = inimigo6.y + inimigo6.altura * 0.1f;
                    float area_ataque_inimigo6_largura = inimigo6.largura * 0.3f;
                    float area_ataque_inimigo6_altura = inimigo6.altura * 0.5f;

                    if (verificar_colisao(area_ataque_inimigo6_x, area_ataque_inimigo6_y,
                        area_ataque_inimigo6_largura, area_ataque_inimigo6_altura,
                        odisseu.x, odisseu.y, odisseu.largura, odisseu.altura)) {

                        // Verifica imunidade
                        if (odisseu.cooldown_dano <= 0) {
                            odisseu.sofrendo_dano = true;
                            odisseu.vida--;
                            odisseu.frame_atual = 0;
                            odisseu.cooldown_dano = 120; // 2 segundos de imunidade

                            printf("===================================\n");
                            printf("ODISSEU SOFREU DANO DO INIMIGO 6!\n");
                            printf("Vida restante: %d\n", odisseu.vida);
                            printf("===================================\n");
                            if (odisseu.vida <= 0) {
                                printf("ODISSEU FOI DERROTADO!\n");
                                jogo_rodando = false;
                            }
                        }
                    }
                    inimigo6.acerto = false;
                }

            }
            //atualiza flechas
            int i = 0;
            while (i < count_flechas) {
                bool flecha_colidiu = false;

                // Verifica se a flecha atingiu o chão
                if (listaFlechas[i].y >= y_chao) {
                    listaFlechas[i].y = y_chao;
                    listaFlechas[i].tempo_de_vida++;
                }
                else {
                    // Atualiza posição da flecha
                    float x1 = listaFlechas[i].x + listaFlechas[i].vx;
                    float y1 = listaFlechas[i].y - listaFlechas[i].vy;
                    listaFlechas[i].angulo = -atan2f(listaFlechas[i].vy, listaFlechas[i].vx);
                    listaFlechas[i].x = x1;
                    listaFlechas[i].y = y1;
                    listaFlechas[i].vy -= GRAVIDADE;

                    // Verificar colisão com Circe
                    if (escolha_mapa == MAPA_FASE_CIRCE && fase->cenario_atual == 3 && circe.vida > 0) {
                        if (verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            circe.x, circe.y, circe.largura, circe.altura)) {
                            if (circe.vulneravel) {
                                circe.sofrendo_dano = true;
                                circe.vida--;
                                circe.frame_atual = 0;
                            }
                            flecha_colidiu = true;
                        }
                    }

                    // Verificar colisão com inimigos 1, 2, 3
                    if (escolha_mapa == MAPA_FASE_ITACA && fase->cenario_atual == 0) {
                        if (inimigo1.vida > 0 && verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            inimigo1.x, inimigo1.y, inimigo1.largura, inimigo1.altura)) {
                            inimigo1.sofrendo_dano = true;
                            inimigo1.vida--;
                            inimigo1.frame_atual = 0;
                            flecha_colidiu = true;
                        }

                        if (inimigo2.vida > 0 && verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            inimigo2.x, inimigo2.y, inimigo2.largura, inimigo2.altura)) {
                            inimigo2.sofrendo_dano = true;
                            inimigo2.vida--;
                            inimigo2.frame_atual = 0;
                            flecha_colidiu = true;
                        }

                        if (inimigo3.vida > 0 && verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            inimigo3.x, inimigo3.y, inimigo3.largura, inimigo3.altura)) {
                            inimigo3.sofrendo_dano = true;
                            inimigo3.vida--;
                            inimigo3.frame_atual = 0;
                            flecha_colidiu = true;
                        }
                    }

                    // Verificar colisão com inimigos 4, 5, 6
                    if (escolha_mapa == MAPA_FASE_ITACA && fase->cenario_atual == 1 &&
                        inimigo1.vida == 0 && inimigo2.vida == 0 && inimigo3.vida == 0) {

                        if (inimigo4.vida > 0 && verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            inimigo4.x, inimigo4.y, inimigo4.largura, inimigo4.altura)) {
                            inimigo4.sofrendo_dano = true;
                            inimigo4.vida--;
                            inimigo4.frame_atual = 0;
                            flecha_colidiu = true;
                        }

                        if (inimigo5.vida > 0 && verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            inimigo5.x, inimigo5.y, inimigo5.largura, inimigo5.altura)) {
                            inimigo5.sofrendo_dano = true;
                            inimigo5.vida--;
                            inimigo5.frame_atual = 0;
                            flecha_colidiu = true;
                        }

                        if (inimigo6.vida > 0 && verificar_colisao(listaFlechas[i].x, listaFlechas[i].y,
                            listaFlechas[i].largura, listaFlechas[i].altura,
                            inimigo6.x, inimigo6.y, inimigo6.largura, inimigo6.altura)) {
                            inimigo6.sofrendo_dano = true;
                            inimigo6.vida--;
                            inimigo6.frame_atual = 0;
                            flecha_colidiu = true;
                        }
                    }
                }

                // Remover flecha se colidiu ou tempo expirou
                if (flecha_colidiu || listaFlechas[i].tempo_de_vida > 120.0f) {
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

            //--------------------------------- Desenha Quiz ----------------------------------------------//

            if (escolha_mapa == MAPA_FASE_CALYPSO) {
                // Desenhar fundo
                desenhar_cenario(fase, LARGURA_TELA, ALTURA_TELA);

                // Quiz em andamento
                if (estado.perguntaAtual < estado.numPerguntas && !estado.perdeu) {
                    // Passa NULL para os bitmaps (tiramos)
                    desenha_quiz(NULL, NULL, fonte_quiz,
                        &perguntas[estado.perguntaAtual], &estado);
                }
                // Game Over
                else if (estado.perdeu) {
                    al_clear_to_color(al_map_rgb(20, 0, 0));

                    // Caixa de game over
                    al_draw_filled_rectangle(
                        LARGURA_TELA / 2 - 400, ALTURA_TELA / 2 - 200,
                        LARGURA_TELA / 2 + 400, ALTURA_TELA / 2 + 200,
                        al_map_rgb(0, 0, 0));
                    al_draw_rectangle(
                        LARGURA_TELA / 2 - 400, ALTURA_TELA / 2 - 200,
                        LARGURA_TELA / 2 + 400, ALTURA_TELA / 2 + 200,
                        al_map_rgb(255, 0, 0), 5.0f);

                    al_draw_text(fonte_quiz, al_map_rgb(255, 0, 0),
                        LARGURA_TELA / 2, ALTURA_TELA / 2 - 80,
                        ALLEGRO_ALIGN_CENTER, "GAME OVER!");
                    al_draw_text(fonte_quiz, al_map_rgb(255, 255, 255),
                        LARGURA_TELA / 2, ALTURA_TELA / 2,
                        ALLEGRO_ALIGN_CENTER, "Você errou 3 vezes, Atena. Odisseu nunca será liberto.");
                    al_draw_text(fonte_quiz, al_map_rgb(200, 200, 200),
                        LARGURA_TELA / 2, ALTURA_TELA / 2 + 80,
                        ALLEGRO_ALIGN_CENTER, "Pressione ESC para voltar");
                }
                // Quiz completo
                else if (estado.perguntaAtual >= estado.numPerguntas) {
                    al_clear_to_color(al_map_rgb(0, 20, 0));

                    // Caixa de vitória
                    //Retângulo
                    al_draw_filled_rectangle(
                        LARGURA_TELA / 2 - 400, ALTURA_TELA / 2 - 200,
                        LARGURA_TELA / 2 + 400, ALTURA_TELA / 2 + 200,
                        al_map_rgb(0, 0, 0));
                    al_draw_rectangle(
                        LARGURA_TELA / 2 - 400, ALTURA_TELA / 2 - 200,
                        LARGURA_TELA / 2 + 400, ALTURA_TELA / 2 + 200,
                        al_map_rgb(0, 255, 0), 5.0f);

                    al_draw_text(fonte_quiz, al_map_rgb(0, 255, 0),
                        LARGURA_TELA / 2, ALTURA_TELA / 2 - 80,
                        ALLEGRO_ALIGN_CENTER, "PARABÉNS, ATENA.");
                    al_draw_text(fonte_quiz, al_map_rgb(255, 255, 255),
                        LARGURA_TELA / 2, ALTURA_TELA / 2,
                        ALLEGRO_ALIGN_CENTER, "Você reafirmou seu conhecimento e libertou Odisseu.");
                    al_draw_text(fonte_quiz, al_map_rgb(200, 200, 200),
                        LARGURA_TELA / 2, ALTURA_TELA / 2 + 80,
                        ALLEGRO_ALIGN_CENTER, "Pressione ESC para voltar");
                    mapas_disponiveis++;
                }
            }

            else {

                desenhar_cenario(fase, LARGURA_TELA, ALTURA_TELA);

            // Selecionar sprite do Odisseu (SOMENTE se NÃO estiver no Olimpo ou no Poseidon antes de ganhar!)
                if ((escolha_mapa != MAPA_FASE_CALYPSO && escolha_mapa != MAPA_FASE_POSEIDON) || (escolha_mapa == MAPA_FASE_POSEIDON && player_ganha)) {

                    // Selecionar sprite do Odisseu
                    ALLEGRO_BITMAP* sprite_atual_odisseu;
                    int largura_frame_odisseu, altura_frame_odisseu;
                    int estado_odisseu = 0;

                    if (odisseu.desembainhando || odisseu.guardando_espada) estado_odisseu = 2;
                    else if (odisseu.puxando_arco || odisseu.guardando_arco) estado_odisseu = 7;
                    else if (odisseu.atacando) estado_odisseu = 3;
                    else if (odisseu.disparando) estado_odisseu = 6;
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

                    case 6:
                        sprite_atual_odisseu = odisseuMirando;
                        largura_frame_odisseu = largura_frame_Odisseu_mirando;
                        altura_frame_odisseu = altura_frame_Odisseu_mirando;
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
                    case 7:
                        sprite_atual_odisseu = odisseuPuxandoArco;
                        largura_frame_odisseu = largura_frame_Odisseu_puxa_arco;
                        altura_frame_odisseu = altura_frame_Odisseu_puxa_arco;
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
                        al_draw_spline(pontosArco, al_map_rgb(255, 0, 0), 3.0f);
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
                   



                    if (escolha_mapa == MAPA_FASE_ITACA) {
                        // DESENHO DOS INIMIGOS NO CENÁRIO 0 (Inimigos 1, 2 e 3)
                        if (fase->cenario_atual == 0) {
                            // INIMIGO 1

                            if (inimigo1.vida > 0) {
                                ALLEGRO_BITMAP* sprite_inimigo1;
                                int largura_inimigo1, altura_inimigo1;

                                if (inimigo1.atacando) {
                                    sprite_inimigo1 = inimigo1_atacando;
                                    largura_inimigo1 = largura_frame_inimigo1_atacando;
                                    altura_inimigo1 = altura_frame_inimigo1_atacando;
                                }
                                else if (inimigo1.sofrendo_dano) {
                                    sprite_inimigo1 = inimigo1_sofre_dano;
                                    largura_inimigo1 = largura_frame_inimigo1_dano;
                                    altura_inimigo1 = altura_frame_inimigo1_dano;
                                }
                                else {
                                    sprite_inimigo1 = inimigoParado1;
                                    largura_inimigo1 = largura_frame_inimigo1_parado;
                                    altura_inimigo1 = altura_frame_inimigo1_parado;
                                }

                                int flags_inimigo1 = inimigo1.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                                al_draw_scaled_bitmap(sprite_inimigo1,
                                    inimigo1.frame_atual * largura_inimigo1, 0,
                                    largura_inimigo1, altura_inimigo1,
                                    inimigo1.x, inimigo1.y,
                                    inimigo1.largura, inimigo1.altura,
                                    flags_inimigo1);
                            }

                            // INIMIGO 2
                            if (inimigo2.vida > 0) {
                                ALLEGRO_BITMAP* sprite_inimigo2;
                                int largura_inimigo2, altura_inimigo2;

                                if (inimigo2.atacando) {
                                    sprite_inimigo2 = inimigo2_atacando;
                                    largura_inimigo2 = largura_frame_inimigo2_atacando;
                                    altura_inimigo2 = altura_frame_inimigo2_atacando;
                                }
                                else if (inimigo2.sofrendo_dano) {
                                    sprite_inimigo2 = inimigo2_sofre_dano;
                                    largura_inimigo2 = largura_frame_inimigo2_dano;
                                    altura_inimigo2 = altura_frame_inimigo2_dano;
                                }
                                else {
                                    sprite_inimigo2 = inimigoParado2;
                                    largura_inimigo2 = largura_frame_inimigo2_parado;
                                    altura_inimigo2 = altura_frame_inimigo2_parado;
                                }

                                int flags_inimigo2 = inimigo2.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                                al_draw_scaled_bitmap(sprite_inimigo2,
                                    inimigo2.frame_atual * largura_inimigo2, 0,
                                    largura_inimigo2, altura_inimigo2,
                                    inimigo2.x, inimigo2.y,
                                    inimigo2.largura, inimigo2.altura,
                                    flags_inimigo2);
                            }


                            // INIMIGO 3
                            if (inimigo3.vida > 0) {
                                ALLEGRO_BITMAP* sprite_inimigo3;
                                int largura_inimigo3, altura_inimigo3;

                                if (inimigo3.atacando) {
                                    sprite_inimigo3 = inimigo3_atacando;
                                    largura_inimigo3 = largura_frame_inimigo3_atacando;
                                    altura_inimigo3 = altura_frame_inimigo3_atacando;
                                }
                                else if (inimigo3.sofrendo_dano) {
                                    sprite_inimigo3 = inimigo3_sofre_dano;
                                    largura_inimigo3 = largura_frame_inimigo3_dano;
                                    altura_inimigo3 = altura_frame_inimigo3_dano;
                                }
                                else {
                                    sprite_inimigo3 = inimigoParado3;
                                    largura_inimigo3 = largura_frame_inimigo3_parado;
                                    altura_inimigo3 = altura_frame_inimigo3_parado;
                                }

                                int flags_inimigo3 = inimigo3.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                                al_draw_scaled_bitmap(sprite_inimigo3,
                                    inimigo3.frame_atual * largura_inimigo3, 0,
                                    largura_inimigo3, altura_inimigo3,
                                    inimigo3.x, inimigo3.y,
                                    inimigo3.largura, inimigo3.altura,
                                    flags_inimigo3);
                            }
                        }

                        // DESENHO DOS INIMIGOS NO CENÁRIO 1 (Inimigos 4, 5 e 6)
                        if (fase->cenario_atual == 1) {
                            // INIMIGO 4
                            if (inimigo4.vida > 0) {
                                ALLEGRO_BITMAP* sprite_inimigo4;
                                int largura_inimigo4, altura_inimigo4;

                                if (inimigo4.atacando) {
                                    sprite_inimigo4 = inimigo4_atacando;
                                    largura_inimigo4 = largura_frame_inimigo4_atacando;
                                    altura_inimigo4 = altura_frame_inimigo4_atacando;
                                }
                                else if (inimigo4.sofrendo_dano) {
                                    sprite_inimigo4 = inimigo4_sofre_dano;
                                    largura_inimigo4 = largura_frame_inimigo4_dano;
                                    altura_inimigo4 = altura_frame_inimigo4_dano;
                                }
                                else {
                                    sprite_inimigo4 = inimigoParado4;
                                    largura_inimigo4 = largura_frame_inimigo4_parado;
                                    altura_inimigo4 = altura_frame_inimigo4_parado;
                                }

                                int flags_inimigo4 = inimigo4.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                                al_draw_scaled_bitmap(sprite_inimigo4,
                                    inimigo4.frame_atual * largura_inimigo4, 0,
                                    largura_inimigo4, altura_inimigo4,
                                    inimigo4.x, inimigo4.y,
                                    inimigo4.largura, inimigo4.altura,
                                    flags_inimigo4);
                            }

                            // INIMIGO 5
                            if (inimigo5.vida > 0) {
                                ALLEGRO_BITMAP* sprite_inimigo5;
                                int largura_inimigo5, altura_inimigo5;

                                if (inimigo5.atacando) {
                                    sprite_inimigo5 = inimigo5_atacando;
                                    largura_inimigo5 = largura_frame_inimigo5_atacando;
                                    altura_inimigo5 = altura_frame_inimigo5_atacando;
                                }
                                else if (inimigo5.sofrendo_dano) {
                                    sprite_inimigo5 = inimigo5_sofre_dano;
                                    largura_inimigo5 = largura_frame_inimigo5_dano;
                                    altura_inimigo5 = altura_frame_inimigo5_dano;
                                }
                                else {
                                    sprite_inimigo5 = inimigoParado5;
                                    largura_inimigo5 = largura_frame_inimigo5_parado;
                                    altura_inimigo5 = altura_frame_inimigo5_parado;
                                }

                                int flags_inimigo5 = inimigo5.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                                al_draw_scaled_bitmap(sprite_inimigo5,
                                    inimigo5.frame_atual * largura_inimigo5, 0,
                                    largura_inimigo5, altura_inimigo5,
                                    inimigo5.x, inimigo5.y,
                                    inimigo5.largura, inimigo5.altura,
                                    flags_inimigo5);
                            }

                            // INIMIGO 6
                            if (inimigo6.vida > 0) {
                                ALLEGRO_BITMAP* sprite_inimigo6;
                                int largura_inimigo6, altura_inimigo6;

                                if (inimigo6.atacando) {
                                    sprite_inimigo6 = inimigo6_atacando;
                                    largura_inimigo6 = largura_frame_inimigo6_atacando;
                                    altura_inimigo6 = altura_frame_inimigo6_atacando;
                                }
                                else if (inimigo6.sofrendo_dano) {
                                    sprite_inimigo6 = inimigo6_sofre_dano;
                                    largura_inimigo6 = largura_frame_inimigo6_dano;
                                    altura_inimigo6 = altura_frame_inimigo6_dano;
                                }
                                else {
                                    sprite_inimigo6 = inimigoParado6;
                                    largura_inimigo6 = largura_frame_inimigo6_parado;
                                    altura_inimigo6 = altura_frame_inimigo6_parado;
                                }

                                int flags_inimigo6 = inimigo6.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                                al_draw_scaled_bitmap(sprite_inimigo6,
                                    inimigo6.frame_atual * largura_inimigo6, 0,
                                    largura_inimigo6, altura_inimigo6,
                                    inimigo6.x, inimigo6.y,
                                    inimigo6.largura, inimigo6.altura,
                                    flags_inimigo6);
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
                }
            }
            desenhar_sobreposicoes(fase, LARGURA_TELA, ALTURA_TELA);

            // Desenhar Hermes no cenário 7 (mesmo comportamento visual do Odisseu parado)
            if (fase->cenario_atual == 1 && escolha_mapa == MAPA_FASE_CIRCE) {
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
            //desenha os porquinhos
            if (fase->cenario_atual == 2 && escolha_mapa == MAPA_FASE_CIRCE) {
                ALLEGRO_BITMAP* sprite_porco1;
                int largura_frame_porco1;
                int altura_frame_porco1;
                int total_frames_porco1;

                sprite_porco1 = porcoParado1;
                largura_frame_porco1 = largura_frame_Porco1_parado;
                altura_frame_porco1 = altura_frame_Porco1_parado;
                total_frames_porco1 = total_frames_porcos_parados;


                int frame_porco1= porco1.frame_atual % total_frames_porco1;
                int flagsPorco1 = porco1.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

                al_draw_scaled_bitmap(
                    sprite_porco1,
                    frame_porco1* largura_frame_porco1, 0,
                    largura_frame_porco1,altura_frame_porco1,
                    porco1.x, porco1.y,
                    porco1.largura, porco1.altura,
                    flagsPorco1);

                ALLEGRO_BITMAP* sprite_porco2;
                int largura_frame_porco2;
                int altura_frame_porco2;
                int total_frames_porco2;

                sprite_porco2 = porcoParado2;
                largura_frame_porco2 = largura_frame_Porco2_parado;
                altura_frame_porco2 = altura_frame_Porco2_parado;
                total_frames_porco2 = total_frames_porcos_parados;


                int frame_porco2 = porco2.frame_atual % total_frames_porco2;
                int flagsporco2 = porco2.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

                al_draw_scaled_bitmap(
                    sprite_porco2,
                    frame_porco2* largura_frame_porco2, 0,
                    largura_frame_porco2, altura_frame_porco2,
                    porco2.x, porco2.y,
                    porco2.largura, porco2.altura,
                    flagsporco2);

                ALLEGRO_BITMAP* sprite_porco3;
                int largura_frame_porco3;
                int altura_frame_porco3;
                int total_frames_porco3;

                sprite_porco3 = porcoParado3;
                largura_frame_porco3 = largura_frame_Porco3_parado;
                altura_frame_porco3 = altura_frame_Porco3_parado;
                total_frames_porco3 = total_frames_porcos_parados;


                int frame_porco3 = porco3.frame_atual % total_frames_porco3;
                int flagsporco3 = porco3.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

                al_draw_scaled_bitmap(
                    sprite_porco3,
                    frame_porco3* largura_frame_porco3, 0,
                    largura_frame_porco3, altura_frame_porco3,
                    porco3.x, porco3.y,
                    porco3.largura, porco3.altura,
                    flagsporco3);
            }
            
            // Desenhar Circe no último cenário
            if (fase->cenario_atual == 3 && escolha_mapa == MAPA_FASE_CIRCE && circe.vida > 0) {
                ALLEGRO_BITMAP* sprite_atual_circe = sprites_circe[circe.sprite_ativo];
                int contagem_frames_sprite_atual = contagem_frames_circe[circe.sprite_ativo];
                int largura_frame_circe = al_get_bitmap_width(sprite_atual_circe) / contagem_frames_sprite_atual;
                int altura_frame_circe = al_get_bitmap_height(sprite_atual_circe);
                int frame_circe = circe.frame_atual % contagem_frames_sprite_atual;

                int flagsCirce = circe.olhando_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                if (circe.angulo == 0) {
                    al_draw_scaled_bitmap(
                        sprite_atual_circe,
                        frame_circe* largura_frame_circe, 0,
                        largura_frame_circe, altura_frame_circe,
                        circe.x, circe.y,
                        circe.largura, circe.altura, flagsCirce);
                }
                else {
                    float circe_scale_x = (float)circe.largura / (float)largura_frame_circe;
                    float circe_scale_y = (float)circe.altura / (float)altura_frame_circe;
                    al_draw_tinted_scaled_rotated_bitmap_region(
                        sprite_atual_circe,
                        frame_circe* largura_frame_circe,0,
                        largura_frame_circe,altura_frame_circe,
                        al_map_rgba_f(1, 1, 1, 1),
                        largura_frame_circe / 2.0f,altura_frame_circe / 2.0f,
                        circe.x + circe.largura / 2.0f,
                        circe.y + circe.altura / 2.0f,
                        circe_scale_x, circe_scale_y,
                        circe.angulo,
                        0
                    );
                }
            }

            if (escolha_mapa == MAPA_FASE_POSEIDON) {
                al_get_mouse_state(&estado_mouse);
                draw_battleship(fase,
                    fonte_quiz,
                    font_titulo,
                    tabuleiro_jogador,
                    tabuleiro_com,
                    setup,
                    navios,
                    contagem_navios,
                    estado_mouse,
                    vertical,
                    turno_jogador,
                    player_ganha,
                    com_ganha,
                    tela
                );

                if (!turno_jogador) {
                    if (sleep_turno == 0) {
                        update_battleship(
                            estado_mouse,
                            false,
                            false,
                            tabuleiro_jogador,
                            tabuleiro_com,
                            &turno_jogador,
                            &setup,
                            navios,
                            contagem_navios,
                            &vertical,
                            &com,
                            &player_ganha,
                            &com_ganha,
                            tela
                        );
                        if (player_ganha) {
                            escolha_mapa = MAPA_VOLTAR_MENU;
                            mapas_disponiveis++;
                        }
                    }
                    else {
                        sleep_turno--;
                    }
                    
                }
            }

            //desenhar coracao de vida
            if ((escolha_mapa != MAPA_FASE_POSEIDON && escolha_mapa != MAPA_FASE_CALYPSO) || player_ganha) {
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
    al_destroy_bitmap(sprites_circe[CIRCE_SPRITE_PARADA]);
    al_destroy_bitmap(sprites_circe[CIRCE_SPRITE_DANO]);

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


    //golpe 
    al_destroy_bitmap(inimigo1_atacando);
	al_destroy_bitmap(inimigo2_atacando);
	al_destroy_bitmap(inimigo3_atacando);
	al_destroy_bitmap(inimigo4_atacando);
	al_destroy_bitmap(inimigo5_atacando);
	al_destroy_bitmap(inimigo6_atacando);

	al_destroy_bitmap(sprite_flecha);
	al_destroy_bitmap(sprite_coracao);

    
    //Todo o resto

    al_destroy_font(fonte_quiz);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(tela_jogo);
    free(listaFlechas);

    return 0;
}
