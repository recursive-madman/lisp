
#include "lisp.h"

#define LISP_F_ARITHMETIC(name, op)                                 \
  int _lisp_f_ ## name (LispExpression *args, LispContext *ctx) {   \
    if(args->type == LISP_NIL) {                                    \
      return 0;                                                     \
    } else {                                                        \
      LispExpression *n = CAR(args);                                \
      LISP_ASSERT_TYPE(n, LISP_NUMBER);                             \
      return n->value.number op _lisp_f_ ## name(CDR(args), ctx);   \
    }                                                               \
  }                                                                 \
  LispExpression *lisp_f_ ## name(LispExpression *args,             \
                                  LispContext *ctx) {               \
    return make_lisp_number(_lisp_f_ ## name(args, ctx));           \
  }

LISP_F_ARITHMETIC(add, +);
LISP_F_ARITHMETIC(sub, -);
LISP_F_ARITHMETIC(mul, *);
LISP_F_ARITHMETIC(div, /);

void lisp_install_functions(LispContext *ctx) {
  lisp_context_declare_function(ctx, "+", lisp_f_add);
  lisp_context_declare_function(ctx, "-", lisp_f_sub);
  lisp_context_declare_function(ctx, "*", lisp_f_mul);
  lisp_context_declare_function(ctx, "/", lisp_f_div);
}
