#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "fase_Polifemo.h"

const numero_cenarios = 6;

bool carregar_cenarios_polifemo(Fase* fase) {
    if (!fase) {
        fprintf(stderr, "Erro: fase é NULL\n");
        return false;
    }

    init_fase(fase, 6);

    if (!fase->cenarios || fase->total_cenarios < 6) {
        fprintf(stderr, "Erro: falha na inicialização da fase\n");
        return false;
    }

    printf("Inicializando cenário 0...\n");
    init_cenario(&fase->cenarios[0], 0, 0);
    fase->cenarios[0].fundo = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo1.png");
    if (!fase->cenarios[0].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 1\n");
        return false;
    }

    printf("Inicializando cenário 1...\n");
    init_cenario(&fase->cenarios[1], 0, 0);
    fase->cenarios[1].fundo = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo2.png");
    if (!fase->cenarios[1].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 2\n");
        return false;
    }

    printf("Inicializando cenário 2...\n");
    init_cenario(&fase->cenarios[2], 0, 0);
    fase->cenarios[2].fundo = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo3.png");
    if (!fase->cenarios[2].fundo) {
        fprintf(stderr, "❌ Erro ao carregar fundo do cenário 3\n");
        return false;
    }

    printf("Inicializando cenário 3...\n");
    init_cenario(&fase->cenarios[3], 1, 0);
    if (!fase->cenarios[3].sobreposicoes) {
        fprintf(stderr, "❌ Erro ao alocar sobreposições para cenário 4\n");
        return false;
    }
    fase->cenarios[3].fundo = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo4.png");
    fase->cenarios[3].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo4arvore.png");
    if (!fase->cenarios[3].fundo || !fase->cenarios[3].sobreposicoes[0]) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposição do cenário 4\n");
        return false;
    }

    printf("Inicializando cenário 4...\n");
    init_cenario(&fase->cenarios[4], 1, 0);
    if (!fase->cenarios[4].sobreposicoes) {
        fprintf(stderr, "❌ Erro ao alocar sobreposições para cenário 5\n");
        return false;
    }
    fase->cenarios[4].fundo = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5.png");
    fase->cenarios[4].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/Fundo_Polifemo5pedra.png");
    if (!fase->cenarios[4].fundo || !fase->cenarios[4].sobreposicoes[0] || !fase->cenarios[4].sobreposicoes[1]) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposições do cenário 5\n");
        return false;
    }

    printf("Inicializando cenário 5...\n");
    init_cenario(&fase->cenarios[5], 1, 0);
    if (!fase->cenarios[5].sobreposicoes) {
        fprintf(stderr, "❌ Erro ao alocar sobreposições para cenário 6\n");
        return false;
    }
    fase->cenarios[5].fundo = al_load_bitmap("./imagensJogo/cenarios/Polifemo/fundo_Polifemo6.png");
    fase->cenarios[5].sobreposicoes[0] = al_load_bitmap("./imagensJogo/cenarios/Polifemo/fundo_Polifemo6rocha.png");
    if (!fase->cenarios[5].fundo || !fase->cenarios[5].sobreposicoes[0]) {
        fprintf(stderr, "❌ Erro ao carregar fundo ou sobreposição do cenário 6\n");
        return false;
    }

    printf("✅ Todos os cenários carregados com sucesso!\n");
    return true;
}
