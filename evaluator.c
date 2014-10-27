
#include "lisp.h"

#define EvalError(...)                          \
  fputs("Evaluation Error: ", stderr);          \
  fprintf(stderr, __VA_ARGS__);                 \
  fputs("\n", stderr);                          \
  abort()

LispExpression *lisp_evaluate(LispExpression *expression,
                              LispContext *ctx) {
  if(NULL == expression) {
    return NULL;
  } if(expression->type == LISP_QUOTE) {
    return expression->value.quoted;
  } else if(expression->type == LISP_CONS) {
    LispExpression *left = expression->value.cons.left;
    if(left->type == LISP_CONS) {
      left = lisp_evaluate(left, ctx);
    }
    if(left->type != LISP_SYMBOL) {
      EvalError("Expected symbol, got %s", LispTypeName(left));
    }
    LispFunction f = lisp_context_find_function(ctx, left->value.symbol);
    if(NULL == f) {
      EvalError("Function not defined: %s", left->value.symbol);
    } else {
      LispExpression *args = lisp_map(expression->value.cons.right,
                                      lisp_evaluate, ctx);
      LISP_REF(args);
      LispExpression *result = f(args, ctx);
      LISP_UNREF(args);
      return result;
    }
  } else if(expression->type == LISP_SYMBOL) {
    LispExpression *value = lisp_alist_find(ctx->variables, expression->value.symbol);
    if(NULL == value) {
      EvalError("Variable value is void: %s", expression->value.symbol);
    } else {
      return value;
    }
  } else {
    return expression;
  }
}


/* LispExpression *lisp_evaluate(LispExpression *expression, */
/*                               LispContext *ctx) { */
/*   fprintf(stderr, "EVAL:\n  "); */
/*   lisp_print_expression(expression, stderr); */
/*   fprintf(stderr, "\nTO:\n  "); */
/*   LispExpression *result = _lisp_evaluate(expression, ctx); */
/*   lisp_print_expression(result, stderr); */
/*   fprintf(stderr, "\n"); */
/*   return result; */
/* } */
