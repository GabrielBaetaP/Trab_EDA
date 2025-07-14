#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char cpf[13];
    char nome[50];
    int nota_final;
} Dados;

int pai(int i);
int filho_esq(int i);
int filho_dir(int i);

void armazena_tam(long int *tam);
int retorna_tam(long int *tam);

void subir(FILE *heap, int ind, long int *tam);
void descer(FILE *heap, int ind, long int *tam);

void escrever(char *nome, Dados *aluno, long int *tam);
void remover(char *nome, long int *tam_heap);
Dados* busca(char *nome, char *cpf, long int *tam);

void constroi(char *nome, char *nomeDados, long int *tam);
int verifica(char *nome);

#endif // HEAP_H