#include "poseidon.h"
#include "stdio.h"

const int NUMERO_CENARIOS = 1;
const int NUMERO_SPRITES = 4;
const int TAMANHO_TILE = 32;

const int SPRITE_AGUA = 0;
const int SPRITE_NAVIO = 1;
const int SPRITE_ACERTO = 2;
const int SPRITE_ERRO = 3;

typedef struct {
	int w;
	int h;
	ALLEGRO_FONT* font;
} CaixaTexto;

static void medida(int line_num, const char* line, int size, void* extra) {
	CaixaTexto* m = extra;

	int width = al_get_text_width(m->font, line);
	if (width > m->w) m->w = width;

	m->h += al_get_font_line_height(m->font);
}


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

static void draw_ship_ghost(Fase* fase, int mouse_x, int mouse_y, int length, bool vertical, InformacoesTela tela) {
	float tile_altura = deixarProporcional(TAMANHO_TILE, tela.altura, ALTURA_TELA_ORIGINAL);
	float tile_largura = deixarProporcional(TAMANHO_TILE, tela.largura, LARGURA_TELA_ORIGINAL);
	int offset_y = (tela.altura / 2) - ((tile_largura * 10) / 2);
	float tabuleiro_jogador_x = (tela.largura * 0.65f) - ((tile_largura * 10) + (tile_largura * 3) / 2);
	
	int tile_x = (mouse_x - tabuleiro_jogador_x) / tile_altura;
	int tile_y = (mouse_y - offset_y)  / tile_largura;
	for (int i = 0; i < length; i++) {
		int x = tile_x + (vertical ? 0 : i);
		int y = tile_y + (vertical ? i : 0);
		if (x < 0 || x >= 10 || y < 0 || y >= 10) continue;
		al_draw_tinted_scaled_bitmap(fase->sprites[SPRITE_NAVIO]
			,al_map_rgba(255, 255, 255, 120),
			0, 0,
			32, 32,
			tabuleiro_jogador_x + x * tile_largura,  offset_y + y * tile_altura,
			tile_largura, tile_altura, 0);
	}
}

bool carregar_sprites_poseidon(Fase* fase) {
	if (!fase) return false;

	fase->sprites[SPRITE_ERRO] = al_load_bitmap("./imagensJogo/objetos/erro.png");
	fase->sprites[SPRITE_NAVIO] = al_load_bitmap("./imagensJogo/objetos/navio.png");
	fase->sprites[SPRITE_AGUA] = al_load_bitmap("./imagensJogo/objetos/agua.png");
	fase->sprites[SPRITE_ACERTO] = al_load_bitmap("./imagensJogo/objetos/acerto.png");

	return true;
}


bool carregar_cenarios_poseidon(Fase* fase)
{
	if (!fase) return false;
	// Initialize the fase with 6 scenarios
	init_fase_sprites(fase, NUMERO_CENARIOS, NUMERO_SPRITES);

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
	int enemy_board[10][10], bool* turno_jogador, bool* setup_phase, Navio ships[], int ship_count, bool* vertical,
	Estado_Computador* ai, bool* player_won, bool* enemy_won, InformacoesTela tela)
{
	int tile_x, tile_y;
	float tile_altura = deixarProporcional(TAMANHO_TILE, tela.altura, ALTURA_TELA_ORIGINAL);
	float tile_largura = deixarProporcional(TAMANHO_TILE, tela.largura, LARGURA_TELA_ORIGINAL);
	int offset_y = (tela.altura / 2) - ((tile_largura * 10) / 2);
	float tabuleiro_jogador_x = (tela.largura * 0.65f) - ((tile_largura * 10) + (tile_largura * 3) / 2);
	float tabuleiro_inimigo_x = (tela.largura * 0.65f) + ((tile_largura * 3) / 2);

	if (*setup_phase) {
		if (mouse_right) *vertical = !*vertical;
		if (mouse_left) {
			tile_x = (mouse.x - tabuleiro_jogador_x) / tile_largura;
			tile_y = (mouse.y - offset_y) / tile_altura;
			for (int s = 0; s < ship_count; s++) {
				if (!ships[s].placed) {
					if (can_place_ship(player_board, tile_x, tile_y, ships[s].length, *vertical)) {
						for (int i = 0; i < ships[s].length; i++) {
							int px = tile_x + (*vertical ? 0 : i);
							int py = tile_y + (*vertical ? i : 0);
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

		if (!*turno_jogador) {
			ai_shoot(player_board, ai);
			*turno_jogador = true;
		}

		if (*turno_jogador && mouse_left) {
			tile_x = (mouse.x - tabuleiro_inimigo_x) / tile_largura;
			tile_y = (mouse.y - offset_y) / tile_altura;
			if (tile_x >= 0 && tile_x < 10 && tile_y >= 0 && tile_y < 10) {
				if (enemy_board[tile_y][tile_x] == 0) enemy_board[tile_y][tile_x] = 3;
				else if (enemy_board[tile_y][tile_x] == 1) enemy_board[tile_y][tile_x] = 2;
				*turno_jogador = false;
			}
		}

		if (check_victory(enemy_board)) *player_won = true;
		if (check_victory(player_board)) *enemy_won = true;
	}
}

void draw_battleship(Fase* fase, ALLEGRO_FONT* font, ALLEGRO_FONT* font_titulo, int player_board[10][10], int enemy_board[10][10], bool setup_phase, Navio ships[],
	int ship_count, ALLEGRO_MOUSE_STATE mouse, bool vertical,bool turno_jogador, bool player_won, bool enemy_won, InformacoesTela tela) {

	float tile_altura = deixarProporcional(TAMANHO_TILE, tela.altura, ALTURA_TELA_ORIGINAL);
	float tile_largura = deixarProporcional(TAMANHO_TILE, tela.largura, LARGURA_TELA_ORIGINAL);
	int offset_y = (tela.altura / 2) - ((tile_largura * 10) / 2);
	float tabuleiro_jogador_x = (tela.largura * 0.65f) - ((tile_largura * 10) + (tile_largura * 3) / 2);
	float tabuleiro_inimigo_x = (tela.largura * 0.65f) + ((tile_largura * 3) / 2);

	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++) {
			int v = player_board[y][x];
			int sprite = SPRITE_AGUA;
			if (v == 1) sprite = SPRITE_NAVIO;
			else if (v == 2) sprite = SPRITE_ACERTO;
			else if (v == 3) sprite = SPRITE_ERRO;
			
			float tile_x = tabuleiro_jogador_x + x * tile_largura;
			float tile_y = offset_y + y * tile_altura;

			al_draw_scaled_bitmap(fase->sprites[sprite],
				0,0,
				32,32,
				tile_x,tile_y,
				tile_largura,tile_altura,0);
		}

	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++) {
			int v = enemy_board[y][x];
			int sprite = SPRITE_AGUA;
			if (!setup_phase) {
				if (v == 2) sprite = SPRITE_ACERTO;
				else if (v == 3) sprite = SPRITE_ERRO;
			}
			float tile_x = tabuleiro_inimigo_x + x * tile_largura;
			float tile_y = offset_y + y * tile_altura;

			al_draw_scaled_bitmap(fase->sprites[sprite],
				0, 0,
				32, 32,
				tile_x, tile_y,
				tile_largura, tile_altura, 0);
		}

	char* text_tutorial;
	float tutorial_x = tela.largura * 0.05f;
	float tutorial_y = offset_y - 30;
	CaixaTexto m = { 0, 0, font };

	if (setup_phase) {
		int navios_colocados = 0;
		for (int s = 0; s < ship_count; s++) {
			if (ships[s].placed) {
				navios_colocados++;
			}
		}
		for (int s = 0; s < ship_count; s++) {
			if (!ships[s].placed) {
				draw_ship_ghost(fase, mouse.x, mouse.y, ships[s].length, vertical, tela);
				break;
			}
		}

		int indicador_x = tabuleiro_jogador_x;
		al_draw_rectangle(
			indicador_x - (tile_largura), offset_y - (tile_altura),
			indicador_x + (tile_largura * 11), offset_y + (tile_altura * 11),
			al_map_rgb(0, 255, 0),
			4);

		text_tutorial = "FASE DE PREPARACAO!\n\n Posicione seus navios com o botao esquerdo do mouse.\n\nGire seus navios com o botao direito do mouse.\n\n %d\\%d Navios Colocados";

		al_do_multiline_text(font, (tela.largura * 0.40f) - tutorial_x, text_tutorial, medida, &m);

		if ((tela.largura * 0.40f) - tutorial_x < m.w) m.w = (tela.largura * 0.40f) - tutorial_x;

		al_draw_filled_rectangle(
			tutorial_x - 20, tutorial_y - 10,
			tutorial_x + m.w + 20, tutorial_y + m.h + 10,
			al_map_rgb(0, 0, 0));
		al_draw_rectangle(
			tutorial_x - 20, tutorial_y - 10,
			tutorial_x + m.w + 20, tutorial_y + m.h + 10,
			al_map_rgb(255, 255, 255), 4);

		al_draw_multiline_textf(font, al_map_rgb(255, 255, 255),
			tutorial_x, tutorial_y,
			(tela.largura * 0.40f) - tutorial_x, al_get_font_line_height(font),
			0, text_tutorial, navios_colocados,ship_count);
	}
	else {
		if (!player_won && !enemy_won) {
			int indicador_x = !turno_jogador ? tabuleiro_jogador_x : tabuleiro_inimigo_x;
			al_draw_rectangle(
				indicador_x - (tile_largura), offset_y - (tile_altura),
				indicador_x + (tile_largura * 11), offset_y + (tile_altura * 11),
				turno_jogador ? al_map_rgb(0, 255, 0) : al_map_rgb(255, 0, 0),
				4);

			text_tutorial = "FASE DE BATALHA!\n\nClique no quadrante inimigo com o botao esquerdo do mouse para atirar uma bala de canhao!";

			al_do_multiline_text(font, (tela.largura * 0.40f) - tutorial_x, text_tutorial, medida, &m);

			if ((tela.largura * 0.40f) - tutorial_x < m.w) m.w = (tela.largura * 0.40f) - tutorial_x;

			al_draw_filled_rectangle(
				tutorial_x - 20, tutorial_y - 10,
				tutorial_x + m.w + 20, tutorial_y + m.h + 10,
				al_map_rgb(0, 0, 0));
			al_draw_rectangle(
				tutorial_x - 20, tutorial_y - 10,
				tutorial_x + m.w + 20, tutorial_y + m.h + 10,
				al_map_rgb(255, 255, 255), 4);

			al_draw_multiline_text(font, al_map_rgb(255, 255, 255),
				tutorial_x, tutorial_y,
				(tela.largura * 0.40f) - tutorial_x, al_get_font_line_height(font),
				0, text_tutorial);
		}
		else {
			char* text_resultado = "";
			if (player_won) {
				text_resultado = "Odisseu Venceu!";
			}
			else if (enemy_won) {
				text_resultado = "Odisseu Perdeu!";
			}
			float resultado_x = (tela.largura / 2) - (al_get_text_width(font_titulo, text_resultado) / 2);
			float resultado_y = tela.altura / 2 - al_get_font_line_height(font_titulo) / 2;

			al_draw_filled_rectangle(
				resultado_x - 20, resultado_y - 10,
				resultado_x + al_get_text_width(font_titulo, text_resultado) + 20, resultado_y + al_get_font_line_height(font_titulo) + 10,
				al_map_rgb(0, 0, 0));
			al_draw_rectangle(
				resultado_x - 20, resultado_y - 10,
				resultado_x + al_get_text_width(font_titulo, text_resultado) + 20, resultado_y + al_get_font_line_height(font_titulo) + 10,
				al_map_rgb(255, 255, 255), 4);

			al_draw_text(font_titulo, al_map_rgb(255, 255, 255),
				resultado_x, resultado_y,
				0, text_resultado);
		}
	}

	char* text_principal = "BATALHA NAVAL";
	float titulo_x = (tela.largura / 2) - (al_get_text_width(font_titulo, text_principal) / 2);
	float titulo_y = tela.altura * 0.1f;

	al_draw_filled_rectangle(
		titulo_x - 20, titulo_y - 10,
		titulo_x + al_get_text_width(font_titulo, text_principal) + 20, titulo_y + al_get_font_line_height(font_titulo) + 10,
		al_map_rgb(0, 0, 0));
	al_draw_rectangle(
		titulo_x - 20, titulo_y - 10,
		titulo_x + al_get_text_width(font_titulo, text_principal) + 20, titulo_y + al_get_font_line_height(font_titulo) + 10,
		al_map_rgb(255, 255, 255),4);

	al_draw_text(font_titulo, al_map_rgb(255, 255, 255),
		titulo_x, titulo_y,
		0, text_principal);
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