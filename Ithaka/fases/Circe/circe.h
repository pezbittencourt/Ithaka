#ifndef CIRCE_H
#define CIRCE_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "../../fase.h"
#include "../../informacoes_tela.h"
#include "../../util.h"

#define CIRCE_NUMERO_SPRITES			13
#define CIRCE_SPRITE_PARADA				0
#define CIRCE_SPRITE_DANO				1
#define CIRCE_SPRITE_CORVO_PARADO		2
#define CIRCE_SPRITE_CIRCE_CORVO		3
#define CIRCE_SPRITE_CORVO_CIRCE		4
#define CIRCE_SPRITE_CORVO_ATACA		5
#define CIRCE_SPRITE_BATALHA			6
#define CIRCE_SPRITE_BATALHA_HIT		7
#define CIRCE_SPRITE_TIGRE_PARADO		8
#define CIRCE_SPRITE_TIGRE_ANDANDO		9
#define CIRCE_SPRITE_TIGRE_ATAQUE		10
#define CIRCE_SPRITE_CIRCE_TIGRE		11
#define CIRCE_SPRITE_TIGRE_CIRCE		12



// Funções principais
bool carregar_cenarios_circe(Fase* fase);
void processar_acao_circe(Personagem* odisseu, Personagem* circe, int* circe_stall, InformacoesTela tela);
void atualizar_circe(Personagem* circe, Personagem odisseu, InformacoesTela tela, int* circe_stall);
void atualizar_tamanho_circe(Personagem* circe, InformacoesTela tela, bool tigre);
#endif