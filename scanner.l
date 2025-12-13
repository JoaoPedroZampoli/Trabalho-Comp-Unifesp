%{
#include "globals.h"
#include "parser.tab.h"

int lineno = 1;
char tokenString[MAXTOKENLEN+1];
%}

/* Declarando as palavras reservadas e tokens*/
digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"else"          { return ELSE; }
"if"            { return IF; }
"int"           { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }

"+"             { return MAIS; }
"-"             { return MENOS; }
"*"             { return VEZES; }
"/"             { return DIVIDIDO; }

"<"             { return LESSTHAN; }
"<="            { return LESSEQUAL; }
">"             { return GREATERTHAN; }
">="            { return GREATEREQUAL; }
"=="            { return EQUAL; }
"!="            { return NOTEQUAL; }
"="             { return ASSIGN; }

";"             { return SEMICOLON; }
","             { return COMMA; }
"("             { return LEFTPAREN; }
")"             { return RIGHTPAREN; }
"["             { return LEFTBRACKET; }
"]"             { return RIGHTBRACKET; }
"{"             { return LEFTBRACE; }
"}"             { return RIGHTBRACE; }

{number}        {
                  yylval.val = atoi(yytext);
                  return NUM;
                }

{identifier}    {
                  yylval.name = strdup(yytext);
                  return ID;
                }

{newline}       { lineno++; }
{whitespace}    { /* pular espaço em branco */ }

"/*"            {
                  char c;
                  int comment_line = lineno;
                  do {
                    c = input();
                    if (c == EOF) {
                      fprintf(stderr, "ERRO LEXICO: comentario nao fechado - LINHA: %d\n", comment_line);
                      return ERROR;
                    }
                    if (c == '\n') lineno++;
                  } while (c != '*' || (c = input()) != '/');
                }

.               {
                  fprintf(stderr, "ERRO LEXICO: '%s' - LINHA: %d\n", yytext, lineno);
                  return ERROR;
                }

%%

int yywrap(void) {
    return 1;
}