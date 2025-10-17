#ifndef MAPA_H
#define MAPA_H

#include <allegro5/allegro.h>

// Códigos de retorno
#define MAPA_SAIR           -1
#define MAPA_VOLTAR_MENU     0
#define MAPA_FASE_POLIFEMO   1
#define MAPA_FASE_CIRCE      2 

int exibir_mapa_inicial(ALLEGRO_DISPLAY* display);

#endif