#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    TIPO_INT,
    TIPO_VOID,
    TIPO_ERRO
} Tipo;

typedef enum {
    SIMP_VAR,
    SIMP_ARRAY,
    SIMP_FUNC,
    SIMP_PARAM
} Classe;

typedef struct Simbolo {
    char *nome;
    Tipo tipo;
    Classe classe;
    int tamanho;
    int lineno;
    struct Simbolo *prox;
} Simbolo;

void entra_escopo(void);
void sai_escopo(void);
void insere_simbolo(char *nome, Tipo tipo, Classe classe, int tamanho, int lineno_val);
void volta_escopo_global(void);
void imprime_tabela(void);
Simbolo *busca_simbolo(char *nome);

#endif
