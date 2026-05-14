#include <stdio.h>

#include "saida.h"

#define LIMITE_COMPRAS_EXIBIDAS 20
#define CONFIANCA_MUITO_FORTE 0.90
#define CONFIANCA_FORTE 0.70

typedef struct {
    const char *nome_arquivo_entrada;
    int total_compras;
    int total_itens;
} DadosGlobais;

typedef struct {
    int muito_fortes;
    int fortes;
    int moderadas_ou_fracas;
} EstatisticasRegras;

/*
 * Este modulo transforma os dados processados em um relatorio objetivo.
 * Conceitos didaticos e analise teorica ficam nos comentarios do codigo e no README.
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
static void imprimir_cabecalho(
    FILE *fp,
    DadosGlobais info
) {
    BaseCompras base_resumo;

    base_resumo.total_transacoes = info.total_compras;
    base_resumo.total_itens = info.total_itens;

    escrever_cabecalho_relatorio(fp, info.nome_arquivo_entrada, &base_resumo);
    escrever_conceitos(fp);
}

static EstatisticasRegras contar_estatisticas_regras(
    ResultadoApriori *resultado
) {
    EstatisticasRegras estatisticas = {0, 0, 0};

    for (int i = 0; i < resultado->total_regras; i++) {
        float confianca = resultado->regras[i].confianca;

        if (confianca >= CONFIANCA_MUITO_FORTE) {
            estatisticas.muito_fortes++;
        } else if (confianca >= CONFIANCA_FORTE) {
            estatisticas.fortes++;
        } else {
            estatisticas.moderadas_ou_fracas++;
        }
    }

    return estatisticas;
}

static void escrever_regras_maior_nivel_confianca(
    FILE *saida,
    ResultadoApriori *resultado
) {
    EstatisticasRegras estatisticas = contar_estatisticas_regras(resultado);

    fprintf(saida, "========================================\n");
    fprintf(saida, " REGRAS DESTACADAS PELO MAIOR NIVEL DE CONFIANCA\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Quantidade encontrada em cada nivel:\n");
    fprintf(saida, "- Regras muito fortes, com confianca >= 90%%: %d\n",
            estatisticas.muito_fortes);
    fprintf(saida, "- Regras fortes, com confianca entre 70%% e 89.99%%: %d\n",
            estatisticas.fortes);
    fprintf(saida, "- Regras moderadas ou fracas, abaixo de 70%%: %d\n",
            estatisticas.moderadas_ou_fracas);
    fprintf(saida, "- Regras incertas, com suporte do conjunto igual a 1: %d\n\n",
            resultado->total_regras_incertas);

    if (resultado->total_regras_incertas > 0) {
        fprintf(saida, "Regras incertas removidas dos niveis de confianca: %d\n\n",
                resultado->total_regras_incertas);
    }

    int nivel_escolhido;

    if (estatisticas.muito_fortes > 0) {
        nivel_escolhido = 1;
        fprintf(saida, "Nivel exibido nesta secao: REGRAS MUITO FORTES\n\n");
    } else if (estatisticas.fortes > 0) {
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

        if (nivel_escolhido == 1 && confianca >= CONFIANCA_MUITO_FORTE) {
            deve_exibir = 1;
        } else if (nivel_escolhido == 2 &&
                   confianca >= CONFIANCA_FORTE &&
                   confianca < CONFIANCA_MUITO_FORTE) {
            deve_exibir = 1;
        } else if (nivel_escolhido == 3 && confianca < CONFIANCA_FORTE) {
            deve_exibir = 1;
        }

        if (deve_exibir) {
            fprintf(saida, "Regra destacada %d: %s -> %s | Confianca: %.2f%%\n",
                    contador,
                    regra.antecedente,
                    regra.consequente,
                    regra.confianca * 100);
            fprintf(saida, "  Itemset completo: %s | Frequencia: %d | Relevancia na base: %.4f%%\n",
                    regra.itemset,
                    regra.frequencia,
                    regra.relevancia * 100);

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

    fprintf(saida, "Compras exibidas: ate %d\n", LIMITE_COMPRAS_EXIBIDAS);
    fprintf(saida, "Total de compras analisadas: %d\n\n", base->total_transacoes);

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
        fprintf(saida, "\nCompras restantes nao exibidas: %d\n",
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

    fprintf(saida, "Total de itens diferentes: %d\n\n", base->total_itens);

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
    if (confianca >= CONFIANCA_MUITO_FORTE) {
        fprintf(saida, "Classificacao: regra muito forte.\n");
    } else if (confianca >= CONFIANCA_FORTE) {
        fprintf(saida, "Classificacao: regra forte.\n");
    } else {
        fprintf(saida, "Classificacao: regra moderada ou fraca.\n");
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

    for (int i = 0; i < resultado->total_regras; i++) {
        RegraAssociacao regra = resultado->regras[i];

        fprintf(saida, "----------------------------------------\n");
        fprintf(saida, "REGRA %d\n", i + 1);
        fprintf(saida, "----------------------------------------\n\n");

        fprintf(saida, "Regra: %s -> %s\n\n", regra.antecedente, regra.consequente);
        fprintf(saida, "- Itemset completo: %s\n", regra.itemset);
        fprintf(saida, "- Consequente: %s\n", regra.consequente);
        fprintf(saida, "- Frequencia do itemset completo: %d\n", regra.frequencia);
        fprintf(saida, "- Suporte do antecedente: %d\n", regra.suporte_antecedente);
        fprintf(saida, "- Suporte do conjunto completo: %d\n", regra.suporte_conjunto);
        fprintf(saida, "- Suporte relativo do conjunto completo: %.4f%%\n",
                regra.suporte * 100);
        fprintf(saida, "- Relevancia na base inteira: %.4f%%\n",
                regra.relevancia * 100);
        fprintf(saida, "- Confianca: %.2f%%\n", regra.confianca * 100);
        fprintf(saida, "- Regra incerta: %s\n\n", regra.incerta ? "sim" : "nao");

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

    if (resultado->total_regras_incertas == 0) {
        fprintf(saida, "Nenhuma regra incerta foi encontrada.\n\n");
        return;
    }

    for (int i = 0; i < resultado->total_regras_incertas; i++) {
        RegraAssociacao regra = resultado->regras_incertas[i];

        fprintf(saida, "Regra incerta %d: %s -> %s | Itemset completo: %s | ",
                i + 1,
                regra.antecedente,
                regra.consequente,
                regra.itemset);
        fprintf(saida, "Suporte do conjunto: %d | Suporte relativo: %.4f%% | ",
                regra.suporte_conjunto,
                regra.suporte * 100);
        fprintf(saida, "Frequencia: %d | Relevancia: %.4f%% | ",
                regra.frequencia,
                regra.relevancia * 100);
        fprintf(saida, "Confianca calculada: %.2f%% | Flag incerta: %s\n",
                regra.confianca * 100,
                regra.incerta ? "sim" : "nao");
    }

    fprintf(saida, "\nTotal de regras incertas: %d\n\n", resultado->total_regras_incertas);
}

static void escrever_resumo_final(
    FILE *saida,
    ResultadoApriori *resultado
) {
    EstatisticasRegras estatisticas = contar_estatisticas_regras(resultado);

    fprintf(saida, "========================================\n");
    fprintf(saida, " RESUMO FINAL\n");
    fprintf(saida, "========================================\n\n");

    fprintf(saida, "Total de regras geradas: %d\n", resultado->total_regras);
    fprintf(saida, "Total de regras incertas por dados insuficientes: %d\n",
            resultado->total_regras_incertas);
    fprintf(saida, "Regras muito fortes, com confianca >= 90%%: %d\n",
            estatisticas.muito_fortes);
    fprintf(saida, "Regras fortes, com confianca entre 70%% e 89.99%%: %d\n",
            estatisticas.fortes);
    fprintf(saida, "Regras moderadas ou fracas, abaixo de 70%%: %d\n",
            estatisticas.moderadas_ou_fracas);
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

    DadosGlobais info = {
        nome_arquivo_entrada,
        base->total_transacoes,
        base->total_itens
    };

    imprimir_cabecalho(saida, info);

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
