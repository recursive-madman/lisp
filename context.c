
#define _GNU_SOURCE

#include <string.h>

#include "lisp.h"

LispContext *lisp_context_create() {
  Allocate(LispContext, ctx);
  ctx->variables = NULL;
  ctx->functions = NULL;
  return ctx;
}

void lisp_context_declare_function(LispContext *ctx, char *symbol,
                                   LispFunction function) {
  LispExpression *old_functions = ctx->functions;
  ctx->functions = lisp_alist_add(old_functions, strdup(symbol),
                                  make_lisp_function(function));
  LISP_UNREF(old_functions);
  LISP_REF(ctx->functions);
}

LispFunction lisp_context_find_function(LispContext *ctx, char *symbol) {
  LispExpression *fun = lisp_alist_find(ctx->functions, symbol);
  if(NULL != fun) {
    return fun->value.function;
  } else {
    return NULL;
  }
}

