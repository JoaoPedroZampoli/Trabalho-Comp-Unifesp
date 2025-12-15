#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"
#include "globals.h"

typedef struct Escopo {
    Simbolo *simbolos;         /* lista de símbolos do escopo */
    struct Escopo *pai;        /* escopo pai para a pilha semântica */
    struct Escopo *prox;        /* próximo escopo na lista */
    int id;                     /* identificador do escopo */
} Escopo;

/* Variavel global de escopo*/
static Escopo *escopo_atual = NULL;
static Escopo *todos_escopos = NULL; /* Cabeça da lista para impressão final */
static Escopo *ultimo_escopo = NULL; /* Cauda da lista */
static int contador_escopos = 0;
Escopo *escopo_global = NULL;

void entra_escopo(void) {
    Escopo *novo = malloc(sizeof(Escopo));
    novo->simbolos = NULL;
    novo->pai = escopo_atual;
    novo->prox = NULL;
    novo->id = contador_escopos++;

    /* Gerenciamento da pilha semântica */
    escopo_atual = novo;

    /* Gerenciamento do histórico (lista encadeada) */
    if (todos_escopos == NULL) {
        todos_escopos = novo;
        ultimo_escopo = novo;
    } else {
        ultimo_escopo->prox = novo;
        ultimo_escopo = novo;
    }
}


void sai_escopo(void) {
    if (escopo_atual != NULL) {
        /* Apenas se move o ponteiro para cima na pilha.
           Não é dado free() aqui, para imprimir no final. */
        escopo_atual = escopo_atual->pai;
    }
}

void insere_simbolo(char *nome, Tipo tipo, Classe classe, int tamanho, int linha_val){
    for(Simbolo *sim = escopo_atual->simbolos; sim != NULL; sim = sim->prox){
        if(strcmp(sim->nome, nome) == 0){
            /* fprintf(stderr, "Erro: símbolo '%s' já declarado neste escopo.\n", nome); // Talvez tirar daqui e só deixar o YYERROR? */
            fprintf(stderr, "ERRO SEMANTICO: identificador '%s' ja declarado neste escopo - LINHA: %d\n", nome, linha_val);
            exit(1);
        }
    }
    Simbolo *novo = (Simbolo*)malloc(sizeof(Simbolo));
    novo->nome = strdup(nome);
    novo->tipo = tipo;
    novo->classe = classe;
    novo->linha = linha_val;
    novo->tamanho = tamanho;
    novo->prox = escopo_atual->simbolos;
    escopo_atual->simbolos = novo;
}

void volta_escopo_global(void) {
    escopo_atual = escopo_global;
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

void imprime_tabela(void) {
    printf("\n=== TABELA DE SIMBOLOS (Completa) ===\n");
    printf("%-15s %-10s %-10s %-10s %-10s\n",
           "Nome", "Tipo", "Classe", "Tamanho", "Linha");
    printf("---------------------------------------------------------------\n");

    /* Itera sobre a lista histórica de todos os escopos criados */
    Escopo *esc = todos_escopos;
    while (esc != NULL) {
        if (esc->simbolos != NULL) {
            printf("\n--- Escopo %d ---\n", esc->id);
            
            Simbolo *s = esc->simbolos;
            while (s != NULL) {
                const char *tipo =
                    (s->tipo == TIPO_INT)  ? "int"  :
                    (s->tipo == TIPO_VOID) ? "void" : "erro";

                const char *classe;
                switch (s->classe) {
                    case SIMP_VAR:   classe = "var";   break;
                    case SIMP_ARRAY: classe = "array"; break;
                    case SIMP_FUNC:  classe = "func";  break;
                    case SIMP_PARAM: classe = "param"; break;
                    default:         classe = "unk";   break;
                }

                printf("%-15s %-10s %-10s %-10d %-10d\n",
                       s->nome, tipo, classe, s->tamanho, s->linha);
                
                s = s->prox;
            }
        }
        esc = esc->prox;
    }
    printf("\n===========================\n");
}

