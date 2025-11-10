
#ifndef OLIMPO_H
#define OLIMPO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>
#include <string.h>
#include "../../fase.h"


typedef struct pergunta {
    char perguntaFeita[250]; //quantidade maxima de characteres que a pergunta pode ter. Em C, precisa definir.
    char opcoes[4][150];
    int respostaCerta;
} pergunta;

typedef struct quiz {
    int perguntaAtual;
    int numPerguntas;
    int respostaSelecionada;
    int erros;
    bool respondida;
    bool perdeu;
} quiz;

//Declaração área clicável das opções

typedef struct {
    int x, y;
    int largura, altura;
    int opcao_id;
    const char* nome;
    bool mouse_sobre;
    ALLEGRO_COLOR cor_normal;
    ALLEGRO_COLOR cor_destaque;
} AreaOpcao;

// Funções principais
bool carregar_cenarios_olimpo(Fase* fase);

// Funções Quiz
void desenha_quiz(ALLEGRO_BITMAP* caixa_dialogo, ALLEGRO_BITMAP* array_opcoes[4], ALLEGRO_FONT* fonte, pergunta* perguntaAtual, quiz* estado);
void processa_resposta(pergunta* perguntas, quiz* estado, int opcaoClicada);
void proximaPergunta(quiz* estado);


int exibir_opcoes_quiz(ALLEGRO_DISPLAY* display);

//Códigos de retorno do ID 

#define CLICAR_OPCAO_1 0    
#define CLICAR_OPCAO_2 1    
#define CLICAR_OPCAO_3 2      
#define CLICAR_OPCAO_4 3        


//-------------------------------------------------

void fazendo_as_perguntas(pergunta perguntas[]);





#endif


