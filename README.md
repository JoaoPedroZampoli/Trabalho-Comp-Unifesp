# Trabalho-Comp-Unifesp


$env:BISON_PKGDATADIR="C:\FLEXWI~1\Bison\share\bison"
bison -d -o seman.tab.c seman.y
cmd /c '"C:\FLEXWI~1\Lex\bin\flex.exe" -t scanner.l > lex.yy.c'
gcc -o seman.exe seman.tab.c lex.yy.c
./seman.exe teste.c