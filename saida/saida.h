#ifndef SAIDA_H
#define SAIDA_H

#include "leitor.h"
#include "apriori.h"

int gerar_arquivo_saida(
    const char *nome_arquivo_saida,
    const char *nome_arquivo_entrada,
    BaseCompras *base,
    ResultadoApriori *resultado
);

#endif