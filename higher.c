
#include "lisp.h"

LispExpression *lisp_map(LispExpression *list, LispFunction mapper,
                         LispContext *ctx) {
  if(list->type == LISP_NIL) {
    return list;
  } else {
    LispCons cons = {
      mapper(CAR(list), ctx),
      lisp_map(CDR(list), mapper, ctx)
    };
    return make_lisp_cons(cons);
  }
}
