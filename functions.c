
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
  if(NULL == args) {
    return make_lisp_cons(NULL, NULL);
  } else {
    return make_lisp_cons(CAR(args), CDR(args) ? CADR(args) : NULL);
  }
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
  return lisp_context_set(ctx, CAR(args), CADR(args));
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

LISP_F(parse) {
  LispExpression *source = CAR(args);
  LISP_ASSERT_TYPE(source, LISP_STRING);
  return lisp_parse(source->value.string);
}

LISP_F(print) {
  if(NULL == args) {
    return NULL;
  }
  LispExpression *arg = CAR(args), *rest = CDR(args);
  if(arg->type == LISP_STRING) {
    printf("%s", arg->value.string);
  } else {
    lisp_print_expression(arg, stdout);
  }
  if(NULL != rest) {
    printf(" ");
    return lisp_f_print(rest, ctx);
  } else {
    printf("\n");
    return NULL;
  }
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
  lisp_context_declare_function(ctx, "inspect-state", lisp_f_inspect_state);
  lisp_context_declare_function(ctx, "typeof", lisp_f_typeof);
  lisp_context_declare_function(ctx, "parse", lisp_f_parse);
  lisp_context_declare_function(ctx, "print", lisp_f_print);
}
