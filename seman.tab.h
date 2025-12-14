
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */

/* Line 1676 of yacc.c  */
#line 14 "seman.y"


static Escopo *escopo_atual = NULL;
static Tipo tipo_funcao_atual = -1; /* fora de função */
static int funcao_tem_retorno;
static int ultima_funcao_eh_main = 0;
static int main_tem_parametros = 0;

typedef enum {
    TIPO_INT,
    TIPO_VOID
} Tipo;

typedef enum {
    SIMP_VAR,
    SIMP_ARRAY,
    SIMP_FUNC,
    SIMP_PARAM
} Classe;

/* entrada da tabela de símbolos */
typedef struct Simbolo {
    char *nome;
    Tipo tipo;
    Classe classe;
    int lineno; 
    int tamanho;              /* array: NUM | 0 caso contrário */
    struct Simbolo *prox;
} Simbolo;

/* escopo (pilha) */
typedef struct Escopo {
    Simbolo *simbolos;
    struct Escopo *pai;
} Escopo;

/* interface da tabela de símbolos */
void entra_escopo(void);
void sai_escopo(void);

void insere_simbolo(char *nome, Tipo tipo, Classe classe, int tamanho);

Simbolo *busca_simbolo(char *nome);
void imprime_tabela(void);



/* Line 1676 of yacc.c  */
#line 88 "seman.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     ELSE = 259,
     WHILE = 260,
     RETURN = 261,
     INT = 262,
     VOID = 263,
     PLUS = 264,
     MINUS = 265,
     TIMES = 266,
     DIV = 267,
     LESSTHAN = 268,
     LESSEQUAL = 269,
     GREATERTHAN = 270,
     GREATEREQUAL = 271,
     EQUAL = 272,
     NOTEQUAL = 273,
     ASSIGN = 274,
     SEMICOLON = 275,
     COMMA = 276,
     LEFTPAREN = 277,
     RIGHTPAREN = 278,
     LEFTBRACE = 279,
     RIGHTBRACE = 280,
     LEFTBRACKET = 281,
     RIGHTBRACKET = 282,
     ERROR = 283,
     ID = 284,
     NUM = 285
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 60 "seman.y"

    int num;
    char *id;
    Tipo tipo;



/* Line 1676 of yacc.c  */
#line 143 "seman.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


