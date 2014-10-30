
#include <stdio.h>

#include "lisp.h"

#define PROMPT(out)                             \
  fprintf(out, "(%d)> ", lisp_count)

void lisp_repl(LispContext *ctx, FILE *in, FILE *out) {
  char buf[1024];
  PROMPT(out);
  LispExpression *input, *output;
  while(NULL != fgets(buf, 1024, in)) {
    LISP_MDBG("-- START PARSE --\n");
    input = lisp_parse(buf);    
    LISP_MDBG("-- END PARSE --\n");
    LISP_REF(input);
    LISP_MDBG("-- START EVAL --\n");
    if(setjmp(lisp_exc_env)) {
      LISP_UNREF(input);
      lisp_print_expression(lisp_current_exception, out);
      LISP_UNREF(lisp_current_exception);
      PROMPT(out);
      continue;
    }
    output = lisp_evaluate(input, ctx);
    LISP_MDBG("-- END EVAL --\n");
    LISP_REF(output);
    LISP_UNREF(input);
    LISP_MDBG("-- START PRINT --\n");
    lisp_print_expression(output, out);
    LISP_MDBG("-- END PRINT --\n");
    LISP_UNREF(output);
    fprintf(out, "\n");
    PROMPT(out);
  }
}
