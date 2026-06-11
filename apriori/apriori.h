#ifndef APRIORI_H
#define APRIORI_H

#include "leitor.h"

#define MAX_REGRAS 50000
#define MAX_REGRAS_INCERTAS 50000
#define MAX_ITEMSETS_FREQUENTES 50000
#define MAX_ITEMSETS_INCERTOS 50000
#define MAX_CANDIDATOS_REGISTRADOS 50000

/* Valores padrao usados quando nenhuma configuracao diferente e informada. */
#define SUPORTE_MINIMO_PADRAO 2
#define CONFIANCA_MINIMA_PADRAO 0.70

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

    /* Dados usados para explicar os calculos de suporte, confianca e lift. */
    int suporte_antecedente;
    int suporte_consequente;
    int suporte_conjunto;
    int frequencia;
    int total_transacoes;

    float suporte;
    float confianca;
    float lift;
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
    int total_candidatos_gerados;
    int total_candidatos_podados;
    int total_candidatos;
    int total_descartados_suporte;
} NivelApriori;

typedef struct {
    /* Parametros escolhidos para esta execucao do algoritmo. */
    int suporte_minimo;
    float confianca_minima;

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

    /*
     * Estes registros permitem que a saida mostre o que foi descartado
     * por suporte e o que foi podado antes mesmo da contagem.
     */
    ItemsetFrequente candidatos_infrequentes[MAX_CANDIDATOS_REGISTRADOS];
    int total_candidatos_infrequentes;
    ItemsetFrequente candidatos_podados[MAX_CANDIDATOS_REGISTRADOS];
    int total_candidatos_podados;

    NivelApriori niveis[MAX_ITENS + 1];
    int total_niveis;
} ResultadoApriori;

void inicializar_resultado(ResultadoApriori *resultado);

void configurar_parametros_apriori(
    ResultadoApriori *resultado,
    int suporte_minimo,
    float confianca_minima
);

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

int aplicar_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
);

#endif
