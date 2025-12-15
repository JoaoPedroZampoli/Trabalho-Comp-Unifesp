# Trabalho-Comp-Unifesp


Steps que eu (Zampoli) estou usando para rodar o Flex e o Bison no Windows pelo PowerShell:
- $env:BISON_PKGDATADIR="C:\FLEXWI~1\Bison\share\bison"
- bison -d -o seman.tab.c seman.y
- cmd /c '"C:\FLEXWI~1\Lex\bin\flex.exe" -t scanner.l > lex.yy.c'
- gcc -o seman.exe seman.tab.c lex.yy.c
- ./seman.exe teste.c


1️⃣ Gerar o parser (AST + semântica local):
$env:BISON_PKGDATADIR="C:\FLEXWI~1\Bison\share\bison"
bison -d -o parser.tab.c parser.y

2️⃣ Gerar o scanner:
cmd /c '"C:\FLEXWI~1\Lex\bin\flex.exe" -t scanner.l > lex.yy.c'

3️⃣ Compilar TUDO junto:
gcc -Wall -g parser.tab.c lex.yy.c semantic.c -o compilador.exe

4️⃣ Rodar o compilador:
./compilador.exe teste.c