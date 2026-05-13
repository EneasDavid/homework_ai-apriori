#ifndef APRIORI_H
#define APRIORI_H

#include "leitor.h"

#define MAX_REGRAS 1000

/* Suporte minimo: quantidade minima de compras em que um itemset deve aparecer. */
#define MIN_SUP 2

/* Confianca minima: percentual minimo para aceitar uma regra de associacao. */
#define MIN_CONF 0.70

typedef struct {
    /*
     * Uma regra tem o formato:
     * antecedente -> consequente
     *
     * Exemplo:
     * {leite, pao} -> {manteiga}
     */
    char antecedente[150];
    char consequente[150];

    /* Dados usados para calcular confianca = suporte_conjunto / suporte_antecedente. */
    int suporte_antecedente;
    int suporte_conjunto;

    float confianca;
} RegraAssociacao;

typedef struct {
    RegraAssociacao regras[MAX_REGRAS];
    int total_regras;
} ResultadoApriori;

void inicializar_resultado(ResultadoApriori *resultado);

int calcular_suporte_1_item(
    BaseCompras *base,
    int item
);

int calcular_suporte_2_itens(
    BaseCompras *base,
    int item_a,
    int item_b
);

int calcular_suporte_3_itens(
    BaseCompras *base,
    int item_a,
    int item_b,
    int item_c
);

void aplicar_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
);

#endif
