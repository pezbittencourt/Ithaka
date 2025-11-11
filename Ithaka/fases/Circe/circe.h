#ifndef CIRCE_H
#define CIRCE_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "../../fase.h"
#include "../../informacoes_tela.h"

#define CIRCE_NUMERO_SPRITES			6
#define CIRCE_SPRITE_PARADA				0
#define CIRCE_SPRITE_DANO				1
#define CIRCE_SPRITE_CORVO_PARADO		2
#define CIRCE_SPRITE_CIRCE_CORVO		3
#define CIRCE_SPRITE_CORVO_CIRCE		4
#define CIRCE_SPRITE_CORVO_ATACA		5

// Funções principais
bool carregar_cenarios_circe(Fase* fase);
void processar_acao_circe(Personagem* odisseu, Personagem* circe, int* circe_stall, InformacoesTela tela);
void atualizar_circe(Personagem* circe, Personagem odisseu, InformacoesTela tela);
void movimento_ataque_corvo(Personagem* circe, InformacoesTela tela);
void move_to_target(Personagem* circe);
#endif