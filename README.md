# Trabalho-Comp-Unifesp


# Trabalho-Comp-Unifesp
Compilador para a linguagem C- (citada no Livro "Compiladores - Princípios e Práticas" de Kenneth C. Louden), implementado em C utilizando Flex e Bison
**Disciplina:** Compiladores - Unifesp (Universidade Federal de São Paulo).

## Requisitos
- **GCC** (MinGW no Windows)
- **Flex** (gerador de scanner)
- **Bison** (gerador de parser)
- **Graphviz** (opcional, para visualizar AST)

## Estrutura do Projeto
- `parser.y`: Arquivo de definição da gramática utilizando Bison.
- `scanner.l`: Arquivo de definição do analisador léxico utilizando Flex.
- `semantic.h` e `semantic.c`: Implementação da análise semântica.
- `symbol_table.c` e `symbol_table.h`: Implementação da tabela de símbolos.

## Como Compilar e Executar

### Usando Makefile (recomendado)

```bash
make              # Compila o compilador
make test         # Executa com arquivo de teste
make ast          # Gera imagem da AST (requer Graphviz)
make clean        # Remove arquivos gerados
```

Observação:  Pode ser necessário alterar o caminho de instalação do Bison no MakeFile também

### Manualmente (Windows PowerShell)
1. Abra o PowerShell.
2. Navegue até o diretório do projeto.
3. Execute os seguintes comandos:

- $env:BISON_PKGDATADIR="C:\FLEXWI~1\Bison\share\bison" ou onde o bison estiver instalado em sua máquina
- bison -d parser.y
- flex scanner.l
- gcc -Wall -g parser.tab.c lex.yy.c semantic.c symbol_table.c -o compilador.exe
- ./compilador.exe teste.c

### Para Visualizar a AST (GraphViz)
- dot -Tpng arvore.dot -o arvore.png
- dot -Tsvg arvore.dot -o arvore.svg  (alternativa)

## Possíveis Problemas

- %define parse.error verbose não funciona:	Substituir por %error-verbose
- dot não reconhecido:	Instalar Graphviz ```winget install Graphviz.Graphviz```

## Exemplos de uso:
- `teste.c`: Arquivo de exemplo para testar o compilador.

## Referência
Baseado no projeto de compilador sugerido pelo Apêndice A do livro:

LOUDEN, Kenneth C. Compiladores: Princípios e Práticas. Thomson, 2004.
