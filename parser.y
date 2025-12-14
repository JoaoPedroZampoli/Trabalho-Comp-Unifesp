%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int lineno;

void yyerror(const char* s);

/* --- TIPOS DE NÓS DA AST --- */
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ARRAY_DECL,
    NODE_FUN_DECL,
    NODE_PARAM,
    NODE_ARRAY_PARAM,
    NODE_COMPOUND,
    NODE_IF,
    NODE_WHILE,
    NODE_RETURN,
    NODE_ASSIGN,
    NODE_OP,
    NODE_RELOP,
    NODE_VAR,
    NODE_ARRAY_ACCESS,
    NODE_CALL,
    NODE_NUM,
    NODE_TYPE_INT,
    NODE_TYPE_VOID
} NodeType;

/* --- ESTRUTURA DA AST --- */
typedef struct AST {
    NodeType type;
    union {
        int num;              // para NUM
        char* name;           // para ID 
        char op;              // para operadores 
        char* relop;          // para operadores relacionais
    } data;
    struct AST* filhos[4];     // filhos (até 4: tipo, nome, params, corpo, etc.)
    struct AST* irmao;         // irmão (para listas)
    int lineno;               // linha no código fonte
} AST;

/* --- PROTÓTIPOS DA AST --- */
AST* newNode(NodeType type);
AST* newNumNode(int val);
AST* newIdNode(char* name);
AST* newOpNode(char op, AST* left, AST* right);
AST* newRelopNode(char* op, AST* left, AST* right);
void printAST(AST* tree, int indent);
void freeAST(AST* node);

/* Variável global para a raiz da AST */
AST* arvoreSintatica = NULL;
%}

%union {
  int val; 
  char* name; 
  struct AST* node;
}

%token <val> NUM
%token <name> ID
%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES DIV
%token LESSTHAN LESSEQUAL GREATERTHAN GREATEREQUAL EQUAL NOTEQUAL
%token ASSIGN
%token SEMICOLON COMMA
%token LEFTPAREN RIGHTPAREN LEFTBRACKET RIGHTBRACKET LEFTBRACE RIGHTBRACE
%token ERROR

/* Precedência e associatividade */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left LESSTHAN LESSEQUAL GREATERTHAN GREATEREQUAL EQUAL NOTEQUAL
%left PLUS MINUS
%left TIMES DIV

/* Tipos dos não-terminais */
%type <node> programa declaracao_lista declaracao var_declaracao fun_declaracao
%type <node> tipo_especificador params param_lista param composto_decl
%type <node> local_declaracoes statement_lista statement expressao_decl
%type <node> selecao_decl iteracao_decl retorno_decl expressao var
%type <node> simples_expressao relacional soma_expressao soma termo mult fator
%type <node> ativacao args arg_lista

%start programa

%%

/* 1. programa → declaração-lista */
programa
    : declaracao_lista
        { arvoreSintatica = $1; }
    ;

/* 2. declaração-lista → declaração-lista declaração | declaração */
declaracao_lista
    : declaracao_lista declaracao
        {
            AST* t = $1;
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | declaracao
        { $$ = $1; }
    ;

/* 3. declaração → var-declaração | fun-declaração */
declaracao
    : var_declaracao
        { $$ = $1; }
    | fun_declaracao
        { $$ = $1; }
    ;

/* 4. var-declaração → tipo-especificador ID ; | tipo-especificador ID [ NUM ] ; */
var_declaracao
    : tipo_especificador ID SEMICOLON
        {
            $$ = newNode(NODE_VAR_DECL);
            $$->filhos[0] = $1;
            $$->data.name = $2;
        }
    | tipo_especificador ID LEFTBRACKET NUM RIGHTBRACKET SEMICOLON
        {
            $$ = newNode(NODE_ARRAY_DECL);
            $$->filhos[0] = $1;
            $$->data.name = $2;
            $$->filhos[1] = newNumNode($4);
        }
    ;

/* 5. tipo-especificador → int | void */
tipo_especificador
    : INT
        { $$ = newNode(NODE_TYPE_INT); }
    | VOID
        { $$ = newNode(NODE_TYPE_VOID); }
    ;

/* 6. fun-declaração → tipo-especificador ID ( params ) composto-decl */
fun_declaracao
    : tipo_especificador ID LEFTPAREN params RIGHTPAREN composto_decl
        {
            $$ = newNode(NODE_FUN_DECL);
            $$->filhos[0] = $1;
            $$->data.name = $2;
            $$->filhos[1] = $4;
            $$->filhos[2] = $6;
        }
    ;

/* 7. params → param-lista | void */
params
    : param_lista
        { $$ = $1; }
    | VOID
        { $$ = newNode(NODE_TYPE_VOID); }
    ;

/* 8. param-lista → param-lista , param | param */
param_lista
    : param_lista COMMA param
        {
            AST* t = $1;
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = $3;
                $$ = $1;
            } else {
                $$ = $3;
            }
        }
    | param
        { $$ = $1; }
    ;

/* 9. param → tipo-especificador ID | tipo-especificador ID [ ] */
param
    : tipo_especificador ID
        {
            $$ = newNode(NODE_PARAM);
            $$->filhos[0] = $1;
            $$->data.name = $2;
        }
    | tipo_especificador ID LEFTBRACKET RIGHTBRACKET
        {
            $$ = newNode(NODE_ARRAY_PARAM);
            $$->filhos[0] = $1;
            $$->data.name = $2;
        }
    ;

/* 10. composto-decl → { local-declarações statement-lista } */
composto_decl
    : LEFTBRACE local_declaracoes statement_lista RIGHTBRACE
        {
            $$ = newNode(NODE_COMPOUND);
            $$->filhos[0] = $2;
            $$->filhos[1] = $3;
        }
    ;

/* 11. local-declarações → local-declarações var-declaração | vazio */
local_declaracoes
    : local_declaracoes var_declaracao
        {
            AST* t = $1;
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | /* vazio */
        { $$ = NULL; }
    ;

/* 12. statement-lista → statement-lista statement | vazio */
statement_lista
    : statement_lista statement
        {
            AST* t = $1;
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    | /* vazio */
        { $$ = NULL; }
    ;

/* 13. statement → expressão-decl | composto-decl | seleção-decl | iteração-decl | retorno-decl */
statement
    : expressao_decl
        { $$ = $1; }
    | composto_decl
        { $$ = $1; }
    | selecao_decl
        { $$ = $1; }
    | iteracao_decl
        { $$ = $1; }
    | retorno_decl
        { $$ = $1; }
    ;

/* 14. expressão-decl → expressão ; | ; */
expressao_decl
    : expressao SEMICOLON
        { $$ = $1; }
    | SEMICOLON
        { $$ = NULL; }
    ;

/* 15. seleção-decl → if ( expressão ) statement | if ( expressão ) statement else statement */
selecao_decl
    : IF LEFTPAREN expressao RIGHTPAREN statement %prec LOWER_THAN_ELSE
        {
            $$ = newNode(NODE_IF);
            $$->filhos[0] = $3;
            $$->filhos[1] = $5;
            $$->filhos[2] = NULL;
        }
    | IF LEFTPAREN expressao RIGHTPAREN statement ELSE statement
        {
            $$ = newNode(NODE_IF);
            $$->filhos[0] = $3;
            $$->filhos[1] = $5;
            $$->filhos[2] = $7;
        }
    ;

/* 16. iteração-decl → while ( expressão ) statement */
iteracao_decl
    : WHILE LEFTPAREN expressao RIGHTPAREN statement
        {
            $$ = newNode(NODE_WHILE);
            $$->filhos[0] = $3;
            $$->filhos[1] = $5;
        }
    ;

/* 17. retorno-decl → return ; | return expressão ; */
retorno_decl
    : RETURN SEMICOLON
        {
            $$ = newNode(NODE_RETURN);
            $$->filhos[0] = NULL;
        }
    | RETURN expressao SEMICOLON
        {
            $$ = newNode(NODE_RETURN);
            $$->filhos[0] = $2;
        }
    ;

/* 18. expressão → var = expressão | simples-expressão */
expressao
    : var ASSIGN expressao
        {
            $$ = newNode(NODE_ASSIGN);
            $$->filhos[0] = $1;
            $$->filhos[1] = $3;
        }
    | simples_expressao
        { $$ = $1; }
    ;

/* 19. var → ID | ID [ expressão ] */
var
    : ID
        {
            $$ = newNode(NODE_VAR);
            $$->data.name = $1;
        }
    | ID LEFTBRACKET expressao RIGHTBRACKET
        {
            $$ = newNode(NODE_ARRAY_ACCESS);
            $$->data.name = $1;
            $$->filhos[0] = $3;
        }
    ;

/* 20. simples-expressão → soma-expressão relacional soma-expressão | soma-expressão */
simples_expressao
    : soma_expressao relacional soma_expressao
        {
            $$ = $2;
            $$->filhos[0] = $1;
            $$->filhos[1] = $3;
        }
    | soma_expressao
        { $$ = $1; }
    ;

/* 21. relacional → <= | < | > | >= | == | != */
relacional
    : LESSEQUAL
        { $$ = newNode(NODE_RELOP); $$->data.relop = strdup("<="); }
    | LESSTHAN
        { $$ = newNode(NODE_RELOP); $$->data.relop = strdup("<"); }
    | GREATERTHAN
        { $$ = newNode(NODE_RELOP); $$->data.relop = strdup(">"); }
    | GREATEREQUAL
        { $$ = newNode(NODE_RELOP); $$->data.relop = strdup(">="); }
    | EQUAL
        { $$ = newNode(NODE_RELOP); $$->data.relop = strdup("=="); }
    | NOTEQUAL
        { $$ = newNode(NODE_RELOP); $$->data.relop = strdup("!="); }
    ;

/* 22. soma-expressão → soma-expressão soma termo | termo */
soma_expressao
    : soma_expressao soma termo
        {
            $$ = $2;
            $$->filhos[0] = $1;
            $$->filhos[1] = $3;
        }
    | termo
        { $$ = $1; }
    ;

/* 23. soma → + | - */
soma
    : PLUS
        { $$ = newNode(NODE_OP); $$->data.op = '+'; }
    | MINUS
        { $$ = newNode(NODE_OP); $$->data.op = '-'; }
    ;

/* 24. termo → termo mult fator | fator */
termo
    : termo mult fator
        {
            $$ = $2;
            $$->filhos[0] = $1;
            $$->filhos[1] = $3;
        }
    | fator
        { $$ = $1; }
    ;

/* 25. mult → * | / */
mult
    : TIMES
        { $$ = newNode(NODE_OP); $$->data.op = '*'; }
    | DIV
        { $$ = newNode(NODE_OP); $$->data.op = '/'; }
    ;

/* 26. fator → ( expressão ) | var | ativação | NUM */
fator
    : LEFTPAREN expressao RIGHTPAREN
        { $$ = $2; }
    | var
        { $$ = $1; }
    | ativacao
        { $$ = $1; }
    | NUM
        { $$ = newNumNode($1); }
    ;

/* 27. ativação → ID ( args ) */
ativacao
    : ID LEFTPAREN args RIGHTPAREN
        {
            $$ = newNode(NODE_CALL);
            $$->data.name = $1;
            $$->filhos[0] = $3;
        }
    ;

/* 28. args → arg-lista | vazio */
args
    : arg_lista
        { $$ = $1; }
    | /* vazio */
        { $$ = NULL; }
    ;

/* 29. arg-lista → arg-lista , expressão | expressão */
arg_lista
    : arg_lista COMMA expressao
        {
            AST* t = $1;
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = $3;
                $$ = $1;
            } else {
                $$ = $3;
            }
        }
    | expressao
        { $$ = $1; }
    ;

%%

/*  Funções para criar nós da AST  */

AST* newNode(NodeType type) {
    AST* node = (AST*) malloc(sizeof(AST));
    if (node == NULL) {
        fprintf(stderr, "Erro: Sem memória para alocar nó da AST\n");
        exit(1);
    }
    node->type = type;
    node->data.name = NULL;
    for (int i = 0; i < 4; i++)
        node->filhos[i] = NULL;
    node->irmao = NULL;
    node->lineno = lineno;
    return node;
}

AST* newNumNode(int val) {
    AST* node = newNode(NODE_NUM);
    node->data.num = val;
    return node;
}

AST* newIdNode(char* name) {
    AST* node = newNode(NODE_VAR);
    node->data.name = name;
    return node;
}

AST* newOpNode(char op, AST* left, AST* right) {
    AST* node = newNode(NODE_OP);
    node->data.op = op;
    node->filhos[0] = left;
    node->filhos[1] = right;
    return node;
}

AST* newRelopNode(char* op, AST* left, AST* right) {
    AST* node = newNode(NODE_RELOP);
    node->data.relop = strdup(op);
    node->filhos[0] = left;
    node->filhos[1] = right;
    return node;
}

/*  Função para imprimir a AST  */

static void printSpaces(int indent) {
    for (int i = 0; i < indent; i++)
        printf("  ");
}

void printAST(AST* tree, int indent) {
    while (tree != NULL) {
        printSpaces(indent);
        switch (tree->type) {
            case NODE_PROGRAM:
                printf("Programa\n");
                break;
            case NODE_VAR_DECL:
                printf("Declaração de Variável: %s\n", tree->data.name);
                break;
            case NODE_ARRAY_DECL:
                printf("Declaração de Array: %s\n", tree->data.name);
                break;
            case NODE_FUN_DECL:
                printf("Declaração de Função: %s\n", tree->data.name);
                break;
            case NODE_PARAM:
                printf("Parâmetro: %s\n", tree->data.name);
                break;
            case NODE_ARRAY_PARAM:
                printf("Parâmetro Array: %s[]\n", tree->data.name);
                break;
            case NODE_COMPOUND:
                printf("Bloco Composto\n");
                break;
            case NODE_IF:
                printf("If\n");
                break;
            case NODE_WHILE:
                printf("While\n");
                break;
            case NODE_RETURN:
                printf("Return\n");
                break;
            case NODE_ASSIGN:
                printf("Atribuição\n");
                break;
            case NODE_OP:
                printf("Operação: %c\n", tree->data.op);
                break;
            case NODE_RELOP:
                printf("Operação Relacional: %s\n", tree->data.relop);
                break;
            case NODE_VAR:
                printf("Variável: %s\n", tree->data.name);
                break;
            case NODE_ARRAY_ACCESS:
                printf("Acesso a Array: %s\n", tree->data.name);
                break;
            case NODE_CALL:
                printf("Chamada de Função: %s\n", tree->data.name);
                break;
            case NODE_NUM:
                printf("Número: %d\n", tree->data.num);
                break;
            case NODE_TYPE_INT:
                printf("Tipo: int\n");
                break;
            case NODE_TYPE_VOID:
                printf("Tipo: void\n");
                break;
            default:
                printf("Nó desconhecido\n");
                break;
        }
        for (int i = 0; i < 4; i++)
            printAST(tree->filhos[i], indent + 1);
        tree = tree->irmao;
    }
}


void freeAST(AST* node) {
    if (node == NULL) return;
    for (int i = 0; i < 4; i++)
        freeAST(node->filhos[i]);
    freeAST(node->irmao);
    if (node->type == NODE_RELOP && node->data.relop != NULL)
        free(node->data.relop);
    free(node);
}

/*  FUNÇÕES PARA GERAR GRAPHVIZ   */

static int nodeCounter = 0;

/* Retorna o label do nó baseado no tipo */
static const char* getNodeLabel(AST* node, char* buffer) {
    switch (node->type) {
        case NODE_PROGRAM:
            sprintf(buffer, "Programa");
            break;
        case NODE_VAR_DECL:
            sprintf(buffer, "VarDecl\\n%s", node->data.name);
            break;
        case NODE_ARRAY_DECL:
            sprintf(buffer, "ArrayDecl\\n%s", node->data.name);
            break;
        case NODE_FUN_DECL:
            sprintf(buffer, "FunDecl\\n%s", node->data.name);
            break;
        case NODE_PARAM:
            sprintf(buffer, "Param\\n%s", node->data.name);
            break;
        case NODE_ARRAY_PARAM:
            sprintf(buffer, "ArrayParam\\n%s[]", node->data.name);
            break;
        case NODE_COMPOUND:
            sprintf(buffer, "Bloco\\n{ }");
            break;
        case NODE_IF:
            sprintf(buffer, "If");
            break;
        case NODE_WHILE:
            sprintf(buffer, "While");
            break;
        case NODE_RETURN:
            sprintf(buffer, "Return");
            break;
        case NODE_ASSIGN:
            sprintf(buffer, "=");
            break;
        case NODE_OP:
            sprintf(buffer, "%c", node->data.op);
            break;
        case NODE_RELOP:
            sprintf(buffer, "%s", node->data.relop);
            break;
        case NODE_VAR:
            sprintf(buffer, "Var\\n%s", node->data.name);
            break;
        case NODE_ARRAY_ACCESS:
            sprintf(buffer, "Array\\n%s[]", node->data.name);
            break;
        case NODE_CALL:
            sprintf(buffer, "Call\\n%s()", node->data.name);
            break;
        case NODE_NUM:
            sprintf(buffer, "%d", node->data.num);
            break;
        case NODE_TYPE_INT:
            sprintf(buffer, "int");
            break;
        case NODE_TYPE_VOID:
            sprintf(buffer, "void");
            break;
        default:
            sprintf(buffer, "???");
            break;
    }
    return buffer;
}

/* Retorna a cor do nó baseado no tipo */
static const char* getNodeColor(AST* node) {
    switch (node->type) {
        case NODE_FUN_DECL:
            return "#FFD700";  /* Dourado - funções */
        case NODE_VAR_DECL:
        case NODE_ARRAY_DECL:
            return "#98FB98";  /* Verde claro - declarações */
        case NODE_IF:
        case NODE_WHILE:
            return "#87CEEB";  /* Azul claro - controle de fluxo */
        case NODE_RETURN:
            return "#FFA07A";  /* Salmão - return */
        case NODE_OP:
        case NODE_RELOP:
            return "#DDA0DD";  /* Lilás - operadores */
        case NODE_CALL:
            return "#F0E68C";  /* Caqui - chamadas */
        case NODE_NUM:
            return "#FFFFFF";  /* Branco - números */
        case NODE_VAR:
        case NODE_ARRAY_ACCESS:
            return "#E0FFFF";  /* Ciano claro - variáveis */
        case NODE_TYPE_INT:
        case NODE_TYPE_VOID:
            return "#D3D3D3";  /* Cinza - tipos */
        case NODE_COMPOUND:
            return "#F5F5DC";  /* Bege - blocos */
        default:
            return "#FFFFFF";
    }
}

/* Gera os nós recursivamente */
static int generateDotNodes(FILE* fp, AST* node) {
    if (node == NULL) return -1;
    
    int myId = nodeCounter++;
    char labelBuffer[128];
    
    fprintf(fp, "  node%d [label=\"%s\", style=filled, fillcolor=\"%s\"];\n",
            myId, getNodeLabel(node, labelBuffer), getNodeColor(node));
    
    /* Processa filhos */
    for (int i = 0; i < 4; i++) {
        if (node->filhos[i] != NULL) {
            int childId = generateDotNodes(fp, node->filhos[i]);
            fprintf(fp, "  node%d -> node%d;\n", myId, childId);
        }
    }
    
    /* Processa irmãos (com seta tracejada) */
    if (node->irmao != NULL) {
        int siblingId = generateDotNodes(fp, node->irmao);
        fprintf(fp, "  node%d -> node%d [style=dashed, color=gray];\n", myId, siblingId);
    }
    
    return myId;
}

/* Gera o arquivo .dot completo */
void generateDotFile(AST* tree, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo '%s'\n", filename);
        return;
    }
    
    fprintf(fp, "digraph AST {\n");
    fprintf(fp, "  graph [rankdir=TB, splines=ortho];\n");
    fprintf(fp, "  node [shape=box, fontname=\"Courier\", fontsize=10];\n");
    fprintf(fp, "  edge [arrowsize=0.8];\n");
    fprintf(fp, "\n");
    
    nodeCounter = 0;
    generateDotNodes(fp, tree);
    
    fprintf(fp, "}\n");
    fclose(fp);
    
    printf("Arquivo GraphViz gerado: %s\n", filename);
    printf("Para visualizar, execute:\n");
    printf("  dot -Tpng %s -o arvore.png\n", filename);
    printf("  dot -Tsvg %s -o arvore.svg\n", filename);
}

/* --- Função main --- */

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            fprintf(stderr, "Erro: Não foi possível abrir o arquivo '%s'\n", argv[1]);
            return 1;
        }
    } else {
        printf("Uso: %s <arquivo.cm> [-dot]\n", argv[0]);
        printf("  -dot  Gera arquivo arvore.dot para GraphViz\n");
        return 1;
    }

    printf("Compilador C-\n");
    printf("Analisando arquivo: %s\n\n", argv[1]);

    int result = yyparse();
    
    if (result == 0) {
        printf("=== Análise Sintática concluída com sucesso! ===\n\n");
        
        /* Verifica se deve gerar arquivo .dot */
        int generateDot = 0;
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-dot") == 0) {
                generateDot = 1;
                break;
            }
        }
        
        if (generateDot) {
            generateDotFile(arvoreSintatica, "arvore.dot");
            printf("\n");
        }
        
        printf("=== Árvore Sintática Abstrata (AST) ===\n\n");
        printAST(arvoreSintatica, 0);
    } else {
        printf("=== Análise Sintática falhou! ===\n");
    }

    freeAST(arvoreSintatica);
    fclose(yyin);
    
    return result;
}

/* Função de erro  */

void yyerror(const char* s) {
    fprintf(stderr, "ERRO SINTÁTICO: %s - LINHA: %d\n", s, lineno);
}
