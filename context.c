
#define _GNU_SOURCE

#include <string.h>

#include "lisp.h"

LispContext *lisp_context_create() {
  Allocate(LispContext, ctx);
  ctx->symbols = NULL;
  return ctx;
}

void lisp_context_declare_function(LispContext *ctx, char *symbol,
                                   LispFunction function) {
  LispExpression *old_symbols = ctx->symbols;
  ctx->symbols = lisp_alist_add(old_symbols, strdup(symbol),
                                make_lisp_function(function));
  LISP_UNREF(old_symbols);
  LISP_REF(ctx->symbols);
}

LispExpression *lisp_context_find(LispContext *ctx, char *symbol) {
  return lisp_alist_find(ctx->symbols, symbol);
}

