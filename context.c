
#include "lisp.h"

LispContext *lisp_context_create() {
  Allocate(LispContext, ctx);
  ctx->variables = make_lisp_nil();
  ctx->functions = make_lisp_nil();
  return ctx;
}

void lisp_context_declare_function(LispContext *ctx, char *symbol,
                                   LispFunction function) {
  ctx->functions = lisp_alist_add(ctx->functions, symbol,
                                  make_lisp_function(function));
}

LispFunction lisp_context_find_function(LispContext *ctx, char *symbol) {
  LispExpression *fun = lisp_alist_find(ctx->functions, symbol);
  if(NULL != fun) {
    return fun->value.function;
  } else {
    return NULL;
  }
}

