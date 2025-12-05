#ifndef DIALOGO_H
#define DIALOGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../fase.h"

typedef struct TextBox {
	bool esquerda;
	ALLEGRO_BITMAP* sprite;
	char* texto, *titulo, *cor_titulo_hex;
} TextBox;

typedef struct ArrayTextBox {
	TextBox* a;
	size_t usado;
	size_t tamanho;
} ArrayTextBox;

bool carregar_dialogo(char* caminho_source, ArrayTextBox* arr_text_box);
bool desenhar_dialogo(ALLEGRO_DISPLAY* display, Cenario cenario, ArrayTextBox textos);
void free_array_dialogo(ArrayTextBox* arr_text_box);
#endif