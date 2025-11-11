#define _CRT_SECURE_NO_WARNINGS
#include "olimpo.h"
#include <stdio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

//============================================================
// CARREGAMENTO DE CENÁRIOS
//============================================================

bool carregar_cenarios_olimpo(Fase* fase)
{
    if (!fase) return false;

    init_fase(fase, 1);

    if (!fase->cenarios || fase->total_cenarios < 1) {
        fprintf(stderr, "Erro: falha na inicialização da fase Olimpo\n");
        return false;
    }

    printf("Inicializando cenário do Olimpo...\n");

    init_cenario(&fase->cenarios[0], 0, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Olimpo/fundoOlimpo.png");

    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "Erro ao carregar fundo do cenário Olimpo\n");
        return false;
    }

    printf("Cenário Olimpo carregado com sucesso!\n");
    return true;
}

//============================================================
// DEFINIÇÃO DAS 12 PERGUNTAS
//============================================================

void fazendo_as_perguntas(pergunta perguntas[])
{
    // Pergunta 1
    strcpy(perguntas[0].perguntaFeita, "No mito de Prometeu, o que o titã roubou dos deuses para dar aos mortais?");
    strcpy(perguntas[0].opcoes[0], "Fogo");
    strcpy(perguntas[0].opcoes[1], "Poder eterno");
    strcpy(perguntas[0].opcoes[2], "Vinho");
    strcpy(perguntas[0].opcoes[3], "A erva sagrada Moli");
    perguntas[0].respostaCerta = 0;

    // Pergunta 2
    strcpy(perguntas[1].perguntaFeita, "Qual foi a única coisa que restou na caixa após Pandora a abrir?");
    strcpy(perguntas[1].opcoes[0], "Amor");
    strcpy(perguntas[1].opcoes[1], "Luxúria");
    strcpy(perguntas[1].opcoes[2], "Esperança");
    strcpy(perguntas[1].opcoes[3], "Miséria");
    perguntas[1].respostaCerta = 2;

    // Pergunta 3
    strcpy(perguntas[2].perguntaFeita, "De quem Ícaro era filho?");
    strcpy(perguntas[2].opcoes[0], "Páris");
    strcpy(perguntas[2].opcoes[1], "Teseu");
    strcpy(perguntas[2].opcoes[2], "Dédalo");
    strcpy(perguntas[2].opcoes[3], "Narciso");
    perguntas[2].respostaCerta = 2;

    // Pergunta 4
    strcpy(perguntas[3].perguntaFeita, "Qual foi o estopim para a Guerra de Tróia?");
    strcpy(perguntas[3].opcoes[0], "O assassinato de Menelau");
    strcpy(perguntas[3].opcoes[1], "O sequestro de Helena");
    strcpy(perguntas[3].opcoes[2], "Conflitos territoriais");
    strcpy(perguntas[3].opcoes[3], "O assassinato de Páris");
    perguntas[3].respostaCerta = 1;

    // Pergunta 5
    strcpy(perguntas[4].perguntaFeita, "Qual foi a condição de Eros para viver com Psiquê?");
    strcpy(perguntas[4].opcoes[0], "Ela nunca poderia ver seu rosto");
    strcpy(perguntas[4].opcoes[1], "Eles nunca poderiam se tocar");
    strcpy(perguntas[4].opcoes[2], "Psiquê jamais teria filhos");
    strcpy(perguntas[4].opcoes[3], "Nunca houve condição");
    perguntas[4].respostaCerta = 0;

    // Pergunta 6
    strcpy(perguntas[5].perguntaFeita, "Qual membro de Urano gerou Vênus ao tocar o mar?");
    strcpy(perguntas[5].opcoes[0], "Sua perna direita");
    strcpy(perguntas[5].opcoes[1], "Seu braço esquerdo");
    strcpy(perguntas[5].opcoes[2], "Seu pé esquerdo");
    strcpy(perguntas[5].opcoes[3], "Seu órgão reprodutor");
    perguntas[5].respostaCerta = 3;

    // Pergunta 7
    strcpy(perguntas[6].perguntaFeita, "De que forma Narciso morreu?");
    strcpy(perguntas[6].opcoes[0], "Afogado no rio");
    strcpy(perguntas[6].opcoes[1], "Amaldiçoado por Afrodite");
    strcpy(perguntas[6].opcoes[2], "Comeu fruto proibido");
    strcpy(perguntas[6].opcoes[3], "Preso em seu reflexo");
    perguntas[6].respostaCerta = 3;

    // Pergunta 8
    strcpy(perguntas[7].perguntaFeita, "Qual criatura habitava o labirinto de Dédalo?");
    strcpy(perguntas[7].opcoes[0], "Harpia");
    strcpy(perguntas[7].opcoes[1], "Centauro");
    strcpy(perguntas[7].opcoes[2], "Quimera");
    strcpy(perguntas[7].opcoes[3], "Minotauro");
    perguntas[7].respostaCerta = 3;

    // Pergunta 9
    strcpy(perguntas[8].perguntaFeita, "Qual foi o castigo de Sísifo?");
    strcpy(perguntas[8].opcoes[0], "Vagar cego eternamente");
    strcpy(perguntas[8].opcoes[1], "Empurrar pedra montanha acima");
    strcpy(perguntas[8].opcoes[2], "Órgãos devorados por corvos");
    strcpy(perguntas[8].opcoes[3], "Preso por correntes");
    perguntas[8].respostaCerta = 1;

    // Pergunta 10
    strcpy(perguntas[9].perguntaFeita, "Qual foi o dom de Midas?");
    strcpy(perguntas[9].opcoes[0], "Ler pensamentos");
    strcpy(perguntas[9].opcoes[1], "Vida eterna");
    strcpy(perguntas[9].opcoes[2], "Transformar tudo em ouro");
    strcpy(perguntas[9].opcoes[3], "Prever o futuro");
    perguntas[9].respostaCerta = 2;

    // Pergunta 11
    strcpy(perguntas[10].perguntaFeita, "Qual titã foi condenado a segurar o céu?");
    strcpy(perguntas[10].opcoes[0], "Prometeu");
    strcpy(perguntas[10].opcoes[1], "Atlas");
    strcpy(perguntas[10].opcoes[2], "Cronos");
    strcpy(perguntas[10].opcoes[3], "Zeus");
    perguntas[10].respostaCerta = 1;

    // Pergunta 12
    strcpy(perguntas[11].perguntaFeita, "Qual foi o castigo de Tântalo?");
    strcpy(perguntas[11].opcoes[0], "Fome e sede em abundância");
    strcpy(perguntas[11].opcoes[1], "Fígado devorado por cobra");
    strcpy(perguntas[11].opcoes[2], "Membros servidos às almas");
    strcpy(perguntas[11].opcoes[3], "Acorrentado eternamente");
    perguntas[11].respostaCerta = 0;
}

//============================================================
// DESENHO DO QUIZ
//============================================================

void desenha_quiz(ALLEGRO_BITMAP* caixa_dialogo,
    ALLEGRO_BITMAP* array_opcoes[4],
    ALLEGRO_FONT* fonte,
    pergunta* perguntaAtual,
    quiz* estado)
{
    if (!caixa_dialogo || !fonte || !perguntaAtual || !estado) return;

    // Desenha caixa de diálogo de fundo
    al_draw_bitmap(caixa_dialogo, 0, 0, 0);

    // Desenha as 4 imagens das opções
    for (int i = 0; i < 4; i++) {
        if (array_opcoes[i]) {
            al_draw_bitmap(array_opcoes[i], 0, 0, 0);
        }
    }

    // Desenha a pergunta (centralizada, assumindo tela 1920x1080)
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        960, 200,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->perguntaFeita);

    // Desenha as 4 opções em layout 2x2
    // Opção 1 (superior esquerda)
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        480, 450,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->opcoes[0]);

    // Opção 2 (superior direita)
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        1440, 450,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->opcoes[1]);

    // Opção 3 (inferior esquerda)
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        480, 650,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->opcoes[2]);

    // Opção 4 (inferior direita)
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        1440, 650,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->opcoes[3]);

    // Mostra contador de erros
    al_draw_textf(fonte, al_map_rgb(255, 100, 100),
        100, 50,
        ALLEGRO_ALIGN_LEFT,
        "Erros: %d/3", estado->erros);

    // Mostra número da pergunta
    al_draw_textf(fonte, al_map_rgb(200, 200, 255),
        1820, 50,
        ALLEGRO_ALIGN_RIGHT,
        "Pergunta %d/%d",
        estado->perguntaAtual + 1, estado->numPerguntas);
}

//============================================================
// VERIFICAÇÃO DE CLIQUE NAS OPÇÕES
//============================================================

int verificar_clique_opcao(int mouse_x, int mouse_y, int largura_tela, int altura_tela)
{
    // Calcula escala proporcional à resolução
    float escala_x = largura_tela / 1920.0f;
    float escala_y = altura_tela / 1080.0f;

    // Define as áreas clicáveis (layout 2x2)
    // Cada área é um retângulo ao redor do texto da opção

    // Opção 1 (superior esquerda) - centro em (480, 450)
    int x1 = (int)(280 * escala_x);
    int y1 = (int)(400 * escala_y);
    int w1 = (int)(400 * escala_x);
    int h1 = (int)(100 * escala_y);

    if (mouse_x >= x1 && mouse_x <= x1 + w1 &&
        mouse_y >= y1 && mouse_y <= y1 + h1) {
        return 0;  // Opção 1
    }

    // Opção 2 (superior direita) - centro em (1440, 450)
    int x2 = (int)(1240 * escala_x);
    int y2 = (int)(400 * escala_y);
    int w2 = (int)(400 * escala_x);
    int h2 = (int)(100 * escala_y);

    if (mouse_x >= x2 && mouse_x <= x2 + w2 &&
        mouse_y >= y2 && mouse_y <= y2 + h2) {
        return 1;  // Opção 2
    }

    // Opção 3 (inferior esquerda) - centro em (480, 650)
    int x3 = (int)(280 * escala_x);
    int y3 = (int)(600 * escala_y);
    int w3 = (int)(400 * escala_x);
    int h3 = (int)(100 * escala_y);

    if (mouse_x >= x3 && mouse_x <= x3 + w3 &&
        mouse_y >= y3 && mouse_y <= y3 + h3) {
        return 2;  // Opção 3
    }

    // Opção 4 (inferior direita) - centro em (1440, 650)
    int x4 = (int)(1240 * escala_x);
    int y4 = (int)(600 * escala_y);
    int w4 = (int)(400 * escala_x);
    int h4 = (int)(100 * escala_y);

    if (mouse_x >= x4 && mouse_x <= x4 + w4 &&
        mouse_y >= y4 && mouse_y <= y4 + h4) {
        return 3;  // Opção 4
    }

    return -1;  // Nenhuma opção foi clicada
}

//============================================================
// PROCESSAR RESPOSTA
//============================================================

void processa_resposta(pergunta* perguntas, quiz* estado, int opcaoClicada)
{
    if (estado->respondida) return;  // Já foi respondida

    estado->respondida = true;
    estado->respostaSelecionada = opcaoClicada;
    estado->timer_feedback = DURACAO_FEEDBACK;  // Inicia timer de 2 segundos

    // Verifica se a resposta está correta
    if (opcaoClicada == perguntas[estado->perguntaAtual].respostaCerta) {
        printf("✓ Resposta CORRETA! Pergunta %d\n", estado->perguntaAtual + 1);
        // Aqui você pode adicionar som de acerto
    }
    else {
        estado->erros++;
        printf("✗ Resposta ERRADA! Erros: %d/3\n", estado->erros);
        // Aqui você pode adicionar som de erro

        if (estado->erros >= 3) {
            estado->perdeu = true;
            printf("GAME OVER! Você perdeu o quiz.\n");
            // Aqui você pode adicionar som de game over
        }
    }
}

//============================================================
// ATUALIZAR TIMER DO QUIZ
//============================================================

void atualizar_timer_quiz(quiz* estado)
{
    if (estado->timer_feedback > 0) {
        estado->timer_feedback--;
    }
}

//============================================================
// PRÓXIMA PERGUNTA
//============================================================

void proximaPergunta(quiz* estado)
{
    // Só avança se o timer de feedback zerou
    if (estado->timer_feedback > 0) {
        return;
    }

    // Valida se ainda há perguntas
    if (estado->perguntaAtual < estado->numPerguntas - 1) {
        estado->perguntaAtual++;
        estado->respondida = false;
        estado->respostaSelecionada = -1;
        estado->timer_feedback = 0;

        printf("Avançando para pergunta %d\n", estado->perguntaAtual + 1);
    }
    else {
        printf("Quiz completo! Total de erros: %d\n", estado->erros);
    }
}