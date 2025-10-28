#include "mapa.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int x, y;
    int largura, altura;
    int fase_id;
    const char* nome;
    bool mouse_sobre;
    ALLEGRO_COLOR cor_normal;
    ALLEGRO_COLOR cor_destaque;
} AreaMapa;

int exibir_mapa_inicial(ALLEGRO_DISPLAY* display) {
    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    float escala_x = tela_largura / 1920.0f;
    float escala_y = tela_altura / 1080.0f;

    AreaMapa areas[] = {
        {
            .x = 177 * escala_x,
            .y = 153 * escala_y,
            .largura = 178 * escala_x,
            .altura = 135 * escala_y,
            .fase_id = MAPA_FASE_SUBMUNDO,
            .nome = "SUBIMUNDO",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(100, 100, 100, 150),
            .cor_destaque = al_map_rgba(150, 150, 150, 100)
        },
        {
            .x = 485 * escala_x,
            .y = 626 * escala_y,
            .largura = 165 * escala_x,
            .altura = 148 * escala_y,
            .fase_id = MAPA_FASE_POLIFEMO,
            .nome = "POLIFEMO",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(255, 165, 0, 150),
            .cor_destaque = al_map_rgba(255, 200, 0, 100)
        },
        {
            .x = 270 * escala_x,
            .y = 307 * escala_y,
            .largura = 109 * escala_x,
            .altura = 114 * escala_y,
            .fase_id = MAPA_FASE_CIRCE,
            .nome = "CIRCE",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(200, 150, 200, 150),
            .cor_destaque = al_map_rgba(255, 100, 255, 100)
        },
        {
            .x = 875 * escala_x,
            .y = 600 * escala_y,
            .largura = 99 * escala_x,
            .altura = 121 * escala_y,
            .fase_id = MAPA_FASE_ITACA,
            .nome = "ITACA",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(50, 150, 50, 150),
            .cor_destaque = al_map_rgba(100, 255, 100, 100)
        },
        {
            .x = 704 * escala_x,
            .y = 680 * escala_y,
            .largura = 153 * escala_x,
            .altura = 121 * escala_y,
            .fase_id = MAPA_FASE_POSEIDON,
            .nome = "POSEIDON",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(0, 100, 200, 150),
            .cor_destaque = al_map_rgba(100, 200, 255, 100)
        },
        {
            .x = tela_largura * 0.45,
            .y = tela_altura * 0.15,
            .largura = 130 * escala_x,
            .altura = 50 * escala_y,
            .fase_id = MAPA_FASE_OLIMPO,
            .nome = "OLIMPO",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(255, 255, 100, 150),
            .cor_destaque = al_map_rgba(255, 255, 200, 100)
        }
    };

    int num_areas = sizeof(areas) / sizeof(areas[0]);

    for (int i = 0; i < num_areas; i++) {
        printf("[MAPA] Área %s: X=%.0f Y=%.0f L=%.0f A=%.0f\n",
            areas[i].nome,
            areas[i].x, areas[i].y,
            areas[i].largura, areas[i].altura);
    }

    ALLEGRO_BITMAP* mapa = al_load_bitmap("./imagensJogo/cenarios/mapa/mapaInicial.png");
    if (!mapa) {
        printf("Erro ao carregar imagem do mapa.\n");
        return MAPA_SAIR;
    }

    if (!al_init_primitives_addon()) {
        printf("Erro ao inicializar primitivas.\n");
        al_destroy_bitmap(mapa);
        return MAPA_SAIR;
    }

    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        al_destroy_bitmap(mapa);
        return MAPA_SAIR;
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));

    bool rodando = true;
    int escolha = MAPA_VOLTAR_MENU;
    bool redesenhar = true;

    while (rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
            escolha = MAPA_SAIR;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                printf("[MAPA] ESC pressionado - Voltando ao menu\n");
                rodando = false;
                escolha = MAPA_VOLTAR_MENU;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            int mouse_x = evento.mouse.x;
            int mouse_y = evento.mouse.y;
            bool mudou_estado = false;

            for (int i = 0; i < num_areas; i++) {
                bool estava_sobre = areas[i].mouse_sobre;
                areas[i].mouse_sobre = (mouse_x >= areas[i].x &&
                    mouse_x <= areas[i].x + areas[i].largura &&
                    mouse_y >= areas[i].y &&
                    mouse_y <= areas[i].y + areas[i].altura);

                if (estava_sobre != areas[i].mouse_sobre) {
                    mudou_estado = true;
                    if (areas[i].mouse_sobre) {
                        printf("[MAPA] Mouse sobre %s\n", areas[i].nome);
                    }
                }
            }

            if (mudou_estado) {
                redesenhar = true;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                int mouse_x = evento.mouse.x;
                int mouse_y = evento.mouse.y;
                bool clicou_em_algo = false;

                for (int i = 0; i < num_areas; i++) {
                    if (mouse_x >= areas[i].x && mouse_x <= areas[i].x + areas[i].largura &&
                        mouse_y >= areas[i].y && mouse_y <= areas[i].y + areas[i].altura) {

                        printf("[MAPA] Clique em %s - Iniciando fase %d\n",
                            areas[i].nome, areas[i].fase_id);
                        rodando = false;
                        escolha = areas[i].fase_id;
                        clicou_em_algo = true;
                        break;
                    }
                }

                if (!clicou_em_algo) {
                    printf("[MAPA] Clique fora das áreas (X=%d Y=%d)\n", mouse_x, mouse_y);
                }
            }
        }

        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_draw_scaled_bitmap(
                mapa,
                0, 0,
                al_get_bitmap_width(mapa),
                al_get_bitmap_height(mapa),
                0, 0,
                tela_largura, tela_altura,
                0
            );

            for (int i = 0; i < num_areas; i++) {
                if (areas[i].mouse_sobre) {
                    al_draw_filled_rectangle(
                        areas[i].x, areas[i].y,
                        areas[i].x + areas[i].largura,
                        areas[i].y + areas[i].altura,
                        areas[i].cor_destaque
                    );

                    al_draw_rectangle(
                        areas[i].x, areas[i].y,
                        areas[i].x + areas[i].largura,
                        areas[i].y + areas[i].altura,
                        areas[i].cor_normal,
                        3
                    );
                }
                else {
                    al_draw_rectangle(
                        areas[i].x, areas[i].y,
                        areas[i].x + areas[i].largura,
                        areas[i].y + areas[i].altura,
                        areas[i].cor_normal,
                        1
                    );
                }
            }

            al_flip_display();
        }
    }

    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(mapa);

    return escolha;
}