#include <stdio.h>
#include <stdlib.h>

#include "apriori.h"

typedef struct {
    int itens[MAX_ITENS];
    int tamanho;
    int suporte;
} CandidatoApriori;

void inicializar_resultado(ResultadoApriori *resultado) {
    resultado->total_regras = 0;
    resultado->total_regras_incertas = 0;
    resultado->total_itemsets_frequentes = 0;
    resultado->total_itemsets_incertos = 0;
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

static void copiar_itens(
    int destino[],
    const int origem[],
    int tamanho
) {
    for (int i = 0; i < tamanho; i++) {
        destino[i] = origem[i];
    }
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

static int adicionar_candidato(
    CandidatoApriori candidatos[],
    int *total_candidatos,
    int max_candidatos,
    const int itens[],
    int tamanho
) {
    if (candidato_existe(candidatos, *total_candidatos, itens, tamanho)) {
        return 1;
    }

    if (*total_candidatos >= max_candidatos) {
        printf("Aviso: limite maximo de candidatos atingido no nivel %d.\n", tamanho);
        return 0;
    }

    copiar_itens(candidatos[*total_candidatos].itens, itens, tamanho);
    candidatos[*total_candidatos].tamanho = tamanho;
    candidatos[*total_candidatos].suporte = 0;
    (*total_candidatos)++;

    return 1;
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

static int possui_subconjunto_infrequente(
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

static int pode_fazer_join(
    ItemsetFrequente *a,
    ItemsetFrequente *b
) {
    for (int i = 0; i < a->tamanho - 1; i++) {
        if (a->itens[i] != b->itens[i]) {
            return 0;
        }
    }

    return 1;
}

static void fazer_join(
    ItemsetFrequente *a,
    ItemsetFrequente *b,
    int candidato[]
) {
    copiar_itens(candidato, a->itens, a->tamanho);
    candidato[a->tamanho] = b->itens[b->tamanho - 1];
}

static int candidato_passa_no_prune(
    ResultadoApriori *resultado,
    NivelApriori *nivel_anterior,
    const int candidato[],
    int tamanho
) {
    return !possui_subconjunto_infrequente(
        resultado,
        nivel_anterior,
        candidato,
        tamanho
    );
}

static int gerar_candidatos_com_join_e_prune(
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

            if (!pode_fazer_join(a, b)) {
                continue;
            }

            int candidato[MAX_ITENS];
            fazer_join(a, b, candidato);

            if (!candidato_passa_no_prune(resultado, nivel_anterior, candidato, tamanho_novo)) {
                continue;
            }

            if (!adicionar_candidato(
                    candidatos,
                    &total_candidatos,
                    max_candidatos,
                    candidato,
                    tamanho_novo)) {
                return total_candidatos;
            }
        }
    }

    return total_candidatos;
}

static int adicionar_itemset_em_lista(
    ItemsetFrequente itemsets[],
    int *total_itemsets,
    int max_itemsets,
    const char *nome_lista,
    const int itens[],
    int tamanho,
    int suporte
) {
    if (*total_itemsets >= max_itemsets) {
        printf("Aviso: limite maximo de itemsets %s atingido.\n", nome_lista);
        return 0;
    }

    ItemsetFrequente *itemset = &itemsets[*total_itemsets];
    copiar_itens(itemset->itens, itens, tamanho);
    itemset->tamanho = tamanho;
    itemset->suporte = suporte;
    (*total_itemsets)++;

    return 1;
}

static int adicionar_itemset_frequente(
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho,
    int suporte
) {
    return adicionar_itemset_em_lista(
        resultado->itemsets_frequentes,
        &resultado->total_itemsets_frequentes,
        MAX_ITEMSETS_FREQUENTES,
        "frequentes",
        itens,
        tamanho,
        suporte
    );
}

static int adicionar_itemset_incerto(
    ResultadoApriori *resultado,
    const int itens[],
    int tamanho,
    int suporte
) {
    return adicionar_itemset_em_lista(
        resultado->itemsets_incertos,
        &resultado->total_itemsets_incertos,
        MAX_ITEMSETS_INCERTOS,
        "incertos",
        itens,
        tamanho,
        suporte
    );
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

static void gerar_l1(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    NivelApriori *l1 = criar_nivel(resultado, 1, base->total_itens);

    for (int item = 0; item < base->total_itens; item++) {
        int itens[] = {item};
        int suporte = calcular_suporte_itemset(base, itens, 1);

        if (suporte >= MIN_SUP) {
            if (adicionar_itemset_frequente(resultado, itens, 1, suporte)) {
                l1->total++;
            }
        }
    }
}

static void classificar_candidato_por_suporte(
    ResultadoApriori *resultado,
    NivelApriori *nivel_atual,
    CandidatoApriori *candidato,
    int suporte
) {
    if (suporte >= MIN_SUP) {
        if (adicionar_itemset_frequente(
                resultado,
                candidato->itens,
                candidato->tamanho,
                suporte)) {
            nivel_atual->total++;
        }

        return;
    }

    if (suporte == 1) {
        adicionar_itemset_incerto(
            resultado,
            candidato->itens,
            candidato->tamanho,
            suporte
        );
    }
}

static void contar_suporte_e_formar_lk(
    BaseCompras *base,
    ResultadoApriori *resultado,
    NivelApriori *nivel_atual,
    CandidatoApriori candidatos[],
    int total_candidatos
) {
    for (int i = 0; i < total_candidatos; i++) {
        int suporte =
            calcular_suporte_itemset(base, candidatos[i].itens, candidatos[i].tamanho);

        classificar_candidato_por_suporte(
            resultado,
            nivel_atual,
            &candidatos[i],
            suporte
        );
    }
}

static int executar_nivel_k(
    BaseCompras *base,
    ResultadoApriori *resultado,
    int k
) {
    NivelApriori *nivel_anterior = &resultado->niveis[k - 1];
    CandidatoApriori *candidatos =
        malloc(sizeof(CandidatoApriori) * MAX_ITEMSETS_FREQUENTES);

    if (candidatos == NULL) {
        printf("Erro: memoria insuficiente para gerar candidatos.\n");
        return 0;
    }

    int total_candidatos =
        gerar_candidatos_com_join_e_prune(
            resultado,
            nivel_anterior,
            candidatos,
            MAX_ITEMSETS_FREQUENTES
        );

    if (total_candidatos == 0) {
        free(candidatos);
        return 0;
    }

    NivelApriori *nivel_atual = criar_nivel(resultado, k, total_candidatos);

    contar_suporte_e_formar_lk(
        base,
        resultado,
        nivel_atual,
        candidatos,
        total_candidatos
    );

    free(candidatos);

    return nivel_atual->total > 0;
}

static void executar_busca_level_wise(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    gerar_l1(base, resultado);

    int k = 2;

    while (k <= MAX_ITENS && resultado->niveis[k - 1].total > 0) {
        if (!executar_nivel_k(base, resultado, k)) {
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

void aplicar_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    executar_busca_level_wise(base, resultado);
    gerar_metadados_apriori(base, resultado);
}
