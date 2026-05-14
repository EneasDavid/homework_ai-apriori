#include <stdio.h>
#include <string.h>

#include "leitor.h"
#include "apriori.h"

static int falhas = 0;

#define VERIFICAR(condicao, mensagem) \
    do { \
        if (!(condicao)) { \
            printf("FALHA: %s\n", mensagem); \
            falhas++; \
        } \
    } while (0)

static void adicionar_item_teste(
    BaseCompras *base,
    int indice,
    const char *nome
) {
    strcpy(base->itens[indice], nome);
}

static void montar_base_basica(BaseCompras *base) {
    inicializar_base(base);

    base->total_itens = 2;
    base->total_transacoes = 3;

    adicionar_item_teste(base, 0, "leite");
    adicionar_item_teste(base, 1, "pao");

    base->transacoes[0][0] = 1;
    base->transacoes[0][1] = 1;

    base->transacoes[1][0] = 1;
    base->transacoes[1][1] = 1;

    base->transacoes[2][0] = 1;
}

static int regra_existe(
    ResultadoApriori *resultado,
    const char *antecedente,
    const char *consequente
) {
    for (int i = 0; i < resultado->total_regras; i++) {
        if (strcmp(resultado->regras[i].antecedente, antecedente) == 0 &&
            strcmp(resultado->regras[i].consequente, consequente) == 0) {
            return 1;
        }
    }

    return 0;
}

static void testar_operacao_basica_apriori(void) {
    static BaseCompras base;
    static ResultadoApriori resultado;

    montar_base_basica(&base);
    inicializar_resultado(&resultado);

    VERIFICAR(aplicar_apriori(&base, &resultado) == 1,
              "Apriori deve executar em uma base valida.");

    int leite[] = {0};
    int pao[] = {1};
    int leite_pao[] = {0, 1};

    VERIFICAR(calcular_suporte_itemset(&base, leite, 1) == 3,
              "Suporte de {leite} deve ser 3.");
    VERIFICAR(calcular_suporte_itemset(&base, pao, 1) == 2,
              "Suporte de {pao} deve ser 2.");
    VERIFICAR(calcular_suporte_itemset(&base, leite_pao, 2) == 2,
              "Suporte de {leite, pao} deve ser 2.");

    VERIFICAR(resultado.total_niveis == 2,
              "A busca level-wise deve encontrar L1 e L2.");
    VERIFICAR(resultado.niveis[1].total == 2,
              "L1 deve ter dois itemsets frequentes.");
    VERIFICAR(resultado.niveis[2].total == 1,
              "L2 deve ter um itemset frequente.");
    VERIFICAR(resultado.total_itemsets_frequentes == 3,
              "Devem existir tres itemsets frequentes no total.");

    VERIFICAR(buscar_suporte_itemset(&resultado, leite_pao, 2) == 2,
              "Suporte de {leite, pao} deve estar armazenado em L2.");

    VERIFICAR(resultado.total_regras == 1,
              "Apenas uma regra deve atingir MIN_CONF.");
    VERIFICAR(regra_existe(&resultado, "{pao}", "{leite}"),
              "A regra {pao} -> {leite} deve existir.");
    VERIFICAR(!regra_existe(&resultado, "{leite}", "{pao}"),
              "A regra {leite} -> {pao} nao deve existir com MIN_CONF 70%.");

    VERIFICAR(resultado.regras[0].frequencia == 2,
              "Frequencia da regra valida deve ser 2.");
    VERIFICAR(resultado.regras[0].incerta == 0,
              "Regra valida nao deve ser marcada como incerta.");
}

static void testar_base_invalida(void) {
    static BaseCompras base;
    static ResultadoApriori resultado;

    inicializar_base(&base);
    inicializar_resultado(&resultado);

    VERIFICAR(aplicar_apriori(&base, &resultado) == 0,
              "Apriori deve rejeitar base sem transacoes e sem itens.");
}

int main(void) {
    testar_operacao_basica_apriori();
    testar_base_invalida();

    if (falhas > 0) {
        printf("\n%d teste(s) falharam.\n", falhas);
        return 1;
    }

    printf("Todos os testes do Apriori passaram.\n");
    return 0;
}
