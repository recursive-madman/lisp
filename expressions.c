
#include "lisp.h"

#define MakeLisp(lisp_type, name, val)          \
  Allocate(LispExpression, expression);         \
  expression->type = LISP_ ## lisp_type;        \
  expression->value.name = val;                 \
  return expression;

char *lisp_type_names[LISP_TYPE_MAX] = {
  "nil", "symbol", "number", "string", "cons", "quote", "function"
};

void destroy_lisp(LispExpression *exp) {
  switch(exp->type) {
  case LISP_SYMBOL:
    free(exp->value.symbol);
    break;
  case LISP_STRING:
    free(exp->value.string.ptr);
    break;
  case LISP_CONS:
    // FIXME: this can free potentially used expressions!
    destroy_lisp(CAR(exp));
    destroy_lisp(CDR(exp));
    break;
  case LISP_QUOTE:
    destroy_lisp(exp->value.quoted);
    break;
  default:
    // no value to free.
  }
  free(exp);
}

LispExpression *make_lisp_nil() {
  MakeLisp(NIL, nil, NULL);
}

LispExpression *make_lisp_number(int number) {
  MakeLisp(NUMBER, number, number);
}

LispExpression *make_lisp_string(LispString string) {
  MakeLisp(STRING, string, string);
}

LispExpression *make_lisp_symbol(char *symbol) {
  MakeLisp(SYMBOL, symbol, symbol);
}

LispExpression *make_lisp_cons(LispCons cons) {
  MakeLisp(CONS, cons, cons);
}

LispExpression *make_lisp_quote(LispExpression *quoted) {
  MakeLisp(QUOTE, quoted, quoted);
}

LispExpression *make_lisp_function(LispFunction function) {
  MakeLisp(QUOTE, function, function);
}
