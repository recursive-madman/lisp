
#include "lisp.h"

int lisp_count = 0;

#define MakeLisp(lisp_type, name, val)          \
  Allocate(LispExpression, expression);         \
  LISP_MDBG("CREATE " __STRING(lisp_type)         \
            " (0x%x)\n", (int)expression);        \
  lisp_count++;                                 \
  expression->type = LISP_ ## lisp_type;        \
  expression->value.name = val;                 \
  return expression;

char *lisp_type_names[LISP_TYPE_MAX] = {
  "symbol", "number", "string", "cons", "quote", "function"
};

void destroy_lisp(LispExpression *exp) {
# ifdef LISP_DEBUG_MEMORY
  fprintf(stderr, "DESTROY ");
  lisp_print_expression(exp, stderr);
  fprintf(stderr, " (0x%x)\n", (int)exp);
# endif
  lisp_count--;
  switch(exp->type) {
  case LISP_SYMBOL:
    free(exp->value.symbol);
    break;
  case LISP_STRING:
    free(exp->value.string.ptr);
    break;
  case LISP_CONS:
    LISP_UNREF(CAR(exp));
    LISP_UNREF(CDR(exp));
    break;
  case LISP_QUOTE:
    LISP_UNREF(exp->value.quoted);
    break;
  default:
    // no value to free.
    break;
  }
  free(exp);
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

LispExpression *make_lisp_cons(LispExpression *left, LispExpression *right) {
  LispCons cons = { left, right };
  LISP_REF(left);
  LISP_REF(right);
  MakeLisp(CONS, cons, cons);
}

LispExpression *make_lisp_quote(LispExpression *quoted) {
  LISP_REF(quoted);
  MakeLisp(QUOTE, quoted, quoted);
}

LispExpression *make_lisp_function(LispFunction function) {
  MakeLisp(FUNCTION, function, function);
}
