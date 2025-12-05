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
    fase->cenarios[0].dialogo_caminho = "./dialogo/source/dialogoAtenaZeusFundoOlimpo.txt";
    fase->cenarios[0].dialogo_completo = false;

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
    strcpy(perguntas[0].perguntaFeita, "No mito de Prometeu, o que o tita roubou dos deuses para dar aos mortais?");
    strcpy(perguntas[0].opcoes[0], "Fogo");
    strcpy(perguntas[0].opcoes[1], "Poder eterno");
    strcpy(perguntas[0].opcoes[2], "Vinho");
    strcpy(perguntas[0].opcoes[3], "A erva sagrada Moli");
    perguntas[0].respostaCerta = 0;

    // Pergunta 2
    strcpy(perguntas[1].perguntaFeita, "Qual foi a unica coisa que restou na caixa apos Pandora a abrir?");
    strcpy(perguntas[1].opcoes[0], "Amor");
    strcpy(perguntas[1].opcoes[1], "Luxuria");
    strcpy(perguntas[1].opcoes[2], "Esperanca");
    strcpy(perguntas[1].opcoes[3], "Miseria");
    perguntas[1].respostaCerta = 2;

    // Pergunta 3
    strcpy(perguntas[2].perguntaFeita, "De quem Icaro era filho?");
    strcpy(perguntas[2].opcoes[0], "Paris");
    strcpy(perguntas[2].opcoes[1], "Teseu");
    strcpy(perguntas[2].opcoes[2], "Dedalo");
    strcpy(perguntas[2].opcoes[3], "Narciso");
    perguntas[2].respostaCerta = 2;

    // Pergunta 4
    strcpy(perguntas[3].perguntaFeita, "Qual foi o estopim para a Guerra de Troia?");
    strcpy(perguntas[3].opcoes[0], "O assassinato de Menelau");
    strcpy(perguntas[3].opcoes[1], "O sequestro de Helena");
    strcpy(perguntas[3].opcoes[2], "Conflitos territoriais");
    strcpy(perguntas[3].opcoes[3], "O assassinato de Paris");
    perguntas[3].respostaCerta = 1;

    // Pergunta 5
    strcpy(perguntas[4].perguntaFeita, "Qual foi a condição de Eros para viver com Psique?");
    strcpy(perguntas[4].opcoes[0], "Ela nunca poderia ver seu rosto");
    strcpy(perguntas[4].opcoes[1], "Eles nunca poderiam se tocar");
    strcpy(perguntas[4].opcoes[2], "Psique jamais teria filhos");
    strcpy(perguntas[4].opcoes[3], "Nunca houve condição");
    perguntas[4].respostaCerta = 0;

    // Pergunta 6
    strcpy(perguntas[5].perguntaFeita, "Qual membro de Urano gerou Venus ao tocar o mar?");
    strcpy(perguntas[5].opcoes[0], "Sua perna direita");
    strcpy(perguntas[5].opcoes[1], "Seu braço esquerdo");
    strcpy(perguntas[5].opcoes[2], "Seu pe esquerdo");
    strcpy(perguntas[5].opcoes[3], "Seu orgao reprodutor");
    perguntas[5].respostaCerta = 3;

    // Pergunta 7
    strcpy(perguntas[6].perguntaFeita, "De que forma Narciso morreu?");
    strcpy(perguntas[6].opcoes[0], "Afogado no rio");
    strcpy(perguntas[6].opcoes[1], "Amaldicoado por Afrodite");
    strcpy(perguntas[6].opcoes[2], "Comeu fruto proibido");
    strcpy(perguntas[6].opcoes[3], "Preso em seu reflexo");
    perguntas[6].respostaCerta = 3;

    // Pergunta 8
    strcpy(perguntas[7].perguntaFeita, "Qual criatura habitava o labirinto de Dedalo?");
    strcpy(perguntas[7].opcoes[0], "Harpia");
    strcpy(perguntas[7].opcoes[1], "Centauro");
    strcpy(perguntas[7].opcoes[2], "Quimera");
    strcpy(perguntas[7].opcoes[3], "Minotauro");
    perguntas[7].respostaCerta = 3;

    // Pergunta 9
    strcpy(perguntas[8].perguntaFeita, "Qual foi o castigo de Sisifo?");
    strcpy(perguntas[8].opcoes[0], "Vagar cego eternamente");
    strcpy(perguntas[8].opcoes[1], "Empurrar pedra montanha acima");
    strcpy(perguntas[8].opcoes[2], "Orgaos devorados por corvos");
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
    strcpy(perguntas[10].perguntaFeita, "Qual tita foi condenado a segurar o ceu?");
    strcpy(perguntas[10].opcoes[0], "Prometeu");
    strcpy(perguntas[10].opcoes[1], "Atlas");
    strcpy(perguntas[10].opcoes[2], "Cronos");
    strcpy(perguntas[10].opcoes[3], "Zeus");
    perguntas[10].respostaCerta = 1;

    // Pergunta 12
    strcpy(perguntas[11].perguntaFeita, "Qual foi o castigo de Tantalo?");
    strcpy(perguntas[11].opcoes[0], "Fome e sede em abundancia");
    strcpy(perguntas[11].opcoes[1], "Figado devorado por cobra");
    strcpy(perguntas[11].opcoes[2], "Membros servidos as almas");
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

    ALLEGRO_DISPLAY* display = al_get_current_display();
    int largura_tela = al_get_display_width(display);
    int altura_tela = al_get_display_height(display);

    // ========== CAIXA DE DIÁLOGO PRINCIPAL ==========
    int caixa_x = largura_tela * 0.06f;
    int caixa_y = altura_tela * 0.70f;
    int caixa_largura = largura_tela * 0.87f;
    int caixa_altura = altura_tela * 0.25f;

    al_draw_filled_rectangle(caixa_x, caixa_y,
        caixa_x + caixa_largura, caixa_y + caixa_altura,
        al_map_rgb(0, 0, 0));
    al_draw_rectangle(caixa_x, caixa_y,
        caixa_x + caixa_largura, caixa_y + caixa_altura,
        al_map_rgb(255, 255, 255), 3.0f);

    // ========== RETÂNGULOS DAS 4 OPÇÕES (2x2) ==========
    int opcao_largura = largura_tela * 0.30f;
    int opcao_altura = altura_tela * 0.10f;
    int espaco_h = largura_tela * 0.02f;
    int espaco_v = altura_tela * 0.02f;

    int base_x = largura_tela * 0.20f;
    int base_y = altura_tela * 0.43f;

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

    // ========== DESENHAR RETÂNGULOS DAS OPÇÕES ==========
    for (int i = 0; i < 4; i++) {
        ALLEGRO_COLOR cor_fundo;
        ALLEGRO_COLOR cor_borda = al_map_rgb(255, 255, 255);
        float espessura_borda = 2.0f;

        if (estado->respondida) {
            if (i == perguntaAtual->respostaCerta) {
                cor_fundo = al_map_rgb(0, 100, 0);
                cor_borda = al_map_rgb(0, 255, 0);
                espessura_borda = 4.0f;
            }
            else if (i == estado->respostaSelecionada) {
                cor_fundo = al_map_rgb(100, 0, 0);
                cor_borda = al_map_rgb(255, 0, 0);
                espessura_borda = 4.0f;
            }
            else {
                cor_fundo = al_map_rgb(30, 30, 30);
            }
        }
        else {
            cor_fundo = al_map_rgb(0, 0, 0);
        }

        al_draw_filled_rectangle(
            opcoes_x[i], opcoes_y[i],
            opcoes_x[i] + opcao_largura, opcoes_y[i] + opcao_altura,
            cor_fundo);

        al_draw_rectangle(
            opcoes_x[i], opcoes_y[i],
            opcoes_x[i] + opcao_largura, opcoes_y[i] + opcao_altura,
            cor_borda, espessura_borda);
    }

    // ========== DESENHAR TEXTOS DAS OPÇÕES (MULTILINE EIN) ==========
    int altura_fonte = al_get_font_line_height(fonte);
    float max_largura_opcao = opcao_largura * 0.85f; 

    for (int i = 0; i < 4; i++) {
        int centro_x = opcoes_x[i] + (opcao_largura / 2);
        int topo_y = opcoes_y[i] + 20;  // Pequena margem do topo

        al_draw_multiline_text(fonte, al_map_rgb(255, 255, 255),
            centro_x,
            topo_y,
            max_largura_opcao,
            altura_fonte,  
            ALLEGRO_ALIGN_CENTER,
            perguntaAtual->opcoes[i]);
    }

    // ========== TEXTO DA PERGUNTA COM MULTILINE ==========
    float max_largura_pergunta = caixa_largura * 0.95f;  // 95% da largura da caixa
    int topo_pergunta_y = caixa_y + 60;  // Pequena margem do topo

    al_draw_multiline_text(fonte, al_map_rgb(255, 255, 255),
        largura_tela / 2,
        topo_pergunta_y,
        max_largura_pergunta,
        altura_fonte + 2,
        ALLEGRO_ALIGN_CENTER,
        perguntaAtual->perguntaFeita);

    // ========== INFORMAÇÕES (ERROS E NÚMERO DA PERGUNTA) ==========
    al_draw_textf(fonte, al_map_rgb(255, 255, 255),
        largura_tela * 0.05f, altura_tela * 0.05f,
        ALLEGRO_ALIGN_LEFT,
        "Erros: %d/3", estado->erros);

    al_draw_textf(fonte, al_map_rgb(255, 255, 255),
        largura_tela * 0.95f, altura_tela * 0.05f,
        ALLEGRO_ALIGN_RIGHT,
        "Pergunta %d/%d",
        estado->perguntaAtual + 1, estado->numPerguntas);

    // ========== MENSAGEM DE FEEDBACK (ACERTO/ERRO) ==========
    if (estado->respondida) {
        bool acertou = (estado->respostaSelecionada == perguntaAtual->respostaCerta);

        if (acertou) {
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
                ALLEGRO_ALIGN_CENTER, "PARABENS! ACERTOU!");
        }
        else {
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
    int opcao_largura = largura_tela * 0.30f;
    int opcao_altura = altura_tela * 0.10f;
    int espaco_h = largura_tela * 0.02f;
    int espaco_v = altura_tela * 0.02f;


    int base_x = largura_tela * 0.20f;
    int base_y = altura_tela * 0.43f;

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
            printf("GAME OVER! Você perdeu o quiz dos Deuses.\n");
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