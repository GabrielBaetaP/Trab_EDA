#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#ifndef TARVBP_H
#define TARVBP_H

#define TAM 10

typedef struct {
    char cpf[13];
    char nome[50];
    int nota_final;
}Dados;

typedef struct {
    int eh_folha;
    int num_chaves;
    long long ponteiro_pai;
    char chaves[TAM * 2][13];
    long long filhos[TAM * 2 + 1];
    long long endereco;
    long long reg[TAM * 2];
    long long proximo_no;
}TARVBP;

long long constroi_arvore(const char* arq_dados_origem, const char* arq_idx_nome, const char* arq_dados_arvore);
void TARVBP_reseta_contador_nos();
long long TARVBP_insere(long long endereco_raiz, Dados* d, const char* arq_idx_nome, const char* arq_dados_nome);
long long busca_cpf(const char* arq_idx_nome, const char* cpf, long long endereco_raiz);
long long TARVBP_remove(long long endereco_raiz, const char* cpf, const char* arq_idx_nome);
void buscar_e_imprimir_dados(long long endereco_raiz, const char* cpf_busca, const char* arq_idx_nome, const char* arq_dados_nome);

#endif //TARVBP_H
