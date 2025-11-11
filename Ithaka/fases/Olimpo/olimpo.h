#ifndef OLIMPO_H
#define OLIMPO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>
#include <string.h>
#include "../../fase.h"

// ============================================================
// ESTRUTURAS DE DADOS
// ============================================================

// Estrutura de uma pergunta do quiz
typedef struct pergunta {
    char perguntaFeita[250];  // Texto da pergunta
    char opcoes[4][150];      // 4 opções de resposta
    int respostaCerta;        // Índice da resposta correta (0-3)
} pergunta;

// Estrutura do estado do quiz
typedef struct quiz {
    int perguntaAtual;        // Índice da pergunta atual (0-11)
    int numPerguntas;         // Total de perguntas (12)
    int respostaSelecionada;  // Última opção selecionada pelo jogador
    int erros;                // Contador de erros
    bool respondida;          // Se a pergunta atual já foi respondida
    bool perdeu;              // Se o jogador perdeu (3 erros)
    int timer_feedback;       // Timer para mostrar feedback antes de avançar
} quiz;

// Estrutura para áreas clicáveis das opções
typedef struct {
    int x, y;                 // Posição na tela
    int largura, altura;      // Dimensões da área
    int opcao_id;             // ID da opção (0-3)
    const char* nome;         // Nome descritivo
    bool mouse_sobre;         // Se o mouse está sobre a área
    ALLEGRO_COLOR cor_normal;    // Cor quando não está selecionada
    ALLEGRO_COLOR cor_destaque;  // Cor quando mouse está sobre
} AreaOpcao;

// ============================================================
// CONSTANTES
// ============================================================

// IDs das opções (usados para identificar qual foi clicada)
#define CLICAR_OPCAO_1 0    
#define CLICAR_OPCAO_2 1    
#define CLICAR_OPCAO_3 2      
#define CLICAR_OPCAO_4 3

// Duração do feedback em frames (60 frames = 1 segundo)
#define DURACAO_FEEDBACK 120  // 2 segundos para ver se acertou/errou

// ============================================================
// DECLARAÇÕES DE FUNÇÕES
// ============================================================

// --- Carregamento de Cenários ---
bool carregar_cenarios_olimpo(Fase* fase);

// --- Sistema de Perguntas ---
void fazendo_as_perguntas(pergunta perguntas[]);

// --- Sistema de Quiz ---
void desenha_quiz(ALLEGRO_BITMAP* caixa_dialogo,
    ALLEGRO_BITMAP* array_opcoes[4],
    ALLEGRO_FONT* fonte,
    pergunta* perguntaAtual,
    quiz* estado);

void processa_resposta(pergunta* perguntas,
    quiz* estado,
    int opcaoClicada);

void proximaPergunta(quiz* estado);

// --- Detecção de Áreas Clicáveis ---
int verificar_clique_opcao(int mouse_x, int mouse_y, int largura_tela, int altura_tela);

// --- Utilitários ---
void atualizar_timer_quiz(quiz* estado);

#endif // OLIMPO_H