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
    fprintf(saida, "- Regras moderadas ou fracas, abaixo de 70%%: %d\n\n", regras_moderadas);

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

static void escrever_itemsets_frequentes_1(
    FILE *saida,
    BaseCompras *base
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " ITEMSETS FREQUENTES DE TAMANHO 1\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Aqui aparecem os itens individuais que atingiram o suporte minimo.\n\n");

    int total = 0;

    for (int i = 0; i < base->total_itens; i++) {
        int suporte = calcular_suporte_1_item(base, i);

        if (suporte >= MIN_SUP) {
            fprintf(saida, "{%s} -> suporte = %d\n", base->itens[i], suporte);
            total++;
        }
    }

    fprintf(saida, "\nTotal de itemsets frequentes de tamanho 1: %d\n\n", total);
}

static void escrever_itemsets_frequentes_2(
    FILE *saida,
    BaseCompras *base
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " ITEMSETS FREQUENTES DE TAMANHO 2\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Aqui aparecem pares de itens que ocorreram juntos pelo menos %d vezes.\n\n", MIN_SUP);

    int total = 0;

    for (int i = 0; i < base->total_itens; i++) {
        for (int j = i + 1; j < base->total_itens; j++) {
            int suporte = calcular_suporte_2_itens(base, i, j);

            if (suporte >= MIN_SUP) {
                fprintf(saida, "{%s, %s} -> suporte = %d\n",
                        base->itens[i],
                        base->itens[j],
                        suporte);
                total++;
            }
        }
    }

    fprintf(saida, "\nTotal de itemsets frequentes de tamanho 2: %d\n\n", total);
}

static void escrever_itemsets_frequentes_3(
    FILE *saida,
    BaseCompras *base
) {
    fprintf(saida, "========================================\n");
    fprintf(saida, " ITEMSETS FREQUENTES DE TAMANHO 3\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Aqui aparecem trios de itens que ocorreram juntos pelo menos %d vezes.\n\n", MIN_SUP);

    int total = 0;

    for (int i = 0; i < base->total_itens; i++) {
        for (int j = i + 1; j < base->total_itens; j++) {
            for (int k = j + 1; k < base->total_itens; k++) {
                int suporte = calcular_suporte_3_itens(base, i, j, k);

                if (suporte >= MIN_SUP) {
                    fprintf(saida, "{%s, %s, %s} -> suporte = %d\n",
                            base->itens[i],
                            base->itens[j],
                            base->itens[k],
                            suporte);
                    total++;
                }
            }
        }
    }

    fprintf(saida, "\nTotal de itemsets frequentes de tamanho 3: %d\n\n", total);
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

    escrever_itemsets_frequentes_1(saida, base);
    escrever_itemsets_frequentes_2(saida, base);
    escrever_itemsets_frequentes_3(saida, base);

    escrever_regras(saida, resultado);
    escrever_resumo_final(saida, resultado);

    fclose(saida);

    return 1;
}
