%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    extern int yylex(void);
    extern int yyparse(void);
    extern FILE *yyin;
    extern int lineno;

    void yyerror(const char *s);
%}

%code requires {

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
}

%union {
    int num;
    char *id;
    Tipo tipo;
}

%token IF ELSE WHILE RETURN
%token INT VOID

%token PLUS MINUS TIMES DIV
%token LESSTHAN LESSEQUAL GREATERTHAN GREATEREQUAL EQUAL NOTEQUAL
%token ASSIGN
%token SEMICOLON COMMA
%token LEFTPAREN RIGHTPAREN
%token LEFTBRACE RIGHTBRACE
%token LEFTBRACKET RIGHTBRACKET
%token ERROR

%token <id> ID
%token <num> NUM

%type <tipo> tipo_especificador

%start programa

%%

/* =========================================================
   Gramática e Ações Semânticas
   ========================================================= */

programa
    : declaracao_lista
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
            //    fprintf(stderr, "Variável '%s' não pode ser do tipo void.\n", $2);
               fprintf(stderr, "ERRO SEMANTICO: variavel '%s' nao pode ser void - LINHA: %d\n", $2, lineno);
               exit(1);
            }
            insere_simbolo($2, $1, SIMP_VAR, 0);
        }
    | tipo_especificador ID LEFTBRACKET NUM RIGHTBRACKET SEMICOLON
        {
            if ($1 == TIPO_VOID) {
            //    fprintf(stderr, "Array '%s' não pode ser do tipo void.\n", $2);
               fprintf(stderr, "ERRO SEMANTICO: variavel '%s' nao pode ser void - LINHA: %d\n", $2, lineno);
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
    : tipo_especificador ID
      {
          insere_simbolo($2, $1, SIMP_FUNC, 0);

          tipo_funcao_atual = $1;
          funcao_tem_retorno = 0;
          main_tem_parametros = 0;

          ultima_funcao_eh_main =
              ($1 == TIPO_VOID && strcmp($2, "main") == 0);

          entra_escopo();   /* escopo da função */
      }
      LEFTPAREN params RIGHTPAREN
      composto_decl
      {
          if (tipo_funcao_atual == TIPO_INT && !funcao_tem_retorno) {
              fprintf(stderr,
                "ERRO SEMANTICO: funcao '%s' deve retornar valor - LINHA: %d\n",
                $2, lineno);
              exit(1);
          }

          sai_escopo();   /* fecha escopo da função */
          tipo_funcao_atual = -1;
      }
    ;



params
    : param_lista
        {
            if (ultima_funcao_eh_main)
                main_tem_parametros = 1;
        }
    | VOID
    ;

param_lista
    : param_lista COMMA param
    | param
    ;

param
    : tipo_especificador ID
        {
            if ($1 == TIPO_VOID) {
            //    fprintf(stderr, "Parâmetro '%s' não pode ser do tipo void.\n", $2);
               fprintf(stderr, "ERRO SEMANTICO: variavel '%s' nao pode ser void - LINHA: %d\n", $2, lineno);
               exit(1);
            }
            insere_simbolo($2, $1, SIMP_PARAM, 0);
        }
    | tipo_especificador ID LEFTBRACKET RIGHTBRACKET
        {
            if ($1 == TIPO_VOID) {
                fprintf(stderr, "ERRO SEMANTICO: variavel '%s' nao pode ser void - LINHA: %d\n", $2, lineno);
                // fprintf(stderr, "Parâmetro array '%s' não pode ser do tipo void.\n", $2);
                exit(1);
            }
            insere_simbolo($2, $1, SIMP_PARAM, 0); /* tamanho 0 para parâmetro array */
        }
    ;

/* Blocos */
composto_decl
    : LEFTBRACE
      {
          entra_escopo();   /* novo bloco */
      }
      local_declaracoes statement_lista RIGHTBRACE
      {
          sai_escopo();     /* fecha bloco */
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
      {
          /* return fora de função */
          if (tipo_funcao_atual == -1) {
              fprintf(stderr,
                "ERRO SEMANTICO: return fora de funcao - LINHA: %d\n",
                lineno);
              exit(1);
          }

          /* função int sem valor */
          if (tipo_funcao_atual == TIPO_INT) {
              fprintf(stderr,
                "ERRO SEMANTICO: funcao int deve retornar valor - LINHA: %d\n",
                lineno);
              exit(1);
          }
      }
    | RETURN expressao SEMICOLON
      {
          /* return fora de função */
          if (tipo_funcao_atual == -1) {
              fprintf(stderr,
                "ERRO SEMANTICO: return fora de funcao - LINHA: %d\n",
                lineno);
              exit(1);
          }

          /* função void retornando valor */
          if (tipo_funcao_atual == TIPO_VOID) {
              fprintf(stderr,
                "ERRO SEMANTICO: funcao void nao pode retornar valor - LINHA: %d\n",
                lineno);
              exit(1);
          }

          funcao_tem_retorno = 1;
      }
    ;

/* Expressões */
expressao
    : var ASSIGN expressao
    | simples_expressao
    ;   

var
    : ID
        {
            if(busca_simbolo($1) == NULL){
                fprintf(stderr, "ERRO SEMANTICO: variavel '%s' nao declarada - LINHA: %d\n", $1, lineno);
                exit(1);
            }
        }
    | ID LEFTBRACKET expressao RIGHTBRACKET
        {
            Simbolo *sim = busca_simbolo($1);
            if(sim == NULL){
                // fprintf(stderr, "Erro: array '%s' não declarado.\n", $1);
                fprintf(stderr, "ERRO SEMANTICO: array '%s' nao declarado - LINHA: %d\n", $1, lineno);
                exit(1);
            }
            if(sim->classe != SIMP_ARRAY){
                fprintf(stderr, "ERRO SEMANTICO: '%s' nao e um array - LINHA: %d\n", $1, lineno);
                // fprintf(stderr, "Erro: '%s' não é um array.\n", $1);
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
    : PLUS
    | MINUS /* Fazendo isso para facilitar futuras implementações */
    ;

termo
    : termo mult fator
    | fator
    ;

mult
    : TIMES
    | DIV
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
            if(sim == NULL){
                fprintf(stderr, "ERRO SEMANTICO: identificador '%s' - LINHA: %d\n", $1, lineno);
                // fprintf(stderr, "Erro: função '%s' não declarada.\n", $1);
                exit(1);
            }
            if(sim->classe != SIMP_FUNC){
                fprintf(stderr, "ERRO SEMANTICO: identificador '%s' - LINHA: %d\n", $1, lineno);
                // fprintf(stderr, "Erro: '%s' não é uma função.\n", $1);
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

/* Tabela de Símbolos */

void entra_escopo(void) {
    Escopo *novo = (Escopo*)malloc(sizeof(Escopo));
    novo->simbolos = NULL;
    novo->pai = escopo_atual;
    escopo_atual = novo;
}

void sai_escopo(void) {
    Escopo *antigo = escopo_atual;
    escopo_atual = escopo_atual->pai;

    /* liberando memória dos símbolos */
    Simbolo *sim = antigo->simbolos;
    while(sim != NULL) {
        Simbolo *temp = sim;
        sim = sim->prox;
        free(temp->nome);
        free(temp);
    }
    free(antigo);
}

void insere_simbolo(char *nome, Tipo tipo, Classe classe, int tamanho){
    for(Simbolo *sim = escopo_atual->simbolos; sim != NULL; sim = sim->prox){
        if(strcmp(sim->nome, nome) == 0){
            /* fprintf(stderr, "Erro: símbolo '%s' já declarado neste escopo.\n", nome); // Talvez tirar daqui e só deixar o YYERROR? */
            fprintf(stderr, "ERRO SEMANTICO: identificador '%s' ja declarado neste escopo - LINHA: %d\n", nome, lineno);
            yyerror("identificador duplicado no mesmo escopo");
            exit(1);
        }
    }
    Simbolo *novo = (Simbolo*)malloc(sizeof(Simbolo));
    novo->nome = strdup(nome);
    novo->tipo = tipo;
    novo->classe = classe;
    novo->lineno = lineno;
    novo->tamanho = tamanho;
    novo->prox = escopo_atual->simbolos;
    escopo_atual->simbolos = novo;
}

Simbolo *busca_simbolo(char *nome){
    for(Escopo *esc = escopo_atual; esc != NULL; esc = esc->pai){
        for(Simbolo *sim = esc->simbolos; sim != NULL; sim = sim->prox){
            if(strcmp(sim->nome, nome) == 0){
                return sim;
            }
        }
    }
    return NULL;
}

void imprime_tabela(void){
    printf("\n=== TABELA DE SIMBOLOS ===\n");
    printf("%-15s %-10s %-15s %-10s %-10s\n", 
           "Nome", "Tipo", "Classe", "Tamanho", "Linha");
    printf("------------------------------------------------------------------\n");
    
    int nivel = 0;
    for(Escopo *esc = escopo_atual; esc != NULL; esc = esc->pai){
        if (esc->simbolos != NULL) {
            printf("\n--- Escopo %d ---\n", nivel);
        }
        
        for(Simbolo *sim = esc->simbolos; sim != NULL; sim = sim->prox){
            const char *tipo_str = (sim->tipo == TIPO_INT) ? "int" : "void";
            const char *classe_str;
            
            switch(sim->classe) {
                case SIMP_VAR:   classe_str = "var"; break;
                case SIMP_ARRAY: classe_str = "array"; break;
                case SIMP_FUNC:  classe_str = "func"; break;
                case SIMP_PARAM: classe_str = "param"; break;
                default:         classe_str = "unknown"; break;
            }
            
            printf("%-15s %-10s %-15s %-10d %-10d\n",
                   sim->nome, tipo_str, classe_str, 
                   sim->tamanho, sim->lineno);
        }
        nivel++;
    }
    printf("\n===========================\n\n");
}

int main(int argc, char **argv){
    entra_escopo(); /* escopo global */

    insere_simbolo("input", TIPO_INT, SIMP_FUNC, 0);
    insere_simbolo("output", TIPO_VOID, SIMP_FUNC, 0);

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }
    yyparse();
    if (!ultima_funcao_eh_main) {
        fprintf(stderr, "ERRO SEMANTICO: ultima funcao deve ser 'void main(void)'\n");
        exit(1);
    }
    if (ultima_funcao_eh_main && main_tem_parametros) {
        fprintf(stderr, "ERRO SEMANTICO: main deve ser declarado como void main(void)\n");
        exit(1);
    }
    imprime_tabela();
    sai_escopo();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Erro Semântico: %s\n", s);
}