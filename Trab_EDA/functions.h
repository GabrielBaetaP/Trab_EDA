#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Math.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define TAM 5

typedef struct {
    long long cpf;
    char nome[50];
    int nota_final;
}Dados;

typedef struct {
    int eh_folha;
    int num_chaves;
    long ponteiro_pai;
    int filhos_sao_folhas;
    long long chaves[TAM * 2 - 1];
    long filhos[TAM * 2];
    Dados dados[TAM * 2 - 1];
    long proximo_no_folha;
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
long long cpf_string_para_long(const char* cpf_str);

void inicializar_arvbp(TARVBP* arvbp, int eh_folha);
void inicializar(const char* arq_meta, const char* arq_idx, const char* arq_dados);
long get_end_raiz(FILE *arq_meta);
void ler_metadados(const char* arq_meta, long* end_raiz, int* eh_folha);
void escrever_metadados(const char* arq_meta, long end_raiz, int eh_folha);
void ler_no(FILE *f_idx, FILE *f_dados, long end, int eh_folha, TARVBP *arvbp);
bool buscar_cpf(long long cpf_alvo, Dados *result);
long end_final(const char *arquivo);
void escrever_no(const char* arq_idx, const char* arq_dados, long end, TARVBP* arvbp);
void divisao_interno(FILE *f_idx, FILE *f_dados, long end_cheio, long long chave_vinda_baixo, long end_dir, long long* chave_acima, long* end_novo);
int insere_aux(FILE* f_idx, FILE* f_dados, long end, int eh_folha, long long* chave_promovida, long* end_novo, Dados* novo);
void inserir_dados(Dados* novo);


// TARVBP* TARVBP_cria();
// int TARVBP_tem_id(TARVBP *a, long int id, int t);
// TARVBP* TARVBP_busca(TARVBP* a, long int id, int t);
// TARVBP* TARVBP_insere(TARVBP* a, Dados *novo);
// TARVBP* TARVBP_retira(TARVBP* a, unsigned long int id, int t);
// void TARVBP_libera(TARVBP* a, int t);
