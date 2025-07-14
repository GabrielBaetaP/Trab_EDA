#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Math.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define TAM 100

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

extern const char *nomes[100];
extern const char *sobrenomes[100];
extern const char *a[100];
extern const char *b[100];

long long TARVBP_insere(long long endereco_raiz, Dados* d, const char* arq_idx_nome, const char* arq_dados_nome);
long long busca_cpf(const char* arq_idx_nome, const char* cpf, long long endereco_raiz);

long long TARVBP_remove(long long endereco_raiz, const char* cpf, const char* arq_idx_nome);

#endif //FUNCTIONS_H

void gerador(const char **n, const char **s, const char *arquivo);
void gerador_nota(const char *arquivo);
void leitura_escrita(const char *arquivo_bin, const char *arquivo_txt);
void leitura_escrita_nota(const char *arquivo_bin, const char *arquivo_txt);
long long cpf_string_para_long(const char* cpf_str);


