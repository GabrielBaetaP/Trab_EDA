#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef HASH_H
#define HASH_H

typedef struct {
    char cpf[13];
    char nome[50];
    int nota_final;
}Dados;

#endif

Dados inicializar();

Dados novo_dado(char *name, int nota_final, char* cpf);

int h(char *cpf, int k);

int busca(FILE* file, char* cpf);

int insere(FILE* file, Dados student, int *num_elem);

void remover(FILE* file, char* cpf);

void inicializa_arq(FILE* file);

void constroi(FILE* file_hash, FILE* file_student);
