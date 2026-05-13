#ifndef APRIORI_H
#define APRIORI_H

#include "leitor.h"

#define MAX_REGRAS 1000
#define MIN_SUP 2
#define MIN_CONF 0.70

typedef struct {
    char antecedente[150];
    char consequente[150];

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