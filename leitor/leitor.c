#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "leitor.h"

static void remover_quebra_linha(char *texto) {
    texto[strcspn(texto, "\n")] = '\0';
    texto[strcspn(texto, "\r")] = '\0';
}

static void remover_espacos(char *texto) {
    int inicio = 0;
    int fim = strlen(texto) - 1;

    while (isspace((unsigned char) texto[inicio])) {
        inicio++;
    }

    while (fim >= inicio && isspace((unsigned char) texto[fim])) {
        fim--;
    }

    int j = 0;

    for (int i = inicio; i <= fim; i++) {
        texto[j++] = texto[i];
    }

    texto[j] = '\0';
}

static void converter_para_minusculo(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = tolower((unsigned char) texto[i]);
    }
}

static int buscar_item(BaseCompras *base, char *nome_item) {
    for (int i = 0; i < base->total_itens; i++) {
        if (strcmp(base->itens[i], nome_item) == 0) {
            return i;
        }
    }

    return -1;
}

static int adicionar_item(BaseCompras *base, char *nome_item) {
    int posicao = buscar_item(base, nome_item);

    if (posicao != -1) {
        return posicao;
    }

    if (base->total_itens >= MAX_ITENS) {
        printf("Erro: limite maximo de itens atingido.\n");
        exit(1);
    }

    strcpy(base->itens[base->total_itens], nome_item);
    base->total_itens++;

    return base->total_itens - 1;
}

void inicializar_base(BaseCompras *base) {
    base->total_itens = 0;
    base->total_transacoes = 0;

    for (int i = 0; i < MAX_TRANSACOES; i++) {
        for (int j = 0; j < MAX_ITENS; j++) {
            base->transacoes[i][j] = 0;
        }
    }
}

int ler_arquivo_compras(
    const char *nome_arquivo,
    BaseCompras *base
) {
    FILE *arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir o arquivo '%s'.\n", nome_arquivo);
        return 0;
    }

    char linha[MAX_LINHA];

    while (fgets(linha, MAX_LINHA, arquivo) != NULL) {
        if (base->total_transacoes >= MAX_TRANSACOES) {
            printf("Erro: limite maximo de transacoes atingido.\n");
            fclose(arquivo);
            return 0;
        }

        remover_quebra_linha(linha);

        if (strlen(linha) == 0) {
            continue;
        }

        char *item = strtok(linha, ",");

        while (item != NULL) {
            remover_espacos(item);
            converter_para_minusculo(item);

            if (strlen(item) > 0) {
                int posicao = adicionar_item(base, item);
                base->transacoes[base->total_transacoes][posicao] = 1;
            }

            item = strtok(NULL, ",");
        }

        base->total_transacoes++;
    }

    fclose(arquivo);

    return 1;
}