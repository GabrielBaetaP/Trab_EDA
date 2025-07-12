#include "functions.h"
int nos_atual = 0;

TARVBP* TARVBP_cria() {
    TARVBP *no = (TARVBP*)malloc(sizeof(TARVBP));
    no->folha = 1;
    no->num_chaves = 0;
    int i = 0;
    for(; i < TAM * 2 - 1; i++){
    	no->chaves[i] = -1;
        no->filhos[i] = -1;
   		no->reg[i] = -1;
    }
    no->filhos[i] = -1;
    no->proximo_no = -1; // -1 indica que não há próximo
    return no;
}

long get_reg_raiz(FILE *arq_i){
    long reg;
    fseek(arq_i, 0, SEEK_SET);
    fread(&reg, sizeof(long), 1, arq_i);
    return reg;
}

void ler_no(FILE *arq_i, long reg, TARVBP *no){
    fseek(arq_i, reg, SEEK_SET);
    fread(no, sizeof(TARVBP), 1, arq_i);
}

long busca_cpf(FILE* arq_idx, int cpf) {
    if (!arq_idx) return -1;

    long reg_atual = get_reg_raiz(arq_idx);
    TARVBP no_atual;

    while (reg_atual != -1) {
        ler_no(arq_idx, reg_atual, &no_atual);

        if (no_atual.folha) {
            for (int i = 0; i < no_atual.num_chaves && no_atual.chaves[i] <= cpf; i++) {
                if (no_atual.chaves[i] == cpf) {
                    return no_atual.reg[i];
                }
            }
            return -1;
        }
        else {
            int i = 0;
            while (i < no_atual.num_chaves && no_atual.chaves[i] < cpf) i++;
            reg_atual = no_atual.filhos[i];
        }
    }
    return -1;
}

void escrever_no(FILE *arq_i, long reg, TARVBP* no){
    int result = fseek(arq_i, reg, SEEK_SET);
    if(result != 0) return;
    fwrite(no, sizeof(TARVBP), 1, arq_i);
}

TARVBP* divisao(TARVBP* a, TARVBP* p, int t, int i){
    nos_atual++;
    char buffer[10];
    sprintf(buffer, "%d",nos_atual);
    TARVBP* z = TARVBP_cria(buffer, t);
    z->folha = a->folha;
    if(!a->folha){
        z->num_chaves = t-1;
        for(int j = 0;j<t-1;j++){
            z->chaves[j] = a->chaves[j+t];
        }
        for(int k = 0;k<t;k++){
            strcpy(z->filhos[k],a->filhos[t+k]);
            strcpy(a->filhos[t+k],"");
        }
    }
    else{
        z->n_chaves = t;
        for(int j = 0;j<t;j++){
            z->reg[j] = a->reg[j+t-1];
        }
        strcpy(z->prox, a->prox);
        strcpy(a->prox, z->arq);
    }
    for(int j = p->n_chaves;j>i-1;j--){
        strcpy(p->filhos[j+1], p->filhos[j]);
    }
    strcpy(p->filhos[i],z->arq);
    for(int k = p->n_chaves;k>i-1;k--){
        p->chaves[k] = p->chaves[k-1];
    }
    if(!a->folha){          //se for folha da erro, porque s escreve registro quando folha
        p->chaves[i-1] = a->chaves[t-1];
    }
    else{
        p->chaves[i-1] = a->reg[t-1]->id;
    }
    a->n_chaves=t-1;
    p->n_chaves++;
    for(int jk = t-1;jk< (2*t) -1;jk++){
        a->reg[jk] = NULL;
    }
    esc_no(a->arq,a);
    esc_no(z->arq,z);
    esc_no(p->arq,p);
    TARVBP_libera(z, t);
    return p;
}


TARVBP* insere_n_completo(TARVBP* a, TIM* IM, int t){
    int i;
    if(a->folha){ //finalmente insere de fato
        i = a->num_chaves-1;
        while((i>-1)&& (IM->id < a->reg[i]->id )){
            a->reg[i+1] = a->reg[i];
            i--;
        }
        a->reg[i+1] = IM;
        a->num_chaves++;
        esc_no(a->arq, a);
        return a;
    }
    i = a->n_chaves-1;
    while((i>-1) && (IM->id < a->chaves[i])){
        i--;
    }
    TARVBP *f = le_no(a->filhos[i+1], t);
    i++;
    if(f->num_chaves == ((2*t)-1)){ // no cheio
        a = divisao(f,a,t,i+1);
        TARVBP_libera(f, t);
        f = NULL;
        if((a->folha) && (a->reg[i]->id < IM->id)     ||    (!a->folha) && (a->chaves[i] < IM ->id))i++;
    }
    if(!f){    //divisao foi feita
        f = le_no(a->filhos[i],t);
    }
    f = insere_n_completo(f, IM, t);
    esc_no(f->arq, f);
    TARVBP_libera(f,t);
    return a;
}


TARVBP* TARVBP_insere(TARVBP* a, TIM* IM, int t){
    if(TARVBP_tem_id(a, IM->id, t) == 1)return a;
    if(!a){
        printf("????????????????????????\n\n\n");
        exit(1);
    }
    if(a->n_chaves != (2*t)-1){
        a = insere_n_completo(a, IM, t);
        return a;
    }
    nos_atual++;
    char buf[11];
    sprintf(buf,"%d",nos_atual);
    TARVBP* p = TARVBP_cria(buf, t);
    strcpy(p->filhos[0],a->arq);
    p->folha = 0;
    p = divisao(a,p,t,1);
    TARVBP_libera(a,t);
    p = insere_n_completo(p, IM, t);
    return p;
}


