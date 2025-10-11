#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "menu/menu_inicial.h"
#include "fases/Polifemus/fase_Polifemo.h"

typedef struct {
    int largura;
    int altura;
} InformacoesTela;

typedef struct {
    float x, y;
    int largura, altura, frame_atual, contador_animacao;
    bool olhando_direita, olhando_esquerda, andando, desembainhando,
        sofrendo_dano, guardando_espada, atacando, tem_espada;
} Personagem;

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

float deixarProporcional(float posicao, float tamanho_tela, float tamanho_tela_original) {
    return (posicao * tamanho_tela) / tamanho_tela_original;
}

bool verificar_colisao(float objeto1_x, float objeto1_y, float objeto1_largura, float objeto1_altura,
    float objeto2_x, float objeto2_y, float objeto2_largura, float objeto2_altura) {
    return (objeto1_x < objeto2_x + objeto2_largura &&
        objeto1_x + objeto1_largura > objeto2_x &&
        objeto1_y < objeto2_y + objeto2_altura &&
        objeto1_y + objeto1_altura > objeto2_y);
}

int main(void) {
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_install_mouse()) return -1;
    if (!al_init_image_addon()) return -1;

    InformacoesTela tela = obter_resolucao_tela_atual();
    const int ALTURA_TELA_ORIGINAL = 1080;
    const int LARGURA_TELA_ORIGINAL = 1920;
    const int LARGURA_TELA = tela.largura;
    const int ALTURA_TELA = tela.altura;
    const int ALTURA_SPRITE = 250, LARGURA_SPRITE = 250;
    int ALTURA_PERSONAGEM = deixarProporcional(ALTURA_SPRITE, ALTURA_TELA, ALTURA_TELA_ORIGINAL);
    int LARGURA_PERSONAGEM = deixarProporcional(LARGURA_SPRITE, LARGURA_TELA, LARGURA_TELA_ORIGINAL);

    ALLEGRO_DISPLAY* tela_jogo = criar_tela_cheia(tela);
    if (!tela_jogo) {
        printf("Falha ao criar display.\n");
        return -1;
    }

	// Menu inicial
    if (!menu_inicial(tela_jogo)) {
        printf("Erro ao carregar menu.\n");
        al_destroy_display(tela_jogo);
        return -1;
    }

    // Cenários Polifemo
    CenarioPolifemo cenario;
    if (!carregar_cenarios_polifemo(&cenario)) {
        printf("Erro ao carregar cenários.\n");
        al_destroy_display(tela_jogo);
        return -1;
    }

    // Carregar sprites
    ALLEGRO_BITMAP* sprite_odisseuParado = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiParado.png");
    ALLEGRO_BITMAP* sprite_odisseuAndando = al_load_bitmap("./imagensJogo/personagens/Odisseu/andandoSemEspada.png");
    ALLEGRO_BITMAP* sprite_odisseuDesembainhar = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiDesembainhar.png");
    ALLEGRO_BITMAP* sprite_odisseuAtacando = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiAtacando.png");
    ALLEGRO_BITMAP* sprite_odisseuParadoEspada = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiParadoEspada.png");
    ALLEGRO_BITMAP* sprite_odisseuAndandoEspada = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiAndandoEspada.png");
    ALLEGRO_BITMAP* sprite_circeparada = al_load_bitmap("./imagensJogo/personagens/Circe/circeparada.png");
    ALLEGRO_BITMAP* sprite_circeDano = al_load_bitmap("./imagensJogo/personagens/Circe/circeDano.png");

    if (!sprite_odisseuParado || !sprite_odisseuAndando || !sprite_odisseuDesembainhar ||
        !sprite_odisseuAtacando || !sprite_odisseuParadoEspada || !sprite_odisseuAndandoEspada ||
        !sprite_circeparada || !sprite_circeDano) {
        printf("Erro ao carregar imagens dos personagens.\n");
        return -1;
    }

    // Configurações de frames
    int total_frames_parado = 5;
    int largura_frame_parado = al_get_bitmap_width(sprite_odisseuParado) / total_frames_parado;
    int altura_frame_parado = al_get_bitmap_height(sprite_odisseuParado);

    int total_frames_parado_espada = 5;
    int largura_frame_parado_espada = al_get_bitmap_width(sprite_odisseuParadoEspada) / total_frames_parado_espada;
    int altura_frame_parado_espada = al_get_bitmap_height(sprite_odisseuParadoEspada);

    int total_frames_circeparada = 5;
    int largura_frame_circeparada = al_get_bitmap_width(sprite_circeparada) / total_frames_circeparada;
    int altura_frame_circeparada = al_get_bitmap_height(sprite_circeparada);

    int total_frames_dano = 7;
    int largura_frame_dano = al_get_bitmap_width(sprite_circeDano) / total_frames_dano;
    int altura_frame_dano = al_get_bitmap_height(sprite_circeDano);

    int total_frames_andando = 6;
    int largura_frame_andando = al_get_bitmap_width(sprite_odisseuAndando) / total_frames_andando;
    int altura_frame_andando = al_get_bitmap_height(sprite_odisseuAndando);

    int total_frames_andando_espada = 6;
    int largura_frame_andando_espada = al_get_bitmap_width(sprite_odisseuAndandoEspada) / total_frames_andando_espada;
    int altura_frame_andando_espada = al_get_bitmap_height(sprite_odisseuAndandoEspada);

    int total_frames_desembainhar = 7;
    int largura_frame_desembainhar = al_get_bitmap_width(sprite_odisseuDesembainhar) / total_frames_desembainhar;
    int altura_frame_desembainhar = al_get_bitmap_height(sprite_odisseuDesembainhar);

    int total_frames_atacando = 6;
    int largura_frame_atacando = al_get_bitmap_width(sprite_odisseuAtacando) / total_frames_atacando;
    int altura_frame_atacando = al_get_bitmap_height(sprite_odisseuAtacando);

    const int VELOCIDADE_ANIMACAO_DESEMBAINHAR = 6;

    // Inicializar personagens
    Personagem odisseu = {
        .x = (LARGURA_TELA / 2) - (LARGURA_TELA / 2.5),
        .y = deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
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

    Personagem circe = {
        .x = LARGURA_TELA - (LARGURA_TELA / 3),
        .y = deixarProporcional(750, ALTURA_TELA, ALTURA_TELA_ORIGINAL),
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

    bool ataque_ativado = false;
    int duracao_ataque = 0;
    const int DURACAO_MAXIMA_ATAQUE = 15;

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

    while (jogo_rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jogo_rodando = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                jogo_rodando = false;

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
            al_get_keyboard_state(&estado_teclado);

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

            const float velocidade_odisseu = 800.0f / 60.0f;
            odisseu.x += odisseu_direcao_x * velocidade_odisseu;
            odisseu.y = limitar_valor(odisseu.y, 0, ALTURA_TELA - odisseu.altura);
            odisseu.andando = (odisseu_direcao_x != 0.0f);

            // Atualizar transição de cenário
            atualizar_transicao_cenario(&cenario, &odisseu.x, odisseu.largura, LARGURA_TELA);

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
                    if (odisseu.frame_atual >= total_frames_atacando) {
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
                        odisseu.frame_atual %= (odisseu.tem_espada ? total_frames_andando_espada : total_frames_andando);
                    else
                        odisseu.frame_atual %= (odisseu.tem_espada ? total_frames_parado_espada : total_frames_parado);
                    odisseu.contador_animacao = 0;
                }
                break;
            }

            // Atualizar Circe
            if (cenario.cenario_atual == 5) {
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

            // Verificar colisão
            if (ataque_ativado) {
                duracao_ataque++;
                float area_ataque_x = odisseu.olhando_direita ? odisseu.x + odisseu.largura * 0.5f : odisseu.x - odisseu.largura * 0.3f;
                float area_ataque_y = odisseu.y + odisseu.altura * 0.1f;
                float area_ataque_largura = odisseu.largura * 0.1f;
                float area_ataque_altura = odisseu.altura * 0.1f;

                if (verificar_colisao(area_ataque_x, area_ataque_y, area_ataque_largura, area_ataque_altura,
                    circe.x, circe.y, circe.largura, circe.altura)) {
                    circe.sofrendo_dano = true;
                    circe.frame_atual = 0;
                }

                if (duracao_ataque >= DURACAO_MAXIMA_ATAQUE)
                    ataque_ativado = false;
            }

            redesenhar_tela = true;
        }

        if (redesenhar_tela && al_is_event_queue_empty(fila_eventos)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Desenhar cenário
            desenhar_cenario_polifemo(&cenario, LARGURA_TELA, ALTURA_TELA);

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
                sprite_atual_odisseu = sprite_odisseuDesembainhar;
                largura_frame_odisseu = largura_frame_desembainhar;
                altura_frame_odisseu = altura_frame_desembainhar;
                break;
            case 3:
                sprite_atual_odisseu = sprite_odisseuAtacando;
                largura_frame_odisseu = largura_frame_atacando;
                altura_frame_odisseu = altura_frame_atacando;
                break;
            case 1:
                sprite_atual_odisseu = sprite_odisseuAndando;
                largura_frame_odisseu = largura_frame_andando;
                altura_frame_odisseu = altura_frame_andando;
                break;
            case 5:
                sprite_atual_odisseu = sprite_odisseuAndandoEspada;
                largura_frame_odisseu = largura_frame_andando_espada;
                altura_frame_odisseu = altura_frame_andando_espada;
                break;
            case 4:
                sprite_atual_odisseu = sprite_odisseuParadoEspada;
                largura_frame_odisseu = largura_frame_parado_espada;
                altura_frame_odisseu = altura_frame_parado_espada;
                break;
            case 0:
            default:
                sprite_atual_odisseu = sprite_odisseuParado;
                largura_frame_odisseu = largura_frame_parado;
                altura_frame_odisseu = altura_frame_parado;
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

            // Desenhar sobreposições
            desenhar_sobreposicoes_polifemo(&cenario, LARGURA_TELA, ALTURA_TELA);

            // Desenhar Circe no cenário 5
            if (cenario.cenario_atual == 5) {
                ALLEGRO_BITMAP* sprite_atual_circe = circe.sofrendo_dano ? sprite_circeDano : sprite_circeparada;
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

            al_flip_display();
            redesenhar_tela = false;
        }
    }

    // Limpeza
    al_destroy_bitmap(sprite_odisseuParado);
    al_destroy_bitmap(sprite_odisseuAndando);
    al_destroy_bitmap(sprite_odisseuDesembainhar);
    al_destroy_bitmap(sprite_odisseuAtacando);
    al_destroy_bitmap(sprite_odisseuParadoEspada);
    al_destroy_bitmap(sprite_odisseuAndandoEspada);
    al_destroy_bitmap(sprite_circeparada);
    al_destroy_bitmap(sprite_circeDano);

    destruir_cenarios_polifemo(&cenario);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(tela_jogo);

    return 0;
}