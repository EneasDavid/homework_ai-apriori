#include <stdio.h>

#include "leitor/leitor.h"
#include "apriori/apriori.h"
#include "saida/saida.h"

static int ler_parametros_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    int suporte_minimo;
    float confianca_percentual;

    printf("\nConfigure os criterios do Apriori:\n\n");
    printf("Suporte minimo e a quantidade minima de compras em que um itemset\n");
    printf("deve aparecer para ser considerado frequente.\n");
    printf("Digite o suporte minimo em ocorrencias (exemplo: 2): ");

    if (scanf("%d", &suporte_minimo) != 1 ||
        suporte_minimo <= 0 ||
        suporte_minimo > base->total_transacoes) {
        printf("Erro: informe um suporte entre 1 e %d.\n", base->total_transacoes);
        return 0;
    }

    printf("\nConfianca minima e o percentual minimo para aceitar uma regra\n");
    printf("de associacao, como {pao} -> {leite}.\n");
    printf("Digite a confianca minima entre 0 e 100 (exemplo: 70): ");

    if (scanf("%f", &confianca_percentual) != 1 ||
        confianca_percentual < 0 ||
        confianca_percentual > 100) {
        printf("Erro: informe uma confianca entre 0 e 100.\n");
        return 0;
    }

    configurar_parametros_apriori(
        resultado,
        suporte_minimo,
        confianca_percentual / 100.0f
    );

    return 1;
}

int main() {
    /*
     * O main organiza o fluxo geral.
     * A leitura, o processamento Apriori e a escrita do relatorio ficam em modulos separados.
     */
    char nome_arquivo_entrada[100];
    char nome_arquivo_saida[] = "regras_associacao.txt";

    static BaseCompras base;
    static ResultadoApriori resultado;

    printf("========================================\n");
    printf(" ALGORITMO APRIORI - VERSAO MODULAR\n");
    printf("========================================\n\n");

    printf("Digite o nome do arquivo de compras: ");
    scanf("%99s", nome_arquivo_entrada);

    inicializar_base(&base);
    inicializar_resultado(&resultado);

    printf("\n[1/3] Lendo arquivo de compras...\n");

    if (!ler_arquivo_compras(nome_arquivo_entrada, &base)) {
        printf("Erro ao ler o arquivo de entrada.\n");
        return 1;
    }

    printf("Arquivo lido com sucesso.\n");
    printf("Total de compras: %d\n", base.total_transacoes);
    printf("Total de itens diferentes: %d\n", base.total_itens);

    if (!ler_parametros_apriori(&base, &resultado)) {
        return 1;
    }

    printf("Suporte minimo: %d ocorrencias (%.2f%% nesta base)\n",
           resultado.suporte_minimo,
           100.0f * resultado.suporte_minimo / base.total_transacoes);
    printf("Confianca minima: %.2f%%\n", resultado.confianca_minima * 100);

    printf("\n[2/3] Aplicando algoritmo Apriori...\n");

    if (!aplicar_apriori(&base, &resultado)) {
        printf("Erro ao aplicar o algoritmo Apriori.\n");
        return 1;
    }

    printf("Apriori executado com sucesso.\n");
    printf("Total de regras encontradas: %d\n", resultado.total_regras);

    printf("\n[3/3] Gerando arquivo de saida...\n");

    if (!gerar_arquivo_saida(
        nome_arquivo_saida,
        nome_arquivo_entrada,
        &base,
        &resultado
    )) {
        printf("Erro ao gerar o arquivo de saida.\n");
        return 1;
    }

    printf("Arquivo de saida gerado com sucesso: %s\n", nome_arquivo_saida);

    printf("\nProcessamento finalizado.\n");

    return 0;
}
