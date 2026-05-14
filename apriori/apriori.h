#ifndef APRIORI_H
#define APRIORI_H

#include "leitor.h"

#define MAX_REGRAS 50000
#define MAX_REGRAS_INCERTAS 50000
#define MAX_ITEMSETS_FREQUENTES 50000
#define MAX_ITEMSETS_INCERTOS 50000

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
    char antecedente[500];
    char consequente[500];
    char itemset[1000];

    /* Dados usados para calcular confianca = suporte_conjunto / suporte_antecedente. */
    int suporte_antecedente;
    int suporte_conjunto;
    int frequencia;
    int total_transacoes;

    float suporte;
    float confianca;
    float relevancia;
    int incerta;
} RegraAssociacao;

typedef struct {
    int itens[MAX_ITENS];
    int tamanho;
    int suporte;
} ItemsetFrequente;

typedef struct {
    int tamanho;
    int inicio;
    int total;
    int total_candidatos;
} NivelApriori;

typedef struct {
    RegraAssociacao regras[MAX_REGRAS];
    int total_regras;

    /*
     * Regras incertas sao regras cujo itemset completo apareceu em apenas
     * uma compra. Elas podem ter confianca alta, mas nao entram como regras
     * validas porque nao ha dados suficientes.
     */
    RegraAssociacao regras_incertas[MAX_REGRAS_INCERTAS];
    int total_regras_incertas;

    /*
     * Guarda explicitamente os niveis L1, L2, ..., Lk encontrados pelo Apriori.
     * Cada NivelApriori aponta para uma faixa dentro de itemsets_frequentes.
     */
    ItemsetFrequente itemsets_frequentes[MAX_ITEMSETS_FREQUENTES];
    int total_itemsets_frequentes;
    ItemsetFrequente itemsets_incertos[MAX_ITEMSETS_INCERTOS];
    int total_itemsets_incertos;
    NivelApriori niveis[MAX_ITENS + 1];
    int total_niveis;
} ResultadoApriori;

void inicializar_resultado(ResultadoApriori *resultado);

int calcular_suporte_itemset(
    BaseCompras *base,
    const int itens[],
    int tamanho
);

int buscar_suporte_itemset(
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho
);

void gerar_metadados_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
);

void aplicar_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
);

#endif
