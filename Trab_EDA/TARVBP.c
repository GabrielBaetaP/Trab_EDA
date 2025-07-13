#include "functions.h"

void inicializar_arvbp(TARVBP* arvbp, int eh_folha) {
    memset(arvbp, 0, sizeof(TARVBP));
    arvbp->eh_folha = eh_folha;
    arvbp->num_chaves = 0;
    arvbp->ponteiro_pai = -1L;
    arvbp->filhos_sao_folhas = 0;
    arvbp->proximo_no_folha = -1L;
}


void inicializar(const char* arq_meta, const char* arq_idx, const char* arq_dados) {
    FILE* f_meta = fopen(arq_meta, "wb");
    FILE* f_idx = fopen(arq_idx, "wb");
    FILE* f_dados = fopen(arq_dados, "wb");

    TARVBP arvbp;
    inicializar_arvbp(&arvbp, 1);
    long end_raiz = 0L;

    escrever_no(arq_idx, arq_dados, end_raiz, &arvbp);

    int raiz_eh_folha = 1;
    escrever_metadados(arq_meta, end_raiz, raiz_eh_folha);

    fclose(f_meta);
    fclose(f_idx);
    fclose(f_dados);
}

void ler_metadados(const char* arq_meta, long* end_raiz, int* eh_folha) {
    FILE* f_meta = fopen(arq_meta, "rb");
    fread(end_raiz, sizeof(long), 1, f_meta);
    fread(eh_folha, sizeof(int), 1, f_meta);
    fclose(f_meta);
}

void escrever_metadados(const char* arq_meta, long end_raiz, int eh_folha) {
    FILE* f_meta = fopen(arq_meta, "wb");
    fwrite(&end_raiz, sizeof(long), 1, f_meta);
    fwrite(&eh_folha, sizeof(int), 1, f_meta);
    fclose(f_meta);
}

long get_end_raiz(FILE *arq_meta){
    long end_raiz;
    fseek(arq_meta, 0, SEEK_SET);
    fread(&end_raiz, sizeof(long), 1, arq_meta);
    return end_raiz;
}

void ler_no(FILE *f_idx, FILE *f_dados, long end, int eh_folha, TARVBP *arvbp) {
    FILE *arquivo = eh_folha ? f_dados : f_idx;
    fseek(arquivo, end, SEEK_SET);
    fread(arvbp, sizeof(TARVBP), 1, arquivo);
}

bool buscar_cpf(long long cpf_alvo, Dados *result) {
    FILE* f_meta = fopen("meta.bin", "rb");
    FILE* f_idx = fopen("indice.bin", "rb");
    FILE* f_dados = fopen("dados.bin", "rb");

    long end_atual;
    int eh_folha;

    fread(&end_atual, sizeof(long), 1, f_meta);
    fread(&eh_folha, sizeof(int), 1, f_meta);

    TARVBP arvbp;

    while (end_atual != -1) {
        ler_no(f_idx, f_dados, end_atual, eh_folha, &arvbp);
        if (arvbp.eh_folha) {
            for (int i = 0; i < arvbp.num_chaves; i++) {
                if (arvbp.dados[i].cpf == cpf_alvo) {
                    if (result) *result = arvbp.dados[i];
                    return true;
                }
            }
        }
        else {
            int i = 0;
            while (i < arvbp.num_chaves && cpf_alvo >= arvbp.chaves[i]) i++;
            end_atual = arvbp.filhos[i];
            eh_folha = arvbp.filhos_sao_folhas;
        }
    }

    fclose(f_meta);
    fclose(f_idx);
    fclose(f_dados);
    return false;
}

long end_final(const char *arquivo) {
    char arq[50];
    strcpy(arq, arquivo);
    strcat(arq, ".bin");
    FILE *f = fopen(arq, "ab");
    long end = ftell(f);
    fclose(f);
    return end;
}

void escrever_no(const char* arq_idx, const char* arq_dados, long end, TARVBP* arvbp){
    const char *arquivo = arvbp->eh_folha ? arq_dados : arq_idx;
    FILE *f = fopen(arquivo, "rb+");
    fseek(f, end, SEEK_SET);
    fwrite(arvbp, sizeof(TARVBP), 1, f);
    fclose(f);
}



void divisao_interno(FILE *f_idx, FILE *f_dados, long end_cheio, long long chave_vinda_baixo, long end_dir, long long* chave_acima, long* end_novo) {
    long long chaves_temp[2 * TAM];
    long filhos_temp[2 * TAM + 1];

    TARVBP no_cheio;
    ler_no(f_idx, f_dados, end_cheio, 0, &no_cheio);

    int i = 0;
    while (i < no_cheio.num_chaves && chave_vinda_baixo > no_cheio.chaves[i]) {
        i++;
    }

    memcpy(chaves_temp, no_cheio.chaves, i * sizeof(long long));
    memcpy(filhos_temp, no_cheio.filhos, (i + 1) * sizeof(long));

    chaves_temp[i] = chave_vinda_baixo;
    filhos_temp[i + 1] = end_dir;

    memcpy(&chaves_temp[i + 1], &no_cheio.chaves[i], (no_cheio.num_chaves - i) * sizeof(long long));
    memcpy(&filhos_temp[i + 2], &no_cheio.filhos[i + 1], (no_cheio.num_chaves - i) * sizeof(long));
    *end_novo = end_final("indice.bin");
    TARVBP no_irmao;
    inicializar_arvbp(&no_irmao, 0); // 0 = nó interno
    no_irmao.ponteiro_pai = no_cheio.ponteiro_pai;
    no_irmao.filhos_sao_folhas = no_cheio.filhos_sao_folhas;


    int ponto_meio = TAM - 1;
    *chave_acima = chaves_temp[ponto_meio];

    no_cheio.num_chaves = ponto_meio;
    no_irmao.num_chaves = (2 * TAM - 1) - ponto_meio;

    memcpy(no_cheio.chaves, chaves_temp, sizeof(long long) * ponto_meio);
    memcpy(no_cheio.filhos, filhos_temp, sizeof(long) * (ponto_meio + 1));

    memcpy(no_irmao.chaves, chaves_temp + ponto_meio + 1, sizeof(long long) * no_irmao.num_chaves);
    memcpy(no_irmao.filhos, filhos_temp + ponto_meio + 1, sizeof(long) * (no_irmao.num_chaves + 1));

    escrever_no("indice.bin", "dados.bin", end_cheio, &no_cheio);
    escrever_no("indice.bin", "dados.bin", *end_novo, &no_irmao);
}

void divisao_folha(FILE* f_dados, long end, TARVBP* arvbp, Dados* novo, long long* chave_promovida, long* end_novo){
    Dados dados[TAM * 2];
    int i = 0;
    while (i < arvbp->num_chaves && novo->cpf > arvbp->dados[i].cpf) {
        i++;
    }
    memcpy(dados, arvbp->dados, i * sizeof(Dados));
    dados[i] = *novo;
    memcpy(&dados[i + 1], &arvbp->dados[i], (arvbp->num_chaves - i) * sizeof(Dados));


    *end_novo = end_final("dados.bin");

    TARVBP arvbp_irmao;
    inicializar_arvbp(&arvbp_irmao, 1);
    arvbp->ponteiro_pai = arvbp->ponteiro_pai;
    int ponto_divisao = TAM;
    arvbp->num_chaves = ponto_divisao;
    arvbp_irmao.num_chaves = (2 * TAM) - ponto_divisao;

    memcpy(arvbp->dados, dados, sizeof(Dados) * arvbp->num_chaves);
    memcpy(arvbp_irmao.dados, dados + ponto_divisao, sizeof(Dados) * arvbp_irmao.num_chaves);

    arvbp_irmao.proximo_no_folha = arvbp->proximo_no_folha;
    arvbp->proximo_no_folha = *end_novo;

    *chave_promovida = arvbp_irmao.dados[0].cpf;

    escrever_no("indice.bin", "dados.bin", end, arvbp);
    escrever_no("indice.bin", "dados.bin", *end_novo, &arvbp_irmao);
}

int insere_aux(FILE* f_idx, FILE* f_dados, long end, int eh_folha, long long* chave_promovida, long* end_novo, Dados* novo) {
    TARVBP arvbp;
    ler_no(f_idx, f_dados, end, eh_folha, &arvbp);
    if (arvbp.eh_folha) {
        if (arvbp.num_chaves < (TAM * 2 -1)) {
            int i = arvbp.num_chaves - 1;
            while (i >= 0 && novo->cpf < arvbp.dados[i].cpf) {
                arvbp.dados[i + 1] = arvbp.dados[i];
                i--;
            }
            arvbp.dados[i + 1] = *novo;
            arvbp.num_chaves++;
            escrever_no("indice.bin", "dados.bin", end, &arvbp);
            return 0;
        }
        divisao_folha(f_dados, end, &arvbp, novo, chave_promovida, end_novo);
        return 1;
    }
    else {
        int i = 0;
        while (i < arvbp.num_chaves && novo->cpf >= arvbp.chaves[i]) {
            i++;
        }
        int split = insere_aux(f_idx, f_dados, arvbp.filhos[i], arvbp.filhos_sao_folhas, chave_promovida, end_novo, novo);
        if (split == 1) {
            if (arvbp.num_chaves < TAM * 2 - 1) {
                i = arvbp.num_chaves - 1;
                while (i >= 0 && *chave_promovida < arvbp.chaves[i]) {
                    arvbp.chaves[i + 1] = arvbp.chaves[i];
                    arvbp.filhos[i + 2] = arvbp.filhos[i + 1];
                    i--;
                }
                arvbp.chaves[i + 1] = *chave_promovida;
                arvbp.filhos[i + 2] = *end_novo;
                arvbp.num_chaves++;
                escrever_no("indice.bin", "dados.bin", end, &arvbp);
                return 0;
            }
            divisao_interno(f_idx, f_dados, end, *chave_promovida, *end_novo, chave_promovida, end_novo);
                return 1; // O split continua se propagando para o próximo nível.
            }
        }
    return 0;
}

void inserir_dados(Dados* novo) {
    if (buscar_cpf(novo->cpf, NULL)) {
        printf("AVISO: CPF %lld ja cadastrado!\n", novo->cpf);
        return;
    }

    FILE* f_meta = fopen("meta.bin", "rb+");
    FILE* f_idx = fopen("indice.bin", "rb+");
    FILE* f_dados = fopen("dados.bin", "rb+");

    long end_raiz;
    int eh_folha;
    ler_metadados("meta.bin", &end_raiz, &eh_folha);

    long long chave_promovida;
    long end_novo;
    int split = insere_aux(f_idx, f_dados, end_raiz, eh_folha, &chave_promovida, &end_novo, novo);

    if (split == 1) {
        long novo_end_raiz = end_final("indice.bin");
        TARVBP nova_raiz;
        inicializar_arvbp(&nova_raiz, 0);
        nova_raiz.num_chaves = 1;
        nova_raiz.chaves[0] = chave_promovida;
        nova_raiz.filhos[0] = end_raiz;
        nova_raiz.filhos[1] = end_novo;
        nova_raiz.filhos_sao_folhas = eh_folha;

        TARVBP filho_esq, filho_dir;
        ler_no(f_idx, f_dados, nova_raiz.filhos[0], eh_folha, &filho_esq);
        ler_no(f_idx, f_dados, nova_raiz.filhos[1], eh_folha, &filho_dir);

        filho_esq.ponteiro_pai = novo_end_raiz;
        filho_dir.ponteiro_pai = novo_end_raiz;

        escrever_no("indice.bin", "dados.bin", nova_raiz.filhos[0], &filho_esq);
        escrever_no("indice.bin", "dados.bin", nova_raiz.filhos[1], &filho_dir);

        escrever_no("indice.bin", "dados.bin", novo_end_raiz, &nova_raiz);

        escrever_metadados("meta.bin", novo_end_raiz, 0);
    }

    fclose(f_meta);
    fclose(f_idx);
    fclose(f_dados);
}






