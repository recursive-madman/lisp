
#include "lisp.h"

LispExpression *lisp_map_native(LispExpression *list, LispNativeFunction mapper,
                                LispContext *ctx) {
  if(list == NULL) {
    return list;
  } else {
    return make_lisp_cons(mapper(CAR(list), ctx),
                          lisp_map_native(CDR(list), mapper, ctx));
  }
}
