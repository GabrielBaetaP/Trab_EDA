#include <string.h>
#include <stdlib.h>
#include "Hash.h"

#define hash_tam 100000

Dados inicializar() {
    Dados dado;
    memset(dado.nome, 0, 50);
    memset(dado.cpf, 0, 13);
    strncpy(dado.nome, "Vazio", 49);
    strncpy(dado.cpf, "-1", 12);
    dado.nota_final = -1;
    return dado;
}

Dados novo_dado(char *name, int nota_final, char* cpf) {
    Dados novo;
    memset(novo.nome, 0, 50);
    memset(novo.cpf, 0, 13);
    strncpy(novo.nome, name, 49);
    strncpy(novo.cpf, cpf, 12);
    novo.nota_final = nota_final;
    return novo;
}

int h(char *cpf, int k) {
    long long int cpf_val = atoll(cpf);
    srand(cpf_val / 100);
    return (rand() % hash_tam + k) % hash_tam;
}

int busca(FILE* arq, char* cpf) {
    rewind(arq);

    int tentativa = 0;
    int pos;
    Dados alvo;

    while (tentativa < hash_tam) {
        pos = h(cpf, tentativa);
        fseek(arq, (sizeof(Dados) * pos), SEEK_SET);
        fread(&alvo, sizeof(Dados), 1, arq);
        if (strcmp(alvo.cpf, cpf) == 0) return pos;
        if (strcmp(alvo.cpf, "-1") == 0) break;
        tentativa++;
    }

    printf("Erro na busca. Estudante inexistente.\n");
    return -1;
}

int insere(FILE* arq, Dados dado, int *num_elem) {
    if (!arq) {
        printf("Erro na inserção. Não foi possível abrir o arquivo.\n");
        if(num_elem) (*num_elem)--;
        return -1;
    }

    int tentativa = 0;
    int pos;
    Dados is_ocu;

    while (tentativa < hash_tam) {
        pos = h(dado.cpf, tentativa); // Chama a nova função h
        fseek(arq, (sizeof(Dados) * pos), SEEK_SET);
        fread(&is_ocu, sizeof(Dados), 1, arq);

        if (strcmp(is_ocu.cpf, dado.cpf) == 0) {
            printf("Erro na inserção. Aluno com CPF %s ja inserido.\n", dado.cpf);
            if(num_elem) (*num_elem)--;
            return -1;
        }

        if (strcmp(is_ocu.cpf, "-1") == 0) {
            fseek(arq, (sizeof(Dados) * pos), SEEK_SET);
            fwrite(&dado, sizeof(Dados), 1, arq);
            return tentativa;
        }
        tentativa++;
    }

    printf("Erro na inserção. Tabela Hash cheia.\n");
    if(num_elem) (*num_elem)--;
    return -1;
}

void remover(FILE* arq, char* cpf) {
    if (!arq) {
        printf("Erro na remoção. Arquivo inválido.\n");
        return;
    }

    int pos = busca(arq, cpf);
    if (pos == -1) return;


    fseek(arq, (sizeof(Dados) * pos), SEEK_SET);

    Dados alvo = inicializar();

    fwrite(&alvo, sizeof(Dados), 1, arq);
    printf("Estudante com CPF %s removido com sucesso.\n", cpf);
}

void inicializa_arq(FILE* arq) {
    if (!arq) {
        printf("Erro na preparação. Arquivo inválido.\n");
        return;
    }
    rewind(arq);
    Dados dado = inicializar();
    for (int i = 0; i < hash_tam; i++) {
        fwrite(&dado, sizeof(Dados), 1, arq);
    }
}

void constroi(FILE* f_hash, FILE* f_dados) {
    inicializa_arq(f_hash);
    rewind(f_dados);

    int total_colisao = 0;
    int qtd_dados = 0;
    Dados dado;

    while(fread(&dado, sizeof(Dados), 1, f_dados) == 1){
        if (strcmp(dado.cpf, "-1") != 0) {
            int colisoes = insere(f_hash, dado, &qtd_dados);
            if (colisoes != -1) {
                total_colisao += colisoes;
                qtd_dados++;
            }
        }
    }

    printf("\n%d alunos foram inseridos com um total de %d colisões!\n", qtd_dados, total_colisao);
}