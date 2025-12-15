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
    int lineno;
} AST;

/* ===== Variaveis globais ===== */

static Tipo tipo_funcao_atual = TIPO_ERRO;

/* ===== Prototipacao ===== */

static Tipo checkNode(AST* node);
static Tipo checkExpr(AST* node);
static void semanticError(const char* msg, int lineno);

/* =========================================================
   Funcao principal da analise semântica
   ========================================================= */

static Tipo getAstType(AST* typeNode) {
    if (typeNode->type == NODE_TYPE_INT) return TIPO_INT;
    if (typeNode->type == NODE_TYPE_VOID) return TIPO_VOID;
    return TIPO_ERRO;
}

void semanticAnalysis(AST* root) {
    entra_escopo();

    insere_simbolo("input", TIPO_INT, SIMP_FUNC, 0, 0);
    insere_simbolo("output", TIPO_VOID, SIMP_FUNC, 0, 0);

    checkNode(root);

    // sai_escopo();
}

/* =========================================================
   Percorre nós que nao retornam tipo
   ========================================================= */

static Tipo checkNode(AST* node) {
    if (node == NULL) return TIPO_VOID;

    switch (node->type) {

        // case NODE_PROGRAM: {
        //     AST* decl = node;
        //     while (decl != NULL) {
        //         checkNode(decl);
        //         decl = decl->irmao;
        //     }
        //     break;
        // }

        case NODE_FUN_DECL: {
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_FUNC, 0, node->lineno);

            Simbolo* s = busca_simbolo(node->data.name);
            tipo_funcao_atual = s->tipo;

            entra_escopo();

            checkNode(node->filhos[1]); /* params */
            checkNode(node->filhos[2]); /* corpo */

            sai_escopo();

            tipo_funcao_atual = TIPO_ERRO;
            break;
        }

        case NODE_PARAM:
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_PARAM, 0, node->lineno);
            break;
        case NODE_ARRAY_PARAM:
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_ARRAY, 0, node->lineno);
            break;
        case NODE_VAR_DECL:
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_VAR, 0, node->lineno);
            break;
        case NODE_ARRAY_DECL:
            insere_simbolo(node->data.name, getAstType(node->filhos[0]), SIMP_ARRAY, node->filhos[1]->data.num, node->lineno);
                break;

        case NODE_COMPOUND:
            entra_escopo();
            checkNode(node->filhos[0]);
            checkNode(node->filhos[1]);
            sai_escopo();
            break;



        case NODE_IF:
            if (checkExpr(node->filhos[0]) != TIPO_INT)
                semanticError("condicao do if deve ser int", node->lineno);

            checkNode(node->filhos[1]);
            checkNode(node->filhos[2]);
            break;

        case NODE_WHILE:
            if (checkExpr(node->filhos[0]) != TIPO_INT)
                semanticError("condicao do while deve ser int", node->lineno);

            checkNode(node->filhos[1]);
            break;

        case NODE_RETURN:
            if (node->filhos[0] != NULL) {
                Tipo t = checkExpr(node->filhos[0]);
                if (tipo_funcao_atual == TIPO_VOID)
                    semanticError("return com valor em funcao void", node->lineno);
                if (t != tipo_funcao_atual)
                    semanticError("tipo de retorno incompativel", node->lineno);
            }
            break;

        case NODE_ASSIGN: {
            Tipo t1 = checkExpr(node->filhos[0]);
            Tipo t2 = checkExpr(node->filhos[1]);

            if (t1 != t2)
                semanticError("atribuicao com tipos incompativeis", node->lineno);
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

/* =========================================================
   Checagem de expressões (retornando o tipo)
   ========================================================= */

static Tipo checkExpr(AST* node) {
    if (node == NULL) return TIPO_VOID;

    switch (node->type) {

        case NODE_NUM:
            return TIPO_INT;

        case NODE_VAR: {
            Simbolo* s = busca_simbolo(node->data.name);
            if (s == NULL)
                semanticError("variavel nao declarada", node->lineno);
            return s->tipo;
        }

        case NODE_ARRAY_ACCESS: {
            Simbolo* s = busca_simbolo(node->data.name);
            if (s == NULL)
                semanticError("array nao declarado", node->lineno);

            if (checkExpr(node->filhos[0]) != TIPO_INT)
                semanticError("indice de array deve ser int", node->lineno);

            return s->tipo;
        }

        case NODE_OP: {
            Tipo t1 = checkExpr(node->filhos[0]);
            Tipo t2 = checkExpr(node->filhos[1]);

            if (t1 != TIPO_INT || t2 != TIPO_INT)
                semanticError("operacao aritmética exige operandos int", node->lineno);

            return TIPO_INT;
        }

        case NODE_RELOP: {
            Tipo t1 = checkExpr(node->filhos[0]);
            Tipo t2 = checkExpr(node->filhos[1]);

            if (t1 != TIPO_INT || t2 != TIPO_INT)
                semanticError("operacao relacional exige operandos int", node->lineno);

            return TIPO_INT;
        }

        case NODE_CALL: {
            Simbolo* s = busca_simbolo(node->data.name);
            if (s == NULL)
                semanticError("chamada de funcao nao declarada", node->lineno);

            /* (opcional) checar número de argumentos */
            return s->tipo;
        }

        default:
            return TIPO_VOID;
    }
}

/* =========================================================
   Tratamento de erro semântico
   ========================================================= */

static void semanticError(const char* msg, int lineno) {
    fprintf(stderr,
        "ERRO SEMANTICO: %s - LINHA: %d\n",
        msg, lineno);
    exit(1);
}
