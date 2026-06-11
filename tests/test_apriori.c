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

static RegraAssociacao *buscar_regra(
    ResultadoApriori *resultado,
    const char *antecedente,
    const char *consequente
) {
    for (int i = 0; i < resultado->total_regras; i++) {
        if (strcmp(resultado->regras[i].antecedente, antecedente) == 0 &&
            strcmp(resultado->regras[i].consequente, consequente) == 0) {
            return &resultado->regras[i];
        }
    }

    return NULL;
}

static float diferenca_absoluta(float a, float b) {
    float diferenca = a - b;
    return diferenca < 0 ? -diferenca : diferenca;
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
              "Apenas uma regra deve atingir a confianca minima padrao.");
    VERIFICAR(regra_existe(&resultado, "{pao}", "{leite}"),
              "A regra {pao} -> {leite} deve existir.");
    VERIFICAR(!regra_existe(&resultado, "{leite}", "{pao}"),
              "A regra {leite} -> {pao} nao deve existir com confianca minima 70%.");

    VERIFICAR(resultado.regras[0].frequencia == 2,
              "Frequencia da regra valida deve ser 2.");
    VERIFICAR(resultado.regras[0].incerta == 0,
              "Regra valida nao deve ser marcada como incerta.");
}

static void montar_base_didatica(BaseCompras *base) {
    inicializar_base(base);

    base->total_itens = 3;
    base->total_transacoes = 6;

    adicionar_item_teste(base, 0, "leite");
    adicionar_item_teste(base, 1, "pao");
    adicionar_item_teste(base, 2, "manteiga");

    int transacoes[][3] = {
        {1, 1, 0},
        {1, 1, 0},
        {1, 1, 0},
        {1, 0, 1},
        {1, 0, 1},
        {0, 1, 1}
    };

    for (int t = 0; t < base->total_transacoes; t++) {
        for (int i = 0; i < base->total_itens; i++) {
            base->transacoes[t][i] = transacoes[t][i];
        }
    }
}

static void testar_exemplo_didatico(void) {
    static BaseCompras base;
    static ResultadoApriori resultado;

    montar_base_didatica(&base);
    inicializar_resultado(&resultado);

    VERIFICAR(aplicar_apriori(&base, &resultado) == 1,
              "Apriori deve executar na base didatica.");
    VERIFICAR(resultado.niveis[2].total_candidatos_gerados == 3,
              "C2 deve conter as tres combinacoes dos tres itens de L1.");
    VERIFICAR(resultado.niveis[2].total_descartados_suporte == 1,
              "{pao, manteiga} deve ser reprovado por suporte.");
    VERIFICAR(resultado.niveis[3].total_candidatos_gerados == 1,
              "O join deve gerar {leite, pao, manteiga} em C3.");
    VERIFICAR(resultado.niveis[3].total_candidatos_podados == 1,
              "A Apriori Property deve podar {leite, pao, manteiga}.");
    VERIFICAR(resultado.niveis[3].total_candidatos == 0,
              "O trio podado nao deve ter seu suporte contado.");
    VERIFICAR(resultado.niveis[3].total == 0,
              "L3 deve ficar vazio depois da poda.");
    VERIFICAR(resultado.total_candidatos_infrequentes == 1,
              "A base didatica deve registrar {pao, manteiga} como infrequente.");
    VERIFICAR(resultado.total_candidatos_podados == 1,
              "A base didatica deve registrar o trio como candidato podado.");

    RegraAssociacao *pao_para_leite =
        buscar_regra(&resultado, "{pao}", "{leite}");

    VERIFICAR(pao_para_leite != NULL,
              "A regra {pao} -> {leite} deve passar pela confianca minima.");

    if (pao_para_leite != NULL) {
        VERIFICAR(diferenca_absoluta(pao_para_leite->confianca, 0.75f) < 0.001f,
                  "A confianca de {pao} -> {leite} deve ser 75%.");
        VERIFICAR(diferenca_absoluta(pao_para_leite->lift, 0.90f) < 0.001f,
                  "O lift de {pao} -> {leite} deve ser 0.90.");
    }
}

static void testar_parametros_personalizados(void) {
    static BaseCompras base;
    static ResultadoApriori resultado;

    montar_base_basica(&base);
    inicializar_resultado(&resultado);
    configurar_parametros_apriori(&resultado, 3, 0.60f);

    VERIFICAR(aplicar_apriori(&base, &resultado) == 1,
              "Apriori deve aceitar parametros personalizados validos.");
    VERIFICAR(resultado.niveis[1].total == 1,
              "Com suporte minimo 3, apenas {leite} deve permanecer em L1.");
    VERIFICAR(resultado.total_regras == 0,
              "Sem itemsets frequentes de tamanho 2, nenhuma regra deve ser gerada.");
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
    testar_exemplo_didatico();
    testar_parametros_personalizados();
    testar_base_invalida();

    if (falhas > 0) {
        printf("\n%d teste(s) falharam.\n", falhas);
        return 1;
    }

    printf("Todos os testes do Apriori passaram.\n");
    return 0;
}
