
#include <string.h>

char *strdup(char*);

#include "lisp.h"

#define LISP_F(name)                                    \
  LispExpression *lisp_f_ ## name(LispExpression *args, \
                                  LispContext *ctx)

#define LISP_F_ARITHMETIC(name, op)                               \
  int _lisp_f_ ## name (LispExpression *args, LispContext *ctx) { \
    if(NULL == args) {                                            \
      return 0;                                                   \
    } else {                                                      \
      LispExpression *n = CAR(args);                              \
      LISP_ASSERT_TYPE(n, LISP_NUMBER);                           \
      return n->value.number op _lisp_f_ ## name(CDR(args), ctx); \
    }                                                             \
  }                                                               \
  LISP_F(name) {                                                  \
    return make_lisp_number(_lisp_f_ ## name(args, ctx));         \
  }

LISP_F_ARITHMETIC(add, +);
LISP_F_ARITHMETIC(sub, -);
LISP_F_ARITHMETIC(mul, *);
LISP_F_ARITHMETIC(div, /);

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
  ctx->variables = lisp_alist_add(ctx->variables, s->value.symbol, v);
  return v;
};

int lisp_eq(LispExpression *a, LispExpression *b) {
  if(a == NULL && b == NULL) {
    return 1;
  } else if(a == NULL || b == NULL) {
    return 0;
  } else if(a->type == b->type) {
    switch(a->type) {
    case LISP_SYMBOL:
      if(strcmp(a->value.symbol, b->value.symbol) == 0) {
        return 1;
      }
      break;
    case LISP_STRING:
      if(strcmp(a->value.string.ptr, b->value.string.ptr) == 0) {
        return 1;
      }
      break;
    case LISP_NUMBER:
      if(a->value.number == b->value.number) {
        return 1;
      } break;
    case LISP_CONS:
      return lisp_eq(a->value.cons.left, b->value.cons.left) &&
        lisp_eq(a->value.cons.right, b->value.cons.right);
    case LISP_QUOTE:
      return lisp_eq(a->value.quoted, b->value.quoted);
    default:
      // rest never happens.
      break;
    }
  }
  return 0;
}

LISP_F(eq) {
  return lisp_eq(CAR(args), CADR(args)) ?
    make_lisp_symbol(strdup("t")) :
    NULL;
};

void lisp_install_functions(LispContext *ctx) {
  lisp_context_declare_function(ctx, "+", lisp_f_add);
  lisp_context_declare_function(ctx, "-", lisp_f_sub);
  /* lisp_context_declare_function(ctx, "*", lisp_f_mul); */
  /* lisp_context_declare_function(ctx, "/", lisp_f_div); */
  /* lisp_context_declare_function(ctx, "cons", lisp_f_cons); */
  /* lisp_context_declare_function(ctx, "car", lisp_f_car); */
  /* lisp_context_declare_function(ctx, "cdr", lisp_f_cdr); */
  /* lisp_context_declare_function(ctx, "set", lisp_f_set); */
  /* lisp_context_declare_function(ctx, "eq", lisp_f_eq); */
}
