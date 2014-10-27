
#include "lisp.h"

LispExpression *lisp_alist_find(LispExpression *alist, char *symbol) {
  if(NULL == alist) {
    return NULL;
  } else {
    LISP_ASSERT_TYPE(alist, LISP_CONS);
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
  return make_lisp_cons(make_lisp_cons(make_lisp_symbol(symbol),
                                       value),
                        alist);
}
