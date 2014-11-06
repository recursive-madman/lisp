
#define _GNU_SOURCE

#include <string.h>

#include "lisp.h"

LispContext *lisp_context_create(LispContext *parent) {
  Allocate(LispContext, ctx);
  ctx->parent = parent;
  ctx->symbols = symbol_table_create(256);
  return ctx;
}

static void unref_symbol(LispExpression *value, LispContext *ctx) {
  LISP_UNREF(value);
}

void lisp_context_destroy(LispContext *ctx) {
  symbol_table_destroy(ctx->symbols, unref_symbol, ctx);
  free(ctx);
}

void lisp_context_declare_function(LispContext *ctx, char *symbol,
                                   LispNativeFunction function) {
  LispExpression *fun = make_lisp_function_native(function);
  LISP_REF(fun);
  LispExpression *previous = symbol_table_add(ctx->symbols, symbol, fun);
  LISP_UNREF(previous);
}

LispExpression *lisp_context_find(LispContext *ctx, char *symbol) {
  int found = 0;
  LispExpression *expr = symbol_table_lookup(ctx->symbols, symbol, &found);
  if(! found) {
    if(ctx->parent) {
      return lisp_context_find(ctx->parent, symbol);
    } else {
      lisp_throw(make_lisp_exception("NameError", "Symbol not defined: %s", symbol));
      return NULL; // never reached (but -Wall will complain about this)
    }
  } else {
    return expr;
  }
}

LispExpression *lisp_context_set(LispContext *ctx, LispExpression *symbol,
                                 LispExpression *value) {
  LISP_ASSERT_TYPE(symbol, LISP_SYMBOL);
  LISP_REF(value);
  LispExpression *previous = symbol_table_add(ctx->symbols, symbol->value.symbol, value);
  LISP_UNREF(previous);
  return value;
}
