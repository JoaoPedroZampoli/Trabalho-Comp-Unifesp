%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "semantic.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int lineno;
extern char lastToken[];

void yyerror(const char* s);

int errosSintaticos = 0;

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

AST* arvoreSintatica = NULL;

/* operações nas quádruplas */
typedef enum {
    OP_ADD,         /* t = a + b */
    OP_SUB,         /* t = a - b */
    OP_MUL,         /* t = a * b */
    OP_DIV,         /* t = a / b */
    OP_ASSIGN,      /* t = a */
    OP_ASSIGN_ARR,  /* t[i] = a */
    OP_ARR_ACCESS,  /* t = a[i] */
    OP_LT,          /* t = a < b */
    OP_LE,          /* t = a <= b */
    OP_GT,          /* t = a > b */
    OP_GE,          /* t = a >= b */
    OP_EQ,          /* t = a == b */
    OP_NE,          /* t = a != b */
    OP_LABEL,       /* L: */
    OP_GOTO,        /* goto L */
    OP_IF_FALSE,    /* if_false a goto L */
    OP_IF_TRUE,     /* if_true a goto L */
    OP_PARAM,       /* param a */
    OP_CALL,        /* t = call f, n */
    OP_RETURN,      /* return a */
    OP_FUNC_START,  /* início de função */
    OP_FUNC_END,    /* fim de função */
    OP_HALT         /* halt (fim do programa) */
} QuadOp;

typedef struct Quadruplas {
    QuadOp op;
    char* arg1;
    char* arg2;
    char* result;
    struct Quadruplas* next;
} Quadruplas;

Quadruplas* quadList = NULL;
Quadruplas* quadTail = NULL;

int tempCount = 0;
int labelCount = 0;

/* Protótipos das funções de código intermediário */
char* newTemp(void);
char* newLabel(void);
void emit(QuadOp op, char* arg1, char* arg2, char* result);
void printQuadruplass(void);
char* genCode(AST* node);
const char* quadOpToString(QuadOp op);

%}

%union {
  int num; 
  char* id; 
  struct AST* node;
}

/* mensagens de erro detalhadas */
%define parse.error verbose

%token <num> NUM
%token <id> ID
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
    printf("execute:  dot -Tpng %s -o arvore.png\n", filename);
}

/* FUNÇÕES DE CÓDIGO INTERMEDIÁRIO */

char* newTemp(void) {
    char* temp = (char*) malloc(16);
    sprintf(temp, "t%d", tempCount++);
    return temp;
}

char* newLabel(void) {
    char* label = (char*) malloc(16);
    sprintf(label, "L%d", labelCount++);
    return label;
}

void emit(QuadOp op, char* arg1, char* arg2, char* result) {
    Quadruplas* quad = (Quadruplas*) malloc(sizeof(Quadruplas));
    quad->op = op;
    quad->arg1 = arg1 ? strdup(arg1) : NULL;
    quad->arg2 = arg2 ? strdup(arg2) : NULL;
    quad->result = result ? strdup(result) : NULL;
    quad->next = NULL;
    
    if (quadList == NULL) {
        quadList = quad;
        quadTail = quad;
    } else {
        quadTail->next = quad;
        quadTail = quad;
    }
}

const char* quadOpToString(QuadOp op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_ASSIGN: return "=";
        case OP_ASSIGN_ARR: return "[]=";
        case OP_ARR_ACCESS: return "=[]";
        case OP_LT: return "<";
        case OP_LE: return "<=";
        case OP_GT: return ">";
        case OP_GE: return ">=";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        case OP_LABEL: return "label";
        case OP_GOTO: return "goto";
        case OP_IF_FALSE: return "if_false";
        case OP_IF_TRUE: return "if_true";
        case OP_PARAM: return "param";
        case OP_CALL: return "call";
        case OP_RETURN: return "return";
        case OP_FUNC_START: return "func_start";
        case OP_FUNC_END: return "func_end";
        case OP_HALT: return "halt";
        default: return "???";
    }
}

void printQuadruplass(void) {
    printf("\n--- CODIGO INTERMEDIARIO (3 ENDERECOS) ---\n\n");
    int lineNum = 0;
    Quadruplas* q = quadList;
    
    while (q != NULL) {
        printf("%3d: ", lineNum++);
        
        switch (q->op) {
            /* Operações aritméticas */
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            /* Operações relacionais */
            case OP_LT:
            case OP_LE:
            case OP_GT:
            case OP_GE:
            case OP_EQ:
            case OP_NE:
                printf("%s = %s %s %s\n", q->result, q->arg1, quadOpToString(q->op), q->arg2);
                break;
                
            case OP_ASSIGN:
                printf("%s = %s\n", q->result, q->arg1);
                break;
                
            case OP_ASSIGN_ARR:
                printf("%s[%s] = %s\n", q->result, q->arg1, q->arg2);
                break;
                
            case OP_ARR_ACCESS:
                printf("%s = %s[%s]\n", q->result, q->arg1, q->arg2);
                break;
                
            case OP_LABEL:
                printf("%s:\n", q->result);
                break;
                
            case OP_GOTO:
                printf("goto %s\n", q->result);
                break;
                
            case OP_IF_FALSE:
                printf("if_false %s goto %s\n", q->arg1, q->result);
                break;
                
            case OP_IF_TRUE:
                printf("if_true %s goto %s\n", q->arg1, q->result);
                break;
                
            case OP_PARAM:
                printf("param %s\n", q->arg1);
                break;
                
            case OP_CALL:
                if (q->result != NULL)
                    printf("%s = call %s, %s\n", q->result, q->arg1, q->arg2);
                else
                    printf("call %s, %s\n", q->arg1, q->arg2);
                break;
                
            case OP_RETURN:
                if (q->arg1 != NULL)
                    printf("return %s\n", q->arg1);
                else
                    printf("return\n");
                break;
                
            case OP_FUNC_START:
                printf("func_start %s\n", q->result);
                break;
                
            case OP_FUNC_END:
                printf("func_end %s\n", q->result);
                break;
                
            case OP_HALT:
                printf("halt\n");
                break;
                
            default:
                printf("???\n");
        }
        q = q->next;
    }
    printf("\n");
}


static int countArgs(AST* argList) {
    int count = 0;
    AST* arg = argList;
    while (arg != NULL) {
        count++;
        arg = arg->irmao;
    }
    return count;
}

static void genArgs(AST* argList) {
    if (argList == NULL) return;
    
    AST* arg = argList;
    while (arg != NULL) {
        char* argVal = genCode(arg);
        emit(OP_PARAM, argVal, NULL, NULL);
        arg = arg->irmao;
    }
}

char* genCode(AST* node) {
    if (node == NULL) return NULL;
    
    char* t1;
    char* t2;
    char* temp;
    char* label1;
    char* label2;
    char numStr[32];
    
    switch (node->type) {
        case NODE_PROGRAM:
            {
                AST* decl = node;
                while (decl != NULL) {
                    genCode(decl);
                    decl = decl->irmao;
                }
            }
            emit(OP_HALT, NULL, NULL, NULL);
            return NULL;
            
        case NODE_VAR_DECL:
        case NODE_ARRAY_DECL:
        case NODE_TYPE_INT:
        case NODE_TYPE_VOID:
            /* Declarações de variáveis não geram código no intermediário */
            if (node->irmao != NULL)
                genCode(node->irmao);
            return NULL;
            
        case NODE_FUN_DECL:
            emit(OP_FUNC_START, NULL, NULL, node->data.name);
            genCode(node->filhos[2]);
            emit(OP_FUNC_END, NULL, NULL, node->data.name);

            if (node->irmao != NULL)
                genCode(node->irmao);
            return NULL;
            
        case NODE_PARAM:
        case NODE_ARRAY_PARAM:
            return NULL;
            
        case NODE_COMPOUND:
            genCode(node->filhos[0]);
            genCode(node->filhos[1]);
            return NULL;
            
        case NODE_IF:
            t1 = genCode(node->filhos[0]);  /* condição */
            label1 = newLabel();
            emit(OP_IF_FALSE, t1, NULL, label1);
            genCode(node->filhos[1]);  /* then */
            if (node->filhos[2] != NULL) {
                label2 = newLabel();
                emit(OP_GOTO, NULL, NULL, label2);
                emit(OP_LABEL, NULL, NULL, label1);
                genCode(node->filhos[2]);  /* else */
                emit(OP_LABEL, NULL, NULL, label2);
            } else {
                emit(OP_LABEL, NULL, NULL, label1);
            }

            if (node->irmao != NULL)
                genCode(node->irmao);
            return NULL;
            
        case NODE_WHILE:
            label1 = newLabel();  /* início do loop */
            label2 = newLabel();  /* fim do loop */
            emit(OP_LABEL, NULL, NULL, label1);
            t1 = genCode(node->filhos[0]);  /* condição */
            emit(OP_IF_FALSE, t1, NULL, label2);
            genCode(node->filhos[1]);  /* corpo */
            emit(OP_GOTO, NULL, NULL, label1);
            emit(OP_LABEL, NULL, NULL, label2);

            if (node->irmao != NULL)
                genCode(node->irmao);
            return NULL;
            
        case NODE_RETURN:
            if (node->filhos[0] != NULL) {
                t1 = genCode(node->filhos[0]);
                emit(OP_RETURN, t1, NULL, NULL);
            } else {
                emit(OP_RETURN, NULL, NULL, NULL);
            }

            if (node->irmao != NULL)
                genCode(node->irmao);
            return NULL;
            
        case NODE_ASSIGN:
            t1 = genCode(node->filhos[1]);  /* expressão */
            
            if (node->filhos[0]->type == NODE_ARRAY_ACCESS) {
                /* Atribuição a array: var[idx] = expr */
                char* idx = genCode(node->filhos[0]->filhos[0]);
                emit(OP_ASSIGN_ARR, idx, t1, node->filhos[0]->data.name);
            } else {
                /* Atribuição simples: var = expr */
                emit(OP_ASSIGN, t1, NULL, node->filhos[0]->data.name);
            }

            if (node->irmao != NULL)
                genCode(node->irmao);
            return t1;
            
        case NODE_OP:
            t1 = genCode(node->filhos[0]);
            t2 = genCode(node->filhos[1]);
            temp = newTemp();
            
            switch (node->data.op) {
                case '+':
                    emit(OP_ADD, t1, t2, temp);
                    break;
                case '-':
                    emit(OP_SUB, t1, t2, temp);
                    break;
                case '*':
                    emit(OP_MUL, t1, t2, temp);
                    break;
                case '/':
                    emit(OP_DIV, t1, t2, temp);
                    break;
            }
            return temp;
            
        case NODE_RELOP:
            t1 = genCode(node->filhos[0]);
            t2 = genCode(node->filhos[1]);
            temp = newTemp();
            
            if (strcmp(node->data.relop, "<") == 0)
                emit(OP_LT, t1, t2, temp);
            else if (strcmp(node->data.relop, "<=") == 0)
                emit(OP_LE, t1, t2, temp);
            else if (strcmp(node->data.relop, ">") == 0)
                emit(OP_GT, t1, t2, temp);
            else if (strcmp(node->data.relop, ">=") == 0)
                emit(OP_GE, t1, t2, temp);
            else if (strcmp(node->data.relop, "==") == 0)
                emit(OP_EQ, t1, t2, temp);
            else if (strcmp(node->data.relop, "!=") == 0)
                emit(OP_NE, t1, t2, temp);
            
            return temp;
            
        case NODE_VAR:
            return strdup(node->data.name);
            
        case NODE_ARRAY_ACCESS:
            t1 = genCode(node->filhos[0]);  /* índice */
            temp = newTemp();
            emit(OP_ARR_ACCESS, node->data.name, t1, temp);
            return temp;
            
        case NODE_CALL:
            /* Gera código para os argumentos */
            genArgs(node->filhos[0]);
            /* Conta argumentos */
            sprintf(numStr, "%d", countArgs(node->filhos[0]));
            /* Gera chamada */
            temp = newTemp();
            emit(OP_CALL, node->data.name, numStr, temp);

            if (node->irmao != NULL)
                genCode(node->irmao);
            
            return temp;
            
        case NODE_NUM:
            sprintf(numStr, "%d", node->data.num);
            return strdup(numStr);
            
        default:
            if (node->irmao != NULL)
                genCode(node->irmao);
            return NULL;
    }
}

/* Função main */

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            fprintf(stderr, "Erro: Não foi possível abrir o arquivo '%s'\n", argv[1]);
            return 1;
        }
    } else {
        printf("Uso: %s <arquivo.cm>\n", argv[0]);
        return 1;
    }

    printf("Compilador C-\n");
    printf("Compilando arquivo: %s\n\n", argv[1]);

    int result = yyparse();
    
    if (result == 0 && errosSintaticos == 0) {        
        generateDotFile(arvoreSintatica, "arvore.dot");
        printf("\n");
        
        semanticAnalysis(arvoreSintatica);


        /* gera código intermediário */
        genCode(arvoreSintatica);
        printQuadruplass();
        imprime_tabela();
    } else {
        printf("\n--- Compilacao falhou com %d erro(s) sintatico(s) ---\n", errosSintaticos);
    }
    sai_escopo();
    freeAST(arvoreSintatica);
    fclose(yyin);
    
    return result;
}

/* Função de erro  */

void yyerror(const char* s) {
    errosSintaticos++;
        
    if (strstr(s, "unexpected") != NULL) {
        /* Formato: "syntax error, unexpected X, expecting Y" */
        char* unexpected = strstr(s, "unexpected");
        char* expecting = strstr(s, "expecting");
        
        if (unexpected != NULL) {
            char tokenInesperado[64] = "";
            char tokenEsperado[256] = "";
            
            sscanf(unexpected, "unexpected %63[^,\n]", tokenInesperado);
            
            if (expecting != NULL) {
                strcpy(tokenEsperado, expecting + 10);
                char* nl = strchr(tokenEsperado, '\n');
                if (nl) *nl = '\0';
            }
            
            if (strlen(tokenEsperado) > 0) {
                fprintf(stderr, "ERRO SINTATICO: token inesperado '%s', esperado '%s' - LINHA: %d\n", 
                        tokenInesperado, tokenEsperado, lineno);
            } else {
                fprintf(stderr, "ERRO SINTATICO: token inesperado '%s' - LINHA: %d\n", 
                        tokenInesperado, lineno);
            }
            return;
        }
    }
    
    fprintf(stderr, "ERRO SINTATICO: %s - LINHA: %d\n", s, lineno);
}
