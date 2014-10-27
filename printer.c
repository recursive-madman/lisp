
#include <stdio.h>

#include "lisp.h"

void lisp_print_expression(LispExpression *expression, FILE *stream) {
  switch(expression->type) {
  case LISP_NIL:
    fputs("nil", stream);
    break;
  case LISP_SYMBOL:
    fputs(expression->value.symbol, stream);
    break;
  case LISP_NUMBER:
    fprintf(stream, "%d", expression->value.number);
    break;
  case LISP_STRING:
    fprintf(stream, "%s", expression->value.string.ptr);
    break;
  case LISP_CONS:
    fputc('(', stream);
    lisp_print_expression(CAR(expression), stream);
    fputc(' ', stream);
    lisp_print_expression(CDR(expression), stream);
    fputc(')', stream);
    break;
  case LISP_QUOTE:
    fputc('\'', stream);
    lisp_print_expression(expression->value.quoted, stream);
    break;
  case LISP_FUNCTION:
    fprintf(stream, "<procedure@0x%x>", (int)expression->value.function);
    break;
  case LISP_TYPE_MAX:
    // never reached.
    // (hopefully)
    break;
  }
}
