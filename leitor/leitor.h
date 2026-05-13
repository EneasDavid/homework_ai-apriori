#ifndef LEITOR_H
#define LEITOR_H

/*
 * Limites fixos usados pela matriz em memoria.
 * Cada linha representa uma compra e cada coluna representa um item conhecido.
 */
#define MAX_TRANSACOES 10000
#define MAX_ITENS 100
#define MAX_NOME_ITEM 50
#define MAX_LINHA 500

typedef struct {
    /*
     * itens guarda o nome de cada produto.
     * transacoes[t][i] vale 1 quando a compra t possui o item i.
     */
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
