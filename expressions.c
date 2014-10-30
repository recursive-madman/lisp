
#include "lisp.h"

int lisp_count = 0;
int mdbg_depth = 0;

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
  LISP_MDBG("DESTROY ");
  lisp_print_expression(exp, stderr);
  fprintf(stderr, " (0x%x)\n", (int)exp);
  mdbg_depth++;
# endif
  lisp_count--;
  switch(exp->type) {
  case LISP_SYMBOL:
    free(exp->value.symbol);
    break;
  case LISP_STRING:
    free(exp->value.string);
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
  mdbg_depth--;
  LISP_MDBG("DONE DESTROY 0x%x\n", (int)exp);
}

LispExpression *make_lisp_number(int number) {
  MakeLisp(NUMBER, number, number);
}

LispExpression *make_lisp_string(char *string) {
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


int lisp_eq(LispExpression *a, LispExpression *b) {
  if(a == NULL && b == NULL) {
    return 1;
  } else if(a == NULL || b == NULL) {
    return 0;
  } else if(a->type == b->type) {
    switch(a->type) {
    case LISP_SYMBOL:
      if(strcmp(a->value.symbol, b->value.symbol) == 0) {
        return 1;
      }
      break;
    case LISP_STRING:
      if(strcmp(a->value.string, b->value.string) == 0) {
        return 1;
      }
      break;
    case LISP_NUMBER:
      if(a->value.number == b->value.number) {
        return 1;
      } break;
    case LISP_CONS:
      return lisp_eq(a->value.cons.left, b->value.cons.left) &&
        lisp_eq(a->value.cons.right, b->value.cons.right);
    case LISP_QUOTE:
      return lisp_eq(a->value.quoted, b->value.quoted);
    default:
      // rest never happens.
      break;
    }
  }
  return 0;
}
