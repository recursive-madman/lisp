
#ifndef LISP_H
#define LISP_H

#include <setjmp.h>

#include <declaration.h>

extern int mdbg_depth;

#ifdef LISP_DEBUG_MEMORY
void mdbg_init(int n);
void mdbg_done();
# define LISP_MDBG(...) {                       \
    for(int _mdb=0;_mdb<mdbg_depth;_mdb++) {    \
      fprintf(stderr, "  ");                    \
    }                                           \
    fprintf(stderr, __VA_ARGS__);               \
  }
#else
# define LISP_MDBG(...)
#endif

typedef struct _LispExpression LispExpression;
typedef struct _LispContext LispContext;

typedef enum _LispExpressionType {
  LISP_SYMBOL, LISP_NUMBER, LISP_STRING, LISP_CONS,
  LISP_QUOTE, LISP_FUNCTION, LISP_EXCEPTION, LISP_TYPE_MAX
} LispExpressionType;

extern char *lisp_type_names[LISP_TYPE_MAX];

#define LispTypeName(expression)                \
  (expression ? lisp_type_names[expression->type] : "nil")

typedef LispExpression *(*LispNativeFunction)(LispExpression *args,
                                              LispContext *ctx);

DeclareType(LispFunction, {
    LispNativeFunction native;
    LispExpression *definition;
  });

#define LISP_ASSERT_TYPE(e, t) {                                        \
    if(e->type != t) {                                                  \
      lisp_throw(make_lisp_exception("TypeError",                       \
                                     "Expected %s, got %s instead.",    \
                                     lisp_type_names[t],                \
                                     lisp_type_names[e->type]));        \
    }                                                                   \
  }

#define CAR(cell) cell->value.cons.left
#define CDR(cell) cell->value.cons.right
#define CADR(cell) CAR(CDR(cell))
#define CDDR(cell) CDR(CDR(cell))
#define CAAR(cell) CAR(CAR(cell))
#define CDAR(cell) CDR(CAR(cell))

// exception handling
extern jmp_buf lisp_exc_env;
extern char *lisp_trace[];
extern int lisp_trace_index;
extern LispExpression *lisp_current_exception;

DeclareType(LispCons, {
    LispExpression *left;
    LispExpression *right;
  });

DeclareType(LispException, {
    char *name;
    char *message;
  });

DeclareUnion(LispExpressionValue, {
    int number;
    char *symbol;
    char *string;
    LispCons cons;
    LispExpression *quoted;
    LispFunction function;
    LispException exception;
  });

DeclareType(LispExpression, {
    LispExpressionType type;
    LispExpressionValue value;
    int ref;
  });

DeclareType(LispContext, {
    LispExpression *symbols; // this is an alist
    LispContext *parent;
  });

#define LISP_SAFE_DESTROY(expr) {               \
    if(expr->ref == 0) {                        \
      destroy_lisp(expr);                       \
    }                                           \
  }


#define LISP_REF(expr)                                        \
  if(NULL != expr) {                                          \
    expr->ref++;                                              \
    LISP_MDBG("REF 0x%x (%d)\n", (int)expr, expr->ref);       \
  }

#define LISP_UNREF(expr)                                        \
  if(NULL != expr) {                                            \
    expr->ref--;                                                \
    LISP_MDBG("UNREF 0x%x (%d)\n", (int)expr, expr->ref);       \
    LISP_SAFE_DESTROY(expr);                                    \
  }

extern int lisp_count; // number of allocated objects.

// expressions
LispExpression *make_lisp_number(int number);
LispExpression *make_lisp_string(char *string);
LispExpression *make_lisp_symbol(char *symbol);
LispExpression *make_lisp_cons(LispExpression *left, LispExpression *right);
LispExpression *make_lisp_quote(LispExpression *quoted);
LispExpression *make_lisp_function_native(LispNativeFunction function);
LispExpression *make_lisp_function(LispExpression *definition);
LispExpression *make_lisp_exception(char *name, char *message, ...);

int lisp_eq(LispExpression *a, LispExpression *b);

void destroy_lisp(LispExpression *exp);

// parser
LispExpression *lisp_parse(char *source);

// context
LispContext *lisp_context_create(LispContext *parent);
void lisp_context_destroy(LispContext *ctx);
LispExpression *lisp_context_find(LispContext *ctx, char *symbol);
LispExpression *lisp_context_set(LispContext *ctx, LispExpression *symbol,
                                 LispExpression *value);
void lisp_context_declare_function(LispContext *ctx, char *symbol,
                                   LispNativeFunction function);
// evaluator
LispExpression *lisp_evaluate(LispExpression *expression,
                              LispContext *ctx);
LispExpression *lisp_evaluate_function(LispExpression *expression,
                                       LispContext *ctx);
void lisp_throw(LispExpression *exc);

// alist
LispExpression *lisp_alist_find(LispExpression *alist, char *symbol);
LispExpression *lisp_alist_find_novoid(LispExpression *alist, char *symbol,
                                       int *is_void);
LispExpression *lisp_alist_add(LispExpression *alist, char *symbol,
                               LispExpression *value);
void alist_inspect(LispExpression *alist, FILE *stream);

// higher
LispExpression *lisp_map_native(LispExpression *list, LispNativeFunction mapper,
                                LispContext *ctx);

// printer
void lisp_print_expression(LispExpression *expression, FILE *stream);

// repl
void lisp_repl(LispContext *ctx, FILE *in, FILE *out);

// functions
void lisp_install_functions(LispContext *ctx);

#endif
