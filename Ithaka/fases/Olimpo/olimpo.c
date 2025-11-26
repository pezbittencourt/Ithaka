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
// DESENHO DO QUIZ - COM RETÂNGULOS
//============================================================

void desenha_quiz(ALLEGRO_BITMAP* caixa_dialogo,
    ALLEGRO_BITMAP* array_opcoes[4],
    ALLEGRO_FONT* fonte,
    pergunta* perguntaAtual,
    quiz* estado)
{
    if (!fonte || !perguntaAtual || !estado) return;

    // Obter dimensões da tela
    ALLEGRO_DISPLAY* display = al_get_current_display();
    int largura_tela = al_get_display_width(display);
    int altura_tela = al_get_display_height(display);

    // ========== CAIXA DE DIÁLOGO PRINCIPAL  ==========
    int caixa_x = largura_tela * 0.08f;
    int caixa_y = altura_tela * 0.60f;
    int caixa_largura = largura_tela * 0.84f;
    int caixa_altura = altura_tela * 0.32f;

    // Retângulo preto com borda branca
    al_draw_filled_rectangle(caixa_x, caixa_y,
        caixa_x + caixa_largura, caixa_y + caixa_altura,
        al_map_rgb(0, 0, 0));
    al_draw_rectangle(caixa_x, caixa_y,
        caixa_x + caixa_largura, caixa_y + caixa_altura,
        al_map_rgb(255, 255, 255), 3.0f);

    // ========== RETÂNGULOS DAS 4 OPÇÕES (2x2) ==========
    int opcao_largura = largura_tela * 0.38f;
    int opcao_altura = altura_tela * 0.12f;
    int espaco_h = largura_tela * 0.04f;
    int espaco_v = altura_tela * 0.02f;

    // Posições base (superior esquerda do grid 2x2)
    int base_x = largura_tela * 0.12f;
    int base_y = altura_tela * 0.35f;

    // Array com posições das 4 opções
    int opcoes_x[4] = {
        base_x,                          // Opção 1 (esquerda cima)
        base_x + opcao_largura + espaco_h,  // Opção 2 (direita cima)
        base_x,                          // Opção 3 (esquerda baixo)
        base_x + opcao_largura + espaco_h   // Opção 4 (direita baixo)
    };

    int opcoes_y[4] = {
        base_y,                          // Opção 1
        base_y,                          // Opção 2
        base_y + opcao_altura + espaco_v,   // Opção 3
        base_y + opcao_altura + espaco_v    // Opção 4
    };

    // Desenhar cada opção
    for (int i = 0; i < 4; i++) {
        ALLEGRO_COLOR cor_fundo;
        ALLEGRO_COLOR cor_borda = al_map_rgb(255, 255, 255);
        float espessura_borda = 2.0f;

        // Se a pergunta foi respondida, mostrar feedback visual
        if (estado->respondida) {
            if (i == perguntaAtual->respostaCerta) {
                // Resposta correta - verde
                cor_fundo = al_map_rgb(0, 100, 0);
                cor_borda = al_map_rgb(0, 255, 0);
                espessura_borda = 4.0f;
            }
            else if (i == estado->respostaSelecionada) {
                // Resposta errada selecionada - vermelho
                cor_fundo = al_map_rgb(100, 0, 0);
                cor_borda = al_map_rgb(255, 0, 0);
                espessura_borda = 4.0f;
            }
            else {
                // Outras opções - cinza escuro
                cor_fundo = al_map_rgb(30, 30, 30);
            }
        }
        else {
            // Antes de responder - preto
            cor_fundo = al_map_rgb(0, 0, 0);
        }

        // Desenhar retângulo
        al_draw_filled_rectangle(
            opcoes_x[i], opcoes_y[i],
            opcoes_x[i] + opcao_largura, opcoes_y[i] + opcao_altura,
            cor_fundo);

        al_draw_rectangle(
            opcoes_x[i], opcoes_y[i],
            opcoes_x[i] + opcao_largura, opcoes_y[i] + opcao_altura,
            cor_borda, espessura_borda);

        // Desenhar texto da opção (centralizado)
        al_draw_text(fonte, al_map_rgb(255, 255, 255),
            opcoes_x[i] + opcao_largura / 2,
            opcoes_y[i] + opcao_altura / 2 - 15,
            ALLEGRO_ALIGN_CENTER,
            perguntaAtual->opcoes[i]);
    }

    // ========== TEXTO DA PERGUNTA (ACIMA DAS OPÇÕES) ==========
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
        largura_tela / 2, altura_tela * 0.15f,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->perguntaFeita);

    // ========== INFORMAÇÕES DO JOGO ==========

    // Contador de erros (canto superior esquerdo)
    al_draw_textf(fonte, al_map_rgb(255, 100, 100),
        largura_tela * 0.05f, altura_tela * 0.05f,
        ALLEGRO_ALIGN_LEFT,
        "Erros: %d/3", estado->erros);

    // Número da pergunta (canto superior direito)
    al_draw_textf(fonte, al_map_rgb(200, 200, 255),
        largura_tela * 0.95f, altura_tela * 0.05f,
        ALLEGRO_ALIGN_RIGHT,
        "Pergunta %d/%d",
        estado->perguntaAtual + 1, estado->numPerguntas);

    // ========== FEEDBACK VISUAL APÓS RESPONDER ==========
    if (estado->respondida) {
        bool acertou = (estado->respostaSelecionada == perguntaAtual->respostaCerta);

        if (acertou) {
            // Mensagem de acerto
            al_draw_filled_rectangle(
                largura_tela / 2 - 200, altura_tela * 0.25f - 30,
                largura_tela / 2 + 200, altura_tela * 0.25f + 30,
                al_map_rgba(0, 150, 0, 220));
            al_draw_rectangle(
                largura_tela / 2 - 200, altura_tela * 0.25f - 30,
                largura_tela / 2 + 200, altura_tela * 0.25f + 30,
                al_map_rgb(0, 255, 0), 3.0f);
            al_draw_text(fonte, al_map_rgb(255, 255, 255),
                largura_tela / 2, altura_tela * 0.25f - 10,
                ALLEGRO_ALIGN_CENTER, "CORRETO!");
        }
        else {
            // Mensagem de erro
            al_draw_filled_rectangle(
                largura_tela / 2 - 200, altura_tela * 0.25f - 30,
                largura_tela / 2 + 200, altura_tela * 0.25f + 30,
                al_map_rgba(150, 0, 0, 220));
            al_draw_rectangle(
                largura_tela / 2 - 200, altura_tela * 0.25f - 30,
                largura_tela / 2 + 200, altura_tela * 0.25f + 30,
                al_map_rgb(255, 0, 0), 3.0f);
            al_draw_text(fonte, al_map_rgb(255, 255, 255),
                largura_tela / 2, altura_tela * 0.25f - 10,
                ALLEGRO_ALIGN_CENTER, "ERRADO!");
        }
    }
}

//============================================================
// VERIFICAÇÃO DE CLIQUE NAS OPÇÕES
//============================================================

int verificar_clique_opcao(int mouse_x, int mouse_y, int largura_tela, int altura_tela)
{
    // Mesmas dimensões usadas no desenho
    int opcao_largura = largura_tela * 0.38f;
    int opcao_altura = altura_tela * 0.12f;
    int espaco_h = largura_tela * 0.04f;
    int espaco_v = altura_tela * 0.02f;

    int base_x = largura_tela * 0.12f;
    int base_y = altura_tela * 0.35f;

    // Posições das opções
    int opcoes_x[4] = {
        base_x,
        base_x + opcao_largura + espaco_h,
        base_x,
        base_x + opcao_largura + espaco_h
    };

    int opcoes_y[4] = {
        base_y,
        base_y,
        base_y + opcao_altura + espaco_v,
        base_y + opcao_altura + espaco_v
    };

    // Verificar qual opção foi clicada
    for (int i = 0; i < 4; i++) {
        if (mouse_x >= opcoes_x[i] && mouse_x <= opcoes_x[i] + opcao_largura &&
            mouse_y >= opcoes_y[i] && mouse_y <= opcoes_y[i] + opcao_altura) {
            return i;
        }
    }

    return -1;  // Nenhuma opção clicada
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
    }
    else {
        estado->erros++;
        printf("✗ Resposta ERRADA! Erros: %d/3\n", estado->erros);

        if (estado->erros >= 3) {
            estado->perdeu = true;
            printf("GAME OVER! Você perdeu o quiz.\n");
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