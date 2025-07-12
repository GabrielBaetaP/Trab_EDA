#include "functions.c"

int main(void) {
    gerador(nomes, sobrenomes, "nome");
    gerador(a, b, "cpf");
    gerador_nota("nota");
    leitura_escrita("nome", "nome");
    leitura_escrita("cpf", "cpf");
    leitura_escrita_nota("nota", "nota");
}