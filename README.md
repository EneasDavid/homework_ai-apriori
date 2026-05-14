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
│   ├── metadados_apriori.c
│   └── apriori.h
└── saida/
    ├── saida.c
    └── saida.h
```

## O que cada arquivo deve fazer

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Deve apenas coordenar o fluxo: ler entrada, chamar Apriori e gerar saida. |
| `leitor/leitor.h` | Deve declarar a estrutura da base de compras e seus limites. |
| `leitor/leitor.c` | Deve ler o arquivo de compras, normalizar itens e preencher a base em memoria. |
| `apriori/apriori.h` | Deve declarar as estruturas, constantes e funcoes publicas do modulo Apriori. |
| `apriori/apriori.c` | Deve conter somente o algoritmo Apriori: suporte, join, prune, busca level-wise e itemsets encontrados. |
| `apriori/metadados_apriori.c` | Deve transformar os itemsets do Apriori em metadados prontos para a saida, como regras, confianca, relevancia e flag incerta. |
| `saida/saida.h` | Deve declarar a funcao publica que gera o relatorio. |
| `saida/saida.c` | Deve apenas organizar e escrever o relatorio usando os dados ja calculados. |
| `Makefile` | Deve compilar, executar e limpar os arquivos gerados do projeto. |
| `tests/test_apriori.c` | Deve validar automaticamente a operacao basica do Apriori. |

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

Resumo importante:

- `apriori.c` nao deve formatar texto do relatorio.
- `saida.c` nao deve calcular Apriori, confianca ou relevancia.
- `metadados_apriori.c` deve ser a ponte entre o algoritmo e a saida.

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
gcc main.c leitor/leitor.c apriori/apriori.c apriori/metadados_apriori.c saida/saida.c -Ileitor -Iapriori -Isaida -o programa_apriori
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
| `make test` | Compila e executa os testes automaticos do Apriori. |
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
| Suporte dos itemsets | `apriori/apriori.c` |
| Confianca e regras de associacao | `apriori/metadados_apriori.c` |
| Relevancia da regra na base inteira | `apriori/metadados_apriori.c` |
| Regras incertas com suporte igual a 1 | `apriori/apriori.c`, `apriori/metadados_apriori.c` e `saida/saida.c` |
| Parametros `MIN_SUP`, `MIN_CONF` e estruturas de regra | `apriori/apriori.h` |
| Escrita de arquivo com `fprintf` | `saida/saida.c` |
| Compilacao com `make` | `Makefile` |

---

## Erros comuns

### Arquivo de entrada nao encontrado

Verifique se `compras.txt` esta na raiz do projeto.

### Arquivo sem dados validos

O leitor rejeita arquivo vazio, arquivo sem transacoes validas e linhas formadas apenas por espacos ou virgulas.

### Dados fora dos limites

O programa valida:

- mais de `MAX_TRANSACOES` transacoes;
- mais de `MAX_ITENS` itens diferentes;
- linha maior que `MAX_LINHA`;
- nome de item maior que `MAX_NOME_ITEM`;
- regras acima de `MAX_REGRAS` ou `MAX_REGRAS_INCERTAS`.

Itens repetidos na mesma compra sao avisados e ignorados.

### Erro ao compilar por causa dos headers

Use `make`. Ele ja inclui os caminhos:

```bash
-Ileitor -Iapriori -Isaida
```

### Limite maximo atingido

Os limites da base ficam em `leitor/leitor.h`.
