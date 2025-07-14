#include "TARVBP.h"

int nos_atual = 0;

void ler_no(const char* arq_idx, long long endereco, TARVBP* no);
void escrever_no(const char* arq_idx, TARVBP* no);
TARVBP criar_no();
void balancear_no_apos_remocao(long long endereco_no, const char* arq_idx);
void divisao(TARVBP* pai, int i_filho, TARVBP* filho, const char* arq_idx);
void insere_recursivo(TARVBP* no, Dados* d, const char* arq_idx, const char* arq_dados);
int remover_chave_de_no(TARVBP *no, const char* cpf);

void TARVBP_reseta_contador_nos() {
    nos_atual = 0;
}

TARVBP criar_no() {
    TARVBP no;
    memset(&no, 0, sizeof(TARVBP));
    no.eh_folha = 1;
    no.proximo_no = -1;
    no.ponteiro_pai = -1;
    for(int i = 0; i < TAM * 2 + 1; i++) no.filhos[i] = -1;
    no.endereco = nos_atual * sizeof(TARVBP);
    nos_atual++;
    return no;
}

long long TARVBP_insere(long long endereco_raiz, Dados* d, const char* arq_idx_nome, const char* arq_dados_nome) {
    if (busca_cpf(arq_idx_nome, d->cpf, endereco_raiz) != -1) {
        return endereco_raiz;
    }

    TARVBP raiz_no;
    if (endereco_raiz == -1) {
        raiz_no = criar_no();
        escrever_no(arq_idx_nome, &raiz_no);
        endereco_raiz = raiz_no.endereco;
    } else {
        ler_no(arq_idx_nome, endereco_raiz, &raiz_no);
    }

    if (raiz_no.num_chaves == (2 * TAM - 1)) {
        TARVBP nova_raiz = criar_no();
        nova_raiz.eh_folha = 0;
        nova_raiz.filhos[0] = raiz_no.endereco;
        raiz_no.ponteiro_pai = nova_raiz.endereco;

        escrever_no(arq_idx_nome, &raiz_no);

        divisao(&nova_raiz, 0, &raiz_no, arq_idx_nome);
        insere_recursivo(&nova_raiz, d, arq_idx_nome, arq_dados_nome);

        endereco_raiz = nova_raiz.endereco;
    } else {
        insere_recursivo(&raiz_no, d, arq_idx_nome, arq_dados_nome);
    }
    return endereco_raiz;
}


void insere_recursivo(TARVBP* no, Dados* d, const char* arq_idx, const char* arq_dados) {
    if (no->eh_folha) {
        int i = no->num_chaves - 1;

        while (i >= 0 && strcmp(d->cpf, no->chaves[i]) < 0) {
            strcpy(no->chaves[i + 1], no->chaves[i]);
            no->reg[i + 1] = no->reg[i];
            i--;
        }

        strcpy(no->chaves[i + 1], d->cpf);
        no->num_chaves++;

        FILE* f_dados = fopen(arq_dados, "ab");
        if (f_dados) {
            fseek(f_dados, 0, SEEK_END);
            no->reg[i + 1] = ftell(f_dados);
            fwrite(d, sizeof(Dados), 1, f_dados);
            fclose(f_dados);
        }
        escrever_no(arq_idx, no);
    }
    else {
        int i = no->num_chaves - 1;
        while (i >= 0 && strcmp(d->cpf, no->chaves[i]) < 0) {
            i--;
        }
        i++;

        TARVBP filho;
        ler_no(arq_idx, no->filhos[i], &filho);

        if (filho.num_chaves == (2 * TAM - 1)) {
            divisao(no, i, &filho, arq_idx);
            if (strcmp(d->cpf, no->chaves[i]) > 0) {
                i++;
            }
        }

        TARVBP filho_correto;
        ler_no(arq_idx, no->filhos[i], &filho_correto);
        insere_recursivo(&filho_correto, d, arq_idx, arq_dados);
    }
}


void divisao(TARVBP* pai, int i_filho, TARVBP* filho, const char* arq_idx) {
    TARVBP novo_irmao = criar_no();
    novo_irmao.eh_folha = filho->eh_folha;
    novo_irmao.ponteiro_pai = pai->endereco;

    if (!filho->eh_folha) {
        novo_irmao.num_chaves = TAM - 1;
        for (int j = 0; j < TAM - 1; j++) strcpy(novo_irmao.chaves[j], filho->chaves[j + TAM]);
        for (int j = 0; j < TAM; j++) novo_irmao.filhos[j] = filho->filhos[j + TAM];
    } else {
        novo_irmao.num_chaves = TAM;
        for (int j = 0; j < TAM; j++) {
            strcpy(novo_irmao.chaves[j], filho->chaves[j + TAM - 1]);
            novo_irmao.reg[j] = filho->reg[j + TAM - 1];
        }
        novo_irmao.proximo_no = filho->proximo_no;
        filho->proximo_no = novo_irmao.endereco;
    }
    filho->num_chaves = TAM - 1;

    for (int j = pai->num_chaves; j > i_filho; j--) {
        strcpy(pai->chaves[j], pai->chaves[j - 1]);
        pai->filhos[j + 1] = pai->filhos[j];
    }
    strcpy(pai->chaves[i_filho], (!filho->eh_folha) ? filho->chaves[TAM - 1] : novo_irmao.chaves[0]);
    pai->filhos[i_filho + 1] = novo_irmao.endereco;
    pai->num_chaves++;

    if (!novo_irmao.eh_folha) {
        for (int i = 0; i <= novo_irmao.num_chaves; i++) {
            if (novo_irmao.filhos[i] == -1) continue;
            TARVBP neto;
            ler_no(arq_idx, novo_irmao.filhos[i], &neto);
            neto.ponteiro_pai = novo_irmao.endereco;
            escrever_no(arq_idx, &neto);
        }
    }
    escrever_no(arq_idx, pai);
    escrever_no(arq_idx, filho);
    escrever_no(arq_idx, &novo_irmao);
}

void ler_no(const char* arq_idx, long long endereco, TARVBP* no) {
    if (endereco == -1) return;
    FILE* f = fopen(arq_idx, "rb");
    if (!f) return;
    fseek(f, endereco, SEEK_SET);
    fread(no, sizeof(TARVBP), 1, f);
    fclose(f);
}

void escrever_no(const char* arq_idx, TARVBP* no) {
    FILE* f = fopen(arq_idx, "rb+");
    if (!f) {
        f = fopen(arq_idx, "wb+");
        if (!f) return;
    }
    fseek(f, no->endereco, SEEK_SET);
    fwrite(no, sizeof(TARVBP), 1, f);
    fclose(f);
}

long long busca_cpf(const char* arq_idx_nome, const char* cpf, long long endereco_raiz) {
    if (endereco_raiz == -1) return -1;
    TARVBP no_atual;
    ler_no(arq_idx_nome, endereco_raiz, &no_atual);
    while (!no_atual.eh_folha) {
        int i = 0;
        while (i < no_atual.num_chaves && strcmp(cpf, no_atual.chaves[i]) > 0) {
            i++;
        }
        if (i < no_atual.num_chaves && strcmp(cpf, no_atual.chaves[i]) == 0) {
            i++;
        }
        ler_no(arq_idx_nome, no_atual.filhos[i], &no_atual);
    }
    for (int i = 0; i < no_atual.num_chaves; i++) {
        if (strcmp(cpf, no_atual.chaves[i]) == 0) return no_atual.reg[i];
    }
    return -1;
}

long long TARVBP_remove(long long endereco_raiz, const char* cpf, const char* arq_idx_nome) {
    if (endereco_raiz == -1 || busca_cpf(arq_idx_nome, cpf, endereco_raiz) == -1) {
        printf("AVISO: CPF '%s' nao encontrado para remocao.\n", cpf);
        return endereco_raiz;
    }
    TARVBP no;
    ler_no(arq_idx_nome, endereco_raiz, &no);

    while (!no.eh_folha) {
        int i = 0;
        while (i < no.num_chaves && strcmp(cpf, no.chaves[i]) >= 0) i++;
        ler_no(arq_idx_nome, no.filhos[i], &no);
    }

    int idx_removido = remover_chave_de_no(&no, cpf);
    if(idx_removido == -1) return endereco_raiz;

    escrever_no(arq_idx_nome, &no);

    if (no.num_chaves < TAM - 1 && no.ponteiro_pai != -1) {
        balancear_no_apos_remocao(no.endereco, arq_idx_nome);
    }

    TARVBP raiz_final;
    ler_no(arq_idx_nome, endereco_raiz, &raiz_final);
    if (!raiz_final.eh_folha && raiz_final.num_chaves == 0) {
        TARVBP nova_raiz;
        ler_no(arq_idx_nome, raiz_final.filhos[0], &nova_raiz);
        nova_raiz.ponteiro_pai = -1;
        escrever_no(arq_idx_nome, &nova_raiz);
        return nova_raiz.endereco;
    }
    printf("CPF '%s' removido com sucesso.\n", cpf);
    return endereco_raiz;
}

void balancear_no_apos_remocao(long long endereco_no, const char* arq_idx) {
    TARVBP no, pai, irmao;
    int i_filho;

    ler_no(arq_idx, endereco_no, &no);
    if (no.ponteiro_pai == -1) return;

    ler_no(arq_idx, no.ponteiro_pai, &pai);
    for (i_filho = 0; i_filho <= pai.num_chaves; i_filho++) {
        if (pai.filhos[i_filho] == no.endereco) break;
    }

    if (i_filho > 0) {
        ler_no(arq_idx, pai.filhos[i_filho - 1], &irmao);
        if (irmao.num_chaves + no.num_chaves < 2 * TAM - 1) {
            int pos_inicial_filhos_movidos = irmao.num_chaves + (!irmao.eh_folha ? 1 : 0);

            if (!irmao.eh_folha) {
                strcpy(irmao.chaves[irmao.num_chaves++], pai.chaves[i_filho - 1]);
            }
            for (int i = 0; i < no.num_chaves; i++) {
                strcpy(irmao.chaves[irmao.num_chaves], no.chaves[i]);
                if (irmao.eh_folha) irmao.reg[irmao.num_chaves] = no.reg[i];
                irmao.filhos[irmao.num_chaves] = no.filhos[i];
                irmao.num_chaves++;
            }
            if (!irmao.eh_folha) irmao.filhos[irmao.num_chaves] = no.filhos[no.num_chaves];
            if (irmao.eh_folha) irmao.proximo_no = no.proximo_no;

            int num_chaves_pai_antigo = pai.num_chaves;
            remover_chave_de_no(&pai, pai.chaves[i_filho - 1]);
            for(int i = i_filho; i < num_chaves_pai_antigo; i++) {
                pai.filhos[i] = pai.filhos[i+1];
            }
            pai.filhos[num_chaves_pai_antigo] = -1;


            escrever_no(arq_idx, &irmao);
            escrever_no(arq_idx, &pai);

             if (!irmao.eh_folha) {
                for(int i = pos_inicial_filhos_movidos; i <= irmao.num_chaves; i++) {
                     TARVBP neto;
                     ler_no(arq_idx, irmao.filhos[i], &neto);
                     neto.ponteiro_pai = irmao.endereco;
                     escrever_no(arq_idx, &neto);
                }
            }

            if (pai.ponteiro_pai != -1 && pai.num_chaves < TAM - 1) {
                balancear_no_apos_remocao(pai.endereco, arq_idx);
            }
            return;
        }
    }

    if (i_filho < pai.num_chaves) {
        ler_no(arq_idx, pai.filhos[i_filho + 1], &irmao);
        if (no.num_chaves + irmao.num_chaves < 2 * TAM - 1) {
            int pos_inicial_filhos_movidos = no.num_chaves + (!no.eh_folha ? 1 : 0);

            if(!no.eh_folha){
                strcpy(no.chaves[no.num_chaves++], pai.chaves[i_filho]);
            }
            for (int i = 0; i < irmao.num_chaves; i++) {
                strcpy(no.chaves[no.num_chaves], irmao.chaves[i]);
                if(no.eh_folha) no.reg[no.num_chaves] = irmao.reg[i];
                no.filhos[no.num_chaves] = irmao.filhos[i];
                no.num_chaves++;
            }
            if(!no.eh_folha) no.filhos[no.num_chaves] = irmao.filhos[irmao.num_chaves];
            if(no.eh_folha) no.proximo_no = irmao.proximo_no;

            int num_chaves_pai_antigo = pai.num_chaves;
            remover_chave_de_no(&pai, pai.chaves[i_filho]);
             for(int i = i_filho + 1; i < num_chaves_pai_antigo; i++) {
                pai.filhos[i] = pai.filhos[i+1];
            }
            pai.filhos[num_chaves_pai_antigo] = -1;

            escrever_no(arq_idx, &no);
            escrever_no(arq_idx, &pai);

            if (!no.eh_folha) {
                for(int i = pos_inicial_filhos_movidos; i <= no.num_chaves; i++) {
                     TARVBP neto;
                     ler_no(arq_idx, no.filhos[i], &neto);
                     neto.ponteiro_pai = no.endereco;
                     escrever_no(arq_idx, &neto);
                }
            }

            if (pai.ponteiro_pai != -1 && pai.num_chaves < TAM - 1) {
                balancear_no_apos_remocao(pai.endereco, arq_idx);
            }
        }
    }
}

int remover_chave_de_no(TARVBP *no, const char* cpf) {
    int i = 0;
    while(i < no->num_chaves && strcmp(cpf, no->chaves[i]) > 0) i++;
    if (i == no->num_chaves || strcmp(cpf, no->chaves[i]) != 0) return -1;

    for (int j = i; j < no->num_chaves - 1; j++) {
        strcpy(no->chaves[j], no->chaves[j + 1]);
        if (no->eh_folha) no->reg[j] = no->reg[j+1];
    }
    no->num_chaves--;
    return i;
}

void buscar_e_imprimir_dados(long long endereco_raiz, const char* cpf_busca, const char* arq_idx_nome, const char* arq_dados_nome) {
    long long endereco_dado = busca_cpf(arq_idx_nome, cpf_busca, endereco_raiz);

    if (endereco_dado == -1) {
        printf("\n>> CPF '%s' NAO ENCONTRADO na base de dados.\n", cpf_busca);
        return;
    }

    FILE* arq_dados = fopen(arq_dados_nome, "rb");
    if (!arq_dados) {
        perror("Erro critico ao abrir o arquivo de dados (ArqDados.bin)");
        return;
    }

    fseek(arq_dados, endereco_dado, SEEK_SET);

    Dados dados_aluno;
    if(fread(&dados_aluno, sizeof(Dados), 1, arq_dados) != 1) {
        printf("Erro ao ler os dados do aluno do arquivo.\n");
        fclose(arq_dados);
        return;
    }
    fclose(arq_dados);

    printf("\n--- Dados Encontrados para o CPF: %s ---\n", dados_aluno.cpf);
    printf("   Nome: %s\n", dados_aluno.nome);
    printf("   Nota Final: %d\n", dados_aluno.nota_final);
    printf("-----------------------------------------\n");
}


long long constroi_arvore(const char* arq_dados_origem, const char* arq_idx_nome, const char* arq_dados_arvore) {
    printf("Iniciando a construcao da arvore a partir de '%s'...\n", arq_dados_origem);

    remove(arq_idx_nome);
    remove(arq_dados_arvore);

    FILE* f_origem = fopen(arq_dados_origem, "rb");
    if (!f_origem) {
        perror("Erro: Nao foi possivel abrir o arquivo de dados de origem");
        return -1;
    }

    TARVBP_reseta_contador_nos();
    long long raiz = -1;
    Dados dado_lido;
    int contador = 0;

    while (fread(&dado_lido, sizeof(Dados), 1, f_origem) == 1) {
        raiz = TARVBP_insere(raiz, &dado_lido, arq_idx_nome, arq_dados_arvore);
        contador++;
    }

    printf("Construcao finalizada. %d registros totais foram processados.\n", contador);
    fclose(f_origem);
    return raiz;
}