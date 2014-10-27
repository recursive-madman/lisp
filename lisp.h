

#ifndef LISP_H
#define LISP_H

#include <declaration.h>

typedef struct _LispExpression LispExpression;
typedef struct _LispContext LispContext;

typedef enum _LispExpressionType {
  LISP_NIL, LISP_SYMBOL, LISP_NUMBER, LISP_STRING,
  LISP_CONS, LISP_QUOTE, LISP_FUNCTION, LISP_TYPE_MAX
} LispExpressionType;

extern char *lisp_type_names[LISP_TYPE_MAX];

#define LispTypeName(expression)                \
  lisp_type_names[expression->type]

typedef LispExpression *(*LispFunction)(LispExpression *args,
                                        LispContext *ctx);

#define LISP_ASSERT_TYPE(e, t) {                                        \
    if(e->type != t) {                                                  \
      fprintf(stderr,                                                   \
              "Lisp type incorrect! Expected %s, got %s instead!\n",    \
              lisp_type_names[t], lisp_type_names[e->type]);            \
      abort();                                                          \
    }                                                                   \
  }

#define CAR(cell) cell->value.cons.left
#define CDR(cell) cell->value.cons.right

DeclareType(LispString, {
    char *ptr;
    unsigned int len;
  });

DeclareType(LispCons, {
    LispExpression *left;
    LispExpression *right;
  });

DeclareUnion(LispExpressionValue, {
    void *nil;
    int number;
    char *symbol;
    LispString string;
    LispCons cons;
    LispExpression *quoted;
    LispFunction function;
  });

DeclareType(LispExpression, {
    LispExpressionType type;
    LispExpressionValue value;
  });

DeclareType(LispContext, {
    LispExpression *variables;
    LispExpression *functions;
  });

// expressions
LispExpression *make_lisp_nil();
LispExpression *make_lisp_number(int number);
LispExpression *make_lisp_string(LispString string);
LispExpression *make_lisp_symbol(char *symbol);
LispExpression *make_lisp_cons(LispCons cons);
LispExpression *make_lisp_quote(LispExpression *quoted);
LispExpression *make_lisp_function(LispFunction function);

void destroy_lisp(LispExpression *exp);

// parser
LispExpression *lisp_parse(char *source);

// context
LispContext *lisp_context_create();
LispFunction lisp_context_find_function(LispContext *ctx, char *symbol);
void lisp_context_declare_function(LispContext *ctx, char *symbol,
                                   LispFunction function);
// evaluator
LispExpression *lisp_evaluate(LispExpression *expression,
                              LispContext *ctx);

// alist
LispExpression *lisp_alist_find(LispExpression *alist, char *symbol);
LispExpression *lisp_alist_add(LispExpression *alist, char *symbol,
                               LispExpression *value);

// higher
LispExpression *lisp_map(LispExpression *list, LispFunction mapper,
                         LispContext *ctx);

// printer
void lisp_print_expression(LispExpression *expression, FILE *stream);

// repl
void lisp_repl(LispContext *ctx, FILE *in, FILE *out);

// functions
void lisp_install_functions(LispContext *ctx);

#endif
