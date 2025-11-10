#define _CRT_SECURE_NO_WARNINGS
#include "olimpo.h"
#include <stdio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h> // para desenhar retângulos de destaque

//============================================================
// Carregamento dos cenários do Olimpo
//============================================================

bool carregar_cenarios_olimpo(Fase* fase)
{
    if (!fase) return false;

    // Inicializa a fase com 1 cenário
    init_fase(fase, 1);

    // Verifica se a alocação foi bem-sucedida
    if (!fase->cenarios || fase->total_cenarios < 1) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");

    init_cenario(&fase->cenarios[0], 0, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Olimpo/fundoOlimpo.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "Erro ao carregar fundo do cenário 1\n");
        return false;
    }

    return true;
}

//============================================================
// Definição das perguntas
//============================================================

void fazendo_as_perguntas(pergunta perguntas[])
{
    // Pergunta 1
    strcpy(perguntas[0].perguntaFeita, "No mito de Prometeu, o que o titã roubou de nós, deuses, para dar aos mortais?");
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
    strcpy(perguntas[3].opcoes[0], "O assassinato de Menelau, rei de Esparta");
    strcpy(perguntas[3].opcoes[1], "O sequestro da rainha espartana Helena");
    strcpy(perguntas[3].opcoes[2], "Conflitos territoriais");
    strcpy(perguntas[3].opcoes[3], "O assassinato de Páris, príncipe troiano");
    perguntas[3].respostaCerta = 1;

    // Pergunta 5
    strcpy(perguntas[4].perguntaFeita, "Qual foi a condição apresentada por Eros a Psiquê?");
    strcpy(perguntas[4].opcoes[0], "Ela nunca poderia ver seu rosto");
    strcpy(perguntas[4].opcoes[1], "Eles nunca poderiam se tocar");
    strcpy(perguntas[4].opcoes[2], "Psiquê jamais poderia prover filhos");
    strcpy(perguntas[4].opcoes[3], "Nunca houve uma condição");
    perguntas[4].respostaCerta = 0;

    // Pergunta 6
    strcpy(perguntas[5].perguntaFeita, "Qual o membro amputado de Urano que gerou Vênus?");
    strcpy(perguntas[5].opcoes[0], "Sua perna direita");
    strcpy(perguntas[5].opcoes[1], "Seu braço esquerdo");
    strcpy(perguntas[5].opcoes[2], "Seu pé esquerdo");
    strcpy(perguntas[5].opcoes[3], "Seu órgão reprodutor");
    perguntas[5].respostaCerta = 3;

    // Pergunta 7
    strcpy(perguntas[6].perguntaFeita, "De que forma Narciso morreu?");
    strcpy(perguntas[6].opcoes[0], "Afogado, após cair no rio");
    strcpy(perguntas[6].opcoes[1], "Amaldiçoado por Afrodite");
    strcpy(perguntas[6].opcoes[2], "Após ingerir um fruto proibido");
    strcpy(perguntas[6].opcoes[3], "Preso em seu próprio reflexo");
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
    strcpy(perguntas[8].opcoes[0], "Ficar cego e vagar eternamente");
    strcpy(perguntas[8].opcoes[1], "Empurrar uma pedra montanha acima");
    strcpy(perguntas[8].opcoes[2], "Ter seus órgãos devorados por corvos");
    strcpy(perguntas[8].opcoes[3], "Andar eternamente preso por correntes");
    perguntas[8].respostaCerta = 1;

    // Pergunta 10
    strcpy(perguntas[9].perguntaFeita, "Qual foi o dom concedido a Midas?");
    strcpy(perguntas[9].opcoes[0], "Ler pensamentos");
    strcpy(perguntas[9].opcoes[1], "Vida eterna");
    strcpy(perguntas[9].opcoes[2], "Tudo o que toca se transforma em ouro");
    strcpy(perguntas[9].opcoes[3], "Prever o futuro");
    perguntas[9].respostaCerta = 2;

    // Pergunta 11
    strcpy(perguntas[10].perguntaFeita, "Qual foi o titã condenado a segurar o céu?");
    strcpy(perguntas[10].opcoes[0], "Prometeu");
    strcpy(perguntas[10].opcoes[1], "Atlas");
    strcpy(perguntas[10].opcoes[2], "Cronos");
    strcpy(perguntas[10].opcoes[3], "Zeus");
    perguntas[10].respostaCerta = 1;

    // Pergunta 12
    strcpy(perguntas[11].perguntaFeita, "Qual foi o castigo de Tântalo?");
    strcpy(perguntas[11].opcoes[0], "Morrer de fome e sede em meio à abundância");
    strcpy(perguntas[11].opcoes[1], "Ter o fígado devorado por uma cobra");
    strcpy(perguntas[11].opcoes[2], "Ter seus membros servidos às almas");
    strcpy(perguntas[11].opcoes[3], "Ficar acorrentado eternamente");
    perguntas[11].respostaCerta = 0;
}

//============================================================
// Opções do Quiz
//============================================================

int exibir_opcoes_quiz(ALLEGRO_DISPLAY* display)
{
    if (!display) return -1;

    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    float escala_x = tela_largura / 1920.0f;
    float escala_y = tela_altura / 1080.0f;

    AreaOpcao areas[] = {
        { 177 * escala_x, 153 * escala_y, 178 * escala_x, 135 * escala_y, CLICAR_OPCAO_1, "OPCAO_1", false,
          al_map_rgba(100, 100, 100, 150), al_map_rgba(150, 150, 150, 100) },
        { 485 * escala_x, 626 * escala_y, 165 * escala_x, 140 * escala_y, CLICAR_OPCAO_2, "OPCAO_2", false,
          al_map_rgba(255, 165, 0, 150), al_map_rgba(255, 200, 0, 100) },
        { 270 * escala_x, 307 * escala_y, 109 * escala_x, 114 * escala_y, CLICAR_OPCAO_3, "OPCAO_3", false,
          al_map_rgba(200, 150, 200, 150), al_map_rgba(255, 100, 255, 100) },
        { 875 * escala_x, 600 * escala_y, 99 * escala_x, 121 * escala_y, CLICAR_OPCAO_4, "OPCAO_4", false,
          al_map_rgba(50, 150, 50, 150), al_map_rgba(100, 255, 100, 100) }
    };

    // Retorna o total de áreas (poderia ser usado externamente)
    return sizeof(areas) / sizeof(areas[0]);
}

//============================================================
// Desenha o Quiz
//============================================================

void desenha_quiz(ALLEGRO_BITMAP* caixa_dialogo, ALLEGRO_BITMAP* array_opcoes[4], ALLEGRO_FONT* fonte, pergunta* perguntaAtual, quiz* estado)
{
    if (!caixa_dialogo || !fonte || !perguntaAtual || !estado) return;

    al_draw_bitmap(caixa_dialogo, 0, 0, 0);

    for (int i = 0; i < 4; i++) {
        if (array_opcoes[i])
            al_draw_bitmap(array_opcoes[i], 0, 0, 0);
    }

    // Pergunta
    al_draw_text(fonte, al_map_rgb(255, 255, 255), 960, 150, ALLEGRO_ALIGN_CENTER, perguntaAtual->perguntaFeita);

    // Opções (posições provisórias)
    for (int i = 0; i < 4; i++) {
        al_draw_textf(fonte, al_map_rgb(255, 255, 255), 960, 300 + i * 100, ALLEGRO_ALIGN_CENTER, "%s", perguntaAtual->opcoes[i]);
    }
}

//============================================================
// Processa a resposta
//============================================================

void processa_resposta(pergunta* perguntas, quiz* estado, int opcaoClicada)
{
    if (estado->respondida) return;

    estado->respondida = true;
    estado->respostaSelecionada = opcaoClicada;

    if (opcaoClicada == perguntas[estado->perguntaAtual].respostaCerta) {
        printf("Resposta certa!\n");
    }
    else {
        estado->erros++;
        printf("Resposta errada! Total de erros: %d\n", estado->erros);

        if (estado->erros >= 3) {
            estado->perdeu = true;
            printf("Game Over!\n");
        }
    }
}

//============================================================
// Próxima Pergunta
//============================================================

void proximaPergunta(quiz* estado)
{
    estado->perguntaAtual++;
    estado->respondida = false;
    estado->respostaSelecionada = -1;
}
