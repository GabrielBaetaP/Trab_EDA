#include "HEAP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pai(int i) {
    if (i <= 0) return -1;
    return (i - 1) / 2;
}

int filho_esq(int i) {
    return 2 * i + 1;
}

int filho_dir(int i) {
    return 2 * i + 2;
}

void armazena_tam(long int *tam){
    FILE *reg_heap = fopen("reg_heap.bin", "wb");
    if (!reg_heap) {
        printf("[guarda_tamanho] Impossivel criar 'reg_heap.bin'!\n");
        exit(1);
    }
    fwrite(tam, sizeof(long int), 1, reg_heap);
    fclose(reg_heap);
}

int retorna_tam(long int *tam_heap) {
    FILE *reg_heap = fopen("reg_heap.bin", "rb");
    if (!reg_heap) {
        *tam_heap = 0; // Se o arquivo não existe, o heap está vazio
        return 0;
    }
    fread(tam_heap, sizeof(long int), 1, reg_heap);
    fclose(reg_heap);
    return 1;
}

void subir(FILE *heap, int ind, long int *tam) {
    if (ind <= 0) return;
    int pai_ind = pai(ind);
    Dados filho, pai_dados;

    fseek(heap, ind * sizeof(Dados), SEEK_SET);
    fread(&filho, sizeof(Dados), 1, heap);

    fseek(heap, pai_ind * sizeof(Dados), SEEK_SET);
    fread(&pai_dados, sizeof(Dados), 1, heap);

    if (filho.nota_final > pai_dados.nota_final) {
        fseek(heap, pai_ind * sizeof(Dados), SEEK_SET);
        fwrite(&filho, sizeof(Dados), 1, heap);

        fseek(heap, ind * sizeof(Dados), SEEK_SET);
        fwrite(&pai_dados, sizeof(Dados), 1, heap);

        fflush(heap);
        subir(heap, pai_ind, tam);
    }
}

void descer(FILE *heap, int ind, long int *tam) {
    int filho_esq_ind = filho_esq(ind);
    int filho_dir_ind = filho_dir(ind);
    int maior_ind = ind;

    Dados pai_dados, filho_esq_dados, filho_dir_dados, aux;

    fseek(heap, ind * sizeof(Dados), SEEK_SET);
    fread(&pai_dados, sizeof(Dados), 1, heap);

    if (filho_esq_ind < *tam) {
        fseek(heap, filho_esq_ind * sizeof(Dados), SEEK_SET);
        fread(&filho_esq_dados, sizeof(Dados), 1, heap);
        if (filho_esq_dados.nota_final > pai_dados.nota_final) {
            maior_ind = filho_esq_ind;
        }
    }

    if (filho_dir_ind < *tam) {
        Dados maior_atual;
        fseek(heap, maior_ind * sizeof(Dados), SEEK_SET);
        fread(&maior_atual, sizeof(Dados), 1, heap);

        fseek(heap, filho_dir_ind * sizeof(Dados), SEEK_SET);
        fread(&filho_dir_dados, sizeof(Dados), 1, heap);

        if (filho_dir_dados.nota_final > maior_atual.nota_final) {
            maior_ind = filho_dir_ind;
        }
    }

    if (maior_ind != ind) {
        fseek(heap, maior_ind * sizeof(Dados), SEEK_SET);
        fread(&aux, sizeof(Dados), 1, heap);

        fseek(heap, ind * sizeof(Dados), SEEK_SET);
        fwrite(&aux, sizeof(Dados), 1, heap);

        fseek(heap, maior_ind * sizeof(Dados), SEEK_SET);
        fwrite(&pai_dados, sizeof(Dados), 1, heap);

        descer(heap, maior_ind, tam);
    }
}

void escrever(char *arq, Dados *dado, long int *tam) {
    FILE *f_heap = fopen(arq, "rb+");
    if (!f_heap) {
        printf("Erro na inserção. Não foi possível abrir o arquivo.\n");
        return;
    }
    fseek(f_heap, (*tam) * sizeof(Dados), SEEK_SET);
    fwrite(dado, sizeof(Dados), 1, f_heap);

    (*tam)++;
    subir(f_heap, *tam - 1, tam);
    armazena_tam(tam);
    fclose(f_heap);
}

void remover(char *arq, long int *tam) {
    FILE *f_heap = fopen(arq, "rb+");
    if (!f_heap) {
        printf("Erro na remoção. Não foi possível abrir o arquivo.\n");
        return;
    }

    if (*tam == 0) {
        printf("Heap está vazio. Nenhum aluno para remover.\n");
        fclose(f_heap);
        return;
    }

    Dados removido;
    fseek(f_heap, 0, SEEK_SET);
    fread(&removido, sizeof(Dados), 1, f_heap);

    Dados ultimo_dado;
    fseek(f_heap, (*tam - 1) * sizeof(Dados), SEEK_SET);
    fread(&ultimo_dado, sizeof(Dados), 1, f_heap);

    fseek(f_heap, 0, SEEK_SET);
    fwrite(&ultimo_dado, sizeof(Dados), 1, f_heap);

    (*tam)--;
    descer(f_heap, 0, tam);

    fclose(f_heap);
    armazena_tam(tam);
    printf("Aluno com maior nota REMOVIDO!\n");
    printf(" -> Nome: %s, CPF: %s, Nota: %d\n", removido.nome, removido.cpf, removido.nota_final);
}

Dados* busca(char *arq, char *cpf, long int *tam) {
    FILE *f_heap = fopen(arq, "rb");
    if (!f_heap) {
        return NULL;
    }

    Dados *dado = (Dados*)malloc(sizeof(Dados));
    if (!dado) return NULL;

    long int cont = 0;
    while(cont < *tam && fread(dado, sizeof(Dados), 1, f_heap) == 1) {
        if (strcmp(dado->cpf, cpf) == 0) {
            fclose(f_heap);
            return dado;
        }
        cont++;
    }

    free(dado);
    fclose(f_heap);
    return NULL;
}

void constroi(char *arq_h, char *arq_d, long int *tam) {
    FILE *f_dados = fopen(arq_d, "rb");
    if (!f_dados) return;

    FILE *f_heap = fopen(arq_h, "wb+");
    if (!f_heap) {
        fclose(f_dados);
        return;
    }

    Dados aluno_temp;
    while(fread(&aluno_temp, sizeof(Dados), 1, f_dados) == 1) {
        fwrite(&aluno_temp, sizeof(Dados), 1, f_heap);
    }
    fclose(f_dados);

    fseek(f_heap, 0, SEEK_END);
    (*tam) = ftell(f_heap) / sizeof(Dados);
    for (int i = (*tam / 2) - 1; i >= 0; i--) {
        descer(f_heap, i, tam);
    }
    armazena_tam(tam);
    printf("Heap com %ld elementos construido com sucesso!\n", *tam);
    fclose(f_heap);
}

int verifica(char *arq) {
    FILE *f_heap = fopen(arq, "rb");
    if (!f_heap) return 0;
    long int tam;
    retorna_tam(&tam);
    if (tam == 0) {
        fclose(f_heap);
        return 1;
    }

    for (int i = 0; i < tam; i++) {
        Dados pai_dados, filho_e_dados, filho_d_dados;

        fseek(f_heap, i * sizeof(Dados), SEEK_SET);
        fread(&pai_dados, sizeof(Dados), 1, f_heap);

        int filho_esq_ind = filho_esq(i);
        if (filho_esq_ind < tam) {
            fseek(f_heap, filho_esq_ind * sizeof(Dados), SEEK_SET);
            fread(&filho_e_dados, sizeof(Dados), 1, f_heap);
            if (filho_e_dados.nota_final > pai_dados.nota_final) {
                printf("Erro de Max-Heap: Pai na pos %d (nota %d) < Filho esquerdo (nota %d)\n", i, pai_dados.nota_final, filho_e_dados.nota_final);
                fclose(f_heap);
                return 0;
            }
        }

        int filho_dir_ind = filho_dir(i);
        if (filho_dir_ind < tam) {
            fseek(f_heap, filho_dir_ind * sizeof(Dados), SEEK_SET);
            fread(&filho_d_dados, sizeof(Dados), 1, f_heap);
            if (filho_d_dados.nota_final > pai_dados.nota_final) {
                printf("Erro de Max-Heap: Pai na pos %d (nota %d) < Filho direito (nota %d)\n", i, pai_dados.nota_final, filho_d_dados.nota_final);
                fclose(f_heap);
                return 0;
            }
        }
    }
    fclose(f_heap);
    return 1;
}