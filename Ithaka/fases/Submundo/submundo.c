#include "submundo.h"
#include "stdio.h"

bool carregar_cenarios_submundo(Fase* fase)
{
    if (!fase) return false;
    init_fase(fase, 1);

    if (!fase->cenarios || fase->total_cenarios < 1) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[0], 0, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Submundo/submundoProfeta.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 1\n");
        return false;
    }

    return true;
}
