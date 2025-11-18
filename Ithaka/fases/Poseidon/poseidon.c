#include "poseidon.h"
#include "stdio.h"

static bool can_place_ship(int board[10][10], int x, int y, int len, bool vertical) {
	for (int i = 0; i < len; i++) {
		int cx = x + (vertical ? 0 : i);
		int cy = y + (vertical ? i : 0);
		if (cx < 0 || cy < 0 || cx >= 10 || cy >= 10) return false;
		if (board[cy][cx] != 0) return false;
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				int nx = cx + dx;
				int ny = cy + dy;
				if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10) {
					if (board[ny][nx] == 1) return false;
				}
			}
		}
	}
	return true;
}



static void ai_shoot(int player_board[10][10], Estado_Computador* ai) {
	while (1) {
		int x = rand() % 10;
		int y = rand() % 10;
		bool used = false;
		for (int i = 0; i < ai->shot_count; i++) {
			if (ai->shots[i].x == x && ai->shots[i].y == y) {
				used = true;
				break;
			}
		}
		if (used) continue;

		ai->shots[ai->shot_count++] = (Celula){ x, y };

		if (player_board[y][x] == 1) player_board[y][x] = 2;
		else if (player_board[y][x] == 0) player_board[y][x] = 3;

		break;
	}
}

static bool check_victory(int board[10][10]) {
	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++)
			if (board[y][x] == 1)
				return false;
	return true;
}

static void draw_ship_ghost(ALLEGRO_BITMAP* ghost, int mouse_x, int mouse_y, int length, bool vertical, InformacoesTela tela) {
	int px = (tela.largura / 2) - (320 + 40 / 2);
	int gy = (tela.altura / 2) - (320 / 2);
	
	int cx = (mouse_x - px) / 32;
	int cy = (mouse_y - gy)  / 32;
	for (int i = 0; i < length; i++) {
		int x = cx + (vertical ? 0 : i);
		int y = cy + (vertical ? i : 0);
		if (x < 0 || x >= 10 || y < 0 || y >= 10) continue;
		al_draw_tinted_bitmap(ghost, al_map_rgba(255, 255, 255, 120), px + x * 32, gy + y * 32, 0);
	}
}


bool carregar_cenarios_poseidon(Fase* fase)
{
	if (!fase) return false;
	// Initialize the fase with 6 scenarios
	init_fase(fase, 1);

	// Verify allocation was successful
	if (!fase->cenarios || fase->total_cenarios < 1) {
		fprintf(stderr, "Erro: falha na inicialização da fase\n");
		return false;
	}

	printf("Inicializando cenário 1...\n");
	init_cenario(&fase->cenarios[0], 0, 0);
	fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Poseidon/fundoPoseidon.png");
	if (!fase->cenarios[0].fundo) {
		fprintf(stderr, "❌ Erro ao carregar fundo do cenário 1\n");
		return false;
	}

	return true;
}

void update_battleship(ALLEGRO_MOUSE_STATE mouse, bool mouse_left, bool mouse_right, int player_board[10][10],
	int enemy_board[10][10], bool* player_turn, bool* setup_phase, Navio ships[], int ship_count, bool* vertical,
	Estado_Computador* ai, bool* player_won, bool* enemy_won, InformacoesTela tela)
{
	int px = (tela.largura / 2) - (320 + 40 / 2);
	int ex = (tela.largura / 2) + (40 / 2);
	int gy = (tela.altura / 2) - (320 / 2);

	if (*setup_phase) {
		if (mouse_right) *vertical = !*vertical;
		if (mouse_left) {
			int cx = (mouse.x - px) / 32;
			int cy = (mouse.y - gy) / 32;
			for (int s = 0; s < ship_count; s++) {
				if (!ships[s].placed) {
					if (can_place_ship(player_board, cx, cy, ships[s].length, *vertical)) {
						for (int i = 0; i < ships[s].length; i++) {
							int px = cx + (*vertical ? 0 : i);
							int py = cy + (*vertical ? i : 0);
							player_board[py][px] = 1;
						}
						ships[s].placed = true;
						bool done = true;
						for (int j = 0; j < ship_count; j++)
							if (!ships[j].placed) done = false;
						if (done) *setup_phase = false;
					}
					break;
				}
			}
		}
		return;
	}

	if (!*player_won && !*enemy_won) {
		if (*player_turn && mouse_left) {
			int cx = (mouse.x - ex) / 32;
			int cy = (mouse.y - gy) / 32;
			if (cx >= 0 && cx < 10 && cy >= 0 && cy < 10) {
				if (enemy_board[cy][cx] == 0) enemy_board[cy][cx] = 3;
				else if (enemy_board[cy][cx] == 1) enemy_board[cy][cx] = 2;
				*player_turn = false;
			}
		}

		if (!*player_turn) {
			ai_shoot(player_board, ai);
			*player_turn = true;
		}

		if (check_victory(enemy_board)) *player_won = true;
		if (check_victory(player_board)) *enemy_won = true;
	}
}

void draw_battleship(ALLEGRO_BITMAP* water, ALLEGRO_BITMAP* ship, ALLEGRO_BITMAP* hit, ALLEGRO_BITMAP* miss, int player_board[10][10], int enemy_board[10][10], bool setup_phase, Navio ships[],
	int ship_count, ALLEGRO_MOUSE_STATE mouse, bool vertical, bool player_won, bool enemy_won, InformacoesTela tela) {

	int px = (tela.largura / 2) - (320 + 40 / 2);
	int ex = (tela.largura / 2) + (40 / 2);
	int gy = (tela.altura / 2) - (320 / 2);

	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++) {
			int v = player_board[y][x];
			ALLEGRO_BITMAP* bmp = water;
			if (v == 1) bmp = ship;
			else if (v == 2) bmp = hit;
			else if (v == 3) bmp = miss;
			al_draw_bitmap(bmp, px + x * 32, gy + y * 32, 0);
		}

	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++) {
			int v = enemy_board[y][x];
			ALLEGRO_BITMAP* bmp = water;
			if (!setup_phase) {
				if (v == 2) bmp = hit;
				else if (v == 3) bmp = miss;
			}
			al_draw_bitmap(bmp, ex + x * 32, gy + y * 32, 0);
		}

	if (setup_phase) {
		for (int s = 0; s < ship_count; s++) {
			if (!ships[s].placed) {
				draw_ship_ghost(ship, mouse.x, mouse.y, ships[s].length, vertical, tela);
				break;
			}
		}
	}
}

void com_config_inicial(int board[10][10], Navio ships[], int count) {
	for (int s = 0; s < count; s++) {
		while (!ships[s].placed) {
			int x = rand() % 10;
			int y = rand() % 10;
			bool vertical = rand() % 2;
			if (can_place_ship(board, x, y, ships[s].length, vertical)) {
				for (int i = 0; i < ships[s].length; i++) {
					int cx = x + (vertical ? 0 : i);
					int cy = y + (vertical ? i : 0);
					board[cy][cx] = 1;
				}
				ships[s].placed = true;
			}
		}
	}
}