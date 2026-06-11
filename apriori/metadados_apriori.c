#include <stdio.h>
#include <string.h>

#include "apriori.h"

static float calcular_relevancia(
    BaseCompras *base,
    int suporte_conjunto
) {
    if (base->total_transacoes == 0) {
        return 0.0f;
    }

    return (float) suporte_conjunto / base->total_transacoes;
}

static void formatar_itemset(
    BaseCompras *base,
    const int itens[],
    int tamanho,
    char destino[],
    int tamanho_destino
) {
    int usados = snprintf(destino, tamanho_destino, "{");

    for (int i = 0; i < tamanho && usados < tamanho_destino; i++) {
        usados += snprintf(
            destino + usados,
            tamanho_destino - usados,
            "%s%s",
            i == 0 ? "" : ", ",
            base->itens[itens[i]]
        );
    }

    if (usados < tamanho_destino) {
        snprintf(destino + usados, tamanho_destino - usados, "}");
    } else {
        destino[tamanho_destino - 1] = '\0';
    }
}

static void preencher_regra(
    BaseCompras *base,
    RegraAssociacao *regra,
    const char *itemset,
    const char *antecedente,
    const char *consequente,
    int suporte_antecedente,
    int suporte_consequente,
    int suporte_conjunto,
    float confianca,
    float lift,
    int incerta
) {
    strcpy(regra->itemset, itemset);
    strcpy(regra->antecedente, antecedente);
    strcpy(regra->consequente, consequente);

    regra->suporte_antecedente = suporte_antecedente;
    regra->suporte_consequente = suporte_consequente;
    regra->suporte_conjunto = suporte_conjunto;
    regra->frequencia = suporte_conjunto;
    regra->total_transacoes = base->total_transacoes;
    regra->suporte = calcular_relevancia(base, suporte_conjunto);
    regra->confianca = confianca;
    regra->lift = lift;
    regra->relevancia = regra->suporte;
    regra->incerta = incerta;
}

static void adicionar_regra(
    BaseCompras *base,
    ResultadoApriori *resultado,
    const char *itemset,
    const char *antecedente,
    const char *consequente,
    int suporte_antecedente,
    int suporte_consequente,
    int suporte_conjunto,
    float confianca,
    float lift
) {
    static int aviso_limite_regras_validas_emitido = 0;

    if (resultado->total_regras >= MAX_REGRAS) {
        if (!aviso_limite_regras_validas_emitido) {
            printf("Aviso: limite maximo de regras validas atingido (%d).\n", MAX_REGRAS);
            aviso_limite_regras_validas_emitido = 1;
        }

        return;
    }

    preencher_regra(
        base,
        &resultado->regras[resultado->total_regras],
        itemset,
        antecedente,
        consequente,
        suporte_antecedente,
        suporte_consequente,
        suporte_conjunto,
        confianca,
        lift,
        0
    );

    resultado->total_regras++;
}

static void adicionar_regra_incerta(
    BaseCompras *base,
    ResultadoApriori *resultado,
    const char *itemset,
    const char *antecedente,
    const char *consequente,
    int suporte_antecedente,
    int suporte_consequente,
    int suporte_conjunto,
    float confianca,
    float lift
) {
    static int aviso_limite_regras_incertas_emitido = 0;

    if (resultado->total_regras_incertas >= MAX_REGRAS_INCERTAS) {
        if (!aviso_limite_regras_incertas_emitido) {
            printf("Aviso: limite maximo de regras incertas atingido (%d).\n",
                   MAX_REGRAS_INCERTAS);
            aviso_limite_regras_incertas_emitido = 1;
        }

        return;
    }

    preencher_regra(
        base,
        &resultado->regras_incertas[resultado->total_regras_incertas],
        itemset,
        antecedente,
        consequente,
        suporte_antecedente,
        suporte_consequente,
        suporte_conjunto,
        confianca,
        lift,
        1
    );

    resultado->total_regras_incertas++;
}

static void gerar_metadados_itemset(
    BaseCompras *base,
    ResultadoApriori *resultado,
    ItemsetFrequente *itemset,
    int incerta
) {
    if (itemset->tamanho < 2 || itemset->tamanho >= 63) {
        return;
    }

    unsigned long long total_mascaras = 1ULL << itemset->tamanho;

    for (unsigned long long mascara = 1; mascara < total_mascaras - 1; mascara++) {
        int antecedente[MAX_ITENS];
        int consequente[MAX_ITENS];
        int total_antecedente = 0;
        int total_consequente = 0;

        for (int i = 0; i < itemset->tamanho; i++) {
            if (mascara & (1ULL << i)) {
                antecedente[total_antecedente++] = itemset->itens[i];
            } else {
                consequente[total_consequente++] = itemset->itens[i];
            }
        }

        int suporte_antecedente;

        if (incerta) {
            suporte_antecedente =
                calcular_suporte_itemset(base, antecedente, total_antecedente);
        } else {
            suporte_antecedente =
                buscar_suporte_itemset(resultado, antecedente, total_antecedente);
        }

        if (suporte_antecedente == 0) {
            continue;
        }

        float confianca = (float) itemset->suporte / suporte_antecedente;
        int suporte_consequente =
            calcular_suporte_itemset(base, consequente, total_consequente);
        float suporte_relativo_consequente =
            (float) suporte_consequente / base->total_transacoes;
        float lift = suporte_relativo_consequente > 0
            ? confianca / suporte_relativo_consequente
            : 0.0f;

        if (!incerta && confianca < resultado->confianca_minima) {
            continue;
        }

        char texto_itemset[1000];
        char texto_antecedente[500];
        char texto_consequente[500];

        formatar_itemset(
            base,
            itemset->itens,
            itemset->tamanho,
            texto_itemset,
            sizeof(texto_itemset)
        );
        formatar_itemset(
            base,
            antecedente,
            total_antecedente,
            texto_antecedente,
            sizeof(texto_antecedente)
        );
        formatar_itemset(
            base,
            consequente,
            total_consequente,
            texto_consequente,
            sizeof(texto_consequente)
        );

        if (incerta) {
            adicionar_regra_incerta(
                base,
                resultado,
                texto_itemset,
                texto_antecedente,
                texto_consequente,
                suporte_antecedente,
                suporte_consequente,
                itemset->suporte,
                confianca,
                lift
            );
        } else {
            adicionar_regra(
                base,
                resultado,
                texto_itemset,
                texto_antecedente,
                texto_consequente,
                suporte_antecedente,
                suporte_consequente,
                itemset->suporte,
                confianca,
                lift
            );
        }
    }
}

void gerar_metadados_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    for (int nivel = 2; nivel <= resultado->total_niveis; nivel++) {
        NivelApriori *nivel_atual = &resultado->niveis[nivel];

        for (int i = 0; i < nivel_atual->total; i++) {
            ItemsetFrequente *itemset =
                &resultado->itemsets_frequentes[nivel_atual->inicio + i];

            gerar_metadados_itemset(base, resultado, itemset, 0);
        }
    }

    for (int i = 0; i < resultado->total_itemsets_incertos; i++) {
        gerar_metadados_itemset(base, resultado, &resultado->itemsets_incertos[i], 1);
    }
}
