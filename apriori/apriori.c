#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apriori.h"

typedef struct {
    int itens[MAX_ITENS];
    int tamanho;
    int suporte;
} CandidatoApriori;

static void gerar_regras_incertas_itemset(
    BaseCompras *base,
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho,
    int suporte
);

void inicializar_resultado(ResultadoApriori *resultado) {
    resultado->total_regras = 0;
    resultado->total_regras_incertas = 0;
    resultado->total_itemsets_frequentes = 0;
    resultado->total_niveis = 0;

    for (int i = 0; i <= MAX_ITENS; i++) {
        resultado->niveis[i].tamanho = 0;
        resultado->niveis[i].inicio = 0;
        resultado->niveis[i].total = 0;
        resultado->niveis[i].total_candidatos = 0;
    }
}

int calcular_suporte_itemset(
    BaseCompras *base,
    const int itens[],
    int tamanho
) {
    int suporte = 0;

    for (int t = 0; t < base->total_transacoes; t++) {
        int contem_todos = 1;

        for (int i = 0; i < tamanho; i++) {
            if (base->transacoes[t][itens[i]] == 0) {
                contem_todos = 0;
                break;
            }
        }

        if (contem_todos) {
            suporte++;
        }
    }

    return suporte;
}

int calcular_suporte_1_item(
    BaseCompras *base,
    int item
) {
    int itens[] = {item};
    return calcular_suporte_itemset(base, itens, 1);
}

int calcular_suporte_2_itens(
    BaseCompras *base,
    int item_a,
    int item_b
) {
    int itens[] = {item_a, item_b};
    return calcular_suporte_itemset(base, itens, 2);
}

int calcular_suporte_3_itens(
    BaseCompras *base,
    int item_a,
    int item_b,
    int item_c
) {
    int itens[] = {item_a, item_b, item_c};
    return calcular_suporte_itemset(base, itens, 3);
}

static int itemsets_iguais(
    const int a[],
    const int b[],
    int tamanho
) {
    for (int i = 0; i < tamanho; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }

    return 1;
}

static int candidato_existe(
    CandidatoApriori candidatos[],
    int total_candidatos,
    const int itens[],
    int tamanho
) {
    for (int i = 0; i < total_candidatos; i++) {
        if (itemsets_iguais(candidatos[i].itens, itens, tamanho)) {
            return 1;
        }
    }

    return 0;
}

static int itemset_esta_em_nivel(
    ResultadoApriori *resultado,
    NivelApriori *nivel,
    const int itens[],
    int tamanho
) {
    for (int i = 0; i < nivel->total; i++) {
        ItemsetFrequente *itemset = &resultado->itemsets_frequentes[nivel->inicio + i];

        if (itemset->tamanho == tamanho &&
            itemsets_iguais(itemset->itens, itens, tamanho)) {
            return 1;
        }
    }

    return 0;
}

static int has_infrequent_subset(
    ResultadoApriori *resultado,
    NivelApriori *nivel_anterior,
    const int candidato[],
    int tamanho
) {
    /*
     * Apriori Property: se algum subconjunto de tamanho k-1 nao esta em Lk-1,
     * entao o candidato de tamanho k nao pode ser frequente.
     */
    for (int removido = 0; removido < tamanho; removido++) {
        int subconjunto[MAX_ITENS];
        int posicao = 0;

        for (int i = 0; i < tamanho; i++) {
            if (i != removido) {
                subconjunto[posicao++] = candidato[i];
            }
        }

        if (!itemset_esta_em_nivel(resultado, nivel_anterior, subconjunto, tamanho - 1)) {
            return 1;
        }
    }

    return 0;
}

static int apriori_gen(
    ResultadoApriori *resultado,
    NivelApriori *nivel_anterior,
    CandidatoApriori candidatos[],
    int max_candidatos
) {
    int total_candidatos = 0;
    int tamanho_novo = nivel_anterior->tamanho + 1;

    for (int i = 0; i < nivel_anterior->total; i++) {
        ItemsetFrequente *a = &resultado->itemsets_frequentes[nivel_anterior->inicio + i];

        for (int j = i + 1; j < nivel_anterior->total; j++) {
            ItemsetFrequente *b = &resultado->itemsets_frequentes[nivel_anterior->inicio + j];
            int pode_juntar = 1;

            for (int p = 0; p < nivel_anterior->tamanho - 1; p++) {
                if (a->itens[p] != b->itens[p]) {
                    pode_juntar = 0;
                    break;
                }
            }

            if (!pode_juntar) {
                continue;
            }

            int candidato[MAX_ITENS];

            for (int p = 0; p < nivel_anterior->tamanho; p++) {
                candidato[p] = a->itens[p];
            }

            candidato[tamanho_novo - 1] = b->itens[nivel_anterior->tamanho - 1];

            if (has_infrequent_subset(resultado, nivel_anterior, candidato, tamanho_novo)) {
                continue;
            }

            if (candidato_existe(candidatos, total_candidatos, candidato, tamanho_novo)) {
                continue;
            }

            if (total_candidatos >= max_candidatos) {
                printf("Aviso: limite maximo de candidatos atingido no nivel %d.\n", tamanho_novo);
                return total_candidatos;
            }

            for (int p = 0; p < tamanho_novo; p++) {
                candidatos[total_candidatos].itens[p] = candidato[p];
            }

            candidatos[total_candidatos].tamanho = tamanho_novo;
            candidatos[total_candidatos].suporte = 0;
            total_candidatos++;
        }
    }

    return total_candidatos;
}

static int adicionar_itemset_frequente(
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho,
    int suporte
) {
    if (resultado->total_itemsets_frequentes >= MAX_ITEMSETS_FREQUENTES) {
        printf("Aviso: limite maximo de itemsets frequentes atingido.\n");
        return 0;
    }

    ItemsetFrequente *itemset =
        &resultado->itemsets_frequentes[resultado->total_itemsets_frequentes];

    for (int i = 0; i < tamanho; i++) {
        itemset->itens[i] = itens[i];
    }

    itemset->tamanho = tamanho;
    itemset->suporte = suporte;
    resultado->total_itemsets_frequentes++;

    return 1;
}

static NivelApriori *criar_nivel(
    ResultadoApriori *resultado,
    int tamanho,
    int total_candidatos
) {
    if (tamanho > MAX_ITENS) {
        return NULL;
    }

    NivelApriori *nivel = &resultado->niveis[tamanho];
    nivel->tamanho = tamanho;
    nivel->inicio = resultado->total_itemsets_frequentes;
    nivel->total = 0;
    nivel->total_candidatos = total_candidatos;

    if (tamanho > resultado->total_niveis) {
        resultado->total_niveis = tamanho;
    }

    return nivel;
}

static void executar_busca_level_wise(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    NivelApriori *l1 = criar_nivel(resultado, 1, base->total_itens);

    for (int item = 0; item < base->total_itens; item++) {
        int suporte = calcular_suporte_1_item(base, item);

        if (suporte >= MIN_SUP) {
            int itens[] = {item};

            if (adicionar_itemset_frequente(resultado, itens, 1, suporte)) {
                l1->total++;
            }
        }
    }

    int k = 2;

    while (k <= MAX_ITENS && resultado->niveis[k - 1].total > 0) {
        NivelApriori *nivel_anterior = &resultado->niveis[k - 1];
        CandidatoApriori *candidatos =
            malloc(sizeof(CandidatoApriori) * MAX_ITEMSETS_FREQUENTES);

        if (candidatos == NULL) {
            printf("Erro: memoria insuficiente para gerar candidatos.\n");
            return;
        }

        int total_candidatos =
            apriori_gen(resultado, nivel_anterior, candidatos, MAX_ITEMSETS_FREQUENTES);

        if (total_candidatos == 0) {
            free(candidatos);
            break;
        }

        NivelApriori *nivel_atual = criar_nivel(resultado, k, total_candidatos);

        for (int i = 0; i < total_candidatos; i++) {
            int suporte =
                calcular_suporte_itemset(base, candidatos[i].itens, candidatos[i].tamanho);

            if (suporte >= MIN_SUP) {
                if (adicionar_itemset_frequente(
                        resultado,
                        candidatos[i].itens,
                        candidatos[i].tamanho,
                        suporte)) {
                    nivel_atual->total++;
                }
            } else if (suporte == 1) {
                gerar_regras_incertas_itemset(
                    base,
                    resultado,
                    candidatos[i].itens,
                    candidatos[i].tamanho,
                    suporte
                );
            }
        }

        free(candidatos);

        if (nivel_atual->total == 0) {
            break;
        }

        k++;
    }
}

int buscar_suporte_itemset(
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho
) {
    if (tamanho <= 0 || tamanho > resultado->total_niveis) {
        return 0;
    }

    NivelApriori *nivel = &resultado->niveis[tamanho];

    for (int i = 0; i < nivel->total; i++) {
        ItemsetFrequente *itemset = &resultado->itemsets_frequentes[nivel->inicio + i];

        if (itemsets_iguais(itemset->itens, itens, tamanho)) {
            return itemset->suporte;
        }
    }

    return 0;
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

static void adicionar_regra_incerta(
    ResultadoApriori *resultado,
    const char *antecedente,
    const char *consequente,
    int suporte_antecedente,
    int suporte_conjunto,
    float confianca
) {
    if (resultado->total_regras_incertas >= MAX_REGRAS_INCERTAS) {
        return;
    }

    RegraAssociacao *regra = &resultado->regras_incertas[resultado->total_regras_incertas];

    strcpy(regra->antecedente, antecedente);
    strcpy(regra->consequente, consequente);

    regra->suporte_antecedente = suporte_antecedente;
    regra->suporte_conjunto = suporte_conjunto;
    regra->confianca = confianca;

    resultado->total_regras_incertas++;
}

static void gerar_regras_incertas_itemset(
    BaseCompras *base,
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho,
    int suporte
) {
    if (tamanho < 2 || tamanho >= 63 || suporte != 1) {
        return;
    }

    unsigned long long total_mascaras = 1ULL << tamanho;

    for (unsigned long long mascara = 1; mascara < total_mascaras - 1; mascara++) {
        int antecedente[MAX_ITENS];
        int consequente[MAX_ITENS];
        int total_antecedente = 0;
        int total_consequente = 0;

        for (int i = 0; i < tamanho; i++) {
            if (mascara & (1ULL << i)) {
                antecedente[total_antecedente++] = itens[i];
            } else {
                consequente[total_consequente++] = itens[i];
            }
        }

        int suporte_antecedente =
            calcular_suporte_itemset(base, antecedente, total_antecedente);

        if (suporte_antecedente == 0) {
            continue;
        }

        float confianca = (float) suporte / suporte_antecedente;
        char texto_antecedente[500];
        char texto_consequente[500];

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

        adicionar_regra_incerta(
            resultado,
            texto_antecedente,
            texto_consequente,
            suporte_antecedente,
            suporte,
            confianca
        );
    }
}

static void gerar_regras_itemset(
    BaseCompras *base,
    ResultadoApriori *resultado,
    ItemsetFrequente *itemset
) {
    if (itemset->tamanho >= 63) {
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

        int suporte_antecedente =
            buscar_suporte_itemset(resultado, antecedente, total_antecedente);

        if (suporte_antecedente == 0) {
            continue;
        }

        float confianca = (float) itemset->suporte / suporte_antecedente;

        if (confianca >= MIN_CONF) {
            char texto_antecedente[500];
            char texto_consequente[500];

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

            adicionar_regra(
                resultado,
                texto_antecedente,
                texto_consequente,
                suporte_antecedente,
                itemset->suporte,
                confianca
            );
        }
    }
}

static void gerar_regras_associacao(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    for (int nivel = 2; nivel <= resultado->total_niveis; nivel++) {
        NivelApriori *nivel_atual = &resultado->niveis[nivel];

        for (int i = 0; i < nivel_atual->total; i++) {
            ItemsetFrequente *itemset =
                &resultado->itemsets_frequentes[nivel_atual->inicio + i];

            gerar_regras_itemset(base, resultado, itemset);
        }
    }
}

void aplicar_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    executar_busca_level_wise(base, resultado);
    gerar_regras_associacao(base, resultado);
}
