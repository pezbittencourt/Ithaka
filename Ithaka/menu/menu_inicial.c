﻿#include "menu_inicial.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>



bool menu_inicial(ALLEGRO_DISPLAY* display) {
    // Obter dimensões da tela
    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    // Timer para animação
    ALLEGRO_TIMER* timer = al_create_timer(0.08);
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_start_timer(timer);

    // Carregar imagens
    ALLEGRO_BITMAP* fundo_menu = al_load_bitmap("./imagensJogo/cenarios/menu/Fundo_Menu.png");
    ALLEGRO_BITMAP* ithaka_Logo = al_load_bitmap("./imagensJogo/cenarios/menu/Ithaka_Logo.png");
    ALLEGRO_BITMAP* fundo_branco = al_load_bitmap("./imagensJogo/cenarios/menu/Fundo_Branco_Transparente.png");
    ALLEGRO_BITMAP* click_sair = al_load_bitmap("./imagensJogo/cenarios/menu/click_Menu.png");
    ALLEGRO_BITMAP* click_iniciar = al_load_bitmap("./imagensJogo/cenarios/menu/click_Iniciar.png");

    if (!fundo_menu || !ithaka_Logo || !fundo_branco || !click_sair || !click_iniciar) {
        fprintf(stderr, "Erro ao carregar imagens do menu.\n");
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        return false;
    }

    // Configuração dos frames
    int num_frames = 4;
    int frame_menu = 0;
    int frame_iniciar = 0;
    int largura_frame_menu = al_get_bitmap_width(click_sair) / num_frames;
    int altura_frame_menu = al_get_bitmap_height(click_sair);
    int largura_frame_iniciar = al_get_bitmap_width(click_iniciar) / num_frames;
    int altura_frame_iniciar = al_get_bitmap_height(click_iniciar);

    // Hitbox click_iniciar (centro)
    int hitbox_iniciar_largura = 400;
    int hitbox_iniciar_altura = 120;
    int hitbox_iniciar_x = tela_largura / 2 - hitbox_iniciar_largura / 2;
    int hitbox_iniciar_y = tela_altura / 2 - hitbox_iniciar_altura / 2;

    // Hitbox click_sair (30% abaixo do centro)
    int hitbox_sair_largura = 400;
    int hitbox_sair_altura = 120;
    int hitbox_sair_x = tela_largura / 2 - hitbox_sair_largura / 2;
    int hitbox_sair_y = tela_altura / 2 + (tela_altura * 0.18) - hitbox_sair_altura / 2;

    bool rodando = true;
    bool mouse_sobre_iniciar = false;
    bool mouse_sobre_sair = false;
    bool resultado = false; // false = sair, true = iniciar jogo

    while (rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        // ESC para sair
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                rodando = false;
                resultado = false;
            }
        }

        // Atualizar estado do mouse
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = evento.mouse.x;
            int my = evento.mouse.y;

            mouse_sobre_iniciar = mx >= hitbox_iniciar_x && mx <= hitbox_iniciar_x + hitbox_iniciar_largura &&
                my >= hitbox_iniciar_y && my <= hitbox_iniciar_y + hitbox_iniciar_altura;

            mouse_sobre_sair = mx >= hitbox_sair_x && mx <= hitbox_sair_x + hitbox_sair_largura &&
                my >= hitbox_sair_y && my <= hitbox_sair_y + hitbox_sair_altura;

            // Detectar cliques
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && evento.mouse.button == 1) {
                if (mouse_sobre_iniciar) {
                    rodando = false;
                    resultado = true; // Iniciar jogo
                }
                else if (mouse_sobre_sair) {
                    rodando = false;
                    resultado = false; // Sair
                }
            }
        }

        // Atualizar animação
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            // click_iniciar
            if (mouse_sobre_iniciar && frame_iniciar < num_frames - 1) frame_iniciar++;
            else if (!mouse_sobre_iniciar && frame_iniciar > 0) frame_iniciar--;

            // click_sair
            if (mouse_sobre_sair && frame_menu < num_frames - 1) frame_menu++;
            else if (!mouse_sobre_sair && frame_menu > 0) frame_menu--;

            // Desenhar
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_scaled_bitmap(fundo_menu, 0, 0, al_get_bitmap_width(fundo_menu), al_get_bitmap_height(fundo_menu),
                0, 0, tela_largura, tela_altura, 0);
            al_draw_scaled_bitmap(ithaka_Logo, 0, 0, al_get_bitmap_width(ithaka_Logo), al_get_bitmap_height(ithaka_Logo),
                0, 0, tela_largura, tela_altura, 0);
            al_draw_scaled_bitmap(fundo_branco, 0, 0, al_get_bitmap_width(fundo_branco), al_get_bitmap_height(fundo_branco),
                0, 0, tela_largura, tela_altura, 0);

            // Desenhar sprites
            al_draw_bitmap_region(click_sair, frame_menu * largura_frame_menu, 0, largura_frame_menu, altura_frame_menu, 0, 0, 0);
            al_draw_bitmap_region(click_iniciar, frame_iniciar * largura_frame_iniciar, 0, largura_frame_iniciar, altura_frame_iniciar, 0, 0, 0);

            al_flip_display();
        }
    }

    // Limpar recursos
    al_destroy_bitmap(fundo_menu);
    al_destroy_bitmap(ithaka_Logo);
    al_destroy_bitmap(fundo_branco);
    al_destroy_bitmap(click_sair);
    al_destroy_bitmap(click_iniciar);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);

    return resultado;
}
