
#include <stdarg.h>

#include "lisp.h"

int lisp_count = 0;
int mdbg_depth = 0;

#ifdef LISP_DEBUG_MEMORY
FILE *mdbg_create = NULL, *mdbg_destroy = NULL;
void mdbg_init(int n) {
  if(NULL != mdbg_create) mdbg_done();
  char buf[100];
  sprintf(buf, "mdbg-create-%d", n);
  mdbg_create = fopen(buf, "w");
  sprintf(buf, "mdbg-destroy-%d", n);
  mdbg_destroy = fopen(buf, "w");
}
void mdbg_done() {
  fclose(mdbg_create);
  fclose(mdbg_destroy);
}
# define LISP_MDBG_CREATE(type, exp)            \
  fprintf(mdbg_create, "%s 0x%x\n", lisp_type_names[type], (int) exp)
# define LISP_MDBG_DESTROY(type, exp)           \
  fprintf(mdbg_destroy, "%s 0x%x\n", lisp_type_names[type], (int) exp)
#else
# define LISP_MDBG_CREATE(type, exp)
# define LISP_MDBG_DESTROY(type, exp)
#endif

#define MakeLisp(lisp_type, name, val)            \
  Allocate(LispExpression, expression);           \
  LISP_MDBG("CREATE " __STRING(lisp_type)         \
            " (0x%x)\n", (int)expression);        \
  LISP_MDBG_CREATE(LISP_ ## lisp_type, expression); \
  lisp_count++;                                 \
  expression->type = LISP_ ## lisp_type;        \
  expression->value.name = val;                 \
  return expression;

char *lisp_type_names[LISP_TYPE_MAX] = {
  "symbol", "number", "string", "cons", "quote", "function", "exception"
};

void destroy_lisp(LispExpression *exp) {
# ifdef LISP_DEBUG_MEMORY
  LISP_MDBG("DESTROY ");
  lisp_print_expression(exp, stderr);
  fprintf(stderr, " (0x%x)\n", (int)exp);
  mdbg_depth++;
  LISP_MDBG_DESTROY(exp->type, exp);
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

LispExpression *make_lisp_function(LispExpression *definition) {
  LispFunction function = { lisp_evaluate_function, definition };
  LISP_REF(definition);
  MakeLisp(FUNCTION, function, function);
}

LispExpression *make_lisp_function_native(LispNativeFunction native) {
  LispFunction function = { native, NULL };
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

LispExpression *make_lisp_exception(char *name, char *format, ...) {
  va_list ap;
  va_start(ap, format);
  char *message = malloc(1024);
  vsnprintf(message, 1024, format, ap);
  va_end(ap);
  message = realloc(message, strlen(message));
  LispException exception = { name, message };
  MakeLisp(EXCEPTION, exception, exception);
}
