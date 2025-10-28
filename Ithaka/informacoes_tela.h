#ifndef INFORMACOES_TELA_H
#define INFORMACOES_TELA_H

#include <allegro5/display.h>

typedef struct InformacoesTela {
    int largura;
    int altura;
} InformacoesTela;

InformacoesTela obter_resolucao_tela_atual();
ALLEGRO_DISPLAY* criar_tela_cheia(InformacoesTela tela);

#endif