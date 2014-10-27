
#include "lisp.h"

#define ASSERT_CONS(exp) {                      \
    if(exp->type != LISP_CONS) {                \
      fprintf(stderr, "No cons!\n");            \
      abort();                                  \
    }                                           \
  }

LispExpression *lisp_alist_find(LispExpression *alist, char *symbol) {
  if(alist->type == LISP_NIL) {
    return NULL;
  } else {
    ASSERT_CONS(alist);
    LispCons cons = alist->value.cons;
    LispCons left = cons.left->value.cons;
    if(strcmp(symbol, left.left->value.symbol) == 0) {
      return left.right;
    } else {
      return lisp_alist_find(cons.right, symbol);
    }
  }
}

LispExpression *lisp_alist_add(LispExpression *alist, char *symbol,
                               LispExpression *value) {
  LispCons cell = { make_lisp_symbol(symbol), value };
  LispCons list = { make_lisp_cons(cell), alist };
  return make_lisp_cons(list);
}
