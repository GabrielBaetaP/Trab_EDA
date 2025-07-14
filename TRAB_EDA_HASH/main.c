#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Hash.h"

const char* nomes[100] = {
    "Vitor", "Gabriel", "Caio", "Lucas", "Alex", "Miguel", "Ana", "Maria", "Raquel", "Beatriz", "Laura", "Helena",
    "Arthur", "Matheus", "Pedro", "Augusto", "Gustavo", "Felipe", "Bruno", "Leonardo", "Rafael", "Thiago", "Daniel", "André",
    "Igor", "Eduardo", "Henrique", "Vinicius", "Diego", "Samuel", "Isabela", "Sophia", "Valentina", "Lorena", "Yasmin", "Carolina",
    "Mariana", "Lívia", "Giovanna", "Eloá", "Fernanda", "Larissa", "Lunna", "Clara", "Cecília", "Isadora", "Esther", "Olívia",
    "Elias", "Otávio", "Murilo", "Davi", "Fernando", "Carlos", "Marcela", "Melissa", "Paula", "Mirela", "Tatiane", "Luiza",
    "Juliana", "Camille", "Renata", "Elisa", "Daniele", "Patrícia", "Sabrina", "Aline", "Vanessa", "Priscila", "Érika", "Milena",
    "Simone", "Bianca", "Adriana", "Cristiane", "Grazi", "Natália", "Cael", "Noah", "Hiago", "Luiz", "Vicente", "Cristian",
    "Jonathan", "Érico", "Edson", "Israel", "Rian", "Anderson", "Mateus", "Emanuel", "Victor", "Raul", "Pablo", "Alan",
    "Renan", "Gilberto", "Valter", "Wellington"
};

const char* sobrenomes[100] = {
    "Lemos", "Baeta", "Conceição", "Duarte", "Gavião", "Guimarães", "Ramos", "Silva", "Santos", "Oliveira", "Souza", "Pereira",
    "Lima", "Ferreira", "Almeida", "Costa", "Carvalho", "Rocha", "Dias", "Martins", "Barbosa", "Moura", "Melo", "Cavalcante",
    "Batista", "Campos", "Monteiro", "Nogueira", "Teixeira", "Farias", "Sales", "Cardoso", "Andrade", "Rezende", "Correia", "Pinto",
    "Azevedo", "Tavares", "Peixoto", "Valverde", "Cunha", "Figueiredo", "Vieira", "Antunes", "Castro", "Fonseca", "Pacheco", "Araújo",
    "Seabra", "Assis", "Bittencourt", "Mendes", "Soares", "Queiroz", "Amorim", "Amado", "Vasconcelos", "Siqueira", "Ribeiro", "Braga",
    "Magalhães", "Cavalcanti", "Chaves", "Barros", "Porto", "Xavier", "Gonçalves", "Esteves", "Bernardes", "Macedo", "Neves", "Pedrosa",
    "Prado", "Freitas", "Camargo", "Moraes", "Beltrão", "Brandão", "Corrêa", "Coelho", "Diniz", "Salazar", "Garcia",
    "Moreira", "Borges", "Aguiar", "Barreto", "Lacerda", "Serpa", "Machado", "Miranda", "Leal", "Henriques", "Varela", "Saldanha",
    "Torres", "Telles", "Paranhos", "Valadão"
};

const char* a[100] = {
    "020066", "032436", "036776", "046828", "049797", "052067", "057840", "060720", "065416", "066827",
    "077429", "082580", "097092", "101447", "107177", "129259", "142233", "146670", "166184", "178973",
    "190635", "197783", "213007", "219737", "246510", "249009", "271363", "276942", "283749", "299363",
    "304459", "310311", "331869", "335179", "352701", "358971", "373665", "384855", "385695", "411796",
    "413542", "436399", "449195", "455853", "473919", "475819", "479367", "482484", "495766", "507264",
    "510821", "511857", "526769", "532543", "537458", "567931", "569910", "576879", "577755", "605817",
    "615063", "618445", "630109", "631061", "654569", "658876", "664334", "664414", "668317", "669283",
    "674794", "676215", "678119", "691343", "691605", "692453", "698127", "702671", "719425", "721734",
    "725255", "735597", "754339", "761849", "765241", "781521", "788761", "791059", "808963", "811452",
    "831170", "831928", "868337", "898946", "900214", "946129", "965690", "967114", "977310", "985226"
};

const char* b[100] = {
    "00583", "01350", "02383", "04492", "04544", "05141", "08540", "09214", "10795", "12049",
    "14479", "15351", "16358", "16409", "16773", "19868", "22192", "22412", "23933", "24108",
    "24419", "25094", "27214", "28577", "29790", "30065", "32149", "32856", "33687", "33863",
    "33922", "33927", "35199", "37241", "37676", "38255", "38765", "39104", "40211", "41928",
    "44284", "45692", "47290", "47568", "48169", "49095", "49570", "51615", "52466", "54432",
    "54945", "55680", "55997", "56957", "57030", "59781", "59945", "60659", "63442", "64232",
    "64299", "66072", "67263", "67273", "68275", "68635", "69347", "69469", "69686", "69767",
    "69809", "71952", "72397", "76249", "77611", "77923", "78488", "78527", "79798", "80213",
    "81671", "82075", "84890", "86695", "86754", "87038", "87167", "87979", "90720", "91160",
    "91512", "92429", "92466", "92526", "92949", "93430", "94210", "94371", "97294", "98955"
};

void gerar_arquivo_de_dados(const char* nome_arquivo) {
    FILE* file = fopen(nome_arquivo, "wb");
    if (!file) {
        perror("Não foi possível criar o arquivo");
        return;
    }

    Dados dado;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            snprintf(dado.nome, sizeof(dado.nome), "%s %s", nomes[i], sobrenomes[j]);
            char aux_cpf[13];
            snprintf(aux_cpf, sizeof(aux_cpf), "%s%s", a[i], b[j]);
            strncpy(dado.cpf, aux_cpf, sizeof(dado.cpf));
            dado.cpf[12] = '\0';
            dado.nota_final = rand() % 101;
            fwrite(&dado, sizeof(Dados), 1, file);
        }
    }
    fclose(file);
    printf("Arquivo de dados '%s' gerado com sucesso!\n", nome_arquivo);
}

int main(void) {
    srand(time(NULL));

    const char* dados_arq = "dados.dat";
    const char* hash_arq = "hash.dat";

    gerar_arquivo_de_dados(dados_arq);

    FILE* f_dados = fopen(dados_arq, "rb");
    FILE* f_hash = fopen(hash_arq, "wb+");

    constroi(f_hash, f_dados);

    int opcao;
    char cpf_input[13];
    char nome_input[50];
    int nota_input;

    while (1) {
        printf("\n----------- MENU INTERATIVO -----------\n");
        printf("1. Inserir novo aluno\n");
        printf("2. Buscar aluno por CPF\n");
        printf("3. Remover aluno por CPF\n");
        printf("0. Sair do programa\n");
        printf("---------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1;
        } else while (getchar() != '\n');

        if (opcao == 0) {
            printf("Encerrando o programa...\n");
            break;
        }

        switch (opcao) {
            case 1: {
                printf("\n--- Inserir Aluno ---\n");
                printf("Digite o CPF (11 digitos): ");
                scanf("%12s", cpf_input);
                while (getchar() != '\n');

                printf("Digite o Nome completo: ");
                fgets(nome_input, sizeof(nome_input), stdin);
                nome_input[strcspn(nome_input, "\n")] = 0;

                printf("Digite a Nota Final: ");
                scanf("%d", &nota_input);
                while (getchar() != '\n');

                Dados novo_aluno = novo_dado(nome_input, nota_input, cpf_input);
                int colisoes = insere(f_hash, novo_aluno, NULL);

                if(colisoes != -1) printf("Aluno inserido com sucesso! (%d colisoes)\n", colisoes);
                else printf("Falha ao inserir aluno. Tabela pode estar cheia ou CPF ja existe.\n");
                fflush(f_hash);
                break;
            }
            case 2: {
                printf("\n--- Buscar Aluno ---\n");
                printf("Digite o CPF a ser buscado: ");
                scanf("%12s", cpf_input);
                while (getchar() != '\n');

                int pos = busca(f_hash, cpf_input);

                if (pos != -1) {
                    Dados aluno_encontrado;
                    fseek(f_hash, pos * sizeof(Dados), SEEK_SET);
                    fread(&aluno_encontrado, sizeof(Dados), 1, f_hash);
                    printf("\n--- Aluno Encontrado ---\n");
                    printf("Nome: %s\n", aluno_encontrado.nome);
                    printf("CPF: %s\n", aluno_encontrado.cpf);
                    printf("Nota: %d\n", aluno_encontrado.nota_final);
                    printf("Posicao no arquivo hash: %d\n", pos);
                } else {
                    printf("Aluno com CPF %s nao encontrado.\n", cpf_input);
                }
                break;
            }
            case 3: {
                printf("\n--- Remover Aluno ---\n");
                printf("Digite o CPF a ser removido: ");
                scanf("%12s", cpf_input);
                while (getchar() != '\n');
                remover(f_hash, cpf_input);
                fflush(f_hash);
                break;
            }
            default:
                printf("\nOpcao invalida! Por favor, tente novamente.\n");
                break;
        }
    }
    fclose(f_dados);
    fclose(f_hash);
    return 0;
}