#include <stdio.h>
#include <string.h>

#include "apriori.h"

void inicializar_resultado(ResultadoApriori *resultado) {
    resultado->total_regras = 0;
}

int calcular_suporte_1_item(
    BaseCompras *base,
    int item
) {
    /* Suporte e a quantidade de compras em que o itemset aparece. */
    int suporte = 0;

    for (int t = 0; t < base->total_transacoes; t++) {
        if (base->transacoes[t][item] == 1) {
            suporte++;
        }
    }

    return suporte;
}

int calcular_suporte_2_itens(
    BaseCompras *base,
    int item_a,
    int item_b
) {
    /* Para pares, a compra precisa conter os dois itens ao mesmo tempo. */
    int suporte = 0;

    for (int t = 0; t < base->total_transacoes; t++) {
        if (base->transacoes[t][item_a] == 1 &&
            base->transacoes[t][item_b] == 1) {
            suporte++;
        }
    }

    return suporte;
}

int calcular_suporte_3_itens(
    BaseCompras *base,
    int item_a,
    int item_b,
    int item_c
) {
    /* Para trios, a compra precisa conter os tres itens ao mesmo tempo. */
    int suporte = 0;

    for (int t = 0; t < base->total_transacoes; t++) {
        if (base->transacoes[t][item_a] == 1 &&
            base->transacoes[t][item_b] == 1 &&
            base->transacoes[t][item_c] == 1) {
            suporte++;
        }
    }

    return suporte;
}

static void adicionar_regra(
    ResultadoApriori *resultado,
    const char *antecedente,
    const char *consequente,
    int suporte_antecedente,
    int suporte_conjunto,
    float confianca
) {
    if (resultado->total_regras >= MAX_REGRAS) {
        return;
    }

    RegraAssociacao *regra = &resultado->regras[resultado->total_regras];

    strcpy(regra->antecedente, antecedente);
    strcpy(regra->consequente, consequente);

    regra->suporte_antecedente = suporte_antecedente;
    regra->suporte_conjunto = suporte_conjunto;
    regra->confianca = confianca;

    resultado->total_regras++;
}

static void gerar_regras_de_pares(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    /*
     * Regra A -> B:
     * confianca = suporte(A e B) / suporte(A)
     */
    for (int i = 0; i < base->total_itens; i++) {
        for (int j = i + 1; j < base->total_itens; j++) {
            int suporte_a = calcular_suporte_1_item(base, i);
            int suporte_b = calcular_suporte_1_item(base, j);
            int suporte_ab = calcular_suporte_2_itens(base, i, j);

            if (suporte_ab >= MIN_SUP) {
                float confianca_a_b = (float) suporte_ab / suporte_a;
                float confianca_b_a = (float) suporte_ab / suporte_b;

                if (confianca_a_b >= MIN_CONF) {
                    char antecedente[150];
                    char consequente[150];

                    sprintf(antecedente, "{%s}", base->itens[i]);
                    sprintf(consequente, "{%s}", base->itens[j]);

                    adicionar_regra(
                        resultado,
                        antecedente,
                        consequente,
                        suporte_a,
                        suporte_ab,
                        confianca_a_b
                    );
                }

                if (confianca_b_a >= MIN_CONF) {
                    char antecedente[150];
                    char consequente[150];

                    sprintf(antecedente, "{%s}", base->itens[j]);
                    sprintf(consequente, "{%s}", base->itens[i]);

                    adicionar_regra(
                        resultado,
                        antecedente,
                        consequente,
                        suporte_b,
                        suporte_ab,
                        confianca_b_a
                    );
                }
            }
        }
    }
}

static void gerar_regras_de_trios(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    /*
     * Para trios frequentes, as regras geradas usam dois itens no antecedente
     * e um item no consequente, por exemplo {A, B} -> {C}.
     */
    for (int i = 0; i < base->total_itens; i++) {
        for (int j = i + 1; j < base->total_itens; j++) {
            for (int k = j + 1; k < base->total_itens; k++) {
                int suporte_abc = calcular_suporte_3_itens(base, i, j, k);

                if (suporte_abc < MIN_SUP) {
                    continue;
                }

                int suporte_ab = calcular_suporte_2_itens(base, i, j);
                int suporte_ac = calcular_suporte_2_itens(base, i, k);
                int suporte_bc = calcular_suporte_2_itens(base, j, k);

                if (suporte_ab > 0) {
                    float confianca = (float) suporte_abc / suporte_ab;

                    if (confianca >= MIN_CONF) {
                        char antecedente[150];
                        char consequente[150];

                        sprintf(antecedente, "{%s, %s}", base->itens[i], base->itens[j]);
                        sprintf(consequente, "{%s}", base->itens[k]);

                        adicionar_regra(
                            resultado,
                            antecedente,
                            consequente,
                            suporte_ab,
                            suporte_abc,
                            confianca
                        );
                    }
                }

                if (suporte_ac > 0) {
                    float confianca = (float) suporte_abc / suporte_ac;

                    if (confianca >= MIN_CONF) {
                        char antecedente[150];
                        char consequente[150];

                        sprintf(antecedente, "{%s, %s}", base->itens[i], base->itens[k]);
                        sprintf(consequente, "{%s}", base->itens[j]);

                        adicionar_regra(
                            resultado,
                            antecedente,
                            consequente,
                            suporte_ac,
                            suporte_abc,
                            confianca
                        );
                    }
                }

                if (suporte_bc > 0) {
                    float confianca = (float) suporte_abc / suporte_bc;

                    if (confianca >= MIN_CONF) {
                        char antecedente[150];
                        char consequente[150];

                        sprintf(antecedente, "{%s, %s}", base->itens[j], base->itens[k]);
                        sprintf(consequente, "{%s}", base->itens[i]);

                        adicionar_regra(
                            resultado,
                            antecedente,
                            consequente,
                            suporte_bc,
                            suporte_abc,
                            confianca
                        );
                    }
                }
            }
        }
    }
}

void aplicar_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    gerar_regras_de_pares(base, resultado);
    gerar_regras_de_trios(base, resultado);
}
