# Compilador e opcoes usadas para construir o projeto.
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
CPPFLAGS = -Ileitor -Iapriori -Isaida

# Nome do executavel e arquivos-fonte do programa.
TARGET = programa_apriori
SRCS = main.c leitor/leitor.c apriori/apriori.c apriori/metadados_apriori.c saida/saida.c

.PHONY: all run clean $(TARGET)

# Alvo padrao: compila o executavel.
all: $(TARGET)

$(TARGET): $(SRCS)
	@rm -f $(TARGET)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SRCS) -o $(TARGET)

# Executa o programa depois de compilar, se necessario.
run: $(TARGET)
	./$(TARGET)

# Remove arquivos gerados pela compilacao e pela execucao.
clean:
	rm -f $(TARGET) regras_associacao.txt
