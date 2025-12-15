# Trabalho-Comp-Unifesp


# Trabalho-Comp-Unifesp
Compilador simples implementado em C utilizando Flex e Bison, como parte do trabalho da disciplina de Compiladores da Unifesp.

## Requisitos
- Flex
- Bison
- GCC
- Make

## Estrutura do Projeto
- `parser.y`: Arquivo de definição da gramática utilizando Bison.
- `scanner.l`: Arquivo de definição do analisador léxico utilizando Flex.
- `semantic.h` e `semantic.c`: Implementação da análise semântica.
- `symbol_table.c` e `symbol_table.h`: Implementação da tabela de símbolos.

## Como Compilar e Executar
1. Abra o PowerShell.
2. Navegue até o diretório do projeto.
3. Execute os seguintes comandos:

- $env:BISON_PKGDATADIR="C:\FLEXWI~1\Bison\share\bison"
- bison -d parser.y
- flex scanner.l
- gcc -Wall -g parser.tab.c lex.yy.c semantic.c symbol_table.c -o compilador.exe
- ./compilador.exe teste.c

### Para Visualizar a AST (GraphViz)
- dot -Tpng arvore.dot -o arvore.png
- dot -Tsvg arvore.dot -o arvore.svg  (alternativa)

## Possível erro:
Trocar "%define parse.error verbose" por "%error-verbose" no arquivo parser.y para Bison 2.x