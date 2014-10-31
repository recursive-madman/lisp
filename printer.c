
#include <stdio.h>

#include "lisp.h"

void lisp_print_expression(LispExpression *expression, FILE *stream) {
  if(expression == NULL) {
    fputs("nil", stream);
    return;
  }
  switch(expression->type) {
  case LISP_SYMBOL:
    fputs(expression->value.symbol, stream);
    break;
  case LISP_NUMBER:
    fprintf(stream, "%d", expression->value.number);
    break;
  case LISP_STRING:
    fprintf(stream, "\"%s\"", expression->value.string);
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
    {
      LispFunction f = expression->value.function;
      if(NULL == f.definition) {
        fprintf(stream, "<native-procedure@0x%x>", (int)f.native);
      } else {
        fprintf(stream, "<procedure@0x%x>", (int)f.definition);
      }
    }
    break;
  case LISP_EXCEPTION:
    fprintf(stream, "Uncaught exception: %s (%s)\n",
            expression->value.exception.message,
            expression->value.exception.name);
    for(int i = 0; i < lisp_trace_index; i++) {
      fprintf(stream, "  in `%s'\n", lisp_trace[i]);
    }
    break;
  default:
    // never reached.
    // (hopefully)
    break;
  }
}
