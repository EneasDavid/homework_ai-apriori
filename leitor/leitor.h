#ifndef LEITOR_H
#define LEITOR_H

#define MAX_TRANSACOES 10000

#define MAX_ITENS 100

#define MAX_NOME_ITEM 50

#define MAX_LINHA 500

typedef struct {
    char itens[MAX_ITENS][MAX_NOME_ITEM];
    int transacoes[MAX_TRANSACOES][MAX_ITENS];

    int total_itens;
    int total_transacoes;
} BaseCompras;

void inicializar_base(BaseCompras *base);

int ler_arquivo_compras(
    const char *nome_arquivo,
    BaseCompras *base
);

#endif