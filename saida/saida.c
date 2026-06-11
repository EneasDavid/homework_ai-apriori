#include <stdio.h>

#include "saida.h"

#define LIMITE_COMPRAS_EXIBIDAS 20
#define LIMITE_ELEMENTOS_EXIBIDOS 50

static void escrever_separador(FILE *saida) {
    fprintf(saida, "============================================================\n");
}

static void escrever_itemset(
    FILE *saida,
    BaseCompras *base,
    ItemsetFrequente *itemset
) {
    fprintf(saida, "{");

    for (int i = 0; i < itemset->tamanho; i++) {
        fprintf(
            saida,
            "%s%s",
            i == 0 ? "" : ", ",
            base->itens[itemset->itens[i]]
        );
    }

    fprintf(saida, "}");
}

static void escrever_itemset_com_suporte(
    FILE *saida,
    BaseCompras *base,
    ItemsetFrequente *itemset
) {
    escrever_itemset(saida, base, itemset);
    fprintf(
        saida,
        " | frequencia = %d | suporte = %d/%d = %.2f%%\n",
        itemset->suporte,
        itemset->suporte,
        base->total_transacoes,
        100.0f * itemset->suporte / base->total_transacoes
    );
}

static int contar_itemsets_do_tamanho(
    ItemsetFrequente itemsets[],
    int total,
    int tamanho
) {
    int encontrados = 0;

    for (int i = 0; i < total; i++) {
        if (itemsets[i].tamanho == tamanho) {
            encontrados++;
        }
    }

    return encontrados;
}

static int itemset_igual(
    ItemsetFrequente *itemset,
    const int itens[],
    int tamanho
) {
    if (itemset->tamanho != tamanho) {
        return 0;
    }

    for (int i = 0; i < tamanho; i++) {
        if (itemset->itens[i] != itens[i]) {
            return 0;
        }
    }

    return 1;
}

static int subconjunto_esta_no_nivel(
    ResultadoApriori *resultado,
    int nivel,
    const int itens[],
    int tamanho
) {
    NivelApriori *nivel_anterior = &resultado->niveis[nivel];

    for (int i = 0; i < nivel_anterior->total; i++) {
        ItemsetFrequente *itemset =
            &resultado->itemsets_frequentes[nivel_anterior->inicio + i];

        if (itemset_igual(itemset, itens, tamanho)) {
            return 1;
        }
    }

    return 0;
}

static void escrever_motivo_poda(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado,
    ItemsetFrequente *candidato
) {
    int tamanho_subconjunto = candidato->tamanho - 1;

    for (int removido = 0; removido < candidato->tamanho; removido++) {
        int subconjunto[MAX_ITENS];
        int posicao = 0;

        for (int i = 0; i < candidato->tamanho; i++) {
            if (i != removido) {
                subconjunto[posicao++] = candidato->itens[i];
            }
        }

        if (subconjunto_esta_no_nivel(
                resultado,
                tamanho_subconjunto,
                subconjunto,
                tamanho_subconjunto)) {
            continue;
        }

        ItemsetFrequente itemset_ausente = {
            .tamanho = tamanho_subconjunto,
            .suporte = 0
        };

        for (int i = 0; i < tamanho_subconjunto; i++) {
            itemset_ausente.itens[i] = subconjunto[i];
        }

        fprintf(saida, "  Motivo: ");
        escrever_itemset(saida, base, &itemset_ausente);
        fprintf(
            saida,
            " nao esta em L%d; pela Apriori Property, o candidato e descartado.\n",
            tamanho_subconjunto
        );
        return;
    }
}

static void escrever_lista_candidatos(
    FILE *saida,
    const char *titulo,
    const char *explicacao,
    BaseCompras *base,
    ResultadoApriori *resultado,
    ItemsetFrequente itemsets[],
    int total_itemsets,
    int tamanho,
    int suporte_foi_contado
) {
    int total_nivel = contar_itemsets_do_tamanho(itemsets, total_itemsets, tamanho);

    if (total_nivel == 0) {
        return;
    }

    fprintf(saida, "%s (%d):\n", titulo, total_nivel);

    if (explicacao != NULL) {
        fprintf(saida, "%s\n", explicacao);
    }

    int exibidos = 0;

    for (int i = 0; i < total_itemsets && exibidos < LIMITE_ELEMENTOS_EXIBIDOS; i++) {
        if (itemsets[i].tamanho != tamanho) {
            continue;
        }

        fprintf(saida, "- ");

        if (suporte_foi_contado) {
            escrever_itemset_com_suporte(saida, base, &itemsets[i]);
        } else {
            escrever_itemset(saida, base, &itemsets[i]);
            fprintf(saida, " | suporte nao contado por causa da poda\n");
            escrever_motivo_poda(saida, base, resultado, &itemsets[i]);
        }

        exibidos++;
    }

    if (total_nivel > exibidos) {
        fprintf(saida, "- ... %d candidato(s) adicional(is) omitido(s)\n",
                total_nivel - exibidos);
    }

    fprintf(saida, "\n");
}

static void escrever_cabecalho(
    FILE *saida,
    const char *nome_arquivo_entrada,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    escrever_separador(saida);
    fprintf(saida, " RELATORIO DIDATICO DO ALGORITMO APRIORI\n");
    escrever_separador(saida);
    fprintf(saida, "\nArquivo de entrada: %s\n", nome_arquivo_entrada);
    fprintf(saida, "Transacoes na base (|D|): %d\n", base->total_transacoes);
    fprintf(saida, "Itens diferentes (|I|): %d\n\n", base->total_itens);

    fprintf(saida, "PARAMETROS INFORMADOS PELO USUARIO\n");
    fprintf(saida, "---------------------------------\n");
    fprintf(saida, "Suporte minimo = %d ocorrencias (suporte minimo absoluto)\n",
            resultado->suporte_minimo);
    fprintf(
        saida,
        "Nesta base, isso equivale a %d/%d = %.2f%% de suporte relativo.\n",
        resultado->suporte_minimo,
        base->total_transacoes,
        100.0f * resultado->suporte_minimo / base->total_transacoes
    );
    fprintf(saida, "Um candidato entra em Lk quando sua frequencia e >= %d.\n\n",
            resultado->suporte_minimo);

    fprintf(saida, "Confianca minima = %.2f%%\n", resultado->confianca_minima * 100);
    fprintf(saida, "Uma regra e aceita quando confianca(A -> B) >= %.2f%%.\n\n",
            resultado->confianca_minima * 100);

    fprintf(saida, "Atencao: o percentual equivalente do suporte minimo muda com o tamanho\n");
    fprintf(saida, "da base, pois esse parametro foi informado como uma CONTAGEM.\n\n");
}

static void escrever_guia_de_leitura(FILE *saida) {
    escrever_separador(saida);
    fprintf(saida, " GUIA DE LEITURA\n");
    escrever_separador(saida);
    fprintf(saida, "\n");
    fprintf(saida, "- Frequencia: numero exato de transacoes que contem o itemset.\n");
    fprintf(saida, "- Suporte relativo: frequencia(X) / |D|. Mede o quanto X e comum.\n");
    fprintf(saida, "- Confianca(A -> B): frequencia(A uniao B) / frequencia(A).\n");
    fprintf(saida, "  Ela representa P(B|A), isto e, a chance de B quando A ocorreu.\n");
    fprintf(saida, "- Lift(A -> B): confianca(A -> B) / suporte(B).\n");
    fprintf(saida, "  Lift > 1 indica associacao positiva; lift = 1 indica independencia;\n");
    fprintf(saida, "  lift < 1 indica que B ocorre menos com A do que na base em geral.\n");
    fprintf(saida, "- Ck: candidatos com k itens. Lk: candidatos de Ck aprovados pelo suporte.\n");
    fprintf(saida, "- Join: combina itemsets frequentes de Lk-1 para criar candidatos Ck.\n");
    fprintf(saida, "- Poda: elimina um candidato se algum subconjunto dele nao esta em Lk-1.\n\n");
    fprintf(saida, "O codigo usa uma ordem interna consistente, definida pela primeira aparicao\n");
    fprintf(saida, "de cada item no arquivo. O Apriori exige ordem canonica, nao necessariamente\n");
    fprintf(saida, "ordem alfabetica.\n\n");
}

static void escrever_base_analisada(
    FILE *saida,
    BaseCompras *base
) {
    escrever_separador(saida);
    fprintf(saida, " BASE ANALISADA\n");
    escrever_separador(saida);
    fprintf(saida, "\nItens na ordem interna: ");

    for (int i = 0; i < base->total_itens; i++) {
        fprintf(saida, "%s%s", i == 0 ? "" : ", ", base->itens[i]);
    }

    fprintf(saida, "\n\nTransacoes exibidas: ate %d\n\n", LIMITE_COMPRAS_EXIBIDAS);

    int limite = base->total_transacoes;

    if (limite > LIMITE_COMPRAS_EXIBIDAS) {
        limite = LIMITE_COMPRAS_EXIBIDAS;
    }

    for (int t = 0; t < limite; t++) {
        fprintf(saida, "T%d = {", t + 1);
        int primeiro = 1;

        for (int i = 0; i < base->total_itens; i++) {
            if (base->transacoes[t][i] == 1) {
                fprintf(saida, "%s%s", primeiro ? "" : ", ", base->itens[i]);
                primeiro = 0;
            }
        }

        fprintf(saida, "}\n");
    }

    if (base->total_transacoes > limite) {
        fprintf(saida, "... %d transacao(oes) adicional(is) omitida(s)\n",
                base->total_transacoes - limite);
    }

    fprintf(saida, "\n");
}

static void escrever_nivel(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado,
    NivelApriori *nivel
) {
    fprintf(saida, "C%d -> L%d\n", nivel->tamanho, nivel->tamanho);
    fprintf(saida, "---------\n");

    if (nivel->tamanho == 1) {
        fprintf(saida, "Candidatos iniciais, um por item: %d\n",
                nivel->total_candidatos_gerados);
    } else {
        fprintf(saida, "Candidatos gerados pelo join: %d\n",
                nivel->total_candidatos_gerados);
    }

    fprintf(saida, "Candidatos podados antes da contagem: %d\n",
            nivel->total_candidatos_podados);
    fprintf(saida, "Candidatos com suporte contado: %d\n", nivel->total_candidatos);
    fprintf(saida, "Itemsets aprovados em L%d: %d\n", nivel->tamanho, nivel->total);
    fprintf(saida, "Candidatos reprovados por suporte: %d\n\n",
            nivel->total_descartados_suporte);

    if (nivel->total > 0) {
        fprintf(saida, "L%d - itemsets frequentes:\n", nivel->tamanho);

        for (int i = 0; i < nivel->total && i < LIMITE_ELEMENTOS_EXIBIDOS; i++) {
            ItemsetFrequente *itemset =
                &resultado->itemsets_frequentes[nivel->inicio + i];
            fprintf(saida, "- ");
            escrever_itemset_com_suporte(saida, base, itemset);
        }

        if (nivel->total > LIMITE_ELEMENTOS_EXIBIDOS) {
            fprintf(saida, "- ... %d itemset(s) adicional(is) omitido(s)\n",
                    nivel->total - LIMITE_ELEMENTOS_EXIBIDOS);
        }

        fprintf(saida, "\n");
    }

    escrever_lista_candidatos(
        saida,
        "Reprovados por suporte",
        "Eles foram contados, mas nao atingiram o suporte minimo informado.",
        base,
        resultado,
        resultado->candidatos_infrequentes,
        resultado->total_candidatos_infrequentes,
        nivel->tamanho,
        1
    );

    escrever_lista_candidatos(
        saida,
        "Podados pela Apriori Property",
        "Algum subconjunto de tamanho k-1 nao era frequente.",
        base,
        resultado,
        resultado->candidatos_podados,
        resultado->total_candidatos_podados,
        nivel->tamanho,
        0
    );
}

static void escrever_execucao(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    escrever_separador(saida);
    fprintf(saida, " EXECUCAO PASSO A PASSO: Ck -> Lk\n");
    escrever_separador(saida);
    fprintf(saida, "\n");

    for (int k = 1; k <= resultado->total_niveis; k++) {
        escrever_nivel(saida, base, resultado, &resultado->niveis[k]);
    }

    NivelApriori *ultimo = &resultado->niveis[resultado->total_niveis];

    if (ultimo->total == 0) {
        fprintf(saida, "Interrupcao: L%d ficou vazio; nao ha base para gerar C%d.\n\n",
                ultimo->tamanho,
                ultimo->tamanho + 1);
    } else {
        fprintf(saida, "Interrupcao: nao foi possivel formar candidatos validos para C%d.\n\n",
                ultimo->tamanho + 1);
    }
}

static const char *interpretar_lift(float lift) {
    if (lift > 1.01f) {
        return "associacao positiva: B aparece mais com A que na base em geral";
    }

    if (lift < 0.99f) {
        return "associacao negativa: B aparece menos com A que na base em geral";
    }

    return "associacao aproximadamente neutra";
}

static void escrever_regra(
    FILE *saida,
    RegraAssociacao *regra,
    int numero,
    ResultadoApriori *resultado
) {
    fprintf(saida, "REGRA %d: %s -> %s\n", numero, regra->antecedente, regra->consequente);
    fprintf(saida, "- Itemset completo: %s\n", regra->itemset);
    fprintf(
        saida,
        "- Suporte: %d/%d = %.2f%% | passa porque %d >= suporte minimo (%d)\n",
        regra->suporte_conjunto,
        regra->total_transacoes,
        regra->suporte * 100,
        regra->suporte_conjunto,
        resultado->suporte_minimo
    );
    fprintf(
        saida,
        "- Confianca: %d/%d = %.2f%% | passa porque %.2f%% >= confianca minima (%.2f%%)\n",
        regra->suporte_conjunto,
        regra->suporte_antecedente,
        regra->confianca * 100,
        regra->confianca * 100,
        resultado->confianca_minima * 100
    );
    fprintf(
        saida,
        "- Leitura: entre %d transacao(oes) com %s, %d tambem continha(m) %s.\n",
        regra->suporte_antecedente,
        regra->antecedente,
        regra->suporte_conjunto,
        regra->consequente
    );
    fprintf(
        saida,
        "- Lift: %.3f = confianca %.2f%% / suporte do consequente %.2f%%\n",
        regra->lift,
        regra->confianca * 100,
        100.0f * regra->suporte_consequente / regra->total_transacoes
    );
    fprintf(saida, "- Interpretacao do lift: %s.\n\n", interpretar_lift(regra->lift));
}

static void escrever_regras(
    FILE *saida,
    ResultadoApriori *resultado
) {
    escrever_separador(saida);
    fprintf(saida, " REGRAS ACEITAS PELA CONFIANCA\n");
    escrever_separador(saida);
    fprintf(saida, "\n");
    fprintf(saida, "Uma regra so chega aqui se o itemset passou pelo suporte minimo e a regra\n");
    fprintf(saida, "passou pela confianca minima. O lift compara a regra com a popularidade\n");
    fprintf(saida, "natural do consequente.\n\n");

    if (resultado->total_regras == 0) {
        fprintf(saida, "Nenhuma regra atingiu a confianca minima.\n\n");
        return;
    }

    int limite = resultado->total_regras;

    if (limite > LIMITE_ELEMENTOS_EXIBIDOS) {
        limite = LIMITE_ELEMENTOS_EXIBIDOS;
    }

    for (int i = 0; i < limite; i++) {
        escrever_regra(saida, &resultado->regras[i], i + 1, resultado);
    }

    if (resultado->total_regras > limite) {
        fprintf(saida, "... %d regra(s) adicional(is) omitida(s)\n\n",
                resultado->total_regras - limite);
    }
}

static void escrever_coincidencias_raras(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    escrever_separador(saida);
    fprintf(saida, " COINCIDENCIAS RARAS\n");
    escrever_separador(saida);
    fprintf(saida, "\n");
    fprintf(saida, "Itemsets com uma unica ocorrencia podem produzir confianca alta por acaso,\n");
    fprintf(saida, "mas nao sao frequentes porque o suporte minimo e %d. Eles nao geram regras validas.\n\n",
            resultado->suporte_minimo);

    if (resultado->total_itemsets_incertos == 0) {
        fprintf(saida, "Nenhum itemset avaliado apareceu exatamente uma vez.\n\n");
        return;
    }

    int limite = resultado->total_itemsets_incertos;

    if (limite > LIMITE_ELEMENTOS_EXIBIDOS) {
        limite = LIMITE_ELEMENTOS_EXIBIDOS;
    }

    for (int i = 0; i < limite; i++) {
        fprintf(saida, "- ");
        escrever_itemset_com_suporte(saida, base, &resultado->itemsets_incertos[i]);
    }

    if (resultado->total_itemsets_incertos > limite) {
        fprintf(saida, "- ... %d itemset(s) adicional(is) omitido(s)\n",
                resultado->total_itemsets_incertos - limite);
    }

    fprintf(saida, "\n");
}

static void escrever_destaques_e_perguntas(
    FILE *saida,
    ResultadoApriori *resultado
) {
    RegraAssociacao *maior_lift = NULL;
    RegraAssociacao *menor_lift = NULL;

    for (int i = 0; i < resultado->total_regras; i++) {
        RegraAssociacao *regra = &resultado->regras[i];

        if (maior_lift == NULL || regra->lift > maior_lift->lift) {
            maior_lift = regra;
        }

        if (menor_lift == NULL || regra->lift < menor_lift->lift) {
            menor_lift = regra;
        }
    }

    escrever_separador(saida);
    fprintf(saida, " DESTAQUES E PERGUNTAS PARA A TURMA\n");
    escrever_separador(saida);
    fprintf(saida, "\n");
    fprintf(saida, "Itemsets frequentes encontrados: %d\n",
            resultado->total_itemsets_frequentes);
    fprintf(saida, "Candidatos infrequentes registrados: %d\n",
            resultado->total_candidatos_infrequentes);
    fprintf(saida, "Candidatos eliminados pela poda: %d\n",
            resultado->total_candidatos_podados);
    fprintf(saida, "Regras aceitas: %d\n\n", resultado->total_regras);

    if (maior_lift != NULL) {
        fprintf(saida, "Maior lift: %s -> %s | confianca %.2f%% | lift %.3f\n",
                maior_lift->antecedente,
                maior_lift->consequente,
                maior_lift->confianca * 100,
                maior_lift->lift);
        fprintf(saida, "Menor lift entre regras aceitas: %s -> %s | confianca %.2f%% | lift %.3f\n\n",
                menor_lift->antecedente,
                menor_lift->consequente,
                menor_lift->confianca * 100,
                menor_lift->lift);
    }

    fprintf(saida, "Perguntas sugeridas:\n");
    fprintf(saida, "1. Qual e a diferenca entre frequencia e suporte relativo?\n");
    fprintf(saida, "2. Por que A -> B e B -> A podem ter confiancas diferentes?\n");
    fprintf(saida, "3. Qual candidato foi eliminado sem ter o suporte contado, e por que?\n");
    fprintf(saida, "4. Uma regra pode passar pela confianca minima e ter lift <= 1? O que isso indica?\n");
}

int gerar_arquivo_saida(
    const char *nome_arquivo_saida,
    const char *nome_arquivo_entrada,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    FILE *saida = fopen(nome_arquivo_saida, "w");

    if (saida == NULL) {
        printf("Erro: nao foi possivel criar o arquivo de saida.\n");
        return 0;
    }

    escrever_cabecalho(saida, nome_arquivo_entrada, base, resultado);
    escrever_guia_de_leitura(saida);
    escrever_base_analisada(saida, base);
    escrever_execucao(saida, base, resultado);
    escrever_regras(saida, resultado);
    escrever_coincidencias_raras(saida, base, resultado);
    escrever_destaques_e_perguntas(saida, resultado);

    fclose(saida);

    return 1;
}
