#include "mapa.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdbool.h>

bool exibir_mapa_inicial(ALLEGRO_DISPLAY* display) {
    // Obter dimensões da tela
    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    // ============ ÁREA 1: POLIFEMO ============
    int area_largura = 150;
    int area_altura = 120;

    int areaX_polifemo = (tela_largura / 3.4) - (area_largura / 2);
    int areaY_polifemo = (tela_altura / 1.55) - (area_altura / 2);

    printf("[MAPA] Área Polifemo: X=%d Y=%d L=%d A=%d\n",
        areaX_polifemo, areaY_polifemo, area_largura, area_altura);

    // ============ ÁREA 2: MENU (CENTRO) ============
    int area_menu_largura = 200;
    int area_menu_altura = 100;

    int areaX_menu = (tela_largura / 2) - (area_menu_largura / 2);
    int areaY_menu = (tela_altura / 2) - (area_menu_altura / 2);

    printf("[MAPA] Área Menu: X=%d Y=%d L=%d A=%d\n",
        areaX_menu, areaY_menu, area_menu_largura, area_menu_altura);

    // Carregar imagem do mapa
    ALLEGRO_BITMAP* mapa = al_load_bitmap("./imagensJogo/cenarios/mapa/mapaInicial.png");
    if (!mapa) {
        printf("Erro ao carregar imagem do mapa.\n");
        return false;
    }

    // Inicializar addon de primitivas
    if (!al_init_primitives_addon()) {
        printf("Erro ao inicializar primitivas.\n");
        al_destroy_bitmap(mapa);
        return false;
    }

    // Criar fila de eventos
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        al_destroy_bitmap(mapa);
        return false;
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));

    bool rodando = true;
    bool confirmar = false;
    bool mouse_sobre_polifemo = false;
    bool mouse_sobre_menu = false;
    bool redesenhar = true;

    while (rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
            confirmar = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                printf("[MAPA] ESC pressionado - Voltando ao menu\n");
                rodando = false;
                confirmar = false;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            int mouse_x = evento.mouse.x;
            int mouse_y = evento.mouse.y;

            // Verificar área Polifemo
            bool estava_sobre_polifemo = mouse_sobre_polifemo;
            mouse_sobre_polifemo = (mouse_x >= areaX_polifemo &&
                mouse_x <= areaX_polifemo + area_largura &&
                mouse_y >= areaY_polifemo &&
                mouse_y <= areaY_polifemo + area_altura);

            // Verificar área Menu
            bool estava_sobre_menu = mouse_sobre_menu;
            mouse_sobre_menu = (mouse_x >= areaX_menu &&
                mouse_x <= areaX_menu + area_menu_largura &&
                mouse_y >= areaY_menu &&
                mouse_y <= areaY_menu + area_menu_altura);

            // Redesenhar se qualquer estado mudou
            if (estava_sobre_polifemo != mouse_sobre_polifemo ||
                estava_sobre_menu != mouse_sobre_menu) {
                redesenhar = true;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {  // Botão esquerdo
                int mouse_x = evento.mouse.x;
                int mouse_y = evento.mouse.y;

                // PRIORIDADE 1: Verificar clique no MENU primeiro
                bool clicou_menu = (mouse_x >= areaX_menu &&
                    mouse_x <= areaX_menu + area_menu_largura &&
                    mouse_y >= areaY_menu &&
                    mouse_y <= areaY_menu + area_menu_altura);

                if (clicou_menu) {
                    printf("[MAPA] Clique no MENU - Voltando...\n");
                    rodando = false;
                    confirmar = false;  // Retorna false = voltar ao menu
                }
                // PRIORIDADE 2: Verificar clique em POLIFEMO
                else {
                    bool clicou_polifemo = (mouse_x >= areaX_polifemo &&
                        mouse_x <= areaX_polifemo + area_largura &&
                        mouse_y >= areaY_polifemo &&
                        mouse_y <= areaY_polifemo + area_altura);

                    if (clicou_polifemo) {
                        printf("[MAPA] Clique em POLIFEMO - Iniciando jogo...\n");
                        rodando = false;
                        confirmar = true;  // Retorna true = iniciar jogo
                    }
                    else {
                        printf("[MAPA] Clique fora (X=%d Y=%d)\n", mouse_x, mouse_y);
                    }
                }
            }
        }

        // Desenhar apenas quando necessário
        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Desenhar mapa ajustado à tela
            al_draw_scaled_bitmap(
                mapa,
                0, 0,
                al_get_bitmap_width(mapa),
                al_get_bitmap_height(mapa),
                0, 0,
                tela_largura, tela_altura,
                0
            );

            // ============ DESENHAR ÁREA POLIFEMO ============
            if (mouse_sobre_polifemo) {
                al_draw_filled_rectangle(
                    areaX_polifemo, areaY_polifemo,
                    areaX_polifemo + area_largura,
                    areaY_polifemo + area_altura,
                    al_map_rgba(255, 255, 0, 80)  // Amarelo
                );

                al_draw_rectangle(
                    areaX_polifemo, areaY_polifemo,
                    areaX_polifemo + area_largura,
                    areaY_polifemo + area_altura,
                    al_map_rgba(255, 255, 0, 255),
                    5
                );
            }
            else {
                al_draw_rectangle(
                    areaX_polifemo, areaY_polifemo,
                    areaX_polifemo + area_largura,
                    areaY_polifemo + area_altura,
                    al_map_rgba(255, 255, 255, 150),
                    3
                );
            }

            // ============ DESENHAR ÁREA MENU ============
            if (mouse_sobre_menu) {
                al_draw_filled_rectangle(
                    areaX_menu, areaY_menu,
                    areaX_menu + area_menu_largura,
                    areaY_menu + area_menu_altura,
                    al_map_rgba(255, 50, 50, 100)  // Vermelho
                );

                al_draw_rectangle(
                    areaX_menu, areaY_menu,
                    areaX_menu + area_menu_largura,
                    areaY_menu + area_menu_altura,
                    al_map_rgba(255, 0, 0, 255),
                    5
                );
            }
            else {
                al_draw_rectangle(
                    areaX_menu, areaY_menu,
                    areaX_menu + area_menu_largura,
                    areaY_menu + area_menu_altura,
                    al_map_rgba(200, 200, 200, 150),
                    3
                );
            }

            al_flip_display();
        }
    }

    // Limpar recursos
    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(mapa);

    return confirmar;
}