
#include <stdio.h>

#include "lisp.h"

void lisp_repl(LispContext *ctx, FILE *in, FILE *out) {
  char buf[1024];
  fprintf(out, "> ");
  LispExpression *input, *output;
  while(NULL != fgets(buf, 1024, in)) {
    input = lisp_parse(buf);
    output = lisp_evaluate(input, ctx);
    lisp_print_expression(output, out);
    lisp_destroy(input);
    lisp_destroy(output);
    fprintf(out, "\n> ");
  }
}
