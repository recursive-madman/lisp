
#include "lisp.h"

LispExpression *lisp_map(LispExpression *list, LispFunction mapper,
                         LispContext *ctx) {
  if(list == NULL) {
    return list;
  } else {
    return make_lisp_cons(mapper(CAR(list), ctx),
                          lisp_map(CDR(list), mapper, ctx));
  }
}
