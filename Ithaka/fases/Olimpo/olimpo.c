#include "olimpo.h"
#include "stdio.h"
#include <allegro5/allegro_font.h>

bool carregar_cenarios_olimpo(Fase* fase)
{
    if (!fase) return false;
    // Initialize the fase with 6 scenarios
    init_fase(fase, 1);

    // Verify allocation was successful
    if (!fase->cenarios || fase->total_cenarios < 1) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[0], 0, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Olimpo/fundoOlimpo.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 1\n");
        return false;
    }

    return true;
}

//Estrutura geral das perguntas //Numero da pergunta (definido na main)
void fazendo_as_perguntas(pergunta perguntas[]) {

    //Pergunta 1:
    strcpy(perguntas[0].perguntaFeita, "No mito de Prometeu, o que o titâ roubou de nós, deuses, para dar aos mortais?");
    strcpy(perguntas[0].opcoes[0], "Fogo");
    strcpy(perguntas[0].opcoes[1], "Poder eterno");
    strcpy(perguntas[0].opcoes[2], "Vinho");
    strcpy(perguntas[0].opcoes[3], "A erva sagrada Moli");
    perguntas[0].respostaCerta = 0;

    //Pergunta 2:
    strcpy(perguntas[1].perguntaFeita, "Qual foi a única coisa que restou na caixa após Pandora a abrir?");
    strcpy(perguntas[1].opcoes[0], "Amor");
    strcpy(perguntas[1].opcoes[1], "Luxúria");
    strcpy(perguntas[1].opcoes[2], "Esperança");
    strcpy(perguntas[1].opcoes[3], "Miséria");
    perguntas[1].respostaCerta = 2;


    //Pergunta 3:
    strcpy(perguntas[2].perguntaFeita, "De quem Ícaro, aquele que teve suas asas de cera derretidas ao contato com o sol, era filho?");
    strcpy(perguntas[2].opcoes[0], "Páris");
    strcpy(perguntas[2].opcoes[1], "Teseu");
    strcpy(perguntas[2].opcoes[2], "Dédalo");
    strcpy(perguntas[2].opcoes[3], "Narciso");
    perguntas[2].respostaCerta = 2;


    //Pergunta 4:
    strcpy(perguntas[3].perguntaFeita, "Dado o contexto, qual foi o estopim para a Guerra de Tróia?");
    strcpy(perguntas[3].opcoes[0], "O assassinato de Menelau, rei de Esparta");
    strcpy(perguntas[3].opcoes[1], "O sequestro da rainha espartana Helena");
    strcpy(perguntas[3].opcoes[2], "Conflitos territoriais");
    strcpy(perguntas[3].opcoes[3], "O Assassinato de Páris, principe Troiano.");
    perguntas[3].respostaCerta = 1;

    //Pergunta 5:
    strcpy(perguntas[4].perguntaFeita, "Qual foi a condição apresentada pelo cupido Eros para que ele e a mortal Psiquê pudessem viver juntos?");
    strcpy(perguntas[4].opcoes[0], "Ela nunca poderia ver seu rosto");
    strcpy(perguntas[4].opcoes[1], "Eles nunca poderiam se tocar");
    strcpy(perguntas[4].opcoes[2], "Psiquê jamais poderia prover filhos");
    strcpy(perguntas[4].opcoes[3], "Nunca houve uma condição");
    perguntas[4].respostaCerta = 0;

    //Pergunta 6:
    strcpy(perguntas[5].perguntaFeita, "Qual o membro amputado de Urano que, ao tocar as espumas do mar, teria gerado Vênus?");
    strcpy(perguntas[5].opcoes[0], "Sua perna direita");
    strcpy(perguntas[5].opcoes[1], "Seu braço esquerdo");
    strcpy(perguntas[5].opcoes[2], "Seu pé esquerdo");
    strcpy(perguntas[5].opcoes[3], "Seu orgão reprodutor");
    perguntas[5].respostaCerta = 3;


    //Pergunta 7:
    strcpy(perguntas[6].perguntaFeita, "De que forma Narciso morreu?");
    strcpy(perguntas[6].opcoes[0], "Afogado, após cair no rio próximo a onde residia");
    strcpy(perguntas[6].opcoes[1], "Amaldiçoado por Afrodite, pois ela invejava sua beleza");
    strcpy(perguntas[6].opcoes[2], "Após ingerir um fruto proíbido pelos deuses");
    strcpy(perguntas[6].opcoes[3], "Preso em seu próprio reflexo, apaixonado");
    perguntas[6].respostaCerta = 3;

    //Pergunta 8:
    strcpy(perguntas[7].perguntaFeita, "Qual era a criatura que habitava o labirinto de Dédalo, em Creta?");
    strcpy(perguntas[7].opcoes[0], "Harpia");
    strcpy(perguntas[7].opcoes[1], "Centauro");
    strcpy(perguntas[7].opcoes[2], "Quimera");
    strcpy(perguntas[7].opcoes[3], "Minotauro");
    perguntas[7].respostaCerta = 3;

    //Pergunta 9:
    strcpy(perguntas[8].perguntaFeita, "Qual foi o castigo do rei Sísifo por nos desafiar e enganar?");
    strcpy(perguntas[8].opcoes[0], "Ficar cego e vagar eternamente pelo submundo");
    strcpy(perguntas[8].opcoes[1], "Empurrar uma pedra montanha acima, a qual sempre rolava de volta");
    strcpy(perguntas[8].opcoes[2], "Ter seus orgâos devorados por corvos e regenerados diariamente");
    strcpy(perguntas[8].opcoes[3], "Andar eternamente pelo tártaro preso por correntes");
    perguntas[8].respostaCerta = 1;

    //Pergunta 10:
    strcpy(perguntas[9].perguntaFeita, "Qual foi o dom concedido a Midas que acabou se tornando sua maldição?");
    strcpy(perguntas[9].opcoes[0], "Ler pensamentos");
    strcpy(perguntas[9].opcoes[1], "Vida eterna");
    strcpy(perguntas[9].opcoes[2], "Tudo o que toca se transforma em ouro");
    strcpy(perguntas[9].opcoes[3], "Prever o futuro");
    perguntas[9].respostaCerta = 2;

    //Pergunta 11:
    strcpy(perguntas[10].perguntaFeita, "Qual foi o titã condenado a segurar o céu sobre os próprios ombros?");
    strcpy(perguntas[10].opcoes[0], "Prometeu");
    strcpy(perguntas[10].opcoes[1], "Atlas");
    strcpy(perguntas[10].opcoes[2], "Cronos");
    strcpy(perguntas[10].opcoes[3], "Zeus");
    perguntas[10].respostaCerta = 1;

    //Pergunta 12:
    strcpy(perguntas[11].perguntaFeita, "Qual foi o castigo de Tântalo após servir seu próprio filho, Pélops, em um banquete conosco");
    strcpy(perguntas[11].opcoes[0], "Morrer de fome e sede em meio a abundância");
    strcpy(perguntas[11].opcoes[1], "Ter o fígado devorado por uma cobra");
    strcpy(perguntas[11].opcoes[2], "Ter seus membros servidos às almas condenadas no Tártaro");
    strcpy(perguntas[11].opcoes[3], "Ficar acorrentado eternamente");
    perguntas[11].respostaCerta = 0;

}

//Mouse sobre as opcoes: 

int exibir_mapa_inicial(ALLEGRO_DISPLAY* display) {
    int tela_largura = al_get_display_width(display);
    int tela_altura = al_get_display_height(display);

    float escala_x = tela_largura / 1920.0f;
    float escala_y = tela_altura / 1080.0f;

    AreaOpcao areas[] = { //!!Arrumar coordenada de todos!!
        {
            .x = 177 * escala_x,
            .y = 153 * escala_y,
            .largura = 178 * escala_x,
            .altura = 135 * escala_y,
            .nome = "OPCAO_1",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(100, 100, 100, 150),
            .cor_destaque = al_map_rgba(150, 150, 150, 100)
        },
        {
            .x = 485 * escala_x,
            .y = 626 * escala_y,
            .largura = 165 * escala_x,
            .altura = 140 * escala_y,
            .nome = "OPCAO_2",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(255, 165, 0, 150),
            .cor_destaque = al_map_rgba(255, 200, 0, 100)
        },
        {
            .x = 270 * escala_x,
            .y = 307 * escala_y,
            .largura = 109 * escala_x,
            .altura = 114 * escala_y,
            .nome = "OPCAO_3",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(200, 150, 200, 150),
            .cor_destaque = al_map_rgba(255, 100, 255, 100)
        },
        {
            .x = 875 * escala_x,
            .y = 600 * escala_y,
            .largura = 99 * escala_x,
            .altura = 121 * escala_y,
            .nome = "OPCAO_4",
            .mouse_sobre = false,
            .cor_normal = al_map_rgba(50, 150, 50, 150),
            .cor_destaque = al_map_rgba(100, 255, 100, 100)
        },
    };
}


//Pergunta atual
void desenha_quiz(ALLEGRO_BITMAP* caixa_dialogo, ALLEGRO_BITMAP* array_opcoes[4], ALLEGRO_FONT* fonte, pergunta* perguntaAtual, quiz* estado) {


    //Desenhando todas as imagens na posição 0 0 (Na teoria, já estão posicionadas corretamente + Array das Opções definido na Main)

    al_draw_bitmap(caixa_dialogo, 0, 0, 0);
    al_draw_bitmap(array_opcoes[0], 0, 0, 0);
    al_draw_bitmap(array_opcoes[1], 0, 0, 0);
    al_draw_bitmap(array_opcoes[2], 0, 0, 0);
    al_draw_bitmap(array_opcoes[3], 0, 0, 0);

    //Desenhando a pergunta (!!Arrumar coordenada!!)

    al_draw_text(fonte, al_map_rgb(255, 255, 255), 100, 100,
        ALLEGRO_ALIGN_CENTER, perguntaAtual->perguntaFeita);

    //Desenhando as opções (!!Arrumar coordenada!!)

    al_draw_text(fonte, al_map_rgb(255, 255, 255), 200, 200,
        ALLEGRO_ALIGN_CENTER, perguntaAtual->opcoes[0]);

    al_draw_text(fonte, al_map_rgb(255, 255, 255), 200, 200,
        ALLEGRO_ALIGN_CENTER, perguntaAtual->opcoes[1]);

    al_draw_text(fonte, al_map_rgb(255, 255, 255), 200, 200,
        ALLEGRO_ALIGN_CENTER, perguntaAtual->opcoes[2]);

    al_draw_text(fonte, al_map_rgb(255, 255, 255), 200, 200,
        ALLEGRO_ALIGN_CENTER, perguntaAtual->opcoes[3]);

}

//Processando a resposta

void processa_resposta(pergunta* perguntas, quiz* estado, int opcaoClicada) {
    if (estado->respondida) return; // Se a pergunta for respondida...

    estado->respondida = true;
    estado->respostaSelecionada = opcaoClicada;

    //... Ver se ela está correta!

    if (opcaoClicada == perguntas[estado->perguntaAtual].respostaCerta) {
       //Ta certa!! Fica verdinho (como nao sei)
    }
    else {
        estado->erros++;
       //Errooou, fica vermelho (Também não sei como...)

    //Ver se o jogador errou 3 vezes

    if (estado->erros >= 3) {
       estado->perdeu = true;
       //Game Over. Volta do inicio da fase (comooo)
        }
    }
}

//Passando para a próxima pergunta

void proximaPergunta(quiz* estado) {
    estado->perguntaAtual++;
    estado->respondida = false;
    estado->respostaSelecionada = -1;

}


    

