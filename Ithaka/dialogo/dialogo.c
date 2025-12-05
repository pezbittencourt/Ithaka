#include "dialogo.h"

static void init_array(ArrayTextBox* arr) {
	arr->a = malloc(sizeof(TextBox));
	arr->usado = 0;
	arr->tamanho = 1;
}

static void insert_array(ArrayTextBox* arr, TextBox e) {
	if (arr->usado == arr->tamanho) {
		arr->tamanho *= 2;
		arr->a = realloc(arr->a, arr->tamanho * sizeof(TextBox));
	}
	arr->a[arr->usado++] = e;
}

bool carregar_dialogo(char* caminho_source, ArrayTextBox* arr_text_box)
{
	ALLEGRO_BITMAP** sprites = malloc(sizeof(ALLEGRO_BITMAP*));
	int sprites_carregados = 0;
	char buffer_linha[512];

	if (arr_text_box->tamanho == 0) {
		init_array(arr_text_box);
	}

	FILE* source_dialogo = fopen(caminho_source,"r");
	
	if (source_dialogo == NULL) {
		printf("ERRO AO ABRIR ARQUIVO DE DIALOGO\n");
		return false;
	}

	while (fgets(buffer_linha, sizeof(buffer_linha), source_dialogo)) {
        printf("%s\n", buffer_linha);
		char* partes[6]; //0-esquerda ou direita, 1-sprite, 2-indice sprite, 3-Titulo, 4-Cor titulo, 5-texto
		int i = 0;
		char* parte = strtok(buffer_linha, "|");
		while (parte != NULL) {
            printf("partes[%d]: %s\n", i, parte);
			partes[i] = parte;
			parte = strtok(NULL, "|");
			i++;
		}
        printf("\n");
		int indice = atoi(partes[2]);

		if (indice > sprites_carregados - 1) {
			sprites = realloc(sprites,++sprites_carregados * sizeof(ALLEGRO_BITMAP*));

			char caminho_sprite[256] = "./imagensJogo/dialogo/perfis/"; 
			strcat(caminho_sprite, partes[1]);
			strcat(caminho_sprite, ".png");
			sprites[indice] = al_load_bitmap(caminho_sprite);
		}

		TextBox dialogo = {
			.esquerda = (strcmp(partes[0], "E") == 0),
			.sprite = sprites[indice],
            .titulo = strdup(partes[3]),
            .cor_titulo_hex = strdup(partes[4]),
			.texto = strdup(partes[5])
		};

		insert_array(arr_text_box, dialogo);
	}

	fclose(source_dialogo);
	return true;
}

bool desenhar_dialogo(ALLEGRO_DISPLAY* display, Cenario cenario, ArrayTextBox textos)
{
    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    float escala_x = tela_largura / 1920.0f;
    float escala_y = tela_altura / 1080.0f;

    float largura_perfil = 960 * escala_x;
    float altura_perfil = 960 * escala_y;

    float posisao_perfil_direita_x = 1060 * escala_x;
    float posicao_perfil_y = 235 * escala_y;
    float texto_y = tela_altura - (tela_altura * 0.20f);
    float titulo_y = tela_altura - (tela_altura * 0.27f);

    ALLEGRO_BITMAP* fundo = cenario.fundo;
    if (!fundo) {
        printf("Erro ao carregar imagem do cenario.\n");
        return false;
    }
    ALLEGRO_BITMAP* caixa_dialogo = al_load_bitmap("./imagensJogo/dialogo/caixa_de_dialogo.png");
    if (!caixa_dialogo) {
        printf("Erro ao carregar imagem da caixa de dialogo.\n");
        return false;
    }
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        al_destroy_bitmap(fundo);
        return false;
    }
    ALLEGRO_FONT* fonte = al_load_ttf_font("./fontes/arial.ttf", 24, 0);

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));

    bool rodando = true;
    bool redesenhar = true;
    int caixa_atual = 0;

    while (rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                rodando = false;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1) {
                caixa_atual++;
                if (caixa_atual == textos.usado) {
                    rodando = false;
                    redesenhar = false;
                }
                else {
                    redesenhar = true;
                }
               
            }
        }

        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_draw_scaled_bitmap(
                fundo,
                0, 0,
                al_get_bitmap_width(fundo),
                al_get_bitmap_height(fundo),
                0, 0,
                tela_largura, tela_altura,
                0
            );

            al_draw_scaled_bitmap(
                caixa_dialogo,
                0, 0,
                al_get_bitmap_width(caixa_dialogo),
                al_get_bitmap_height(caixa_dialogo),
                0, 0,
                tela_largura, tela_altura,
                0
            );

            TextBox dialogo_atual = textos.a[caixa_atual];

            al_draw_scaled_bitmap(
                dialogo_atual.sprite,
                0, 0,
                al_get_bitmap_width(dialogo_atual.sprite),
                al_get_bitmap_height(dialogo_atual.sprite),
                dialogo_atual.esquerda ? -100 : posisao_perfil_direita_x , posicao_perfil_y,
                largura_perfil, altura_perfil,
                0
            );

            float texto_x = dialogo_atual.esquerda ? (tela_altura / 2.0f) + 200.0f * escala_x: (tela_altura / 2.0f) - 200.0f * escala_x;

            al_draw_multiline_text(fonte, al_map_rgb(255, 255, 255),
                texto_x, texto_y,
                (tela_largura * 0.50f) , al_get_font_line_height(fonte),
                0, dialogo_atual.texto);

            float titulo_x = dialogo_atual.esquerda ? (tela_altura / 2.0f) - 25.0f * escala_x : (tela_altura / 2.0f) + 650.0f * escala_x;

            al_draw_filled_rectangle(
                titulo_x - 10, titulo_y - 5,
                titulo_x + al_get_text_width(fonte, dialogo_atual.titulo) + 10, titulo_y + al_get_font_line_height(fonte) + 5,
                al_map_rgb(0, 0, 0));
            al_draw_rectangle(
                titulo_x - 10, titulo_y - 5,
                titulo_x + al_get_text_width(fonte, dialogo_atual.titulo) + 10, titulo_y + al_get_font_line_height(fonte) + 5,
                al_map_rgb(255, 255, 255), 4);

            al_draw_multiline_text(fonte, al_color_html(dialogo_atual.cor_titulo_hex),
                titulo_x, titulo_y,
                (tela_largura * 0.50f), al_get_font_line_height(fonte),
                0, dialogo_atual.titulo);

            al_flip_display();
        }
    }

    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(caixa_dialogo);

    return true;
}

void free_array_dialogo(ArrayTextBox* arr_text_box)
{
	free(arr_text_box->a);
	arr_text_box->a = NULL;
	arr_text_box->usado = arr_text_box->tamanho = 0;
}
