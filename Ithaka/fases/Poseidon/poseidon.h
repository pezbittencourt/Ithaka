#ifndef POSEIDON_H
#define POSEIDON_H

#include <allegro5/allegro.h>
#include <allegro5/mouse.h>
#include <stdbool.h>
#include "../../fase.h"
#include "../../informacoes_tela.h"

typedef struct {
	int x, y;
} Celula;

typedef struct {
	Celula shots[100];
	int shot_count;
} Estado_Computador;

typedef struct {
	int length;
	bool placed;
} Navio;

// Funções principais
bool carregar_cenarios_poseidon(Fase* fase);
void update_battleship(ALLEGRO_MOUSE_STATE mouse,bool mouse_left,bool mouse_right, int player_board[10][10], int enemy_board[10][10], bool* player_turn, bool* setup_phase, Navio ships[], int ship_count, bool* vertical, Estado_Computador* ai, bool* player_won, bool* enemy_won, InformacoesTela tela);
void draw_battleship(ALLEGRO_BITMAP* water, ALLEGRO_BITMAP* ship, ALLEGRO_BITMAP* hit, ALLEGRO_BITMAP* miss, int player_board[10][10], int enemy_board[10][10], bool setup_phase, Navio ships[],int ship_count, ALLEGRO_MOUSE_STATE mouse, bool vertical, bool player_won, bool enemy_won, InformacoesTela tela);
void com_config_inicial(int board[10][10], Navio ships[], int count);
#endif