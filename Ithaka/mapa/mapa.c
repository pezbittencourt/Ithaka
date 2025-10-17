#include "mapa.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdbool.h>

int exibir_mapa_inicial(ALLEGRO_DISPLAY* display) {
    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    // ============ ÁREA 1: POLIFEMO ============
    int area_largura = 150;
    int area_altura = 120;

    int areaX_polifemo = (tela_largura / 3.4) - (area_largura / 2);
    int areaY_polifemo = (tela_altura / 1.55) - (area_altura / 2);

    printf("[MAPA] Área Polifemo: X=%d Y=%d L=%d A=%d\n",
        areaX_polifemo, areaY_polifemo, area_largura, area_altura);

    // ============ ÁREA 2: CIRCE (CENTRO) ============
    int area_circe_largura = 200;
    int area_circe_altura = 100;

    int areaX_circe = (tela_largura / 2) - (area_circe_largura / 2);
    int areaY_circe = (tela_altura / 2) - (area_circe_altura / 2);

    printf("[MAPA] Área Circe (Centro): X=%d Y=%d L=%d A=%d\n",
        areaX_circe, areaY_circe, area_circe_largura, area_circe_altura);

    // Carregar imagem do mapa
    ALLEGRO_BITMAP* mapa = al_load_bitmap("./imagensJogo/cenarios/mapa/mapaInicial.png");
    if (!mapa) {
        printf("Erro ao carregar imagem do mapa.\n");
        return MAPA_SAIR;
    }

    // Inicializar addon de primitivas
    if (!al_init_primitives_addon()) {
        printf("Erro ao inicializar primitivas.\n");
        al_destroy_bitmap(mapa);
        return MAPA_SAIR;
    }

    // Criar fila de eventos
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        al_destroy_bitmap(mapa);
        return MAPA_SAIR;
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));

    bool rodando = true;
    int escolha = MAPA_VOLTAR_MENU;  // Padrão: voltar ao menu
    bool mouse_sobre_polifemo = false;
    bool mouse_sobre_circe = false;
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

            // Verificar área Polifemo
            bool estava_sobre_polifemo = mouse_sobre_polifemo;
            mouse_sobre_polifemo = (mouse_x >= areaX_polifemo &&
                mouse_x <= areaX_polifemo + area_largura &&
                mouse_y >= areaY_polifemo &&
                mouse_y <= areaY_polifemo + area_altura);

            // Verificar área Circe (centro)
            bool estava_sobre_circe = mouse_sobre_circe;
            mouse_sobre_circe = (mouse_x >= areaX_circe &&
                mouse_x <= areaX_circe + area_circe_largura &&
                mouse_y >= areaY_circe &&
                mouse_y <= areaY_circe + area_circe_altura);

            // Redesenhar se qualquer estado mudou
            if (estava_sobre_polifemo != mouse_sobre_polifemo ||
                estava_sobre_circe != mouse_sobre_circe) {
                redesenhar = true;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {  // Botão esquerdo
                int mouse_x = evento.mouse.x;
                int mouse_y = evento.mouse.y;

                // PRIORIDADE 1: Verificar clique em CIRCE (centro)
                bool clicou_circe = (mouse_x >= areaX_circe &&
                    mouse_x <= areaX_circe + area_circe_largura &&
                    mouse_y >= areaY_circe &&
                    mouse_y <= areaY_circe + area_circe_altura);

                if (clicou_circe) {
                    printf("[MAPA] Clique em CIRCE (centro) - Iniciando fase...\n");
                    rodando = false;
                    escolha = MAPA_FASE_CIRCE;
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
                        escolha = MAPA_FASE_POLIFEMO; 
                    }
                    else {
                        printf("[MAPA] Clique fora das áreas (X=%d Y=%d)\n",
                            mouse_x, mouse_y);
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

            // ============ DESENHAR ÁREA CIRCE (CENTRO) ============
            if (mouse_sobre_circe) {
                // Rosa/Roxo para Circe (mágica/feiticeira)
                al_draw_filled_rectangle(
                    areaX_circe, areaY_circe,
                    areaX_circe + area_circe_largura,
                    areaY_circe + area_circe_altura,
                    al_map_rgba(200, 50, 200, 100)  // Roxo semi-transparente
                );

                al_draw_rectangle(
                    areaX_circe, areaY_circe,
                    areaX_circe + area_circe_largura,
                    areaY_circe + area_circe_altura,
                    al_map_rgba(255, 100, 255, 255),  // Borda rosa/roxo
                    5
                );
            }
            else {
                al_draw_rectangle(
                    areaX_circe, areaY_circe,
                    areaX_circe + area_circe_largura,
                    areaY_circe + area_circe_altura,
                    al_map_rgba(200, 150, 200, 150),  // Borda lilás clara
                    3
                );
            }

            al_flip_display();
        }
    }

    // Limpar recursos
    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(mapa);

    return escolha;  // Retorna o código da escolha
}