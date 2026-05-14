# Projeto Apriori em C

Projeto didatico em C que le uma base de compras, aplica o algoritmo Apriori com busca level-wise e gera um relatorio com itemsets frequentes e regras de associacao.

---
## Como compilar

Na raiz do projeto, execute:

```bash
make
```

Isso gera o executavel:

```txt
programa_apriori
```

## Como executar

Execute:

```bash
make run
```

Quando o programa pedir o arquivo de entrada, digite:

```txt
compras.txt
```

O relatorio sera gerado em:

```txt
regras_associacao.txt
```

## Como limpar arquivos gerados

Execute:

```bash
make clean
```

Esse comando remove:

- `programa_apriori`
- `regras_associacao.txt`


---

## Arquitetura do projeto

```txt
APRIORI/
├── main.c
├── Makefile
├── compras.txt
├── regras_associacao.txt
├── leitor/
│   ├── leitor.c
│   └── leitor.h
├── apriori/
│   ├── apriori.c
│   └── apriori.h
└── saida/
    ├── saida.c
    └── saida.h
```

## Responsabilidade de cada parte

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Coordena o fluxo do programa. |
| `leitor/leitor.h` | Define a estrutura `BaseCompras` e os limites da base. |
| `leitor/leitor.c` | Le o arquivo de compras e transforma os dados em memoria. |
| `apriori/apriori.h` | Define as estruturas e parametros das regras de associacao. |
| `apriori/apriori.c` | Executa o Apriori level-wise, gera candidatos Ck, aplica join/prune, calcula suporte, confianca, regras validas e regras incertas. |
| `saida/saida.h` | Declara a funcao de geracao do relatorio. |
| `saida/saida.c` | Escreve o arquivo `regras_associacao.txt`. |
| `Makefile` | Automatiza compilacao, execucao e limpeza do projeto. |

## Fluxo da aplicacao

```txt
compras.txt
   ↓
leitor
   ↓
BaseCompras em memoria
   ↓
apriori
   ↓
ResultadoApriori
   ↓
saida
   ↓
regras_associacao.txt
```

O `main.c` apenas conecta essas etapas. A regra do projeto e manter cada responsabilidade no seu proprio modulo.

---

## Arquivo de entrada

O arquivo de entrada deve ter uma compra por linha.

Cada item da compra deve ser separado por virgula:

```txt
leite,pao,manteiga
arroz,feijao,oleo,sal
cafe,acucar,leite
```

Recomendacoes:

- use nomes sem acento;
- use `_` em nomes compostos, como `molho_de_tomate`;
- mantenha o arquivo na raiz do projeto;
- use `compras.txt` quando o programa pedir o nome do arquivo.

---

## Como o Makefile funciona

O `Makefile` guarda o comando de compilacao do projeto.

Assim, em vez de digitar:

```bash
gcc main.c leitor/leitor.c apriori/apriori.c saida/saida.c -Ileitor -Iapriori -Isaida -o programa_apriori
```

voce usa apenas:

```bash
make
```

Principais alvos:

| Comando | O que faz |
|---|---|
| `make` | Compila o projeto. |
| `make run` | Compila, se necessario, e executa o programa. |
| `make clean` | Remove os arquivos gerados. |

---

## Saida esperada no terminal

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

## Onde estudar cada parte

| Tema | Onde olhar |
|---|---|
| Organizacao do fluxo | `main.c` |
| Leitura de arquivo, `strtok`, normalizacao de texto | `leitor/leitor.c` |
| Matriz de transacoes e limites da base | `leitor/leitor.h` |
| Busca level-wise L1, L2, ..., Lk | `apriori/apriori.c` |
| Join, prune e `has_infrequent_subset` | `apriori/apriori.c` |
| Suporte, confianca e regras de associacao | `apriori/apriori.c` |
| Regras incertas com suporte igual a 1 | `apriori/apriori.c` e `saida/saida.c` |
| Parametros `MIN_SUP`, `MIN_CONF` e estruturas de regra | `apriori/apriori.h` |
| Escrita de arquivo com `fprintf` | `saida/saida.c` |
| Compilacao com `make` | `Makefile` |

---

## Erros comuns

### Arquivo de entrada nao encontrado

Verifique se `compras.txt` esta na raiz do projeto.

### Erro ao compilar por causa dos headers

Use `make`. Ele ja inclui os caminhos:

```bash
-Ileitor -Iapriori -Isaida
```

### Limite maximo atingido

Os limites da base ficam em `leitor/leitor.h`.
