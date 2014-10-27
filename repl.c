
#include <stdio.h>

#include "lisp.h"

void lisp_repl(LispContext *ctx, FILE *in, FILE *out) {
  char buf[1024];
  fprintf(out, "> ");
  while(NULL != fgets(buf, 1024, in)) {
    lisp_print_expression(lisp_evaluate(lisp_parse(buf), ctx), out);
    fprintf(out, "\n> ");
  }
}
