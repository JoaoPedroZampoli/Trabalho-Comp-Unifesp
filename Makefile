# Makefile para o Compilador C-
# Disciplina: Compiladores - Unifesp

# Configuração do Bison (ajuste o caminho conforme necessário)
BISON_PKGDATADIR ?= C:\FLEXWI~1\Bison\share\bison
export BISON_PKGDATADIR

CC = gcc
CFLAGS = -Wall -g
FLEX = flex
BISON = bison

# Arquivos fonte
PARSER = parser.y
SCANNER = scanner.l
SOURCES = semantic.c symbol_table.c

# Arquivos gerados
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h
SCANNER_C = lex.yy.c

# Executável
TARGET = compilador.exe

# Regra padrão
all: $(TARGET)

# Gerar o executável
$(TARGET): $(PARSER_C) $(SCANNER_C) $(SOURCES)
	$(CC) $(CFLAGS) $(PARSER_C) $(SCANNER_C) $(SOURCES) -o $(TARGET)

# Gerar parser com Bison
$(PARSER_C) $(PARSER_H): $(PARSER)
	$(BISON) -d $(PARSER)

# Gerar scanner com Flex
$(SCANNER_C): $(SCANNER) $(PARSER_H)
	C:/FLEXWI~1/Lex/bin/flex.exe -t $(SCANNER) > $(SCANNER_C)

# Executar com arquivo de teste
test: $(TARGET)
	./$(TARGET) Testes/exemplo02Louden.c

# Gerar imagem da AST (requer Graphviz)
ast: arvore.dot
	dot -Tpng arvore.dot -o arvore.png
	@echo "Imagem gerada: arvore.png"

# Limpar arquivos gerados
clean:
	rm -f $(TARGET) $(PARSER_C) $(PARSER_H) $(SCANNER_C) arvore.dot arvore.png

# Limpar tudo exceto o executável
clean-intermediate:
	rm -f $(PARSER_C) $(PARSER_H) $(SCANNER_C)

# Ajuda
help:
	@echo "Comandos disponiveis:"
	@echo "  make          - Compila o compilador"
	@echo "  make test     - Executa com Testes/ok.c"
	@echo "  make ast      - Gera imagem PNG da AST"
	@echo "  make clean    - Remove arquivos gerados"
	@echo "  make help     - Mostra esta ajuda"

.PHONY: all test ast clean clean-intermediate help