#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <allegro5/display.h>
#include "informacoes_tela.h"

#define ALTURA_TELA_ORIGINAL 1080
#define LARGURA_TELA_ORIGINAL 1920

InformacoesTela obter_resolucao_tela_atual();
ALLEGRO_DISPLAY* criar_tela_cheia(InformacoesTela tela);
float deixarProporcional(float posicao, float tamanho_tela, float tamanho_tela_original);

#endif