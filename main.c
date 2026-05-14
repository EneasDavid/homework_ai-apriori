#include <stdio.h>

#include "leitor/leitor.h"
#include "apriori/apriori.h"
#include "saida/saida.h"

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
