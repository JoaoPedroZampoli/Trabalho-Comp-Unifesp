#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "symbol_table.h"

/* ===== Tipos de nós da AST ===== */

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

/* ===== Estrutura da AST ===== */

typedef struct AST {
    NodeType type;
    union {
        int num;
        char* name;
        char op;
        char* relop;
    } data;
    struct AST* filhos[4];
    struct AST* irmao;
    int linha;
} AST;

/* ===== Variaveis globais ===== */

static Tipo tipo_funcao_atual = TIPO_ERRO;
static int funcao_tem_return = 0;       /* Flag para verificar se função int tem return */
static int main_encontrada = 0;          /* Flag para verificar se main existe */
static int main_linha = 0;              /* Linha da declaração de main */
static int dentro_funcao = 0;            /* Flag para verificar se estamos dentro de uma função */

/* ===== Prototipacao ===== */

static Tipo checkNode(AST* node);
static Tipo checkExpr(AST* node);
static void semanticError(const char* msg, int linha);
static void semanticErrorId(const char* id, int linha);
static int countParams(AST* params);
static int isVoidParam(AST* params);

/* Funcao principal da analise semântica */

static Tipo getAstType(AST* typeNode) {
    if (typeNode == NULL) return TIPO_ERRO;
    if (typeNode->type == NODE_TYPE_INT) return TIPO_INT;
    if (typeNode->type == NODE_TYPE_VOID) return TIPO_VOID;
    return TIPO_ERRO;
}

/* Conta o número de parâmetros de uma função */
static int countParams(AST* params) {
    if (params == NULL) return 0;
    if (params->type == NODE_TYPE_VOID) return 0;
    
    int count = 0;
    AST* p = params;
    while (p != NULL) {
        count++;
        p = p->irmao;
    }
    return count;
}

/* Verifica se o parâmetro é void */
static int isVoidParam(AST* params) {
    if (params == NULL) return 1;
    if (params->type == NODE_TYPE_VOID) return 1;
    return 0;
}

void semanticAnalysis(AST* root) {
    entra_escopo();

    /* Insere funções built-in: input() retorna int, output(int) retorna void */
    insere_simbolo("input", TIPO_INT, SIMP_FUNC, 0, 0);
    insere_simbolo("output", TIPO_VOID, SIMP_FUNC, 0, 0);

    main_encontrada = 0;

    checkNode(root);

    /* Verifica se main() foi declarada */
    if (!main_encontrada) {
        fprintf(stderr, "ERRO SEMANTICO: funcao 'main' nao declarada\n");
        exit(1);
    }
}

/* Percorre nós que nao retornam tipo */

static Tipo checkNode(AST* node) {
    if (node == NULL) return TIPO_VOID;

    switch (node->type) {

        case NODE_FUN_DECL: {
            Tipo tipoFunc = getAstType(node->filhos[0]);
            
            insere_simbolo(node->data.name, tipoFunc, SIMP_FUNC, 0, node->linha);

            /* Verifica se é a função main */
            if (strcmp(node->data.name, "main") == 0) {
                main_encontrada = 1;
                main_linha = node->linha;
                
                /* main deve retornar void */
                if (tipoFunc != TIPO_VOID) {
                    semanticError("funcao 'main' deve retornar void", node->linha);
                }
                
                /* main deve ter void como parâmetro ou nenhum parâmetro */
                if (!isVoidParam(node->filhos[1])) {
                    semanticError("funcao 'main' nao deve ter parametros", node->linha);
                }
            }

            Simbolo* s = busca_simbolo(node->data.name);
            tipo_funcao_atual = s->tipo;
            funcao_tem_return = 0;
            dentro_funcao = 1;

            entra_escopo();

            checkNode(node->filhos[1]); /* params */
            checkNode(node->filhos[2]); /* corpo */

            sai_escopo();

            /* Se a função é do tipo int, deve ter pelo menos um return */
            if (tipo_funcao_atual == TIPO_INT && !funcao_tem_return) {
                semanticError("funcao do tipo 'int' deve ter return", node->linha);
            }

            tipo_funcao_atual = TIPO_ERRO;
            dentro_funcao = 0;
            break;
        }

        case NODE_PARAM:
            if (getAstType(node->filhos[0]) == TIPO_VOID) {
                semanticError("parametro nao pode ser do tipo void", node->linha);
            }
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_PARAM, 0, node->linha);
            break;

        case NODE_ARRAY_PARAM:
            if (getAstType(node->filhos[0]) == TIPO_VOID) {
                semanticError("parametro array nao pode ser do tipo void", node->linha);
            }
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_ARRAY, 0, node->linha);
            break;

        case NODE_VAR_DECL:
            if (getAstType(node->filhos[0]) == TIPO_VOID) {
                semanticError("variavel nao pode ser do tipo void", node->linha);
            }
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_VAR, 0, node->linha);
            break;

        case NODE_ARRAY_DECL:
            if (getAstType(node->filhos[0]) == TIPO_VOID) {
                semanticError("array nao pode ser do tipo void", node->linha);
            }
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_ARRAY, node->filhos[1]->data.num, node->linha);
            break;

        case NODE_COMPOUND:
            /* Não criar novo escopo se já estamos entrando na função (escopo já foi criado) */
            checkNode(node->filhos[0]);
            checkNode(node->filhos[1]);
            break;

        case NODE_IF:
            if (checkExpr(node->filhos[0]) != TIPO_INT)
                semanticError("condicao do if deve ser int", node->linha);

            checkNode(node->filhos[1]);
            checkNode(node->filhos[2]);
            break;

        case NODE_WHILE:
            if (checkExpr(node->filhos[0]) != TIPO_INT)
                semanticError("condicao do while deve ser int", node->linha);

            checkNode(node->filhos[1]);
            break;

        case NODE_RETURN:
            if (!dentro_funcao) {
                semanticError("return fora de funcao", node->linha);
            }
            
            if (node->filhos[0] != NULL) {
                Tipo t = checkExpr(node->filhos[0]);
                if (tipo_funcao_atual == TIPO_VOID)
                    semanticError("return com valor em funcao void", node->linha);
                if (t != tipo_funcao_atual && tipo_funcao_atual != TIPO_ERRO)
                    semanticError("tipo de retorno incompativel", node->linha);
                funcao_tem_return = 1;
            } else {
                /* return sem valor */
                if (tipo_funcao_atual == TIPO_INT)
                    semanticError("funcao int deve retornar valor", node->linha);
            }
            break;

        case NODE_ASSIGN: {
            Tipo t1 = checkExpr(node->filhos[0]);
            Tipo t2 = checkExpr(node->filhos[1]);

            if (t1 == TIPO_VOID || t2 == TIPO_VOID)
                semanticError("nao e possivel atribuir tipo void", node->linha);
            if (t1 != t2)
                semanticError("atribuicao com tipos incompativeis", node->linha);
            break;
        }

        default:
            /* Expressões */
            checkExpr(node);
            break;
    }

    if (node->irmao != NULL)
        checkNode(node->irmao);

    return TIPO_VOID;
}

/* Checagem de expressões (retornando o tipo) */

static Tipo checkExpr(AST* node) {
    if (node == NULL) return TIPO_VOID;

    switch (node->type) {

        case NODE_NUM:
            return TIPO_INT;

        case NODE_VAR: {
            Simbolo* s = busca_simbolo(node->data.name);
            if (s == NULL) {
                semanticErrorId(node->data.name, node->linha);
            }
            /* Verifica se está tentando usar uma função como variável */
            if (s->classe == SIMP_FUNC) {
                semanticError("funcao usada como variavel", node->linha);
            }
            return s->tipo;
        }

        case NODE_ARRAY_ACCESS: {
            Simbolo* s = busca_simbolo(node->data.name);
            if (s == NULL) {
                semanticErrorId(node->data.name, node->linha);
            }

            /* Verifica se é realmente um array */
            if (s->classe != SIMP_ARRAY) {
                semanticError("variavel nao e um array", node->linha);
            }

            if (checkExpr(node->filhos[0]) != TIPO_INT)
                semanticError("indice de array deve ser int", node->linha);

            return s->tipo;
        }

        case NODE_OP: {
            Tipo t1 = checkExpr(node->filhos[0]);
            Tipo t2 = checkExpr(node->filhos[1]);

            if (t1 != TIPO_INT || t2 != TIPO_INT)
                semanticError("operacao aritmetica exige operandos int", node->linha);

            return TIPO_INT;
        }

        case NODE_RELOP: {
            Tipo t1 = checkExpr(node->filhos[0]);
            Tipo t2 = checkExpr(node->filhos[1]);

            if (t1 != TIPO_INT || t2 != TIPO_INT)
                semanticError("operacao relacional exige operandos int", node->linha);

            return TIPO_INT;
        }

        case NODE_CALL: {
            Simbolo* s = busca_simbolo(node->data.name);
            if (s == NULL) {
                semanticErrorId(node->data.name, node->linha);
            }

            /* Verifica se é realmente uma função */
            if (s->classe != SIMP_FUNC) {
                semanticError("identificador nao e uma funcao", node->linha);
            }

            /* Verificações específicas para output() */
            if (strcmp(node->data.name, "output") == 0) {
                int numArgs = countParams(node->filhos[0]);
                if (numArgs != 1) {
                    semanticError("output() requer exatamente 1 argumento", node->linha);
                }
                /* Verifica se o argumento é int */
                if (node->filhos[0] != NULL) {
                    Tipo argTipo = checkExpr(node->filhos[0]);
                    if (argTipo != TIPO_INT) {
                        semanticError("argumento de output() deve ser int", node->linha);
                    }
                }
            }

            /* Verificações específicas para input() */
            if (strcmp(node->data.name, "input") == 0) {
                int numArgs = countParams(node->filhos[0]);
                if (numArgs != 0) {
                    semanticError("input() nao recebe argumentos", node->linha);
                }
            }

            return s->tipo;
        }

        default:
            return TIPO_VOID;
    }
}

/* Tratamento de erro semântico */

static void semanticError(const char* msg, int linha) {
    fprintf(stderr, "ERRO SEMANTICO: %s - LINHA: %d\n", msg, linha);
    exit(1);
}

/* Erro semântico com identificador (formato exigido) */
static void semanticErrorId(const char* id, int linha) {
    fprintf(stderr, "ERRO SEMANTICO: identificador '%s' nao declarado - LINHA: %d\n", id, linha);
    exit(1);
}
