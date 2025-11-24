#include <allegro5/display.h>
#include "informacoes_tela.h"
#include <allegro5/monitor.h>

InformacoesTela obter_resolucao_tela_atual() {
    InformacoesTela tela;
    ALLEGRO_MONITOR_INFO informacoes_monitor;
    al_get_monitor_info(0, &informacoes_monitor);
    tela.largura = informacoes_monitor.x2 - informacoes_monitor.x1;
    tela.altura = informacoes_monitor.y2 - informacoes_monitor.y1;
    return tela;
}

ALLEGRO_DISPLAY* criar_tela_cheia(InformacoesTela tela) {
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    ALLEGRO_DISPLAY* tela_jogo = al_create_display(tela.largura, tela.altura);
    if (tela_jogo) return tela_jogo;
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    return al_create_display(tela.largura, tela.altura);
}

float deixarProporcional(float posicao, float tamanho_tela, float tamanho_tela_original) {
    return (posicao * tamanho_tela) / tamanho_tela_original;
}
