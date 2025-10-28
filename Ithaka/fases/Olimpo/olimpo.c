#include "olimpo.h"
#include "stdio.h"

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
