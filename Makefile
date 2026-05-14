# Compilador e opcoes usadas para construir o projeto.
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
CPPFLAGS = -Ileitor -Iapriori -Isaida

# Nome do executavel e arquivos-fonte do programa.
TARGET = programa_apriori
TEST_TARGET = tests/test_apriori
SRCS = main.c leitor/leitor.c apriori/apriori.c apriori/metadados_apriori.c saida/saida.c
TEST_SRCS = tests/test_apriori.c leitor/leitor.c apriori/apriori.c apriori/metadados_apriori.c

.PHONY: all run test clean $(TARGET) $(TEST_TARGET)

# Alvo padrao: compila o executavel.
all: $(TARGET)

$(TARGET): $(SRCS)
	@rm -f $(TARGET)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SRCS) -o $(TARGET)

$(TEST_TARGET): $(TEST_SRCS)
	@rm -f $(TEST_TARGET)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TEST_SRCS) -o $(TEST_TARGET)

# Executa o programa depois de compilar, se necessario.
run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Remove arquivos gerados pela compilacao e pela execucao.
clean:
	rm -f $(TARGET) $(TEST_TARGET) regras_associacao.txt
