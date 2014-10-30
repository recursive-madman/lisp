
#define _GNU_SOURCE

#include <string.h>

#include "lisp.h"

#define LISP_F(name)                                    \
  LispExpression *lisp_f_ ## name(LispExpression *args, \
                                  LispContext *ctx)

#define LISP_F_ARITHMETIC(name, op, identity)                     \
  int _lisp_f_ ## name (LispExpression *args, LispContext *ctx) { \
    if(NULL == args) {                                            \
      return identity;                                            \
    } else {                                                      \
      LispExpression *n = CAR(args);                              \
      LISP_ASSERT_TYPE(n, LISP_NUMBER);                           \
      return n->value.number op _lisp_f_ ## name(CDR(args), ctx); \
    }                                                             \
  }                                                               \
  LISP_F(name) {                                                  \
    return make_lisp_number(_lisp_f_ ## name(args, ctx));         \
  }

LISP_F_ARITHMETIC(add, +, 0);
LISP_F_ARITHMETIC(sub, -, 0);
LISP_F_ARITHMETIC(mul, *, 1);
LISP_F_ARITHMETIC(div, /, 1);

LISP_F(cons) {
  return make_lisp_cons(CAR(args), CADR(args));
}

LISP_F(car) {
  LispExpression *c = CAR(args);
  LISP_ASSERT_TYPE(c, LISP_CONS);
  return CAR(c);
}

LISP_F(cdr) {
  LispExpression *c = CAR(args);
  LISP_ASSERT_TYPE(c, LISP_CONS);
  return CDR(c);
}

LISP_F(set) {
  LispExpression *s = CAR(args);
  LispExpression *v = CAR(CDR(args));
  LISP_ASSERT_TYPE(s, LISP_SYMBOL);
  LispExpression *old_symbols = ctx->symbols;
  ctx->symbols = lisp_alist_add(old_symbols, s->value.symbol, v);
  LISP_REF(ctx->symbols);
  LISP_UNREF(old_symbols);
  return v;
};

LISP_F(eq) {
  return lisp_eq(CAR(args), CADR(args)) ?
    make_lisp_symbol(strdup("t")) :
    NULL;
}

LISP_F(eval) {
  if(args == NULL) {
    return NULL;
  } else {
    return lisp_evaluate(CAR(args), ctx);
  }
}

LISP_F(inspect_state) {
  fprintf(stderr, "Symbols:\n");
  alist_inspect(ctx->symbols, stderr);
  return NULL;
}

LISP_F(typeof) {
  return make_lisp_symbol(strdup(LispTypeName(CAR(args))));
}

void lisp_install_functions(LispContext *ctx) {
  lisp_context_declare_function(ctx, "+", lisp_f_add);
  lisp_context_declare_function(ctx, "-", lisp_f_sub);
  lisp_context_declare_function(ctx, "*", lisp_f_mul);
  lisp_context_declare_function(ctx, "/", lisp_f_div);
  lisp_context_declare_function(ctx, "cons", lisp_f_cons);
  lisp_context_declare_function(ctx, "car", lisp_f_car);
  lisp_context_declare_function(ctx, "cdr", lisp_f_cdr);
  lisp_context_declare_function(ctx, "set", lisp_f_set);
  lisp_context_declare_function(ctx, "eq", lisp_f_eq);
  lisp_context_declare_function(ctx, "eval", lisp_f_eval);
  lisp_context_declare_function(ctx, "typeof", lisp_f_typeof);
  lisp_context_declare_function(ctx, "inspect-state", lisp_f_inspect_state);
}
