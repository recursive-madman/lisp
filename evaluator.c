
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

LispExpression *lisp_current_definition = NULL;

LispExpression *lisp_evaluate_function(LispExpression *args,
                                       LispContext *ctx) {
  LispExpression *arg_names = CAR(lisp_current_definition);
  LispExpression *body = CDR(lisp_current_definition);
  // FIXME: this will leak, when an exception is thrown.
  // SOLUTION: lisp_exc_env should be a stack. then we can save context
  //   here to clean up after us, yet still be able to return to where
  //   it pointed previously.
  LispContext *inner = lisp_context_create(ctx);
  for(LispExpression *rest_names = arg_names, *rest = args, *name, *arg;
      NULL != rest_names;
      rest_names = CDR(rest_names), rest = CDR(args)) {
    name = CAR(rest_names);
    arg = CAR(rest);
    LISP_ASSERT_TYPE(name, LISP_SYMBOL);
    if(NULL != rest_names && NULL == rest) {
      LispThrow("ArgumentError", "Expected more arguments, but none given!");
    }
    LISP_REF(name);
    LISP_REF(arg);
    lisp_context_set(inner, name, arg);
  }
  LispExpression *result;
  for(LispExpression *statement = CAR(body), *rest = CDR(body);;
      statement = CAR(rest), rest = CDR(rest)) {
    result = lisp_evaluate(statement, inner);
    if(rest == NULL) {
      break;
    }
  }
  // TODO: unref context etc...
  return result;
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
    if(strcmp("lambda", left->value.symbol) == 0) {
      LispExpression *args = CADR(expression);
      LISP_ASSERT_TYPE(args, LISP_CONS);
      LispExpression *body = CDDR(expression);
      return make_lisp_function(make_lisp_cons(args, body));
    } else if(strcmp("cond", left->value.symbol) == 0) {
      for(LispExpression *clause = CADR(expression), *rest = CDDR(expression);
          clause != NULL;
          clause = rest ? CAR(rest) : NULL, rest = rest ? CDR(rest) : NULL) {
        LispExpression *condition = CAR(clause);
        if(CDR(clause) == NULL) {
          return lisp_evaluate(condition, ctx);
        } else if(NULL != lisp_evaluate(condition, ctx)) {
          return lisp_evaluate(CADR(clause), ctx);
        }
      }
      return NULL;
    } else {
      LispExpression *args = lisp_map_native(expression->value.cons.right,
                                             lisp_evaluate, ctx);
      LISP_REF(args);
      LispExpression *f_expr = lisp_context_find(ctx, left->value.symbol);
      if(NULL == f_expr || f_expr->type != LISP_FUNCTION) {
        EvalError("Symbol %s isn't set to a function (type: %s)!",
                  left->value.symbol, LispTypeName(f_expr));
      }
      LispNativeFunction f = f_expr->value.function.native;
      lisp_trace_push(left->value.symbol);
      lisp_current_definition = f_expr->value.function.definition;
      LispExpression *result = f(args, ctx);
      lisp_current_definition = NULL;
      LISP_UNREF(args);
      lisp_trace_pop();
      return result;
    }
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
