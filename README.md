# Projeto Apriori em C

Este projeto implementa uma versão modular em C para leitura de uma base de compras, processamento das regras de associação e geração de um relatório final em arquivo `.txt`.

---

## Estrutura do projeto

```txt
APRIORI/
├── apriori/
│   ├── apriori.c
│   └── apriori.h
│
├── leitor/
│   ├── leitor.c
│   └── leitor.h
│
├── saida/
│   ├── saida.c
│   └── saida.h
│
├── compras.txt
├── main.c
└── README.md
```

---
## Comando final recomendado

Compile:

```bash
gcc main.c leitor/leitor.c apriori/apriori.c saida/saida.c -Ileitor -Iapriori -Isaida -o programa_apriori
```

Execute:

```bash
./programa_apriori
```

Digite:

```txt
compras.txt
```

Arquivo gerado:

```txt
regras_associacao.txt
```

----

## Organização geral

O projeto foi separado em módulos para deixar o código mais organizado e fácil de entender.

Cada módulo possui dois arquivos principais:

```txt
arquivo.c
arquivo.h
```

O arquivo `.c` contém a implementação das funções.

O arquivo `.h` contém as declarações das funções, constantes e estruturas que outros arquivos podem usar.

Essa separação permite que o projeto fique dividido por responsabilidade.

---

## O que são arquivos `.h`

Arquivos `.h`, também chamados de **headers**, funcionam como uma espécie de contrato entre os módulos do programa.

Eles informam para os outros arquivos quais funções, estruturas e constantes existem naquele módulo.

Por exemplo, se o `main.c` precisa usar uma função do módulo `leitor`, ele inclui o arquivo:

```c
#include "leitor/leitor.h"
```

Assim, o `main.c` passa a conhecer as funções declaradas em `leitor.h`.

Exemplo simples:

```c
void inicializar_base(BaseCompras *base);
int ler_arquivo_compras(const char *nome_arquivo, BaseCompras *base);
```

Essas linhas dizem que existem funções chamadas `inicializar_base` e `ler_arquivo_compras`.

A implementação real dessas funções fica no arquivo `.c`.

---

## Diferença entre `.c` e `.h`

### Arquivo `.h`

Contém declarações.

Exemplo:

```c
int ler_arquivo_compras(const char *nome_arquivo, BaseCompras *base);
```

Ele diz que a função existe.

### Arquivo `.c`

Contém a implementação.

Exemplo:

```c
int ler_arquivo_compras(const char *nome_arquivo, BaseCompras *base) {
    FILE *arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo.\n");
        return 0;
    }

    return 1;
}
```

Ele diz como a função funciona.

---

## Descrição dos módulos

## `main.c`

É o arquivo principal do programa.

Ele é responsável por conectar todos os módulos.

Fluxo executado pelo `main.c`:

```txt
1. Solicita o nome do arquivo de compras
2. Inicializa as estruturas principais
3. Chama o leitor de arquivo
4. Chama a lógica do Apriori
5. Chama o gerador de relatório
6. Finaliza o programa
```

O `main.c` não deve concentrar toda a lógica do projeto. Ele apenas organiza a execução.

---

## `leitor/leitor.h`

Esse arquivo declara as estruturas e funções usadas para leitura da base de compras.

Ele contém constantes como:

```c
#define MAX_TRANSACOES 10000
#define MAX_ITENS 100
#define MAX_NOME_ITEM 50
#define MAX_LINHA 500
```

Essas constantes controlam os limites do programa.

Exemplo:

```c
#define MAX_TRANSACOES 10000
```

Define que o programa pode ler até 10.000 compras.

---

## `leitor/leitor.c`

Esse arquivo implementa a leitura do arquivo de entrada.

Ele faz tarefas como:

```txt
- abrir o arquivo informado pelo usuário;
- ler cada linha;
- separar os itens por vírgula;
- remover espaços extras;
- converter os nomes dos itens para letras minúsculas;
- armazenar as compras na estrutura BaseCompras.
```

Exemplo de entrada:

```txt
Leite,Pao,Manteiga
leite,pao
PAO,Cafe
```

Após o tratamento, os itens são considerados em minúsculo:

```txt
leite
pao
manteiga
cafe
```

Isso evita diferença entre `Leite`, `leite` e `LEITE`.

---

## `apriori/apriori.h`

Esse arquivo declara as estruturas e funções usadas no processamento das regras.

Ele contém configurações importantes como:

```c
#define MIN_SUP 2
#define MIN_CONF 0.70
```

Essas constantes controlam os critérios mínimos usados pelo programa.

Também ficam nesse arquivo as estruturas ligadas às regras, como:

```c
typedef struct {
    char antecedente[100];
    char consequente[100];
    int suporte_antecedente;
    int suporte_conjunto;
    float confianca;
} RegraAssociacao;
```

Essa estrutura representa uma regra encontrada pelo programa.

---

## `apriori/apriori.c`

Esse arquivo contém a lógica principal de processamento.

Ele é responsável por:

```txt
- calcular suporte de itemsets de tamanho 1;
- calcular suporte de itemsets de tamanho 2;
- calcular suporte de itemsets de tamanho 3;
- gerar regras de associação;
- calcular a confiança das regras;
- armazenar o resultado em uma estrutura própria.
```

Esse módulo não lê arquivo e não gera relatório. Ele apenas processa os dados que já foram carregados pelo módulo `leitor`.

---

## `saida/saida.h`

Esse arquivo declara a função responsável por gerar o relatório final.

Exemplo:

```c
int gerar_arquivo_saida(
    const char *nome_arquivo_saida,
    const char *nome_arquivo_entrada,
    BaseCompras *base,
    ResultadoApriori *resultado
);
```

Essa função é chamada pelo `main.c` depois que o processamento termina.

---

## `saida/saida.c`

Esse arquivo implementa a geração do relatório.

Ele cria o arquivo:

```txt
regras_associacao.txt
```

O relatório contém:

```txt
- informações gerais do processamento;
- conceitos importantes;
- regras destacadas pelo maior nível de confiança;
- amostra das compras lidas;
- itens encontrados;
- itemsets frequentes;
- regras de associação completas;
- resumo final.
```

A última alteração feita neste módulo foi a criação de uma seção de destaque no começo do relatório.

Essa seção mostra somente as regras do maior nível de confiança encontrado.

A ordem usada é:

```txt
1. Regras muito fortes: confiança >= 90%
2. Regras fortes: confiança entre 70% e 89.99%
3. Regras moderadas ou fracas: confiança abaixo de 70%
```

Se existirem regras muito fortes, somente elas aparecem nessa seção inicial.

Os cálculos completos continuam aparecendo mais abaixo, na seção de regras completas.

---

## `compras.txt`

É o arquivo de entrada do programa.

Cada linha representa uma compra.

Cada item da compra deve ser separado por vírgula.

Exemplo:

```txt
leite,pao,manteiga
arroz,feijao,oleo,sal
cafe,acucar,leite
macarrao,molho_de_tomate,sardinha
```

---

## Formato correto do arquivo de entrada

O arquivo deve seguir este padrão:

```txt
item1,item2,item3
item1,item4
item2,item5,item6
```

Exemplo válido:

```txt
leite,pao,manteiga
arroz,feijao,oleo
cafe,acucar,leite
```

Cada quebra de linha representa uma nova compra.

Cada vírgula separa os itens da mesma compra.

---

## Cuidados com o arquivo de entrada

Use nomes sem acento.

Recomendado:

```txt
pao
feijao
acucar
oleo
```

Evite:

```txt
pão
feijão
açúcar
óleo
```

O programa converte letras maiúsculas para minúsculas, mas não remove acentos automaticamente.

Então:

```txt
pao
pão
```

podem ser tratados como itens diferentes.

---

## Nomes compostos

Para nomes compostos, recomenda-se usar `_`.

Exemplo:

```txt
molho_de_tomate
farinha_de_trigo
leite_em_po
carne_seca
```

Evite usar espaços dentro dos nomes dos itens.

---

## Como compilar

Na raiz do projeto, execute:

```bash
gcc main.c leitor/leitor.c apriori/apriori.c saida/saida.c -Ileitor -Iapriori -Isaida -o programa_apriori
```

Esse comando compila todos os arquivos `.c` do projeto.

---

## Explicação do comando de compilação

```bash
gcc
```

Chama o compilador C.

```bash
main.c
```

Compila o arquivo principal.

```bash
leitor/leitor.c
```

Compila o módulo de leitura.

```bash
apriori/apriori.c
```

Compila o módulo de processamento.

```bash
saida/saida.c
```

Compila o módulo de saída.

```bash
-Ileitor -Iapriori -Isaida
```

Informa ao compilador onde procurar arquivos `.h`.

```bash
-o programa_apriori
```

Define o nome do executável gerado.

---

## Como executar no Linux ou macOS

Após compilar, execute:

```bash
./programa_apriori
```

O programa pedirá o nome do arquivo de compras:

```txt
Digite o nome do arquivo de compras:
```

Digite:

```txt
compras.txt
```

---

## Como executar no Windows

Compile com GCC, MinGW ou outro compilador C compatível.

Depois execute:

```bash
programa_apriori.exe
```

Quando o programa pedir o arquivo de entrada, digite:

```txt
compras.txt
```

---

## Saída esperada no terminal

Exemplo:

```txt
========================================
 ALGORITMO APRIORI - VERSAO MODULAR
========================================

Digite o nome do arquivo de compras: compras.txt

[1/3] Lendo arquivo de compras...
Arquivo lido com sucesso.
Total de compras: 700
Total de itens diferentes: 35

[2/3] Aplicando algoritmo Apriori...
Apriori executado com sucesso.
Total de regras encontradas: 216

[3/3] Gerando arquivo de saida...
Arquivo de saida gerado com sucesso: regras_associacao.txt

Processamento finalizado.
```

---

## Arquivo de saída gerado

Após a execução, será criado o arquivo:

```txt
regras_associacao.txt
```

Esse arquivo contém o relatório completo gerado pelo programa.

---

## Fluxo do projeto

```txt
compras.txt
   ↓
leitor
   ↓
base de compras em memoria
   ↓
apriori
   ↓
regras de associacao
   ↓
saida
   ↓
regras_associacao.txt
```

---

## Alterações feitas no projeto

Durante a construção do projeto, foram feitas algumas melhorias importantes.

### 1. Separação em módulos

O código foi dividido em:

```txt
leitor
apriori
saida
main
```

Isso evita deixar toda a lógica dentro do `main.c`.

---

### 2. Leitura por arquivo

O programa passou a receber um arquivo `.txt` como entrada.

Cada linha representa uma compra.

Cada item é separado por vírgula.

---

### 3. Conversão para minúsculas

Os itens lidos do arquivo são convertidos para letras minúsculas.

Isso evita diferença entre:

```txt
Leite
leite
LEITE
```

---

### 4. Geração de arquivo de saída

O programa gera automaticamente o relatório:

```txt
regras_associacao.txt
```

---

### 5. Aumento do limite de transações

O limite de compras foi aumentado para:

```c
#define MAX_TRANSACOES 10000
```

Assim, o programa suporta até 10.000 linhas no arquivo de entrada.

---

### 6. Relatório mais didático

O relatório foi reorganizado para conter:

```txt
- cabeçalho;
- conceitos importantes;
- regras destacadas;
- amostra das compras;
- itens encontrados;
- itemsets frequentes;
- regras completas;
- resumo final.
```

---

### 7. Regras destacadas no início

Foi adicionada uma seção inicial para mostrar somente as regras do maior nível de confiança.

Exemplo:

```txt
REGRAS DESTACADAS PELO MAIOR NIVEL DE CONFIANCA
```

Essa seção não mostra cálculos detalhados.

Ela apenas lista as regras mais importantes.

Os cálculos completos aparecem depois, na seção:

```txt
REGRAS DE ASSOCIACAO GERADAS
```

---

## Erros comuns

### Erro: arquivo não encontrado

Mensagem possível:

```txt
Erro: nao foi possivel abrir o arquivo 'compras.txt'.
```

Verifique se o arquivo `compras.txt` está na raiz do projeto.

Use:

```bash
ls
```

A saída deve mostrar algo como:

```txt
compras.txt
main.c
leitor
apriori
saida
```

---

### Erro: limite máximo de transações atingido

Mensagem possível:

```txt
Erro: limite maximo de transacoes atingido.
```

Verifique o valor em:

```c
#define MAX_TRANSACOES 10000
```

Esse valor fica em:

```txt
leitor/leitor.h
```

---

### Erro ao compilar por causa dos `.h`

Se o compilador não encontrar arquivos `.h`, verifique se o comando de compilação possui:

```bash
-Ileitor -Iapriori -Isaida
```

Esses parâmetros indicam onde estão os headers do projeto.

---