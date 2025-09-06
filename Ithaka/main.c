#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Estrutura para armazenar informações de resolução
typedef struct {
    int largura;
    int altura;
} InformacoesTela;

// Função para detectar a resolução do monitor principal
InformacoesTela obter_resolucao_tela_atual() {
    InformacoesTela tela;
    ALLEGRO_MONITOR_INFO informacoes_monitor;
    al_get_monitor_info(0, &informacoes_monitor);

    tela.largura = informacoes_monitor.x2 - informacoes_monitor.x1;
    tela.altura = informacoes_monitor.y2 - informacoes_monitor.y1;

    printf("Resolução detectada: %dx%d\n", tela.largura, tela.altura);
    return tela;
}

// Função para criar display em full screen com a resolução detectada
ALLEGRO_DISPLAY* criar_tela_cheia(InformacoesTela tela) {
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    ALLEGRO_DISPLAY* tela_jogo = al_create_display(tela.largura, tela.altura);

    if (tela_jogo) return tela_jogo;

    printf("Falha no true fullscreen. Tentando fullscreen window...\n");
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    tela_jogo = al_create_display(tela.largura, tela.altura);

    if (tela_jogo) return tela_jogo;

    printf("Falha no fullscreen. Tentando modo janela...\n");
    al_set_new_display_flags(ALLEGRO_WINDOWED);
    tela_jogo = al_create_display(1024, 576); // fallback 16:9
    return tela_jogo;
}

// Função para limitar um valor entre um mínimo e máximo
static float limitar_valor(float valor, float minimo, float maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

// Função principal do programa
int main(void) {
    // Inicialização do Allegro
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    al_init_image_addon();

    // Obter resolução do monitor
    InformacoesTela tela = obter_resolucao_tela_atual();
    const int LARGURA_TELA = tela.largura;
    const int ALTURA_TELA = tela.altura;
    const int ALTURA_PERSONAGEM = 250, LARGURA_PERSONAGEM = 250;

    // Criar display em full screen
    ALLEGRO_DISPLAY* tela_jogo = criar_tela_cheia(tela);
    if (!tela_jogo) {
        printf("Falha ao criar display.\n");
        return -1;
    }

    // Carregar imagens
    ALLEGRO_BITMAP* sprite_parado = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiParado.png");
    ALLEGRO_BITMAP* sprite_andando = al_load_bitmap("./imagensJogo/personagens/Odisseu/andandoSemEspada.png");
    ALLEGRO_BITMAP* sprite_desembainhar = al_load_bitmap("./imagensJogo/personagens/Odisseu/odiDesembainhar.png");
    ALLEGRO_BITMAP* imagem_fundo = al_load_bitmap("./imagensJogo/cenarios/submundoProfeta.png");

    if (!sprite_parado || !sprite_andando || !sprite_desembainhar || !imagem_fundo) {
        printf("Erro ao carregar imagens.\n");
        return -1;
    }

    // Configurações de animação do sprite parado
    int total_frames_parado = 5;
    int largura_frame_parado = al_get_bitmap_width(sprite_parado) / total_frames_parado;
    int altura_frame_parado = al_get_bitmap_height(sprite_parado);

    // Configurações de animação do sprite andando
    int total_frames_andando = 6;
    int largura_frame_andando = al_get_bitmap_width(sprite_andando) / total_frames_andando;
    int altura_frame_andando = al_get_bitmap_height(sprite_andando);

    // Configurações da animação de desembainhar
    int total_frames_desembainhar = 7;
    int largura_frame_desembainhar = al_get_bitmap_width(sprite_desembainhar) / total_frames_desembainhar;
    int altura_frame_desembainhar = al_get_bitmap_height(sprite_desembainhar);

    int frame_atual = 0;
    int contador_animacao = 0;

    // Estado do personagem
    int posicao_x = 200;
    int posicao_y = 700;
    bool olhando_para_direita = true;
    bool personagem_andando = false;
    bool animando_transicao = false;

    // Sistema de eventos
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    ALLEGRO_TIMER* temporizador = al_create_timer(1.0 / 60.0);

    al_register_event_source(fila_eventos, al_get_display_event_source(tela_jogo));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(temporizador));

    al_start_timer(temporizador);

    // Loop principal do jogo
    bool jogo_rodando = true;
    bool redesenhar_tela = false;

    while (jogo_rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jogo_rodando = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) jogo_rodando = false;

            // Tecla "E" ativa animação de desembainhar (só se parado e não estiver animando)
            if (evento.keyboard.keycode == ALLEGRO_KEY_E && !personagem_andando && !animando_transicao) {
                animando_transicao = true;
                frame_atual = 0;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            ALLEGRO_KEYBOARD_STATE estado_teclado;
            al_get_keyboard_state(&estado_teclado);

            float direcao_x = 0.0f;

            // Só pode se mover se não estiver animando a transição
            if (!animando_transicao) {
                if (al_key_down(&estado_teclado, ALLEGRO_KEY_A) ||
                    al_key_down(&estado_teclado, ALLEGRO_KEY_LEFT)) {
                    direcao_x -= 0.1f;
                    olhando_para_direita = false;
                }
                if (al_key_down(&estado_teclado, ALLEGRO_KEY_D) ||
                    al_key_down(&estado_teclado, ALLEGRO_KEY_RIGHT)) {
                    direcao_x += 0.1f;
                    olhando_para_direita = true;
                }
            }

            if (direcao_x != 0.0f) {
                float comprimento = sqrtf(direcao_x * direcao_x);
                direcao_x /= comprimento;
            }

            const int VELOCIDADE_PERSONAGEM = 250 / 60;
            posicao_x += direcao_x * VELOCIDADE_PERSONAGEM;

            posicao_y = limitar_valor(posicao_y, 0, ALTURA_TELA - ALTURA_PERSONAGEM);

            personagem_andando = (direcao_x != 0.0f);

            // Atualiza animação
            contador_animacao++;
            if (contador_animacao >= 10) {
                frame_atual++;

                if (animando_transicao) {
                    if (frame_atual >= total_frames_desembainhar) {
                        animando_transicao = false; // terminou a animação
                        frame_atual = 0;            // volta para parado
                    }
                }
                else if (personagem_andando) {
                    frame_atual %= total_frames_andando;
                }
                else {
                    frame_atual %= total_frames_parado;
                }

                contador_animacao = 0;
            }

            redesenhar_tela = true;
        }

        if (redesenhar_tela && al_is_event_queue_empty(fila_eventos)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Fundo
            al_draw_scaled_bitmap(
                imagem_fundo, 0, 0,
                al_get_bitmap_width(imagem_fundo),
                al_get_bitmap_height(imagem_fundo),
                0, 0, LARGURA_TELA, ALTURA_TELA, 0
            );

            // Seleciona o sprite a ser usado
            ALLEGRO_BITMAP* sprite_atual;
            int largura_frame, altura_frame;

            if (animando_transicao) {
                sprite_atual = sprite_desembainhar;
                largura_frame = largura_frame_desembainhar;
                altura_frame = altura_frame_desembainhar;
            }
            else if (personagem_andando) {
                sprite_atual = sprite_andando;
                largura_frame = largura_frame_andando;
                altura_frame = altura_frame_andando;
            }
            else {
                sprite_atual = sprite_parado;
                largura_frame = largura_frame_parado;
                altura_frame = altura_frame_parado;
            }

            int flags = olhando_para_direita ? 0 : ALLEGRO_FLIP_HORIZONTAL;

            al_draw_scaled_bitmap(
                sprite_atual,
                frame_atual * largura_frame, 0,
                largura_frame, altura_frame,
                posicao_x, posicao_y,
                LARGURA_PERSONAGEM, ALTURA_PERSONAGEM,
                flags
            );

            al_flip_display();
            redesenhar_tela = false;
        }
    }

    // Limpeza
    al_destroy_bitmap(sprite_parado);
    al_destroy_bitmap(sprite_andando);
    al_destroy_bitmap(sprite_desembainhar);
    al_destroy_bitmap(imagem_fundo);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(tela_jogo);

    return 0;
}
