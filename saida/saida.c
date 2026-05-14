#include <stdio.h>

#include "saida.h"

#define LIMITE_COMPRAS_EXIBIDAS 20

/*
 * Este modulo transforma os dados processados em um relatorio didatico.
 * Os calculos ficam no modulo apriori; aqui apenas escrevemos os resultados.
 */

static void escrever_cabecalho_relatorio(
    FILE *saida,
    const char *nome_arquivo_entrada,
    BaseCompras *base
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " RELATORIO APRIORI - REGRAS DE ASSOCIACAO\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Arquivo de entrada: %s\n", nome_arquivo_entrada);
    fprintf(saida, "Total de compras analisadas: %d\n", base->total_transacoes);
    fprintf(saida, "Total de itens diferentes encontrados: %d\n", base->total_itens);
    fprintf(saida, "Suporte minimo configurado: %d\n", MIN_SUP);
    fprintf(saida, "Confianca minima configurada: %.2f%%\n\n", MIN_CONF * 100);

    fprintf(saida, "Observacao sobre o tratamento dos dados:\n");
    fprintf(saida, "- Todos os itens foram convertidos para letras minusculas.\n");
    fprintf(saida, "- Assim, 'Leite', 'leite' e 'LEITE' sao tratados como o mesmo item.\n");
    fprintf(saida, "- Cada linha do arquivo representa uma compra.\n");
    fprintf(saida, "- Cada item dentro da compra e separado por virgula.\n\n");
}

static void escrever_conceitos(FILE *saida) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " CONCEITOS IMPORTANTES\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "1. Item\n");
    fprintf(saida, "Um item e um produto individual presente em uma compra.\n");
    fprintf(saida, "Exemplo: leite, arroz, feijao, cafe.\n\n");

    fprintf(saida, "2. Transacao\n");
    fprintf(saida, "Uma transacao representa uma compra completa.\n");
    fprintf(saida, "Exemplo: {leite, pao, manteiga}.\n\n");

    fprintf(saida, "3. Itemset\n");
    fprintf(saida, "Um itemset e um conjunto de um ou mais itens que aparecem juntos.\n");
    fprintf(saida, "Exemplo de itemset de tamanho 1: {leite}\n");
    fprintf(saida, "Exemplo de itemset de tamanho 2: {leite, pao}\n");
    fprintf(saida, "Exemplo de itemset de tamanho 3: {leite, pao, manteiga}\n\n");

    fprintf(saida, "4. Suporte\n");
    fprintf(saida, "O suporte indica quantas vezes um itemset aparece na base de compras.\n");
    fprintf(saida, "Exemplo: se {leite, pao} aparece em 56 compras, entao seu suporte e 56.\n\n");

    fprintf(saida, "5. Regra de associacao\n");
    fprintf(saida, "Uma regra de associacao possui o formato:\n");
    fprintf(saida, "{antecedente} -> {consequente}\n\n");

    fprintf(saida, "Exemplo:\n");
    fprintf(saida, "{leite, pao} -> {manteiga}\n\n");

    fprintf(saida, "Interpretacao:\n");
    fprintf(saida, "Quando uma compra possui leite e pao, existe uma tendencia de tambem possuir manteiga.\n\n");

    fprintf(saida, "6. Antecedente\n");
    fprintf(saida, "E a parte esquerda da regra.\n");
    fprintf(saida, "Na regra {leite, pao} -> {manteiga}, o antecedente e {leite, pao}.\n\n");

    fprintf(saida, "7. Consequente\n");
    fprintf(saida, "E a parte direita da regra.\n");
    fprintf(saida, "Na regra {leite, pao} -> {manteiga}, o consequente e {manteiga}.\n\n");

    fprintf(saida, "8. Confianca\n");
    fprintf(saida, "A confianca mede a forca da regra.\n");
    fprintf(saida, "Ela mostra, entre as compras que possuem o antecedente, quantas tambem possuem o consequente.\n\n");

    fprintf(saida, "Formula:\n");
    fprintf(saida, "Confianca = Suporte do conjunto completo / Suporte do antecedente\n\n");

    fprintf(saida, "Exemplo:\n");
    fprintf(saida, "Suporte do antecedente = 79\n");
    fprintf(saida, "Suporte do conjunto completo = 56\n");
    fprintf(saida, "Confianca = 56 / 79\n");
    fprintf(saida, "Confianca = 0.7089\n");
    fprintf(saida, "Confianca = 70.89%%\n\n");

    fprintf(saida, "Interpretacao do exemplo:\n");
    fprintf(saida, "Das 79 compras que possuem o antecedente, 56 tambem possuem o consequente.\n");
    fprintf(saida, "Por isso, a regra tem 70.89%% de confianca.\n\n");

    fprintf(saida, "9. Busca level-wise do Apriori\n");
    fprintf(saida, "O Apriori encontra primeiro L1, depois usa L1 para gerar C2 e L2,\n");
    fprintf(saida, "usa L2 para gerar C3 e L3, e continua ate nao existir mais Lk frequente.\n\n");

    fprintf(saida, "10. Apriori Property\n");
    fprintf(saida, "Todo subconjunto nao vazio de um itemset frequente tambem deve ser frequente.\n");
    fprintf(saida, "Por isso, se um candidato possui algum subconjunto infrequente, ele e podado\n");
    fprintf(saida, "antes da contagem de suporte.\n\n");

    fprintf(saida, "11. Join e prune\n");
    fprintf(saida, "Na etapa de join, itemsets de Lk-1 sao combinados para formar candidatos Ck.\n");
    fprintf(saida, "Isso significa que o algoritmo usa itemsets frequentes de tamanho k-1\n");
    fprintf(saida, "para tentar montar itemsets candidatos de tamanho k.\n\n");

    fprintf(saida, "Exemplo de join:\n");
    fprintf(saida, "Se L2 possui {leite, pao} e {leite, manteiga}, esses dois itemsets\n");
    fprintf(saida, "podem ser juntados porque compartilham o prefixo {leite}.\n");
    fprintf(saida, "O candidato gerado em C3 seria {leite, pao, manteiga}.\n\n");

    fprintf(saida, "Depois vem o prune, que significa poda.\n");
    fprintf(saida, "A ideia e remover candidatos impossiveis antes de gastar tempo contando\n");
    fprintf(saida, "o suporte deles em todas as compras.\n");
    fprintf(saida, "Para isso, o algoritmo olha para todos os subconjuntos de tamanho k-1\n");
    fprintf(saida, "do candidato que acabou de ser gerado.\n");
    fprintf(saida, "Se o candidato tem tamanho 3, ele verifica todos os pares dentro dele.\n");
    fprintf(saida, "Se o candidato tem tamanho 4, ele verifica todos os trios dentro dele,\n");
    fprintf(saida, "e assim por diante.\n\n");

    fprintf(saida, "O motivo e a Apriori Property:\n");
    fprintf(saida, "se um itemset e frequente, todos os seus subconjuntos tambem precisam ser frequentes.\n");
    fprintf(saida, "Logo, se um unico subconjunto do candidato nao esta em Lk-1,\n");
    fprintf(saida, "esse candidato nao tem chance de ser frequente.\n");
    fprintf(saida, "Nesse caso, ele e removido imediatamente e seu suporte nem e contado.\n\n");

    fprintf(saida, "Exemplo de prune:\n");
    fprintf(saida, "Para aceitar o candidato {leite, pao, manteiga}, os subconjuntos\n");
    fprintf(saida, "{leite, pao}, {leite, manteiga} e {pao, manteiga} precisam ser frequentes.\n");
    fprintf(saida, "Se {pao, manteiga} nao estiver em L2, o candidato {leite, pao, manteiga}\n");
    fprintf(saida, "e descartado antes mesmo de contar seu suporte nas compras.\n");
    fprintf(saida, "Isso acontece porque, se {pao, manteiga} ja nao aparece vezes suficientes,\n");
    fprintf(saida, "{leite, pao, manteiga} tambem nao pode aparecer vezes suficientes.\n\n");
}

static void escrever_regras_maior_nivel_confianca(
    FILE *saida,
    ResultadoApriori *resultado
) {
    int regras_muito_fortes = 0;
    int regras_fortes = 0;
    int regras_moderadas = 0;

    for (int i = 0; i < resultado->total_regras; i++) {
        float confianca = resultado->regras[i].confianca;

        if (confianca >= 0.90) {
            regras_muito_fortes++;
        } else if (confianca >= 0.70) {
            regras_fortes++;
        } else {
            regras_moderadas++;
        }
    }

    fprintf(saida, "========================================\n");
    fprintf(saida, " REGRAS DESTACADAS PELO MAIOR NIVEL DE CONFIANCA\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Esta secao apresenta somente as regras do maior nivel de confianca encontrado.\n");
    fprintf(saida, "Os calculos detalhados de suporte e confianca aparecem mais abaixo, na secao ");
    fprintf(saida, "'REGRAS DE ASSOCIACAO GERADAS'.\n\n");

    fprintf(saida, "Quantidade encontrada em cada nivel:\n");
    fprintf(saida, "- Regras muito fortes, com confianca >= 90%%: %d\n", regras_muito_fortes);
    fprintf(saida, "- Regras fortes, com confianca entre 70%% e 89.99%%: %d\n", regras_fortes);
    fprintf(saida, "- Regras moderadas ou fracas, abaixo de 70%%: %d\n", regras_moderadas);
    fprintf(saida, "- Regras incertas, com suporte do conjunto igual a 1: %d\n\n",
            resultado->total_regras_incertas);

    if (resultado->total_regras_incertas > 0) {
        fprintf(saida, "Observacao: regras incertas nao entram no nivel exibido abaixo,\n");
        fprintf(saida, "mesmo quando a confianca delas seria alta, porque possuem dados insuficientes.\n\n");
    }

    int nivel_escolhido;

    if (regras_muito_fortes > 0) {
        nivel_escolhido = 1;
        fprintf(saida, "Nivel exibido nesta secao: REGRAS MUITO FORTES\n\n");
    } else if (regras_fortes > 0) {
        nivel_escolhido = 2;
        fprintf(saida, "Nivel exibido nesta secao: REGRAS FORTES\n\n");
    } else {
        nivel_escolhido = 3;
        fprintf(saida, "Nivel exibido nesta secao: REGRAS MODERADAS OU FRACAS\n\n");
    }

    int contador = 1;

    for (int i = 0; i < resultado->total_regras; i++) {
        RegraAssociacao regra = resultado->regras[i];
        float confianca = regra.confianca;

        int deve_exibir = 0;

        if (nivel_escolhido == 1 && confianca >= 0.90) {
            deve_exibir = 1;
        } else if (nivel_escolhido == 2 && confianca >= 0.70 && confianca < 0.90) {
            deve_exibir = 1;
        } else if (nivel_escolhido == 3 && confianca < 0.70) {
            deve_exibir = 1;
        }

        if (deve_exibir) {
            fprintf(saida, "Regra destacada %d: %s -> %s | Confianca: %.2f%%\n",
                    contador,
                    regra.antecedente,
                    regra.consequente,
                    regra.confianca * 100);

            contador++;
        }
    }

    fprintf(saida, "\nTotal de regras destacadas nesta secao: %d\n\n", contador - 1);
}
static void escrever_amostra_compras(
    FILE *saida,
    BaseCompras *base
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " AMOSTRA DAS COMPRAS LIDAS\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Para evitar um relatorio muito extenso, serao exibidas apenas as primeiras %d compras.\n", LIMITE_COMPRAS_EXIBIDAS);
    fprintf(saida, "O total real de compras analisadas foi: %d.\n\n", base->total_transacoes);

    int limite = base->total_transacoes;

    if (limite > LIMITE_COMPRAS_EXIBIDAS) {
        limite = LIMITE_COMPRAS_EXIBIDAS;
    }

    for (int t = 0; t < limite; t++) {
        fprintf(saida, "Compra %d: ", t + 1);

        for (int i = 0; i < base->total_itens; i++) {
            if (base->transacoes[t][i] == 1) {
                fprintf(saida, "%s ", base->itens[i]);
            }
        }

        fprintf(saida, "\n");
    }

    if (base->total_transacoes > LIMITE_COMPRAS_EXIBIDAS) {
        fprintf(saida, "\n... outras %d compras tambem foram analisadas.\n",
                base->total_transacoes - LIMITE_COMPRAS_EXIBIDAS);
    }

    fprintf(saida, "\n");
}

static void escrever_itens_encontrados(
    FILE *saida,
    BaseCompras *base
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " ITENS DIFERENTES ENCONTRADOS\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Foram encontrados %d itens diferentes na base de compras:\n\n", base->total_itens);

    for (int i = 0; i < base->total_itens; i++) {
        fprintf(saida, "- %s\n", base->itens[i]);
    }

    fprintf(saida, "\n");
}

static void escrever_itemset_formatado(
    FILE *saida,
    BaseCompras *base,
    ItemsetFrequente *itemset
) {
    fprintf(saida, "{");

    for (int i = 0; i < itemset->tamanho; i++) {
        fprintf(saida, "%s%s", i == 0 ? "" : ", ", base->itens[itemset->itens[i]]);
    }

    fprintf(saida, "} -> suporte = %d\n", itemset->suporte);
}

static void escrever_itemsets_frequentes(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " BUSCA LEVEL-WISE APRIORI\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "O algoritmo gerou candidatos Ck a partir de Lk-1 usando join,\n");
    fprintf(saida, "aplicou prune com a Apriori Property e manteve em Lk apenas candidatos\n");
    fprintf(saida, "com suporte maior ou igual a %d.\n\n", MIN_SUP);

    for (int nivel = 1; nivel <= resultado->total_niveis; nivel++) {
        NivelApriori *nivel_atual = &resultado->niveis[nivel];

        fprintf(saida, "========================================\n");
        fprintf(saida, " ITEMSETS FREQUENTES L%d\n", nivel);
        fprintf(saida, "========================================\n\n");

        fprintf(saida, "Tamanho dos itemsets: %d\n", nivel);
        fprintf(saida, "Candidatos C%d gerados/avaliados: %d\n", nivel, nivel_atual->total_candidatos);
        fprintf(saida, "Itemsets frequentes em L%d: %d\n\n", nivel, nivel_atual->total);

        if (nivel_atual->total == 0) {
            fprintf(saida, "Nenhum itemset frequente encontrado neste nivel.\n\n");
            continue;
        }

        for (int i = 0; i < nivel_atual->total; i++) {
            ItemsetFrequente *itemset =
                &resultado->itemsets_frequentes[nivel_atual->inicio + i];

            escrever_itemset_formatado(saida, base, itemset);
        }

        fprintf(saida, "\n");
    }

    fprintf(saida, "Total geral de itemsets frequentes: %d\n\n",
            resultado->total_itemsets_frequentes);
}

static int itemset_e_subconjunto(
    ItemsetFrequente *possivel_subconjunto,
    ItemsetFrequente *possivel_superconjunto
) {
    int encontrados = 0;

    for (int i = 0; i < possivel_subconjunto->tamanho; i++) {
        for (int j = 0; j < possivel_superconjunto->tamanho; j++) {
            if (possivel_subconjunto->itens[i] == possivel_superconjunto->itens[j]) {
                encontrados++;
                break;
            }
        }
    }

    return encontrados == possivel_subconjunto->tamanho;
}

static int itemset_tem_superconjunto_frequente(
    ResultadoApriori *resultado,
    ItemsetFrequente *itemset,
    int exigir_mesmo_suporte
) {
    for (int i = 0; i < resultado->total_itemsets_frequentes; i++) {
        ItemsetFrequente *outro = &resultado->itemsets_frequentes[i];

        if (outro->tamanho <= itemset->tamanho) {
            continue;
        }

        if (exigir_mesmo_suporte && outro->suporte != itemset->suporte) {
            continue;
        }

        if (itemset_e_subconjunto(itemset, outro)) {
            return 1;
        }
    }

    return 0;
}

static void escrever_itemsets_fechados_e_maximais(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    int total_fechados = 0;
    int total_maximais = 0;

    fprintf(saida, "========================================\n");
    fprintf(saida, " ITEMSETS FECHADOS E MAXIMAIS\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Itemset fechado: nao possui superconjunto frequente com o mesmo suporte.\n");
    fprintf(saida, "Itemset maximal: nao possui nenhum superconjunto frequente.\n\n");

    fprintf(saida, "Itemsets fechados:\n\n");

    for (int i = 0; i < resultado->total_itemsets_frequentes; i++) {
        ItemsetFrequente *itemset = &resultado->itemsets_frequentes[i];

        if (!itemset_tem_superconjunto_frequente(resultado, itemset, 1)) {
            escrever_itemset_formatado(saida, base, itemset);
            total_fechados++;
        }
    }

    fprintf(saida, "\nTotal de itemsets fechados: %d\n\n", total_fechados);
    fprintf(saida, "Itemsets maximais:\n\n");

    for (int i = 0; i < resultado->total_itemsets_frequentes; i++) {
        ItemsetFrequente *itemset = &resultado->itemsets_frequentes[i];

        if (!itemset_tem_superconjunto_frequente(resultado, itemset, 0)) {
            escrever_itemset_formatado(saida, base, itemset);
            total_maximais++;
        }
    }

    fprintf(saida, "\nTotal de itemsets maximais: %d\n\n", total_maximais);
}

static int contar_itens_texto_regra(const char *texto) {
    int total = 0;
    int dentro_de_item = 0;

    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == '{' || texto[i] == '}' || texto[i] == ',' || texto[i] == ' ') {
            dentro_de_item = 0;
        } else if (!dentro_de_item) {
            total++;
            dentro_de_item = 1;
        }
    }

    return total;
}

static void escrever_maior_itemset_e_menor_regra(
    FILE *saida,
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    ItemsetFrequente *maior_itemset = NULL;
    int total_maiores = 0;

    for (int i = 0; i < resultado->total_itemsets_frequentes; i++) {
        ItemsetFrequente *itemset = &resultado->itemsets_frequentes[i];

        if (maior_itemset == NULL || itemset->tamanho > maior_itemset->tamanho) {
            maior_itemset = itemset;
            total_maiores = 1;
        } else if (itemset->tamanho == maior_itemset->tamanho) {
            total_maiores++;
        }
    }

    RegraAssociacao *menor_regra = NULL;
    int tamanho_menor_regra = 0;

    for (int i = 0; i < resultado->total_regras; i++) {
        RegraAssociacao *regra = &resultado->regras[i];
        int tamanho_regra =
            contar_itens_texto_regra(regra->antecedente) +
            contar_itens_texto_regra(regra->consequente);

        if (menor_regra == NULL || tamanho_regra < tamanho_menor_regra) {
            menor_regra = regra;
            tamanho_menor_regra = tamanho_regra;
        }
    }

    fprintf(saida, "========================================\n");
    fprintf(saida, " MAIOR ITEMSET E MENOR REGRA VALIDA\n");
    fprintf(saida, "========================================\n\n");

    if (maior_itemset == NULL) {
        fprintf(saida, "Nenhum itemset frequente foi encontrado.\n\n");
    } else {
        fprintf(saida, "Maior itemset frequente encontrado:\n");
        escrever_itemset_formatado(saida, base, maior_itemset);
        fprintf(saida, "Tamanho: %d itens\n", maior_itemset->tamanho);
        fprintf(saida, "Quantidade de itemsets com esse mesmo tamanho: %d\n\n", total_maiores);
    }

    if (menor_regra == NULL) {
        fprintf(saida, "Nenhuma regra valida foi encontrada.\n\n");
    } else {
        fprintf(saida, "Menor regra valida encontrada:\n");
        fprintf(saida, "%s -> %s\n", menor_regra->antecedente, menor_regra->consequente);
        fprintf(saida, "Tamanho: %d itens no total\n", tamanho_menor_regra);
        fprintf(saida, "Suporte do conjunto: %d\n", menor_regra->suporte_conjunto);
        fprintf(saida, "Confianca: %.2f%%\n\n", menor_regra->confianca * 100);
    }
}

static void escrever_classificacao_confianca(
    FILE *saida,
    float confianca
) {
    if (confianca >= 0.90) {
        fprintf(saida, "Classificacao: regra muito forte.\n");
        fprintf(saida, "Motivo: a confianca e maior ou igual a 90%%.\n");
    } else if (confianca >= 0.70) {
        fprintf(saida, "Classificacao: regra forte.\n");
        fprintf(saida, "Motivo: a confianca e maior ou igual a 70%%.\n");
    } else {
        fprintf(saida, "Classificacao: regra moderada ou fraca.\n");
        fprintf(saida, "Motivo: a confianca ficou abaixo de 70%%.\n");
    }
}

static void escrever_regras(
    FILE *saida,
    ResultadoApriori *resultado
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " REGRAS DE ASSOCIACAO GERADAS\n");
    fprintf(saida, "========================================\n\n");

    if (resultado->total_regras == 0) {
        fprintf(saida, "Nenhuma regra atingiu os criterios definidos.\n\n");
        return;
    }

    fprintf(saida, "Cada regra abaixo mostra uma relacao do tipo:\n");
    fprintf(saida, "{antecedente} -> {consequente}\n\n");

    fprintf(saida, "O calculo principal usado e:\n");
    fprintf(saida, "Confianca = Suporte do conjunto completo / Suporte do antecedente\n\n");

    for (int i = 0; i < resultado->total_regras; i++) {
        RegraAssociacao regra = resultado->regras[i];

        fprintf(saida, "----------------------------------------\n");
        fprintf(saida, "REGRA %d\n", i + 1);
        fprintf(saida, "----------------------------------------\n\n");

        fprintf(saida, "Regra encontrada:\n");
        fprintf(saida, "%s -> %s\n\n", regra.antecedente, regra.consequente);

        fprintf(saida, "Leitura da regra:\n");
        fprintf(saida, "Quando uma compra possui %s, existe uma tendencia de tambem possuir %s.\n\n",
                regra.antecedente,
                regra.consequente);

        fprintf(saida, "Dados usados no calculo:\n");
        fprintf(saida, "- Suporte do antecedente: %d\n", regra.suporte_antecedente);
        fprintf(saida, "- Suporte do conjunto completo: %d\n", regra.suporte_conjunto);
        fprintf(saida, "- Confianca: %.2f%%\n\n", regra.confianca * 100);

        fprintf(saida, "Como a confianca foi calculada:\n");
        fprintf(saida, "Confianca = Suporte do conjunto completo / Suporte do antecedente\n");
        fprintf(saida, "Confianca = %d / %d\n",
                regra.suporte_conjunto,
                regra.suporte_antecedente);
        fprintf(saida, "Confianca = %.4f\n", regra.confianca);
        fprintf(saida, "Confianca em porcentagem = %.4f * 100\n", regra.confianca);
        fprintf(saida, "Confianca em porcentagem = %.2f%%\n\n", regra.confianca * 100);

        fprintf(saida, "Interpretacao:\n");
        fprintf(saida, "Das %d compras que possuem %s, %d tambem possuem %s.\n",
                regra.suporte_antecedente,
                regra.antecedente,
                regra.suporte_conjunto,
                regra.consequente);

        fprintf(saida, "Portanto, a regra %s -> %s possui %.2f%% de confianca.\n\n",
                regra.antecedente,
                regra.consequente,
                regra.confianca * 100);

        escrever_classificacao_confianca(saida, regra.confianca);

        fprintf(saida, "\n");
    }
}

static void escrever_regras_incertas(
    FILE *saida,
    ResultadoApriori *resultado
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " REGRAS INCERTAS - DADOS INSUFICIENTES\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Esta secao lista regras cujo itemset completo apareceu em apenas uma compra.\n");
    fprintf(saida, "Elas sao removidas das regras de associacao validas e nao entram no nivel\n");
    fprintf(saida, "exibido por confianca, pois uma unica ocorrencia nao e suficiente para validar\n");
    fprintf(saida, "um padrao de compra.\n\n");

    if (resultado->total_regras_incertas == 0) {
        fprintf(saida, "Nenhuma regra incerta foi encontrada.\n\n");
        return;
    }

    for (int i = 0; i < resultado->total_regras_incertas; i++) {
        RegraAssociacao regra = resultado->regras_incertas[i];

        fprintf(saida, "Regra incerta %d: %s -> %s | Suporte do conjunto: %d | ",
                i + 1,
                regra.antecedente,
                regra.consequente,
                regra.suporte_conjunto);
        fprintf(saida, "Confianca calculada: %.2f%% | Classificacao: incerta\n",
                regra.confianca * 100);
    }

    fprintf(saida, "\nTotal de regras incertas: %d\n\n", resultado->total_regras_incertas);
}

static void escrever_resumo_final(
    FILE *saida,
    ResultadoApriori *resultado
) {
    int regras_muito_fortes = 0;
    int regras_fortes = 0;
    int regras_moderadas = 0;

    for (int i = 0; i < resultado->total_regras; i++) {
        float confianca = resultado->regras[i].confianca;

        if (confianca >= 0.90) {
            regras_muito_fortes++;
        } else if (confianca >= 0.70) {
            regras_fortes++;
        } else {
            regras_moderadas++;
        }
    }

    fprintf(saida, "========================================\n");
    fprintf(saida, " RESUMO FINAL\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Total de regras geradas: %d\n", resultado->total_regras);
    fprintf(saida, "Total de regras incertas por dados insuficientes: %d\n",
            resultado->total_regras_incertas);
    fprintf(saida, "Regras muito fortes, com confianca >= 90%%: %d\n", regras_muito_fortes);
    fprintf(saida, "Regras fortes, com confianca entre 70%% e 89.99%%: %d\n", regras_fortes);
    fprintf(saida, "Regras moderadas ou fracas, abaixo de 70%%: %d\n\n", regras_moderadas);

    fprintf(saida, "Conclusao:\n");
    fprintf(saida, "As regras de associacao ajudam a identificar padroes de compra.\n");
    fprintf(saida, "Quanto maior a confianca, maior a chance de o consequente aparecer quando o antecedente aparece.\n");
    fprintf(saida, "Essas informacoes podem ser usadas para organizar produtos, criar promocoes e entender habitos de consumo.\n");
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

    escrever_cabecalho_relatorio(saida, nome_arquivo_entrada, base);
    escrever_conceitos(saida);

    escrever_regras_maior_nivel_confianca(saida, resultado);

    escrever_amostra_compras(saida, base);
    escrever_itens_encontrados(saida, base);

    escrever_itemsets_frequentes(saida, base, resultado);
    escrever_itemsets_fechados_e_maximais(saida, base, resultado);
    escrever_maior_itemset_e_menor_regra(saida, base, resultado);

    escrever_regras(saida, resultado);
    escrever_regras_incertas(saida, resultado);
    escrever_resumo_final(saida, resultado);

    fclose(saida);

    return 1;
}
