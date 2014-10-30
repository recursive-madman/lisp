
#define _GNU_SOURCE

#include <string.h>

#include "lisp.h"

LispExpression *lisp_alist_find_novoid(LispExpression *alist, char *symbol, int *is_void) {
  if(NULL == alist) {
    *is_void = 1;
    return NULL;
  } else {
    LISP_ASSERT_TYPE(alist, LISP_CONS);
    LispCons cons = alist->value.cons;
    LispCons left = cons.left->value.cons;
    if(strcmp(symbol, left.left->value.symbol) == 0) {
      return left.right;
    } else {
      return lisp_alist_find_novoid(cons.right, symbol, is_void);
    }
  }
}

LispExpression *lisp_alist_find(LispExpression *alist, char *symbol) {
  int is_void = 0; // ignored...
  return lisp_alist_find_novoid(alist, symbol, &is_void);
}

void alist_inspect(LispExpression *alist, FILE *stream) {
  if(NULL != alist) {
    LispExpression *key = CAR(CAR(alist));
    LispExpression *value = CDR(CAR(alist));
    if(key->type != LISP_SYMBOL) {
      fprintf(stream, " NO SYMBOL (but %s) -> ", LispTypeName(key));
    } else {
      fprintf(stream, " %s -> ", key->value.symbol);
    }
    lisp_print_expression(value, stream);
    fputs("\n", stream);
    alist_inspect(CDR(alist), stream);
  }
}

LispExpression *lisp_alist_add(LispExpression *alist, char *symbol,
                               LispExpression *value) {
  /* if(strcmp(CAR(CAR(alist))->value.symbol, symbol) == 0) { */
  /*   // replace car? */
  /* } else if(CDR(alist) == NULL) { */
  /*   // append */
  /* } else { */
  /*   return lisp_alist_add(CDR(alist), symbol, value); */
  /* } */
  return make_lisp_cons(make_lisp_cons(make_lisp_symbol(strdup(symbol)),
                                       value),
                        alist);
}
