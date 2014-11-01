
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "lisp.h"

#define UPDATE_PROMPT(buf)                      \
  snprintf(buf, 128, "(%d)> ", lisp_count)

void lisp_repl(LispContext *ctx, FILE *in, FILE *out) {
  char prompt[128];
  UPDATE_PROMPT(prompt);
  LispExpression *input, *output;
  char *line = NULL;
  for(;;) {
    if(NULL != line) {
      free(line);
    }
    line = readline(prompt);
    if(NULL == line) {
      fprintf(out, "\n"); // C-d shouldn't mess up the shell prompt.
      break;
    }
    if(*line) { // not empty.
      add_history(line); // yay!
    }
    LISP_MDBG("-- START PARSE --\n");
    input = lisp_parse(line);
    LISP_MDBG("-- END PARSE --\n");
    LISP_REF(input);
    LISP_MDBG("-- START EVAL --\n");
    if(setjmp(lisp_exc_env)) {
      LISP_UNREF(input);
      lisp_print_expression(lisp_current_exception, out);
      lisp_trace_index = 0;
      LISP_UNREF(lisp_current_exception);
      UPDATE_PROMPT(prompt);
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
    UPDATE_PROMPT(prompt);
  }
}
