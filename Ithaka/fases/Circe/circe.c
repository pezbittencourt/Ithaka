#include "circe.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>

bool carregar_cenarios_circe(Fase* fase) {
    if (!fase) return false;
    // Initialize the fase with 6 scenarios
    init_fase(fase, 4);

    // Verify allocation was successful
    if (!fase->cenarios || fase->total_cenarios < 4) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[0], 1, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce1.png");
    fase->cenarios[0].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce1.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposicoes do cenário 1\n");
        return false;
    }

    printf("Inicializando cenário 2...\n");
    init_cenario(&fase->cenarios[1], 0, 0);
    fase->cenarios[1].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce2.png");
    if (!fase->cenarios[1].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 2\n");
        return false;
    }

    printf("Inicializando cenário 3...\n");
    init_cenario(&fase->cenarios[2], 2, 0);
    fase->cenarios[2].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce3.png");
    fase->cenarios[2].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Circe/frenteCirce3.png");
    fase->cenarios[2].sobreposicoes[1] = al_load_bitmap("./imagensJogo/cenarios/Circe/efeitoFrenteCirce3.png");
    if (!fase->cenarios[2].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposicoes do cenário 3\n");
        return false;
    }

    printf("Inicializando cenário 4...\n");
    init_cenario(&fase->cenarios[3], 0, 0);
    fase->cenarios[3].fundo = al_load_bitmap("./imagensJogo/cenarios/Circe/fundoCirce4.png");
    if (!fase->cenarios[3].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 4\n");
        return false;
    }

    return true;
}
