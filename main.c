#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "leitor/leitor.h"
#include "apriori/apriori.h"
#include "saida/saida.h"

static int ler_linha(const char *mensagem, char *valor, size_t tamanho) {
    printf("%s", mensagem);

    if (fgets(valor, tamanho, stdin) == NULL) {
        return 0;
    }

    if (strchr(valor, '\n') == NULL) {
        int caractere;

        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
    }

    valor[strcspn(valor, "\r\n")] = '\0';
    return 1;
}

static int ler_inteiro_intervalo(
    const char *mensagem,
    int minimo,
    int maximo,
    int *valor
) {
    char entrada[100];

    while (ler_linha(mensagem, entrada, sizeof(entrada))) {
        char *fim;
        errno = 0;
        long numero = strtol(entrada, &fim, 10);

        while (*fim == ' ' || *fim == '\t') {
            fim++;
        }

        if (errno == 0 &&
            fim != entrada &&
            *fim == '\0' &&
            numero >= minimo &&
            numero <= maximo) {
            *valor = (int) numero;
            return 1;
        }

        printf("  Valor invalido. Informe um numero inteiro entre %d e %d.\n\n",
               minimo,
               maximo);
    }

    return 0;
}

static int ler_percentual(const char *mensagem, float *valor) {
    char entrada[100];

    while (ler_linha(mensagem, entrada, sizeof(entrada))) {
        char *fim;
        errno = 0;
        float numero = strtof(entrada, &fim);

        while (*fim == ' ' || *fim == '\t') {
            fim++;
        }

        if (errno == 0 &&
            fim != entrada &&
            *fim == '\0' &&
            numero == numero &&
            numero >= 0 &&
            numero <= 100) {
            *valor = numero;
            return 1;
        }

        printf("  Valor invalido. Informe um percentual entre 0 e 100.\n\n");
    }

    return 0;
}

static int ler_parametros_apriori(
    BaseCompras *base,
    ResultadoApriori *resultado
) {
    int suporte_minimo;
    float confianca_percentual;

    printf("\n------------------------------------------------------------\n");
    printf(" CONFIGURACAO DA ANALISE\n");
    printf("------------------------------------------------------------\n");
    printf("Base carregada: %d compras e %d itens diferentes.\n\n",
           base->total_transacoes,
           base->total_itens);

    printf("1. Suporte minimo\n");
    printf("   Quantidade minima de compras em que um conjunto de itens\n");
    printf("   deve aparecer para ser considerado frequente.\n");

    char mensagem_suporte[100];
    snprintf(
        mensagem_suporte,
        sizeof(mensagem_suporte),
        "   Informe um valor de 1 a %d: ",
        base->total_transacoes
    );

    if (!ler_inteiro_intervalo(
            mensagem_suporte,
            1,
            base->total_transacoes,
            &suporte_minimo)) {
        return 0;
    }

    printf("\n2. Confianca minima\n");
    printf("   Percentual minimo para aceitar uma regra de associacao,\n");
    printf("   como {pao} -> {leite}.\n");

    if (!ler_percentual(
            "   Informe um percentual de 0 a 100: ",
            &confianca_percentual)) {
        return 0;
    }

    configurar_parametros_apriori(
        resultado,
        suporte_minimo,
        confianca_percentual / 100.0f
    );

    return 1;
}

int main(void) {
    /*
     * O main organiza o fluxo geral.
     * A leitura, o processamento Apriori e a escrita do relatorio ficam em modulos separados.
     */
    char nome_arquivo_entrada[500];
    char nome_arquivo_saida[] = "regras_associacao.md";

    static BaseCompras base;
    static ResultadoApriori resultado;

    printf("============================================================\n");
    printf(" APRIORI | DESCOBERTA DE REGRAS DE ASSOCIACAO\n");
    printf("============================================================\n\n");
    printf("Informe uma base de compras com uma transacao por linha.\n");
    printf("Exemplo de linha: leite,pao,manteiga\n\n");

    do {
        if (!ler_linha("Arquivo de entrada: ", nome_arquivo_entrada,
                       sizeof(nome_arquivo_entrada))) {
            printf("\nEntrada encerrada antes da leitura do arquivo.\n");
            return 1;
        }

        if (nome_arquivo_entrada[0] == '\0') {
            printf("  O nome do arquivo nao pode ficar vazio.\n\n");
        }
    } while (nome_arquivo_entrada[0] == '\0');

    inicializar_base(&base);
    inicializar_resultado(&resultado);

    printf("\n[1/3] Leitura da base\n");
    printf("      Lendo \"%s\"...\n", nome_arquivo_entrada);

    if (!ler_arquivo_compras(nome_arquivo_entrada, &base)) {
        printf("Erro ao ler o arquivo de entrada.\n");
        return 1;
    }

    printf("      Base carregada com sucesso: %d compras, %d itens.\n",
           base.total_transacoes,
           base.total_itens);

    if (!ler_parametros_apriori(&base, &resultado)) {
        printf("\nEntrada encerrada durante a configuracao.\n");
        return 1;
    }

    printf("\nResumo da configuracao:\n");
    printf("- Suporte minimo: %d ocorrencias (%.2f%% da base)\n",
           resultado.suporte_minimo,
           100.0f * resultado.suporte_minimo / base.total_transacoes);
    printf("- Confianca minima: %.2f%%\n", resultado.confianca_minima * 100);

    printf("\n[2/3] Processamento\n");
    printf("      Aplicando o algoritmo Apriori...\n");

    if (!aplicar_apriori(&base, &resultado)) {
        printf("Erro ao aplicar o algoritmo Apriori.\n");
        return 1;
    }

    printf("      Apriori concluido: %d regra(s) encontrada(s).\n",
           resultado.total_regras);

    printf("\n[3/3] Relatorio\n");
    printf("      Gerando arquivo Markdown...\n");

    if (!gerar_arquivo_saida(
        nome_arquivo_saida,
        nome_arquivo_entrada,
        &base,
        &resultado
    )) {
        printf("Erro ao gerar o arquivo de saida.\n");
        return 1;
    }

    printf("      Arquivo gerado com sucesso: %s\n", nome_arquivo_saida);
    printf("\nProcessamento finalizado.\n");

    return 0;
}
