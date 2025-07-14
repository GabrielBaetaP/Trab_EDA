#include "functions.h"
#include <time.h>

Dados* ler_dados(FILE* cpf, FILE* nome, FILE* nota) {
    Dados* d = malloc(sizeof(Dados));
    if (!d) return NULL;
    char linha[500];

    if (!fgets(linha, sizeof(linha), cpf)) { free(d); return NULL; }
    linha[strcspn(linha, "\r\n")] = 0;
    strcpy(d->cpf, linha);

    if (!fgets(linha, sizeof(linha), nome)) { free(d); return NULL; }
    linha[strcspn(linha, "\r\n")] = 0;
    strcpy(d->nome, linha);

    if (!fgets(linha, sizeof(linha), nota)) { free(d); return NULL; }
    linha[strcspn(linha, "\r\n")] = 0;
    d->nota_final = atoi(linha);

    return d;
}

void inic_arq() {
    FILE* f = fopen("ArqIdx.bin", "wb");
    if(f) fclose(f);
    f = fopen("ArqDados.bin", "wb");
    if(f) fclose(f);
}


int main(void) {
    inic_arq();
    TARVBP_reseta_contador_nos();

    FILE* cpf = fopen("cpf.txt", "r");
    FILE* nome = fopen("nome.txt", "r");
    FILE* nota = fopen("nota.txt", "r");

    if (!cpf || !nome || !nota) {
        printf("Erro ao abrir arquivos de entrada.\n");
        return 1;
    }

    int index = 0;
    char buscas[10001][13];
    long long raiz = -1;

    for (int i = 0; i < 10000; i++) {
        Dados* d = ler_dados(cpf, nome, nota);
        if (!d) break;
        strcpy(buscas[index++], d->cpf);
        raiz = TARVBP_insere(raiz, d, "ArqIdx.bin", "ArqDados.bin");

        printf("cpf: %s raiz:%lld:  %d\n", d->cpf, raiz, i);

        free(d);
    }
    printf("\n--- Buscas ---\n");
    int encontrados = 0;
    for (int i = 0; i < index; i++) {
        long long res = busca_cpf("ArqIdx.bin", buscas[i], raiz);
        if (res != -1) {
            printf("%d: achamos: %lld\n", i + 1, res);
            encontrados++;
        } else {
            printf("%d: NAO ACHAMOS: %s\n", i + 1, buscas[i]);
        }
    }
    printf("\n--- Resumo: %d de %d CPFs encontrados com sucesso. ---\n", encontrados, index);

    printf("\n--- Removendo 10 registros ---\n");
    char cpfs_para_remover[10][13];
    srand(time(NULL));

    // Escolhe 10 CPFs aleatórios da lista para remover
    for(int i = 0; i < 10; i++){
        int r_index = rand() % index;
        strcpy(cpfs_para_remover[i], buscas[r_index]);
        // Remove da lista de busca para não procurar por ele depois
        strcpy(buscas[r_index], "");
    }

    for (int i = 0; i < 10; i++) {
        printf("Removendo CPF: %s...\n", cpfs_para_remover[i]);
        raiz = TARVBP_remove(raiz, cpfs_para_remover[i], "ArqIdx.bin");
        printf("CPF %s removido. Nova raiz: %lld\n", cpfs_para_remover[i], raiz);
    }

    // --- Verificação Final ---
    printf("\n--- Verificando a consistência da árvore após a remoção ---\n");
    encontrados = 0;
    int nao_encontrados_corretamente = 0;

    // Verifica se os que NÃO foram removidos AINDA SÃO encontrados
    for (int i = 0; i < index; i++) {
        if(strcmp(buscas[i], "") == 0) continue; // Pula os que foram marcados para remoção
        if (busca_cpf("ArqIdx.bin", buscas[i], raiz) != -1) {
            encontrados++;
        } else {
            printf("ERRO GRAVE: CPF %s deveria existir, mas não foi encontrado!\n", buscas[i]);
        }
    }

    // Verifica se os que FORAM removidos NÃO SÃO mais encontrados
    for (int i = 0; i < 10; i++) {
        if (busca_cpf("ArqIdx.bin", cpfs_para_remover[i], raiz) == -1) {
            nao_encontrados_corretamente++;
        } else {
            printf("ERRO GRAVE: CPF %s deveria ter sido removido, mas foi encontrado!\n", cpfs_para_remover[i]);
        }
    }

    printf("\n--- Resumo Final ---\n");
    printf("Registros que deveriam existir e foram encontrados: %d de %d\n", encontrados, index - 10);
    printf("Registros que deveriam ter sido removidos e não foram encontrados: %d de 10\n", nao_encontrados_corretamente);

    printf("\n======================================================\n");
    printf("MODO DE BUSCA INTERATIVA\n");
    printf("======================================================\n");

    char cpf_digitado[14];
    while(1) {
        printf("\nDigite um CPF para buscar (ou 'sair' para terminar): ");
        // Lê até 13 caracteres para evitar buffer overflow
        if(scanf("%13s", cpf_digitado) != 1) break;

        // Limpa o buffer de entrada para evitar problemas com a próxima leitura
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (strcmp(cpf_digitado, "sair") == 0) {
            break;
        }

        // Chama a nova função para buscar e imprimir os dados
        buscar_e_imprimir_dados(raiz, cpf_digitado, "ArqIdx.bin", "ArqDados.bin");
    }

    printf("\nPrograma finalizado.\n");

    fclose(cpf);
    fclose(nome);
    fclose(nota);

    return 0;
}