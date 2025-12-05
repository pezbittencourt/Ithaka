#include "Itaca.h"
#include "stdio.h"

bool carregar_cenarios_itaca(Fase* fase)
{
    if (!fase) return false;
    init_fase(fase, 3);

    if (!fase->cenarios || fase->total_cenarios < 3) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[0], 0, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Itaca/fundoItaca1.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 1\n");
        return false;
    }

    printf("Inicializando cenário 2...\n");
    init_cenario(&fase->cenarios[1], 0, 0);
    fase->cenarios[1].fundo = al_load_bitmap("./imagensJogo/cenarios/Itaca/fundoItaca2.png");
    if (!fase->cenarios[1].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 2\n");
        return false;
    }

    printf("Inicializando cenário 3...\n");
    init_cenario(&fase->cenarios[2], 0, 0);
    fase->cenarios[2].fundo = al_load_bitmap("./imagensJogo/cenarios/Itaca/fundoItaca3.png");
    if (!fase->cenarios[2].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 3\n");
        return false;
    }
    fase->cenarios[2].dialogo_caminho = "./dialogo/source/dialogoPenelopeOdisseuFundoItaca3.txt";
    fase->cenarios[2].dialogo_completo = false;


    return true;
}
