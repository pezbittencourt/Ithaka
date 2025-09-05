#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int main(void) {
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;

    al_init_image_addon();

    const int LARGURA = 1920, ALTURA = 1080;
    const int alturaOdisseu = 300, larguraOdisseu = 300;

    ALLEGRO_DISPLAY* disp = al_create_display(LARGURA, ALTURA);
    if (!disp) { printf("Falha ao criar display.\n"); return -1; }

    // Carregar apenas o frame parado
    ALLEGRO_BITMAP* odisseu = al_load_bitmap("./Odisseu/odiparado1.png");
    ALLEGRO_BITMAP* fundo = al_load_bitmap("./cenarios/submundoProfeta.png");

    if (!odisseu) { printf("Erro ao carregar odisseu\n"); return -1; }
    if (!fundo) { printf("Erro ao carregar fundo\n"); return -1; }

    int pw = al_get_bitmap_width(odisseu);
    int ph = al_get_bitmap_height(odisseu);

    int x = 200, y = 700;

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); // 60 FPS

    al_register_event_source(fila, al_get_display_event_source(disp));
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));

    al_start_timer(timer);

    bool sair = false;
    bool redraw = false;        

    while (!sair) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) sair = true;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            ALLEGRO_KEYBOARD_STATE ks;
            al_get_keyboard_state(&ks);

            float dx = 0.0f;
            if (al_key_down(&ks, ALLEGRO_KEY_A) || al_key_down(&ks, ALLEGRO_KEY_LEFT))  dx -= 0.1f;
            if (al_key_down(&ks, ALLEGRO_KEY_D) || al_key_down(&ks, ALLEGRO_KEY_RIGHT)) dx += 0.1f;

            if (dx != 0.0f) {
                float len = sqrtf(dx * dx);
                dx /= len;
            }

            const int SPEED = 150 / 60;
            x += dx * SPEED;

            y = clampf(y, 0, ALTURA - alturaOdisseu);

            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(fila)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(fundo, 0, 0, 0);

            // Desenha o Odisseu parado
            al_draw_scaled_bitmap(
                odisseu,
                0, 0, pw, ph,
                x, y,
                larguraOdisseu, alturaOdisseu,
                0
            );

            al_flip_display();
            redraw = false;
        }
    }

    // Limpeza
    al_destroy_bitmap(odisseu);
    al_destroy_bitmap(fundo);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila);
    al_destroy_display(disp);

    return 0;
}
