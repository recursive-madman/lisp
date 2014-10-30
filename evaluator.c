
#define _GNU_SOURCE
#include <string.h>
#include "lisp.h"

#define LispThrow(name, ...)                                        \
  lisp_throw(make_lisp_exception(name, __VA_ARGS__));               \
  return NULL /* never reached */


#define EvalError(...)                                              \
  LispThrow("EvaluationError", __VA_ARGS__)

char *lisp_trace[1024];
int lisp_trace_index = 0;
LispExpression *lisp_current_exception;

jmp_buf lisp_exc_env;

void lisp_trace_push(char *symbol) {
  if(lisp_trace_index == 1023) {
    lisp_throw(make_lisp_exception("StackError", "Stack level too deep!"));
  }
  lisp_trace[lisp_trace_index++] = strdup(symbol);
}

void lisp_trace_pop() {
  if(lisp_trace_index == 0) {
    lisp_throw(make_lisp_exception("StackError", "BUG: Stack underrun occured!"));
  }
  free(lisp_trace[--lisp_trace_index]);
}

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
      LispThrow("TypeError", "Expected symbol, got %s", LispTypeName(left));
    }
    LispExpression *args = lisp_map(expression->value.cons.right,
                                    lisp_evaluate, ctx);
    LISP_REF(args);
    LispExpression *f_expr = lisp_context_find(ctx, left->value.symbol);
    if(NULL == f_expr) {
      LISP_UNREF(args);
      EvalError("Symbol is void: %s", left->value.symbol);
    } else if(f_expr->type != LISP_FUNCTION) {
      EvalError("Symbol %s isn't set to a function!",
                left->value.symbol);
    }
    LispFunction f = f_expr->value.function;
    lisp_trace_push(left->value.symbol);
    LispExpression *result = f(args, ctx);
    LISP_UNREF(args);
    lisp_trace_pop();
    return result;
  } else if(expression->type == LISP_SYMBOL) {
    return lisp_context_find(ctx, expression->value.symbol);
  } else {
    return expression;
  }
}

void lisp_throw(LispExpression *exc) {
  LISP_REF(exc);
  lisp_current_exception = exc;
  longjmp(lisp_exc_env, 1);
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
