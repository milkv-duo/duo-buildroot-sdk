%{
#include <stdio.h>
#include <string.h>
typedef enum {
    IFDEF_IFDEF,
    IFDEF_IFNDEF,
    IFDEF_IF,
} IFDEF_TYPE;
typedef enum {
    EXPR_SCALAR,
    EXPR_FUNC,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_TRINARY,
} EXPR_TYPE; 

typedef enum {
    OP_DEFINED,
    OP_PLUS,
    OP_MINUS,
    OP_MUL,
    OP_DIV,
    OP_LEFT_SHIFT,
    OP_RIGHT_SHIFT,
    OP_MOD,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_BIT_AND,
    OP_BIT_OR,
    OP_BIT_NOT,
    OP_INCLUSIVE,
    OP_EXCLUSIVE,
    OP_LESS,
    OP_GREAT,
    OP_EQUAL,
    OP_LE,
    OP_GE,
    OP_NOT_EQUAL,
} OP_TYPE;

typedef struct expr
{
    EXPR_TYPE type;
    OP_TYPE op;
    int value;
	int defined;
	int specified;
    struct expr *left;
    struct expr *right;
    struct expr *tri;
} EXPR;

typedef struct _symbol
{
	int  defined;
    char *name;
    char *defined_filename;
    int   defined_lineno;
    EXPR *expr;
    struct _symbol *next;
} SYMBOL;
SYMBOL *symbol_define_list = NULL;
SYMBOL *symbol_not_define_list = NULL;

typedef enum
{
    STMT_STRING,
    STMT_IFDEF,
} StatementType;

typedef struct statement
{
    StatementType type;
    struct ifdef_statement *ifdef;
    char *string;
    char *fname;
    int   lineno;
    struct statement *next;
} Statement;
Statement *g_first_stmt = 0;

typedef struct ifdef_statement
{
	int specified; // specified in xxx.conf or not
    int condition;
    IFDEF_TYPE type;
    char *ifdef_string;
    char *else_string;
    char *endif_string;
    Statement *true_stmt;
    Statement *false_stmt;
    char *fname;
    int  lineno;
} IFDEFstatement;

EXPR *symbol_get_expr(char *name);
EXPR *create_defined_expr(char *name);
void clean_ifdef(IFDEFstatement *ifdef);
void clean(Statement *stmt);
void change_file();
EXPR *create_scalar_expr(int value);
EXPR *create_unary_expr(OP_TYPE op, EXPR *_expr);
EXPR *create_binary_expr(EXPR *expr1, OP_TYPE op, EXPR *expr2);
EXPR *create_trinary_expr(EXPR *expr1, EXPR *expr2, EXPR *expr3);
int evaluate_func(OP_TYPE op, EXPR *expr);
int evaluate_unary(OP_TYPE op, EXPR *expr);
int evaluate_binary(EXPR *left, OP_TYPE op, EXPR *right);
int evaluate_trinary(EXPR *expr1, EXPR *expr2, EXPR *expr3);
int expr_evaluate(EXPR *expr);
Statement *create_statement(char *string, IFDEFstatement *ifdef);
IFDEFstatement *create_ifdef_statement(IFDEF_TYPE type, EXPR *expr, Statement *s1, Statement *s2, int has_else);
Statement *add_statement_list(Statement *list, Statement *s);
void dump_ifdef_all(FILE *fp, IFDEFstatement *ifdef);
void dump(FILE *fp, Statement *s);
void output_file(FILE *fout);

extern char *pop_ifdef_string();
extern char *pop_else_string();
extern char *pop_endif_string();
extern int  ifdef_remove;
extern int  evaluate_num;

%}

%union 
{
    char *string;
    int  num;
    double real;
    void *ptr;
}
/*
%type <ptr>     defined_expression defined_unit
%type <ptr>     ifndef_identifier ifdef_identifier
%type  <string> macro_name  
*/
%type  <ptr>    expression precompile precompile_list ifdef_statement 
%type  <ptr>    primary_expression 
%type  <ptr>    postfix_expression
%type  <ptr>    unary_expression
%type  <ptr>    cast_expression
%type  <ptr>    multiplicative_expression
%type  <ptr>    additive_expression
%type  <ptr>    shift_expression
%type  <ptr>    relational_expression
%type  <ptr>    equality_expression
%type  <ptr>    and_expression
%type  <ptr>    exclusive_or_expression
%type  <ptr>    inclusive_or_expression
%type  <ptr>    logical_and_expression
%type  <ptr>    logical_or_expression
%type  <ptr>    conditional_expression
/* %type  <ptr>    assignment_expression 
   %type  <ptr>    constant_expression */

%token <string> IDENTIFIER STRING_LITERAL
/* %token <string> ID_WITH_ARG FILENAME */
%token <string> STRING
%token <num>    CONSTANT 
%token NL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME ELLIPSIS
/*
%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM 
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
*/
%token IFDEF IFNDEF ENDIF DEFINE DEFINED UNDEFINE IF2 ELSE2 INCLUDE OTHER_WORD MACRO_CONCAT 

%start start_point

%left OR_OP AND_OP

%%

primary_expression
    : IDENTIFIER
    { $$ = symbol_get_expr($1); }
    | CONSTANT
    { $$ = create_scalar_expr($1); }
    | DEFINED '(' IDENTIFIER ')'
    { $$ = create_defined_expr($3); }
    | STRING_LITERAL
    { $$ = 0; }
    | '(' ')'
    { $$ = 0; }
    | '(' expression ')'
    { $$ = $2; }

	;
 
postfix_expression
    : primary_expression
    { $$ = $1; }
    | postfix_expression '(' argument_expression_list ')'
    { $$ = $1; }
    /*
    | postfix_expression '[' expression ']'
    { $$ = 0; }
    | postfix_expression '(' ')'
    { $$ = 0; }
    | postfix_expression '.' IDENTIFIER
    { $$ = 0; }
    | postfix_expression PTR_OP IDENTIFIER
    { $$ = 0; }
    | postfix_expression INC_OP
    { $$ = 0; }
    | postfix_expression DEC_OP
    { $$ = 0; }
    */
    ;
argument_expression_list
    : conditional_expression
    | argument_expression_list ',' conditional_expression
    /*
    : argument_expression_list ',' assignment_expression
    | assignment_expression
    */
    ;

unary_expression
    : postfix_expression
    { $$ = $1; }
    | INC_OP unary_expression
    { $$ = 0; }
    | DEC_OP unary_expression
    { $$ = 0; }
    | '&' cast_expression
    { $$ = 0; }
    | '*' cast_expression
    { $$ = 0; }
    | '+' cast_expression
    { $$ = create_unary_expr(OP_PLUS, $2); }
    | '-' cast_expression
    { $$ = create_unary_expr(OP_MINUS, $2); }
    | '~' cast_expression
    { $$ = create_unary_expr(OP_BIT_NOT, $2); }
    | '!' cast_expression
    { $$ = create_unary_expr(OP_NOT, $2); }
    | SIZEOF unary_expression
    { $$ = 0; }
    /*
    | SIZEOF '(' type_name ')'
    { $$ = 0; }
    */
    ;


cast_expression
    : unary_expression
    { $$ = $1; }
    /*
    | '(' type_name ')' cast_expression
    { $$ = 0; }
    */
    ;

multiplicative_expression
    : cast_expression
    { $$ = $1; }
    | multiplicative_expression '*' cast_expression
    { $$ = create_binary_expr($1, OP_MUL, $3); } 
    | multiplicative_expression '/' cast_expression
    { $$ = create_binary_expr($1, OP_DIV, $3); } 
    | multiplicative_expression '%' cast_expression
    { $$ = create_binary_expr($1, OP_MOD, $3); } 
    ;

additive_expression
    : multiplicative_expression
    { $$ = $1; }
    | additive_expression '+' multiplicative_expression
    { $$ = create_binary_expr($1, OP_PLUS, $3); } 
    | additive_expression '-' multiplicative_expression
    { $$ = create_binary_expr($1, OP_MINUS, $3); } 
    ;

shift_expression
    : additive_expression
    { $$ = $1; }
    | shift_expression LEFT_OP additive_expression
    { $$ = create_binary_expr($1, OP_LEFT_SHIFT, $3); } 
    | shift_expression RIGHT_OP additive_expression
    { $$ = create_binary_expr($1, OP_RIGHT_SHIFT, $3); } 
    ;

relational_expression
    : shift_expression
    { $$ = $1; }
    | relational_expression '<' shift_expression
    { $$ = create_binary_expr($1, OP_LESS, $3); } 
    | relational_expression '>' shift_expression
    { $$ = create_binary_expr($1, OP_GREAT, $3); } 
    | relational_expression LE_OP shift_expression
    { $$ = create_binary_expr($1, OP_LE, $3); } 
    | relational_expression GE_OP shift_expression
    { $$ = create_binary_expr($1, OP_GE, $3); } 
    ;

equality_expression
    : relational_expression
    { $$ = $1; }
    | equality_expression EQ_OP relational_expression
    { $$ = create_binary_expr($1, OP_EQUAL, $3); } 
    | equality_expression NE_OP relational_expression
    { $$ = create_binary_expr($1, OP_NOT_EQUAL, $3); } 
    ;

and_expression
    : equality_expression
    { $$ = $1; }
    | and_expression '&' equality_expression
    { $$ = create_binary_expr($1, OP_BIT_AND, $3); } 
    ;

exclusive_or_expression
    : and_expression
    { $$ = $1; }
    | exclusive_or_expression '^' and_expression
    { $$ = create_binary_expr($1, OP_EXCLUSIVE, $3); } 
    ;

inclusive_or_expression
    : exclusive_or_expression
    { $$ = $1; }
    | inclusive_or_expression '|' exclusive_or_expression
    { $$ = create_binary_expr($1, OP_INCLUSIVE, $3); } 
    ;

logical_and_expression
    : inclusive_or_expression
    { $$ = $1; }
    | logical_and_expression AND_OP inclusive_or_expression
    { $$ = create_binary_expr($1, OP_AND, $3); } 
    ;

logical_or_expression
    : logical_and_expression
    { $$ = $1; } 
    | logical_or_expression OR_OP logical_and_expression
    { $$ = create_binary_expr($1, OP_OR, $3); } 
    ;

conditional_expression
    : logical_or_expression
    { $$ = $1; }
    | logical_or_expression '?' expression ':' conditional_expression
    { $$ = create_trinary_expr($1, $3, $5); } 
    ;
/*
assignment_expression
    : conditional_expression
    { $$ = $1; }
    | unary_expression assignment_operator assignment_expression
    { $$ = 0; } 
    ;

assignment_operator
    : '='
    | MUL_ASSIGN
    | DIV_ASSIGN
    | MOD_ASSIGN
    | ADD_ASSIGN
    | SUB_ASSIGN
    | LEFT_ASSIGN
    | RIGHT_ASSIGN
    | AND_ASSIGN
    | XOR_ASSIGN
    | OR_ASSIGN
    ;
*/

expression
    : conditional_expression
    { $$ = $1; }
    /* 
    : assignment_expression
    { $$ = $1; }
    | expression ',' assignment_expression
    { $$ = 0; } 
    */
    ;
/*
constant_expression
    : conditional_expression
    { $$ = $1; }
    ;
declaration
    : declaration_specifiers ';'
    | declaration_specifiers init_declarator_list ';'
    ;

declaration_specifiers
    : storage_class_specifier
    | storage_class_specifier declaration_specifiers
    | type_specifier
    | type_specifier declaration_specifiers
    | type_qualifier
    | type_qualifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list ',' init_declarator
    ;

init_declarator
    : declarator
    | declarator '=' initializer
    ;

storage_class_specifier
    : TYPEDEF
    | EXTERN
    | STATIC
    | AUTO
    | REGISTER
    ;

type_specifier
    : VOID
    | CHAR
    | SHORT
    | INT
    | LONG
    | FLOAT
    | DOUBLE
    | SIGNED
    | UNSIGNED
    | struct_or_union_specifier
    | enum_specifier
    ;

struct_or_union_specifier
    : struct_or_union IDENTIFIER '{' struct_declaration_list '}'
    | struct_or_union '{' struct_declaration_list '}'
    | struct_or_union IDENTIFIER
    ;

struct_or_union
    : STRUCT
    | UNION
    ;

struct_declaration_list
    : struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
    ;

specifier_qualifier_list
    : type_specifier specifier_qualifier_list
    | type_specifier
    | type_qualifier specifier_qualifier_list
    | type_qualifier
    ;

struct_declarator_list
    : struct_declarator
    | struct_declarator_list ',' struct_declarator
    ;

struct_declarator
    : declarator
    | ':' constant_expression
    | declarator ':' constant_expression
    ;

enum_specifier
    : ENUM '{' enumerator_list '}'
    | ENUM IDENTIFIER '{' enumerator_list '}'
    | ENUM IDENTIFIER
    ;

enumerator_list
    : enumerator
    | enumerator_list ',' enumerator
    ;

enumerator
    : IDENTIFIER
    | IDENTIFIER '=' constant_expression
    ;

type_qualifier
    : CONST
    | VOLATILE
    ;

declarator
    : pointer direct_declarator
    | direct_declarator
    ;

direct_declarator
    : IDENTIFIER
    | '(' declarator ')'
    | direct_declarator '[' constant_expression ']'
    | direct_declarator '[' ']'
    | direct_declarator '(' parameter_type_list ')'
    | direct_declarator '(' identifier_list ')'
    | direct_declarator '(' ')'
    ;

pointer
    : '*'
    | '*' type_qualifier_list
    | '*' pointer
    | '*' type_qualifier_list pointer
    ;

type_qualifier_list
    : type_qualifier
    | type_qualifier_list type_qualifier
    ;


parameter_type_list
    : parameter_list
    | parameter_list ',' ELLIPSIS
    ;

parameter_list
    : parameter_declaration
    | parameter_list ',' parameter_declaration
    ;

parameter_declaration
    : declaration_specifiers declarator
    | declaration_specifiers abstract_declarator
    | declaration_specifiers
    ;

identifier_list
    : IDENTIFIER
    | identifier_list ',' IDENTIFIER
    ;

type_name
    : specifier_qualifier_list
    | specifier_qualifier_list abstract_declarator
    ;

abstract_declarator
    : pointer
    | direct_abstract_declarator
    | pointer direct_abstract_declarator
    ;

direct_abstract_declarator
    : '(' abstract_declarator ')'
    | '[' ']'
    | '[' constant_expression ']'
    | direct_abstract_declarator '[' ']'
    | direct_abstract_declarator '[' constant_expression ']'
    | '(' ')'
    | '(' parameter_type_list ')'
    | direct_abstract_declarator '(' ')'
    | direct_abstract_declarator '(' parameter_type_list ')'
    ;

initializer
    : assignment_expression
    | '{' initializer_list '}'
    | '{' initializer_list ',' '}'
    ;

initializer_list
    : initializer
    | initializer_list ',' initializer
    ;

statement
    : labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    ;

labeled_statement
    : IDENTIFIER ':' statement
    | CASE constant_expression ':' statement
    | DEFAULT ':' statement
    ;

compound_statement
    : '{' '}'
    | '{' statement_list '}'
    | '{' declaration_list '}'
    | '{' declaration_list statement_list '}'
    ;

declaration_list
    : declaration
    | declaration_list declaration
    ;

statement_list
    : statement
    | statement_list statement
    ;

expression_statement
    : ';'
    | expression ';'
    ;

selection_statement
    : IF '(' expression ')' statement
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;

iteration_statement
    : WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' 
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_statement expression_statement ')' statement
    | FOR '(' expression_statement expression_statement expression ')' statement
    ;

jump_statement
    : GOTO IDENTIFIER ';'
    | CONTINUE ';'
    | BREAK ';'
    | RETURN ';'
    | RETURN expression ';'
    | RETURN expression 
    ;
translation_unit
    : external_declaration
    | translation_unit external_declaration
    ;
*/
precompile
    : ifdef_statement
    { $$ = create_statement(0, $1); } 
    | STRING
    { $$ = create_statement($1, 0); }
/*
    : include_statement
    | define_statement
*/

    ;
/*
%type  <string> string_list
string_list
    : STRING
    { $$ = $1; }
    | STRING string_list
    { $$ = merge_string($1, $2); }
    ;
*/
start_point
    : 
    | precompile_list
    { g_first_stmt = $1; }
    ;
precompile_list
    : precompile
    { $$ = add_statement_list(0, $1); }
    | precompile_list precompile
    { $$ = add_statement_list($1, $2); }
    ;
/*
include_statement
    : INCLUDE STRING_LITERAL
    { if (!process_include_file($2)) yyerror($2); }
    | INCLUDE '<' FILENAME '>'
    { if (!process_include_file($3)) yyerror($3); }
    ;

define_statement
    : DEFINE macro_name
    { create_symbol($2); }
    | DEFINE macro_name declaration_specifiers
    { create_symbol($2); }
    | UNDEFINE IDENTIFIER
    { remove_symbol($2); }
    | DEFINE macro_name expression 
    { assign_symbol($2, $3); }
    | DEFINE macro_name statement_list
    { create_symbol($2); }
    | DEFINE macro_name declaration
    { create_symbol($2); }
    ;
macro_name
    : IDENTIFIER
      { printf("\nmacro name=%s...\n", $1); $$ = $1;}
    | ID_WITH_ARG ')'
      { $$ = 0; }
    | ID_WITH_ARG argument_expression_list ')'
      { $$ = 0; }
    ;

replace_token_list
    : replace_token
    | replace_token_list replace_token 
    ;
replace_token
    : macro_name
    | CONSTANT
    ;
*/

ifdef_statement
    : IF2 expression ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, 0, 0, 0); }
    | IF2 expression precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, $3, 0, 0); }
    | IF2 expression ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, 0, 0, 1); }
    | IF2 expression precompile_list ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, $3, 0, 1); }
    | IF2 expression ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, 0, $4, 1); }
    | IF2 expression precompile_list ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, $3, $5, 1); }
    | IFDEF expression ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, 0, 0, 0); }
    | IFDEF expression precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, $3, 0, 0); }
    | IFDEF expression ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, 0, 0, 1); }
    | IFDEF expression precompile_list ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, $3, 0, 1); }
    | IFDEF expression ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, 0, $4, 1); }
    | IFDEF expression precompile_list ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, $3, $5, 1); }
    | IFNDEF expression ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, 0, 0, 0); }
    | IFNDEF expression precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, $3, 0, 0); }
    | IFNDEF expression ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, 0, 0, 1); }
    | IFNDEF expression precompile_list ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, $3, 0, 1); }
    | IFNDEF expression ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, 0, $4, 1); }
    | IFNDEF expression precompile_list ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, $3, $5, 1); }
    ;

/*
ifdef_statement
    : IF2 defined_expression ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, 0, 0, 0); }
    | IF2 defined_expression precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, $3, 0, 0); }
    | IF2 defined_expression ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, 0, 0, 1); }
    | IF2 defined_expression precompile_list ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, $3, 0, 1); }
    | IF2 defined_expression ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, 0, $4, 1); }
    | IF2 defined_expression precompile_list ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IF, $2, $3, $5, 1); }
    | IFDEF ifdef_identifier ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, 0, 0, 0); }
    | IFDEF ifdef_identifier precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, $3, 0, 0); }
    | IFDEF ifdef_identifier ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, 0, 0, 1); }
    | IFDEF ifdef_identifier precompile_list ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, $3, 0, 1); }
    | IFDEF ifdef_identifier ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, 0, $4, 1); }
    | IFDEF ifdef_identifier precompile_list ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFDEF, $2, $3, $5, 1); }
    | IFNDEF ifndef_identifier ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, 0, 0, 0); }
    | IFNDEF ifndef_identifier precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, $3, 0, 0); }
    | IFNDEF ifndef_identifier ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, 0, 0, 1); }
    | IFNDEF ifndef_identifier precompile_list ELSE2 ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, $3, 0, 1); }
    | IFNDEF ifndef_identifier ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, 0, $4, 1); }
    | IFNDEF ifndef_identifier precompile_list ELSE2 precompile_list ENDIF
    { $$ = create_ifdef_statement(IFDEF_IFNDEF, $2, $3, $5, 1); }
    ;

ifdef_identifier
    : IDENTIFIER
    { $$ = symbol_evaluate($1); }
    ;
ifndef_identifier
    : IDENTIFIER
    { $$ = symbol_evaluate($1); }
    ;

defined_expression
    : defined_unit
    { $$ = $1; } 
    | '(' defined_expression ')'
    { $$ = $2; } 
    | defined_unit AND_OP defined_expression
    { $$ = ($1 && $3) ? 1 : 0; }
    | defined_unit OR_OP  defined_expression
    { $$ = ($1 || $3) ? 1 : 0; }
    ;

defined_unit
    : DEFINED '(' IDENTIFIER ')'
    { $$ = symbol_defined($3); }
    | '!' DEFINED '(' IDENTIFIER ')'
    { $$ = symbol_not_defined($4); }
    | expression
    { $$ = 1; }
    ;
	*/

/*
external_declaration
    : function_definition
    | declaration
    : precompile_list
    ;

code_body
    : replace_token_list
    ;

function_definition
    : declaration_specifiers declarator declaration_list compound_statement
    | declaration_specifiers declarator compound_statement
    | declarator declaration_list compound_statement
    | declarator compound_statement
    ;
*/
%%
#include <stdio.h>

extern char yytext[];
extern FILE *yyin;
extern int column;
extern int lineno;
extern int process_include_file(char *fname);
extern char *current_file_name();

yyerror(s)
char *s;
{
    fflush(stderr);
    fprintf(stderr, "\n%*s\n%*s\n", column, "^", column, s);
    if (!current_file_name())
        return;

    fprintf(stderr, "==>%s: at line [%d]\n", 
            current_file_name(), lineno);
}
SYMBOL *symbol_lookup(char *name)
{
    SYMBOL *symbol = 0;
    //printf("===> symbol_lookup, name=%s, at L(%d)\n", name, lineno);
    for (symbol = symbol_define_list; symbol; symbol = symbol->next)
    {
        if (!strcmp(name, symbol->name))
        {
            //printf("find symbol:%s, value=%p\n", symbol->name, symbol->expr);
            return symbol;
        }
    }
    for (symbol = symbol_not_define_list; symbol; symbol = symbol->next)
    {
        if (!strcmp(name, symbol->name))
        {
            //printf("find symbol:%s, value=%p\n", symbol->name, symbol->expr);
            return symbol;
        }
    }

    //printf("Symbol(%s) not found\n", name);
    return 0;
}

void remove_symbol(char *name)
{
    SYMBOL *symbol = NULL, *prev = NULL;
    printf("===> remove_lookup\n");
    for (symbol = symbol_define_list; symbol; symbol = symbol->next)
    {
        if (!strcmp(name, symbol->name))
        {
            if (prev) 
                prev->next = symbol->next;
            else
                symbol_define_list = symbol->next;

            symbol->next = NULL;
            free(symbol->name);
            free(symbol);
            break;
        }
        prev = symbol;
    }
    printf("ERROR! can't undef symbol:%s\n", name);
    printf("<=== remove_lookup\n");

}

SYMBOL *create_symbol(char *name, int defined)
{
    SYMBOL *symbol;

    if (!name) return 0;
    symbol = symbol_lookup(name);
    if (symbol)
    {
        printf("ERROR! symbol (%s) redefined at L%d, original at L%d\n",
                name, lineno, symbol->defined_lineno);
        return NULL;
    }
    symbol = malloc(sizeof(SYMBOL));
    symbol->name = strdup(name);
    symbol->defined_lineno   = lineno;
    symbol->defined_filename = current_file_name();
    symbol->expr = 0;
	symbol->defined = defined;
	
	if (defined)
	{
		symbol->next = symbol_define_list;
		symbol_define_list = symbol;
	}
	else
	{
		symbol->next = symbol_not_define_list;
		symbol_not_define_list = symbol;
	}

    return symbol;
}

void assign_symbol(char *name, EXPR *expr)
{
    SYMBOL *symbol = create_symbol(name, 1); 

    if (!symbol)
	{
        printf("ERROR! assign null symbol(%s).\n", name);
        return;
	}
    symbol->expr = expr;
}

EXPR default_specified_expr = {
	.specified = 1,
	.defined   = 0,
    .type  = EXPR_SCALAR,
    .op    = OP_EQUAL,
    .value = 1,
    .left  = 0,
    .right = 0,
    .tri   = 0,
};

EXPR default_not_specified_expr = {
	.specified = 0,
	.defined   = 1,
    .type  = EXPR_SCALAR,
    .op    = OP_EQUAL,
    .value = 1,
    .left  = 0,
    .right = 0,
    .tri   = 0,
};


EXPR *symbol_get_expr(char *name)
{
    SYMBOL *symbol = symbol_lookup(name);
	EXPR *expr = 0;
    //printf("===> symbol_get_expr:%s\n", name);


    if (symbol)
	{
		if (!symbol->expr)
	        expr = &default_specified_expr;
		else
			expr = symbol->expr;

		expr->defined = symbol->defined;
        symbol->expr = expr;
	}
	else
	{
	    expr = &default_not_specified_expr;
	}

    return expr;
}

/*
int symbol_evaluate(char *name)
{
    SYMBOL *symbol;

    symbol = symbol_lookup(name);

    if (!symbol)
    {
        printf("ERROR! symbol %s not defined at L%d.\n", name, lineno);
        return 0;
    }
    if (!symbol->expr)
        return 0;

    return expr_evaluate(symbol->expr);
}
*/

EXPR *create_defined_expr(char *name)
{
    EXPR *expr = malloc(sizeof(EXPR));
	memset(expr, 0, sizeof(EXPR));
	//printf("create defined expr: %s\n", name);
    expr->type = EXPR_FUNC;
    expr->op = OP_DEFINED;
    expr->left = symbol_get_expr(name);
	expr->specified = expr->left->specified;
	expr->defined   = expr->left->defined;
    expr->value = 0;
    return expr;
}


EXPR *create_scalar_expr(int value)
{
    EXPR *expr = malloc(sizeof(EXPR));
	memset(expr, 0, sizeof(EXPR));
    expr->type = EXPR_SCALAR;
    expr->value = value;
	expr->defined = 1;
	if (evaluate_num)
	    expr->specified = 1;
	else
	    expr->specified = 0;
    return expr;
}

EXPR *create_unary_expr(OP_TYPE op, EXPR *_expr)
{
    EXPR *expr = (EXPR *)malloc(sizeof(EXPR));
	memset(expr, 0, sizeof(EXPR));
    expr->type = EXPR_UNARY;
    expr->op = op;
    expr->left = _expr;
	expr->specified = expr->left->specified;
	expr->defined   = expr->left->defined;
    return expr;
}

EXPR *create_binary_expr(EXPR *expr1, OP_TYPE op, EXPR *expr2)
{
    EXPR *expr = 0;

	if (!expr1 && !expr2)
	{
		printf("ERROR, invalid binary expr, at %s, L(%d)\n",
				current_file_name(), lineno);
		return 0;
	}
	expr = malloc(sizeof(EXPR));
	memset(expr, 0, sizeof(EXPR));
    expr->type = EXPR_BINARY;
    expr->op = op;
    expr->left = expr1;
    expr->right= expr2;

	if (!expr1)
	{
		expr->defined = expr2->defined;
		expr->specified = expr2->specified;
	}
	else if (!expr2)
	{
		expr->defined = expr1->defined;
		expr->specified = expr1->specified;
	}
	else
	{
		expr->defined = (expr1->defined) && (expr2->defined);
		expr->specified = (expr1->specified) && (expr2->specified);
	}
    return expr;
}

EXPR *create_trinary_expr(EXPR *expr1, EXPR *expr2, EXPR *expr3)
{
    EXPR *expr = 0;
	if (!expr1 || (!expr2 && !expr3))
	{
		printf("ERROR, invalid tri expr, at %s, L(%d)\n",
				current_file_name(), lineno);
	}
	expr = malloc(sizeof(EXPR));
	memset(expr, 0, sizeof(EXPR));
    expr->type = EXPR_TRINARY;
    expr->left = expr1;
    expr->right= expr2;
    expr->tri = expr3;
	if (!expr3)
	{
		expr->defined = expr2->defined;
		expr->specified = expr2->specified;
	}
	else if (!expr2)
	{
		expr->defined = expr3->defined;
		expr->specified = expr3->specified;
	}
	else
	{
		expr->defined = (expr2->defined) && (expr3->defined);
		expr->specified = (expr2->specified) && (expr3->specified);
	}
    return expr;
}

int evaluate_func(OP_TYPE op, EXPR *expr)
{
    int value;
    if (!expr)
    {
        printf("Invalid function evaluate (op=%d) at L%d\n", op, lineno);
        return 0;
    }

    value = expr_evaluate(expr);
    switch(op)
    {
        case OP_DEFINED:
            return expr->defined;
        default:
            printf("ERROR! unknown function op:%d, at L%d\n", (int)op, lineno);
    }
	return 0;
}

int evaluate_unary(OP_TYPE op, EXPR *expr)
{
    int value;
    if (!expr)
    {
        printf("Invalid unary evaluate (op=%d) at L%d\n", op, lineno);
        return 0;
    }

    value = expr_evaluate(expr);
    switch(op)
    {
        case OP_PLUS:
            return value;
        case OP_MINUS:
            return -value;
        case OP_NOT:
            return !value;;
        case OP_BIT_NOT:
            return ~value;
        default:
            printf("ERROR! unknown unary op:%d, at L%d\n", (int)op, lineno);
    }
}

int evaluate_binary(EXPR *left, OP_TYPE op, EXPR *right)
{
    int value, lvalue, rvalue;
    if (!left || !right)
    {
        printf("Invalid binary evaluate (op=%d) at L%d\n", op, lineno);
        return 0;
    }

    switch(op)
    {
        case OP_PLUS:
            return expr_evaluate(left) + expr_evaluate(right);
        case OP_MINUS:
            return expr_evaluate(left) - expr_evaluate(right);
        case OP_MUL:
            return expr_evaluate(left) * expr_evaluate(right);
        case OP_DIV:
            value = expr_evaluate(right);
            if (!value)
            {
                printf("ERROR! divided by zero at L%d\n", lineno);
                return 0;
            }
            return expr_evaluate(left) / value;
        case OP_LEFT_SHIFT:
            return expr_evaluate(left) << expr_evaluate(right);
        case OP_RIGHT_SHIFT:
            return expr_evaluate(left) >> expr_evaluate(right);
        case OP_MOD:
            return expr_evaluate(left) % expr_evaluate(right);
        case OP_AND:
            return expr_evaluate(left) && expr_evaluate(right);
        case OP_OR:
            return expr_evaluate(left) || expr_evaluate(right);
        case OP_BIT_AND:
            return expr_evaluate(left) & expr_evaluate(right);
        case OP_BIT_OR:
        case OP_INCLUSIVE:
            return expr_evaluate(left) | expr_evaluate(right);
        case OP_EXCLUSIVE:
            return expr_evaluate(left) ^ expr_evaluate(right);
        case OP_LESS:
            return expr_evaluate(left) < expr_evaluate(right) ? 1 : 0;
        case OP_GREAT:
			/*
			lvalue = expr_evaluate(left);
			rvalue = expr_evaluate(right);
			printf("OP_GREAT left=%d, right=%d\n", lvalue, rvalue);
			*/
            return expr_evaluate(left) > expr_evaluate(right) ? 1 : 0;
        case OP_EQUAL:
            return expr_evaluate(left) == expr_evaluate(right) ? 1 : 0;
        case OP_LE:
            return expr_evaluate(left) <= expr_evaluate(right) ? 1 : 0;
        case OP_GE:
            return expr_evaluate(left) >= expr_evaluate(right) ? 1 : 0;
        case OP_NOT_EQUAL:
            return expr_evaluate(left) != expr_evaluate(right) ? 1 : 0;
        default:
            printf("ERROR, unknown binary op:%d, at L%d\n", op, lineno);
    }

}

int evaluate_trinary(EXPR *expr1, EXPR *expr2, EXPR *expr3)
{
    int value;

    if (!expr1 || !expr2 || !expr3)
    {
        printf("Invalid trinary evaluate at L%d\n", lineno);
        return 0;
    }
    value = expr_evaluate(expr1);

    if (value != 0)
        return expr_evaluate(expr2);

    return expr_evaluate(expr3);
}

int expr_evaluate(EXPR *expr)
{
    if (!expr)
    {
        printf("Invalid expr evaluate at L%d\n", lineno);
        return 0;
    }

    switch(expr->type)
    {
        case EXPR_SCALAR:
            return expr->value;
        case EXPR_FUNC:
            return evaluate_func(expr->op, expr->left);
        case EXPR_UNARY:
            return evaluate_unary(expr->op, expr->left);
        case EXPR_BINARY:
            return evaluate_binary(expr->left, expr->op, expr->right);
        case EXPR_TRINARY:
            return evaluate_trinary(expr->left, expr->right, expr->tri);
        default:
            printf("ERROR, unknown expr type: %d, at L%d\n",expr->type, lineno); 
            return 0;
    }   
    return 0;
} 

IFDEFstatement *create_ifdef_statement(IFDEF_TYPE type, EXPR *expr, Statement *s1, Statement *s2, int has_else)
{
    IFDEFstatement *ifdef = malloc(sizeof(IFDEFstatement));

    ifdef->type = type;
    ifdef->specified = expr->specified;

	switch(type)
	{
		case IFDEF_IFDEF:
            ifdef->condition = expr->defined;
			break;
		case IFDEF_IFNDEF:
            ifdef->condition = !expr->defined;
			break;
		case IFDEF_IF:
            ifdef->condition = expr_evaluate(expr);
			break;
	}

    ifdef->true_stmt = s1;
    ifdef->false_stmt = s2;
    ifdef->ifdef_string    = pop_ifdef_string();
    if (has_else)
        ifdef->else_string = pop_else_string();
    else
        ifdef->else_string = 0;
    ifdef->endif_string    = pop_endif_string();
    ifdef->fname = current_file_name();
    ifdef->lineno = lineno;
    //printf("create ifdef: lineno=%d, endif=%s\n", lineno, ifdef->endif_string);
    return ifdef;
}

Statement *create_statement(char *string, IFDEFstatement *ifdef)
{
    Statement *stmt = malloc(sizeof(Statement));
    if (string)
    {
        stmt->type   = STMT_STRING;
        stmt->string = string;
        stmt->ifdef  = 0;
        //printf("create stmt L(%d): %s\n", lineno, stmt->string);
    }
    else
    {
        stmt->type   = STMT_IFDEF;
        stmt->string = 0;
        stmt->ifdef  = ifdef;
        //printf("create stmt L(%d): type=ifdef\n", lineno);
    }
    stmt->next = 0;
    stmt->fname = current_file_name();
    stmt->lineno = lineno;
    return stmt;
}
#if 0
char *merge_string(char *s1, char *s2)
{
    char *buf;
    if (!s1) return s2;
    if (!s2) return s1;

    buf = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(buf, s1);
    strcpy((char *)(buf+strlen(s1)), s2);
    printf("\nmerged [%s]+[%s]=%s\n", s1, s2, buf);
    free(s1);
    free(s2);
    return buf;
}
#endif

Statement *add_statement_list(Statement *list, Statement *s)
{
    Statement *tail = list, *pre = list;
    if (!list)
    {
#if 0
        if (!g_first_stmt) 
        {
            g_first_stmt = s;
            printf("first stmt=%s\n", s->string);
        }
        else
            printf("ERROR! no list, but has g_first_stmt\n");
#endif
        return s;
    }
#if 0
    if (list->string)
    {
        printf("[%s] add ", list->string);
        if (s->string)
            printf("[%s]\n", s->string);
        else
            printf("non string stmt\n");
    }
#endif
    // find the tail of stmts list
    while(tail)
    {
        pre  = tail;
        tail = tail->next;
    }
    pre->next = s;
    return list;
}


void dump_ifdef_all(FILE *fp, IFDEFstatement *ifdef)
{
	if (!ifdef->ifdef_string)
		printf("ERROR, no ifdef_string at %s, L(%d)\n", ifdef->fname, ifdef->lineno);
	fprintf(fp, "%s", ifdef->ifdef_string);
	dump(fp, ifdef->true_stmt);

	if (ifdef->else_string)
	{
		fprintf(fp, "%s", ifdef->else_string);
	    dump(fp, ifdef->false_stmt);
	}

	if (!ifdef->endif_string)
		printf("ERROR, no endif_string at %s, L(%d)\n", ifdef->fname, ifdef->lineno);
	fprintf(fp, "%s", ifdef->endif_string);
}

void dump(FILE *fp, Statement *s)
{
    IFDEFstatement *ifdef;
    if (!fp) return;

    while(s)
    {
        switch(s->type)
        {
            case STMT_STRING:
                if (!s->string)
                    printf("ERROR, no statement string at %s, L(%d)\n", s->fname, s->lineno);
                fprintf(fp, "%s", s->string);
                break;
            case STMT_IFDEF:
                ifdef = s->ifdef;
				if (!ifdef->specified) // not specified in xxx.conf
				{
					// keep all dumped
					dump_ifdef_all(fp, ifdef);
				}
				else
				{
					// specified => use condition to judge dump or not

					if (ifdef->condition)
					{
						if (!ifdef->ifdef_string)
							printf("ERROR, no ifdef_string at %s, L(%d)\n", ifdef->fname, ifdef->lineno);
						if (!ifdef_remove)
							fprintf(fp, "%s", ifdef->ifdef_string);

						dump(fp, ifdef->true_stmt);

						if (!ifdef->endif_string)
							printf("ERROR, no endif_string at %s, L(%d)\n", ifdef->fname, ifdef->lineno);
						if (!ifdef_remove)
							fprintf(fp, "%s", ifdef->endif_string);
					}
					else
					{
						//if (ifdef->else_string)
						{
							if (!ifdef->ifdef_string)
								 printf("ERROR, no not ifdef_string at %s, L(%d)\n", ifdef->fname, ifdef->lineno);
							if (!ifdef_remove)
								fprintf(fp, "%s", ifdef->ifdef_string);

							if (ifdef->else_string && !ifdef_remove)
							{
								fprintf(fp, "%s", ifdef->else_string);
							}
							dump(fp, ifdef->false_stmt);
							if (!ifdef->endif_string)
								 printf("ERROR, no not endif_string at %s, L(%d)\n", ifdef->fname, ifdef->lineno);
							if (!ifdef_remove)
								fprintf(fp, "%s", ifdef->endif_string);
						}
					} // of if(condition) else...
				} // of speficied

                break;
        }
        s = s->next;
    }
}

void clean_ifdef(IFDEFstatement *ifdef)
{
    if (!ifdef)
        return;
    //printf("clean ifdef L(%d)...\n", ifdef->lineno);
    clean(ifdef->true_stmt);
    clean(ifdef->false_stmt);

    if (ifdef->ifdef_string)
        free(ifdef->ifdef_string);
    if (ifdef->else_string)
        free(ifdef->else_string);
    if (ifdef->endif_string)
        free(ifdef->endif_string);
}

void clean(Statement *stmt)
{
	Statement *next;

    if (!stmt)
        return;
    clean_ifdef(stmt->ifdef);

    //printf("clean string(%p): L(%d)...\n", stmt->string, stmt->lineno);

    if (stmt->string)
    {
        //printf("free(%d) %s...\n", stmt->lineno, stmt->string);
        free(stmt->string);
    }
	next = stmt->next;
    //printf("clean stmt(%p) done...\n", stmt);
	free(stmt);

    clean(next);

}

void output_file(FILE *fout)
{
    //printf("\n======== DUMP ==========\n");
    dump(fout, g_first_stmt);
    clean(g_first_stmt);
    //printf("\n======= CLEAN done ====\n");
    g_first_stmt = 0;
}

main()
{
#if 0
    if (!process_file("STDIN"))
    {
        yyerror("STDIN"); // 1st include file
        return;
    }
#endif
    do {
        yyparse();
        change_file();
    } while(yyin);
    //} while(!feof(yyin));
} 

