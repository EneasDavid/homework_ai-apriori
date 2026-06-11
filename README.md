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
exemplo_aula.txt
```

O relatorio em Markdown sera gerado em:

```text
regras_associacao.md
```

## Como limpar arquivos gerados

Execute:

```bash
make clean
```

Esse comando remove:

- `programa_apriori`
- `regras_associacao.md`


---

## Arquitetura do projeto

```txt
APRIORI/
├── main.c
├── Makefile
├── exemplo_aula.txt
├── test_estresse.txt
├── regras_associacao.md
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
| `apriori/metadados_apriori.c` | Deve transformar os itemsets do Apriori em metadados prontos para a saida, como regras, confianca, lift e flag incerta. |
| `saida/saida.h` | Deve declarar a funcao publica que gera o relatorio. |
| `saida/saida.c` | Deve apenas organizar e escrever o relatorio usando os dados ja calculados. |
| `Makefile` | Deve compilar, executar e limpar os arquivos gerados do projeto. |
| `tests/test_apriori.c` | Deve validar automaticamente a operacao basica do Apriori. |

## Fluxo da aplicacao

```txt
exemplo_aula.txt
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
regras_associacao.md
```

O `main.c` apenas conecta essas etapas. A regra do projeto e manter cada responsabilidade no seu proprio modulo.

Resumo importante:

- `apriori.c` nao deve formatar texto do relatorio.
- `saida.c` nao deve calcular Apriori, confianca ou lift.
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
- use `exemplo_aula.txt` para uma demonstracao curta;
- use `test_estresse.txt` para uma base maior.

## Exemplo curto para aula

O arquivo `exemplo_aula.txt` possui apenas seis transacoes:

```txt
leite,pao
leite,pao
leite,pao
leite,manteiga
leite,manteiga
pao,manteiga
```

Ele foi montado para demonstrar:

- diferenca entre frequencia absoluta e suporte relativo;
- fluxo `C1 -> L1`, `C2 -> L2` e `C3 -> L3`;
- `{leite, pao}` frequente, com 3 ocorrencias;
- `{leite, manteiga}` frequente, com 2 ocorrencias;
- `{pao, manteiga}` infrequente, com apenas 1 ocorrencia;
- join que gera `{leite, pao, manteiga}` e poda o trio antes da contagem;
- itemset raro que aparece apenas uma vez.

Ao executar o programa, o usuario informa os dois parametros:

```txt
Suporte minimo em ocorrencias (exemplo: 2)
Confianca minima em percentual (exemplo: 70)
```

Os exemplos sugeridos sao suporte `2` e confianca `70%`. Em
`exemplo_aula.txt`, suporte minimo `2` equivale a `2/6 = 33.33%`. Como o
programa recebe suporte minimo absoluto, esse percentual muda quando a base muda.

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
| `make` | Compila o projeto e executa os testes automaticos. |
| `make run` | Compila, se necessario, e executa o programa. |
| `make test` | Compila e executa os testes automaticos do Apriori. |
| `make clean` | Remove os arquivos gerados. |

---

## Saida esperada no terminal

Exemplo:

```txt
============================================================
 APRIORI | DESCOBERTA DE REGRAS DE ASSOCIACAO
============================================================

Informe uma base de compras com uma transacao por linha.
Exemplo de linha: leite,pao,manteiga

Arquivo de entrada: exemplo_aula.txt

[1/3] Leitura da base
      Lendo "exemplo_aula.txt"...
      Base carregada com sucesso: 6 compras, 3 itens.

------------------------------------------------------------
 CONFIGURACAO DA ANALISE
------------------------------------------------------------
Base carregada: 6 compras e 3 itens diferentes.

1. Suporte minimo
   Quantidade minima de compras em que um conjunto de itens
   deve aparecer para ser considerado frequente.
   Informe um valor de 1 a 6: 2

2. Confianca minima
   Percentual minimo para aceitar uma regra de associacao,
   como {pao} -> {leite}.
   Informe um percentual de 0 a 100: 70

Resumo da configuracao:
- Suporte minimo: 2 ocorrencias (33.33% da base)
- Confianca minima: 70.00%

[2/3] Processamento
      Aplicando o algoritmo Apriori...
      Apriori concluido: 1 regra(s) encontrada(s).

[3/3] Relatorio
      Gerando arquivo Markdown...
      Arquivo gerado com sucesso: regras_associacao.md

Processamento finalizado.
```

---

## Onde estudar cada parte

| Tema | Onde olhar |
|---|---|
| Organizacao do fluxo | `main.c` |
| Leitura de arquivo e normalizacao de texto | `leitor/leitor.c` |
| Matriz de transacoes e limites da base | `leitor/leitor.h` |
| Busca level-wise L1, L2, ..., Lk | `apriori/apriori.c` |
| Join, prune e candidatos descartados | `apriori/apriori.c` |
| Suporte dos itemsets | `apriori/apriori.c` |
| Confianca, lift e regras de associacao | `apriori/metadados_apriori.c` |
| Regras incertas com suporte igual a 1 | `apriori/apriori.c`, `apriori/metadados_apriori.c` e `saida/saida.c` |
| Parametros de suporte e confianca e estruturas de regra | `apriori/apriori.h` |
| Escrita de arquivo com `fprintf` | `saida/saida.c` |
| Compilacao com `make` | `Makefile` |

---

## Complexidade

Considere:

- `T` = numero de transacoes;
- `I` = numero de itens diferentes;
- `R` = numero de regras geradas;
- `K` = maior tamanho de itemset frequente encontrado.

Como o Apriori faz uma busca level-wise, no pior caso ele pode avaliar candidatos de tamanho 1 ate `K`.
Para um tamanho fixo `k`, a contagem de suporte pode custar aproximadamente:

```txt
O(C(I, k) * T * k)
```

Ignorando o fator `k` na leitura assintotica simplificada, a soma ate `K` fica:

```txt
O(T * (I + I^2 + ... + I^K))
```

Como o termo dominante e `I^K`, a forma simplificada e:

```txt
O(T * I^K)
```

Se a analise for limitada didaticamente ate itemsets de tamanho 3:

```txt
Suporte de 1 item: O(I * T)
Suporte de 2 itens: O(I^2 * T)
Suporte de 3 itens: O(I^3 * T)
```

Logo:

```txt
O(T * (I + I^2 + I^3)) = O(T * I^3)
```

Complexidade espacial aproximada:

```txt
Matriz de transacoes: O(T * I)
Lista de itens: O(I)
Lista de regras: O(R)
```

Logo:

```txt
O(T * I + R)
```

Para a base maior de `test_estresse.txt`:

```txt
T = 700
I = 35
Pares possiveis: C(35, 2) = 595
Trios possiveis: C(35, 3) = 6545
```

---

## Erros comuns

### Arquivo de entrada nao encontrado

Verifique se `exemplo_aula.txt` ou o arquivo escolhido esta na raiz do projeto.

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
