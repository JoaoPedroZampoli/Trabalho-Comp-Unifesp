%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    extern int yylex(void);
    extern int yyparse(void);
    extern FILE *yyin;

    void yyerror(const char *s);
%}

%code requires {

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
}

%union {
    int num;
    char *id;
    Tipo tipo;
}

%token IF ELSE WHILE RETURN
%token INT VOID

%token MAIS MENOS VEZES DIVIDIDO
%token LESSTHAN LESSEQUAL GREATERTHAN GREATEREQUAL EQUAL NOTEQUAL
%token ASSIGN
%token SEMICOLON COMMA
%token LEFTPAREN RIGHTPAREN
%token LEFTBRACE RIGHTBRACE
%token LEFTBRACKET RIGHTBRACKET

%token <id> ID
%token <num> NUM

%type <tipo> tipo_especificador

%start programa

%%

/* =========================================================
   Gramática e Ações Semânticas
   ========================================================= */

programa
    : declaracao_lista {imprime_tabela();} /* Imprime tabela ao final */
    ;

declaracao_lista
    : declaracao_lista declaracao
    | declaracao
    ;

declaracao
    : var_declaracao
    | fun_declaracao
    ;

/* Variáveis */
var_declaracao
    : tipo_especificador ID SEMICOLON
        {
            if ($1 == TIPO_VOID) {
               fprintf(stderr, "Variável '%s' não pode ser do tipo void.\n", $2);
               exit(1);
            }
            insere_simbolo($2, $1, SIMP_VAR, 0);
        }
    | tipo_especificador ID LEFTBRACKET NUM RIGHTBRACKET SEMICOLON
        {
            if ($1 == TIPO_VOID) {
               fprintf(stderr, "Array '%s' não pode ser do tipo void.\n", $2);
               exit(1);
            }
            insere_simbolo($2, $1, SIMP_ARRAY, $4);
        }
    ;

tipo_especificador
    : INT    { $$ = TIPO_INT; }
    | VOID   { $$ = TIPO_VOID; }
    ;

/* Funções */
fun_declaracao
    : tipo_especificador ID {
          insere_simbolo($2, $1, SIMP_FUNC, 0);
          entra_escopo(); /* escopo da função */
      }
      LEFTPAREN params RIGHTPAREN
      composto_decl
      {
          sai_escopo();
      }
    ;

params
    : param_lista
    | VOID
        {
            /* função sem parâmetros */
        }
    ;

param_lista
    : param_lista COMMA param
    | param
    ;

param
    : tipo_especificador ID
        {
            if ($1 == TIPO_VOID) {
               fprintf(stderr, "Parâmetro '%s' não pode ser do tipo void.\n", $2);
               exit(1);
            }
            insere_simbolo($2, $1, SIMP_PARAM, 0);
        }
    | tipo_especificador ID LEFTBRACKET RIGHTBRACKET
        {
            if ($1 == TIPO_VOID) {
               fprintf(stderr, "Parâmetro array '%s' não pode ser do tipo void.\n", $2);
               exit(1);
            }
            insere_simbolo($2, $1, SIMP_PARAM, 0); /* tamanho 0 para parâmetro array */
        }
    ;

/* Blocos */
composto_decl
    : LEFTBRACE {
        entra_escopo();
    }
    local_declaracoes statement_lista RIGHTBRACE
    {
        sai_escopo();
    }
    ;

local_declaracoes
    : local_declaracoes var_declaracao
    | /* vazio */
    ;

statement_lista
    : statement_lista statement
    | /* vazio */
    ;

statement
    : expressao_decl
    | composto_decl
    | selection_decl
    | iteration_decl
    | return_decl
    ;

/* Statements/Declarações */
expressao_decl
    : expressao SEMICOLON
    | SEMICOLON
    ;

selection_decl
    : IF LEFTPAREN expressao RIGHTPAREN statement
    | IF LEFTPAREN expressao RIGHTPAREN statement ELSE statement
    ;

iteration_decl
    : WHILE LEFTPAREN expressao RIGHTPAREN statement
    ;

return_decl
    : RETURN SEMICOLON
    | RETURN expressao SEMICOLON
    ;

/* Expressões */
expressao
    : var ASSIGN expressao
    | simples_expressao
    ;   

var
    : ID
        {
            if (busca_simbolo($1) == NULL) {
                fprintf(stderr, "Erro: variável '%s' não declarada.\n", $1);
                exit(1);
            }
        }
    | ID LEFTBRACKET expressao RIGHTBRACKET
        {
            Simbolo *sim = busca_simbolo($1);
            if (sim == NULL) {
                fprintf(stderr, "Erro: array '%s' não declarado.\n", $1);
                exit(1);
            }
            if (sim->classe != SIMP_ARRAY) {
                fprintf(stderr, "Erro: '%s' não é um array.\n", $1);
                exit(1);
            }
        }
    ;

simples_expressao
    : soma_expressao relacional soma_expressao
    | soma_expressao
    ;

relacional
    : LESSTHAN
    | LESSEQUAL
    | GREATERTHAN
    | GREATEREQUAL
    | EQUAL
    | NOTEQUAL
    ;

soma_expressao
    : soma_expressao soma termo
    | termo
    ;

soma
    : MAIS
    | MENOS /* Fazendo isso para facilitar futuras implementações */
    ;

termo
    : termo mult fator
    | fator
    ;

mult
    : VEZES
    | DIVIDIDO
    ;

fator
    : LEFTPAREN expressao RIGHTPAREN
    | var
    | chamada
    | NUM
    ;

chamada
    : ID LEFTPAREN args RIGHTPAREN
        {
            Simbolo *sim = busca_simbolo($1);
            if (sim == NULL) {
                fprintf(stderr, "Erro: função '%s' não declarada.\n", $1);
                exit(1);
            }
            if (sim->classe != SIMP_FUNC) {
                fprintf(stderr, "Erro: '%s' não é uma função.\n", $1);
                exit(1);
            }
        }
    ;

args
    : arg_lista
    | /* vazio */
    ;

arg_lista
    : arg_lista COMMA expressao
    | expressao
    ;

%%