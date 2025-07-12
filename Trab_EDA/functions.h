#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Math.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define TAM 5
#define CPF 12

typedef struct {
    int cpf;
    char nome[50];
    int nota_final;
}Dados;

typedef struct {
    int folha;
    int num_chaves;
    int chaves[TAM * 2 - 1];
    long filhos[TAM * 2];
    long reg[TAM * 2 - 1];
    long int proximo_no;
}TARVBP;

extern const char *nomes[100];
extern const char *sobrenomes[100];
extern const char *a[100];
extern const char *b[100];

#endif //FUNCTIONS_H

void gerador(const char **n, const char **s, const char *arquivo);
void gerador_nota(const char *arquivo);
void leitura_escrita(const char *arquivo_bin, const char *arquivo_txt);
void leitura_escrita_nota(const char *arquivo_bin, const char *arquivo_txt);

TARVBP* TARVBP_cria();
int TARVBP_tem_id(TARVBP *a, long int id, int t);
TARVBP* TARVBP_busca(TARVBP* a, long int id, int t);
void TARVBP_insere(FILE *arq_i, int chave, long reg);
TARVBP* TARVBP_retira(TARVBP* a, unsigned long int id, int t);
void TARVBP_libera(TARVBP* a, int t);
