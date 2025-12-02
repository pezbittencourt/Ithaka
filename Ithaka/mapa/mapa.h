#ifndef MAPA_H
#define MAPA_H

#include <allegro5/allegro.h>

// Códigos de retorno
#define MAPA_SAIR           -1
#define MAPA_VOLTAR_MENU     0
#define MAPA_FASE_POLIFEMO   1
#define MAPA_FASE_CIRCE      2 
#define MAPA_FASE_SUBMUNDO	 3
#define MAPA_FASE_POSEIDON   4
#define MAPA_FASE_CALYPSO	 5
#define MAPA_FASE_ITACA		 6

int exibir_mapa_inicial(ALLEGRO_DISPLAY* display, int mapas_disponiveis);

#endif