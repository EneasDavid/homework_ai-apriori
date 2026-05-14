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
    /* Remove espacos no inicio e no fim para evitar itens duplicados como "leite" e " leite". */
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

static int linha_tem_conteudo_util(const char *linha) {
    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] != ',' && !isspace((unsigned char) linha[i])) {
            return 1;
        }
    }

    return 0;
}

static void converter_para_minusculo(char *texto) {
    /* Normaliza maiusculas/minusculas: "Leite", "LEITE" e "leite" viram o mesmo item. */
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = tolower((unsigned char) texto[i]);
    }
}

static int buscar_item(BaseCompras *base, char *nome_item) {
    /* Procura se o item ja existe para reutilizar a mesma coluna da matriz de transacoes. */
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

    if ((int) strlen(nome_item) >= MAX_NOME_ITEM) {
        printf("Erro: item '%s' possui nome maior que o limite de %d caracteres.\n",
               nome_item,
               MAX_NOME_ITEM - 1);
        return -1;
    }

    if (base->total_itens >= MAX_ITENS) {
        printf("Erro: limite maximo de itens diferentes atingido (%d).\n", MAX_ITENS);
        return -1;
    }

    strcpy(base->itens[base->total_itens], nome_item);
    base->total_itens++;

    return base->total_itens - 1;
}

static int linha_foi_truncada(const char *linha) {
    return strchr(linha, '\n') == NULL && strchr(linha, '\r') == NULL;
}

static void descartar_resto_linha(FILE *arquivo) {
    int caractere;

    while ((caractere = fgetc(arquivo)) != '\n' &&
           caractere != '\r' &&
           caractere != EOF) {
    }
}

static int processar_linha_compras(
    BaseCompras *base,
    char *linha,
    int numero_linha
) {
    int transacao = base->total_transacoes;
    int itens_validos = 0;
    char *inicio = linha;

    while (1) {
        char *fim = strchr(inicio, ',');
        char item[MAX_LINHA];
        int tamanho;

        if (fim == NULL) {
            tamanho = strlen(inicio);
        } else {
            tamanho = fim - inicio;
        }

        if (tamanho >= MAX_LINHA) {
            printf("Erro: item muito grande na linha %d.\n", numero_linha);
            return 0;
        }

        memcpy(item, inicio, tamanho);
        item[tamanho] = '\0';

        remover_espacos(item);
        converter_para_minusculo(item);

        if (strlen(item) == 0) {
            printf("Aviso: item vazio ignorado na linha %d.\n", numero_linha);
        } else {
            int posicao = adicionar_item(base, item);

            if (posicao < 0) {
                printf("Erro: nao foi possivel adicionar item da linha %d.\n", numero_linha);
                return 0;
            }

            if (base->transacoes[transacao][posicao] == 1) {
                printf("Aviso: item repetido '%s' ignorado na linha %d.\n",
                       item,
                       numero_linha);
            } else {
                base->transacoes[transacao][posicao] = 1;
                itens_validos++;
            }
        }

        if (fim == NULL) {
            break;
        }

        inicio = fim + 1;
    }

    if (itens_validos == 0) {
        printf("Aviso: linha %d nao possui nenhum item valido e foi ignorada.\n",
               numero_linha);
        return 1;
    }

    base->total_transacoes++;

    return 1;
}

void inicializar_base(BaseCompras *base) {
    /* A matriz comeca zerada: antes da leitura, nenhuma compra possui nenhum item. */
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
    int numero_linha = 0;

    while (fgets(linha, MAX_LINHA, arquivo) != NULL) {
        numero_linha++;

        if (linha_foi_truncada(linha) && !feof(arquivo)) {
            printf("Erro: linha %d possui mais que %d caracteres.\n",
                   numero_linha,
                   MAX_LINHA - 1);
            descartar_resto_linha(arquivo);
            fclose(arquivo);
            return 0;
        }

        if (base->total_transacoes >= MAX_TRANSACOES) {
            printf("Erro: limite maximo de transacoes atingido (%d).\n", MAX_TRANSACOES);
            fclose(arquivo);
            return 0;
        }

        remover_quebra_linha(linha);

        if (!linha_tem_conteudo_util(linha)) {
            if (strlen(linha) > 0) {
                printf("Aviso: linha %d possui apenas espacos/virgulas e foi ignorada.\n",
                       numero_linha);
            }
            continue;
        }

        if (!processar_linha_compras(base, linha, numero_linha)) {
            fclose(arquivo);
            return 0;
        }
    }

    fclose(arquivo);

    if (numero_linha == 0) {
        printf("Erro: arquivo '%s' esta vazio.\n", nome_arquivo);
        return 0;
    }

    if (base->total_transacoes == 0) {
        printf("Erro: nenhuma transacao valida foi encontrada no arquivo.\n");
        return 0;
    }

    if (base->total_itens == 0) {
        printf("Erro: nenhum item valido foi encontrado no arquivo.\n");
        return 0;
    }

    return 1;
}
