
#include <stdio.h>

#include "lisp.h"

#define PROMPT(out)                             \
  fprintf(out, "(%d)> ", lisp_count)

void lisp_repl(LispContext *ctx, FILE *in, FILE *out) {
  char buf[1024];
  PROMPT(out);
  LispExpression *input, *output;
  while(NULL != fgets(buf, 1024, in)) {
    //fprintf(stderr, "-- START PARSE --\n");
    input = lisp_parse(buf);    
    LISP_REF(input);
    //fprintf(stderr, "-- START EVAL --\n");
    output = lisp_evaluate(input, ctx);
    LISP_REF(output);
    LISP_UNREF(input);
    //fprintf(stderr, "-- START PRINT --\n");
    lisp_print_expression(output, out);
    LISP_UNREF(output);
    fprintf(out, "\n");
    PROMPT(out);
  }
}
