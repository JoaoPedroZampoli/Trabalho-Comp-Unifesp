
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "semantic.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int linha;
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
        int num;              
        char* name;           // para ID 
        char op;              // para operadores 
        char* relop;          // para operadores relacionais
    } data;
    struct AST* filhos[4];     // filhos (até 4: tipo, nome, params, corpo, etc.)
    struct AST* irmao;         // irmão (para listas de elementos como declarações, argumentos, etc.)
    int linha;               // linha no código fonte
} AST;

/* --- PROTÓTIPOS DA AST --- */
AST* newNode(NodeType type);
AST* newNumNode(int val);
AST* newIdNode(char* name);
AST* newOpNode(char op, AST* left, AST* right);
AST* newRelNode(char* op, AST* left, AST* right);
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



/* Line 189 of yacc.c  */
#line 190 "parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUM = 258,
     ID = 259,
     ELSE = 260,
     IF = 261,
     INT = 262,
     RETURN = 263,
     VOID = 264,
     WHILE = 265,
     PLUS = 266,
     MINUS = 267,
     TIMES = 268,
     DIV = 269,
     LESSTHAN = 270,
     LESSEQUAL = 271,
     GREATERTHAN = 272,
     GREATEREQUAL = 273,
     EQUAL = 274,
     NOTEQUAL = 275,
     ASSIGN = 276,
     SEMICOLON = 277,
     COMMA = 278,
     LEFTPAREN = 279,
     RIGHTPAREN = 280,
     LEFTBRACKET = 281,
     RIGHTBRACKET = 282,
     LEFTBRACE = 283,
     RIGHTBRACE = 284,
     ERROR = 285,
     LOWER_THAN_ELSE = 286
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 117 "parser.y"

  int num; 
  char* id; 
  struct AST* node;



/* Line 214 of yacc.c  */
#line 265 "parser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 290 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   97

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNRULES -- Number of states.  */
#define YYNSTATES  103

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   286

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    18,    25,
      27,    29,    30,    38,    40,    42,    46,    48,    51,    56,
      61,    64,    65,    68,    69,    71,    73,    75,    77,    79,
      82,    84,    90,    98,   104,   107,   111,   115,   117,   119,
     124,   128,   130,   132,   134,   136,   138,   140,   142,   146,
     148,   150,   152,   156,   158,   160,   162,   166,   168,   170,
     172,   177,   179,   180,   184
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      33,     0,    -1,    34,    -1,    34,    35,    -1,    35,    -1,
      36,    -1,    38,    -1,    37,     4,    22,    -1,    37,     4,
      26,     3,    27,    22,    -1,     7,    -1,     9,    -1,    -1,
      37,     4,    39,    24,    40,    25,    43,    -1,    41,    -1,
       9,    -1,    41,    23,    42,    -1,    42,    -1,    37,     4,
      -1,    37,     4,    26,    27,    -1,    28,    44,    45,    29,
      -1,    44,    36,    -1,    -1,    45,    46,    -1,    -1,    47,
      -1,    43,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      22,    -1,    22,    -1,     6,    24,    51,    25,    46,    -1,
       6,    24,    51,    25,    46,     5,    46,    -1,    10,    24,
      51,    25,    46,    -1,     8,    22,    -1,     8,    51,    22,
      -1,    52,    21,    51,    -1,    53,    -1,     4,    -1,     4,
      26,    51,    27,    -1,    55,    54,    55,    -1,    55,    -1,
      16,    -1,    15,    -1,    17,    -1,    18,    -1,    19,    -1,
      20,    -1,    55,    56,    57,    -1,    57,    -1,    11,    -1,
      12,    -1,    57,    58,    59,    -1,    59,    -1,    13,    -1,
      14,    -1,    24,    51,    25,    -1,    52,    -1,    60,    -1,
       3,    -1,     4,    24,    61,    25,    -1,    62,    -1,    -1,
      62,    23,    51,    -1,    51,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   157,   157,   163,   174,   180,   182,   188,   194,   206,
     208,   214,   214,   226,   228,   234,   245,   251,   257,   268,
     279,   291,   296,   308,   313,   315,   317,   319,   321,   327,
     329,   335,   342,   353,   363,   368,   377,   383,   389,   394,
     404,   410,   416,   418,   420,   422,   424,   426,   432,   438,
     444,   446,   452,   458,   464,   466,   472,   474,   476,   478,
     484,   494,   497,   502,   513
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUM", "ID", "ELSE", "IF", "INT",
  "RETURN", "VOID", "WHILE", "PLUS", "MINUS", "TIMES", "DIV", "LESSTHAN",
  "LESSEQUAL", "GREATERTHAN", "GREATEREQUAL", "EQUAL", "NOTEQUAL",
  "ASSIGN", "SEMICOLON", "COMMA", "LEFTPAREN", "RIGHTPAREN", "LEFTBRACKET",
  "RIGHTBRACKET", "LEFTBRACE", "RIGHTBRACE", "ERROR", "LOWER_THAN_ELSE",
  "$accept", "programa", "declaracao_lista", "declaracao",
  "var_declaracao", "tipo_especificador", "fun_declaracao", "@1", "params",
  "param_lista", "param", "composto_decl", "local_declaracoes",
  "statement_lista", "statement", "expressao_decl", "selecao_decl",
  "iteracao_decl", "retorno_decl", "expressao", "var", "simples_expressao",
  "relacional", "soma_expressao", "soma", "termo", "mult", "fator",
  "ativacao", "args", "arg_lista", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    36,    36,    37,
      37,    39,    38,    40,    40,    41,    41,    42,    42,    43,
      44,    44,    45,    45,    46,    46,    46,    46,    46,    47,
      47,    48,    48,    49,    50,    50,    51,    51,    52,    52,
      53,    53,    54,    54,    54,    54,    54,    54,    55,    55,
      56,    56,    57,    57,    58,    58,    59,    59,    59,    59,
      60,    61,    61,    62,    62
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     3,     6,     1,
       1,     0,     7,     1,     1,     3,     1,     2,     4,     4,
       2,     0,     2,     0,     1,     1,     1,     1,     1,     2,
       1,     5,     7,     5,     2,     3,     3,     1,     1,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       4,     1,     0,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     9,    10,     0,     2,     4,     5,     0,     6,     1,
       3,    11,     7,     0,     0,     0,     0,     0,    10,     0,
       0,    13,    16,     8,    17,     0,     0,     0,    21,    12,
      15,    18,    23,    20,     0,     0,     0,    59,    38,     0,
       0,     0,    30,     0,    19,    25,    22,    24,    26,    27,
      28,     0,    57,    37,    41,    49,    53,    58,    62,     0,
       0,    34,     0,     0,     0,    29,     0,    50,    51,    43,
      42,    44,    45,    46,    47,     0,     0,    54,    55,     0,
      64,     0,    61,     0,     0,    35,     0,    56,    36,    57,
      40,    48,    52,    60,     0,    39,     0,     0,    63,    31,
      33,     0,    32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,     5,     6,     7,     8,    14,    20,    21,
      22,    45,    32,    35,    46,    47,    48,    49,    50,    51,
      52,    53,    75,    54,    76,    55,    79,    56,    57,    81,
      82
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -75
static const yytype_int8 yypact[] =
{
      49,   -75,   -75,    17,    49,   -75,   -75,    57,   -75,   -75,
     -75,   -18,   -75,    35,    33,    38,    53,    44,    42,    64,
      45,    46,   -75,   -75,    47,    43,    49,    48,   -75,   -75,
     -75,   -75,    49,   -75,    68,     3,   -18,   -75,   -14,    52,
      18,    54,   -75,    12,   -75,   -75,   -75,   -75,   -75,   -75,
     -75,    55,    58,   -75,    32,    50,   -75,   -75,    12,    12,
      12,   -75,    59,    12,    60,   -75,    12,   -75,   -75,   -75,
     -75,   -75,   -75,   -75,   -75,    12,    12,   -75,   -75,    12,
     -75,    61,    51,    56,    62,   -75,    63,   -75,   -75,   -75,
      34,    50,   -75,   -75,    12,   -75,    31,    31,   -75,    75,
     -75,    31,   -75
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -75,   -75,   -75,    78,    65,    -2,   -75,   -75,   -75,   -75,
      66,    69,   -75,   -75,   -68,   -75,   -75,   -75,   -75,   -40,
     -74,   -75,   -75,     9,   -75,    13,   -75,    11,   -75,   -75,
     -75
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -15
static const yytype_int8 yytable[] =
{
      62,    89,    89,    64,    12,    89,    37,    38,    13,    39,
      58,    40,    59,    41,    19,    37,    38,     9,    80,    83,
      84,    37,    38,    86,    19,    42,    88,    43,    99,   100,
      34,    28,    44,   102,    37,    38,    43,    39,    15,    40,
      61,    41,    43,    67,    68,    67,    68,    69,    70,    71,
      72,    73,    74,    42,    98,    43,     1,    16,     2,    28,
       1,    11,    18,    77,    78,    17,    23,   -14,    24,    26,
      25,    28,    36,    27,    94,    31,    60,    65,    63,    66,
     101,    85,    10,    95,    90,    87,    93,    96,    97,    91,
      92,     0,    30,     0,    29,     0,     0,    33
};

static const yytype_int8 yycheck[] =
{
      40,    75,    76,    43,    22,    79,     3,     4,    26,     6,
      24,     8,    26,    10,    16,     3,     4,     0,    58,    59,
      60,     3,     4,    63,    26,    22,    66,    24,    96,    97,
      32,    28,    29,   101,     3,     4,    24,     6,     3,     8,
      22,    10,    24,    11,    12,    11,    12,    15,    16,    17,
      18,    19,    20,    22,    94,    24,     7,    24,     9,    28,
       7,     4,     9,    13,    14,    27,    22,    25,     4,    23,
      25,    28,     4,    26,    23,    27,    24,    22,    24,    21,
       5,    22,     4,    27,    75,    25,    25,    25,    25,    76,
      79,    -1,    26,    -1,    25,    -1,    -1,    32
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    33,    34,    35,    36,    37,    38,     0,
      35,     4,    22,    26,    39,     3,    24,    27,     9,    37,
      40,    41,    42,    22,     4,    25,    23,    26,    28,    43,
      42,    27,    44,    36,    37,    45,     4,     3,     4,     6,
       8,    10,    22,    24,    29,    43,    46,    47,    48,    49,
      50,    51,    52,    53,    55,    57,    59,    60,    24,    26,
      24,    22,    51,    24,    51,    22,    21,    11,    12,    15,
      16,    17,    18,    19,    20,    54,    56,    13,    14,    58,
      51,    61,    62,    51,    51,    22,    51,    25,    51,    52,
      55,    57,    59,    25,    23,    27,    25,    25,    51,    46,
      46,     5,    46
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 158 "parser.y"
    { arvoreSintatica = (yyvsp[(1) - (1)].node); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 164 "parser.y"
    {
            AST* t = (yyvsp[(1) - (2)].node);
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = (yyvsp[(2) - (2)].node);
                (yyval.node) = (yyvsp[(1) - (2)].node);
            } else {
                (yyval.node) = (yyvsp[(2) - (2)].node);
            }
        ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 175 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 181 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 183 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 189 "parser.y"
    {
            (yyval.node) = newNode(NODE_VAR_DECL);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (3)].node);
            (yyval.node)->data.name = (yyvsp[(2) - (3)].id);
        ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 195 "parser.y"
    {
            (yyval.node) = newNode(NODE_ARRAY_DECL);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (6)].node);
            (yyval.node)->data.name = (yyvsp[(2) - (6)].id);
            (yyval.node)->filhos[1] = newNumNode((yyvsp[(4) - (6)].num));
        ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 207 "parser.y"
    { (yyval.node) = newNode(NODE_TYPE_INT); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 209 "parser.y"
    { (yyval.node) = newNode(NODE_TYPE_VOID); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 214 "parser.y"
    { (yyval.node) = newNode(NODE_FUN_DECL); (yyval.node)->data.name = (yyvsp[(2) - (2)].id); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 215 "parser.y"
    {
            (yyval.node) = (yyvsp[(3) - (7)].node);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (7)].node);
            (yyval.node)->filhos[1] = (yyvsp[(5) - (7)].node);
            (yyval.node)->filhos[2] = (yyvsp[(7) - (7)].node);
        ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 227 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 229 "parser.y"
    { (yyval.node) = newNode(NODE_TYPE_VOID); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 235 "parser.y"
    {
            AST* t = (yyvsp[(1) - (3)].node);
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = (yyvsp[(3) - (3)].node);
                (yyval.node) = (yyvsp[(1) - (3)].node);
            } else {
                (yyval.node) = (yyvsp[(3) - (3)].node);
            }
        ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 246 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 252 "parser.y"
    {
            (yyval.node) = newNode(NODE_PARAM);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (2)].node);
            (yyval.node)->data.name = (yyvsp[(2) - (2)].id);
        ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 258 "parser.y"
    {
            (yyval.node) = newNode(NODE_ARRAY_PARAM);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (4)].node);
            (yyval.node)->data.name = (yyvsp[(2) - (4)].id);
        ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 269 "parser.y"
    {
            (yyval.node) = newNode(NODE_COMPOUND);
            (yyval.node)->filhos[0] = (yyvsp[(2) - (4)].node);
            (yyval.node)->filhos[1] = (yyvsp[(3) - (4)].node);
        ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 280 "parser.y"
    {
            AST* t = (yyvsp[(1) - (2)].node);
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = (yyvsp[(2) - (2)].node);
                (yyval.node) = (yyvsp[(1) - (2)].node);
            } else {
                (yyval.node) = (yyvsp[(2) - (2)].node);
            }
        ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 291 "parser.y"
    { (yyval.node) = NULL; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 297 "parser.y"
    {
            AST* t = (yyvsp[(1) - (2)].node);
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = (yyvsp[(2) - (2)].node);
                (yyval.node) = (yyvsp[(1) - (2)].node);
            } else {
                (yyval.node) = (yyvsp[(2) - (2)].node);
            }
        ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 308 "parser.y"
    { (yyval.node) = NULL; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 314 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 316 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 318 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 320 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 322 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 328 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 330 "parser.y"
    { (yyval.node) = NULL; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 336 "parser.y"
    {
            (yyval.node) = newNode(NODE_IF);
            (yyval.node)->filhos[0] = (yyvsp[(3) - (5)].node);
            (yyval.node)->filhos[1] = (yyvsp[(5) - (5)].node);
            (yyval.node)->filhos[2] = NULL;
        ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 343 "parser.y"
    {
            (yyval.node) = newNode(NODE_IF);
            (yyval.node)->filhos[0] = (yyvsp[(3) - (7)].node);
            (yyval.node)->filhos[1] = (yyvsp[(5) - (7)].node);
            (yyval.node)->filhos[2] = (yyvsp[(7) - (7)].node);
        ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 354 "parser.y"
    {
            (yyval.node) = newNode(NODE_WHILE);
            (yyval.node)->filhos[0] = (yyvsp[(3) - (5)].node);
            (yyval.node)->filhos[1] = (yyvsp[(5) - (5)].node);
        ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 364 "parser.y"
    {
            (yyval.node) = newNode(NODE_RETURN);
            (yyval.node)->filhos[0] = NULL;
        ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 369 "parser.y"
    {
            (yyval.node) = newNode(NODE_RETURN);
            (yyval.node)->filhos[0] = (yyvsp[(2) - (3)].node);
        ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 378 "parser.y"
    {
            (yyval.node) = newNode(NODE_ASSIGN);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (3)].node);
            (yyval.node)->filhos[1] = (yyvsp[(3) - (3)].node);
        ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 384 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 390 "parser.y"
    {
            (yyval.node) = newNode(NODE_VAR);
            (yyval.node)->data.name = (yyvsp[(1) - (1)].id);
        ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 395 "parser.y"
    {
            (yyval.node) = newNode(NODE_ARRAY_ACCESS);
            (yyval.node)->data.name = (yyvsp[(1) - (4)].id);
            (yyval.node)->filhos[0] = (yyvsp[(3) - (4)].node);
        ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 405 "parser.y"
    {
            (yyval.node) = (yyvsp[(2) - (3)].node);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (3)].node);
            (yyval.node)->filhos[1] = (yyvsp[(3) - (3)].node);
        ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 411 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 417 "parser.y"
    { (yyval.node) = newNode(NODE_RELOP); (yyval.node)->data.relop = strdup("<="); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 419 "parser.y"
    { (yyval.node) = newNode(NODE_RELOP); (yyval.node)->data.relop = strdup("<"); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 421 "parser.y"
    { (yyval.node) = newNode(NODE_RELOP); (yyval.node)->data.relop = strdup(">"); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 423 "parser.y"
    { (yyval.node) = newNode(NODE_RELOP); (yyval.node)->data.relop = strdup(">="); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 425 "parser.y"
    { (yyval.node) = newNode(NODE_RELOP); (yyval.node)->data.relop = strdup("=="); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 427 "parser.y"
    { (yyval.node) = newNode(NODE_RELOP); (yyval.node)->data.relop = strdup("!="); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 433 "parser.y"
    {
            (yyval.node) = (yyvsp[(2) - (3)].node);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (3)].node);
            (yyval.node)->filhos[1] = (yyvsp[(3) - (3)].node);
        ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 439 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 445 "parser.y"
    { (yyval.node) = newNode(NODE_OP); (yyval.node)->data.op = '+'; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 447 "parser.y"
    { (yyval.node) = newNode(NODE_OP); (yyval.node)->data.op = '-'; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 453 "parser.y"
    {
            (yyval.node) = (yyvsp[(2) - (3)].node);
            (yyval.node)->filhos[0] = (yyvsp[(1) - (3)].node);
            (yyval.node)->filhos[1] = (yyvsp[(3) - (3)].node);
        ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 459 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 465 "parser.y"
    { (yyval.node) = newNode(NODE_OP); (yyval.node)->data.op = '*'; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 467 "parser.y"
    { (yyval.node) = newNode(NODE_OP); (yyval.node)->data.op = '/'; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 473 "parser.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 475 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 477 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 479 "parser.y"
    { (yyval.node) = newNumNode((yyvsp[(1) - (1)].num)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 485 "parser.y"
    {
            (yyval.node) = newNode(NODE_CALL);
            (yyval.node)->data.name = (yyvsp[(1) - (4)].id);
            (yyval.node)->filhos[0] = (yyvsp[(3) - (4)].node);
        ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 495 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 497 "parser.y"
    { (yyval.node) = NULL; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 503 "parser.y"
    {
            AST* t = (yyvsp[(1) - (3)].node);
            if (t != NULL) {
                while (t->irmao != NULL) t = t->irmao;
                t->irmao = (yyvsp[(3) - (3)].node);
                (yyval.node) = (yyvsp[(1) - (3)].node);
            } else {
                (yyval.node) = (yyvsp[(3) - (3)].node);
            }
        ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 514 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2175 "parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 517 "parser.y"


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
    node->linha = linha;
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

AST* newRelNode(char* op, AST* left, AST* right) {
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

/*  FUNÇÕES PARA GERAR GRAPHVIZ  */

static int nodeCounter = 0;

/* Retorna o label do nó baseado no tipo */
static const char* getNodeLabel(AST* node, char* aux) {
    switch (node->type) {
        case NODE_PROGRAM:
            sprintf(aux, "Programa");
            break;
        case NODE_VAR_DECL:
            sprintf(aux, "VarDecl\\n%s", node->data.name);
            break;
        case NODE_ARRAY_DECL:
            sprintf(aux, "ArrayDecl\\n%s", node->data.name);
            break;
        case NODE_FUN_DECL:
            sprintf(aux, "FunDecl\\n%s", node->data.name);
            break;
        case NODE_PARAM:
            sprintf(aux, "Param\\n%s", node->data.name);
            break;
        case NODE_ARRAY_PARAM:
            sprintf(aux, "ArrayParam\\n%s[]", node->data.name);
            break;
        case NODE_COMPOUND:
            sprintf(aux, "Bloco\\n{ }");
            break;
        case NODE_IF:
            sprintf(aux, "If");
            break;
        case NODE_WHILE:
            sprintf(aux, "While");
            break;
        case NODE_RETURN:
            sprintf(aux, "Return");
            break;
        case NODE_ASSIGN:
            sprintf(aux, "=");
            break;
        case NODE_OP:
            sprintf(aux, "%c", node->data.op);
            break;
        case NODE_RELOP:
            sprintf(aux, "%s", node->data.relop);
            break;
        case NODE_VAR:
            sprintf(aux, "Var\\n%s", node->data.name);
            break;
        case NODE_ARRAY_ACCESS:
            sprintf(aux, "Array\\n%s[]", node->data.name);
            break;
        case NODE_CALL:
            sprintf(aux, "Call\\n%s()", node->data.name);
            break;
        case NODE_NUM:
            sprintf(aux, "%d", node->data.num);
            break;
        case NODE_TYPE_INT:
            sprintf(aux, "int");
            break;
        case NODE_TYPE_VOID:
            sprintf(aux, "void");
            break;
        default:
            sprintf(aux, "?");
            break;
    }
    return aux;
}

/* Retorna a cor do nó baseado no tipo */
static const char* getNodeColor(AST* node) {
    switch (node->type) {
        case NODE_FUN_DECL:
            return "#FFD700";  /* funções */
        case NODE_VAR_DECL:
        case NODE_ARRAY_DECL:
            return "#98FB98";  /* declarações */
        case NODE_IF:
        case NODE_WHILE:
            return "#87CEEB";  /* controle de fluxo */
        case NODE_RETURN:
            return "#FFA07A";  /* return */
        case NODE_OP:
        case NODE_RELOP:
            return "#DDA0DD";  /* operadores */
        case NODE_CALL:
            return "#F0E68C";  /* chamadas */
        case NODE_NUM:
            return "#FFFFFF";  /* números */
        case NODE_VAR:
        case NODE_ARRAY_ACCESS:
            return "#E0FFFF";  /* variáveis */
        case NODE_TYPE_INT:
        case NODE_TYPE_VOID:
            return "#D3D3D3";  /* tipos */
        case NODE_COMPOUND:
            return "#F5F5DC";  /* blocos */
        default:
            return "#FFFFFF";
    }
}

/* Gera os nós recursivamente */
static int generateDotNodes(FILE* fp, AST* node) {
    if (node == NULL) return -1;
    
    int myId = nodeCounter++;
    char labelaux[128];
    
    fprintf(fp, "  node%d [label=\"%s\", style=filled, fillcolor=\"%s\"];\n",
            myId, getNodeLabel(node, labelaux), getNodeColor(node));
    
    /* processa filhos */
    for (int i = 0; i < 4; i++) {
        if (node->filhos[i] != NULL) {
            int childId = generateDotNodes(fp, node->filhos[i]);
            fprintf(fp, "  node%d -> node%d;\n", myId, childId);
        }
    }
    
    /* processa irmãos (e coloca seta tracejada) */
    if (node->irmao != NULL) {
        int siblingId = generateDotNodes(fp, node->irmao);
        fprintf(fp, "  node%d -> node%d [style=dashed, color=gray];\n", myId, siblingId);
    }
    
    return myId;
}

/* gera o arquivo .dot  */
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
            /* declarações de variáveis não geram código no intermediário */
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
            genArgs(node->filhos[0]);
            sprintf(numStr, "%d", countArgs(node->filhos[0]));
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
                        tokenInesperado, tokenEsperado, linha);
            } else {
                fprintf(stderr, "ERRO SINTATICO: token inesperado '%s' - LINHA: %d\n", 
                        tokenInesperado, linha);
            }
            return;
        }
    }
    
    fprintf(stderr, "ERRO SINTATICO: %s - LINHA: %d\n", s, linha);
}

