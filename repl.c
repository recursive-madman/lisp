
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "lisp.h"

#define UPDATE_PROMPT(buf)                      \
  snprintf(buf, 128, "(%d)> ", lisp_count)


char *(*repl_readline)(const char *);

// callback for parser to request more input.
char *readmore(void *prompt) {
  return repl_readline((char*)prompt);
}

FILE *repl_input;
char *repl_buffer;

char *simple_readline(const char *prompt) {
  return fgets(repl_buffer, 2048, repl_input);
}

// not reentrant. not very sophisticated.
void lisp_repl(LispContext *ctx, FILE *in, FILE *out) {
  char prompt[128], *continuation_prompt = "      > ";
  UPDATE_PROMPT(prompt);
  LispExpression *input, *output;
  char *line = NULL;

  int read_stdin = 0;

  if(in == stdin) {
    read_stdin = 1;
    repl_readline = readline;
  } else {
    repl_readline = simple_readline;
    repl_input = in;
    repl_buffer = malloc(2048);
  }

# ifdef LISP_DEBUG_MEMORY
  int i = 1;
# endif
  for(;;) {
#   ifdef LISP_DEBUG_MEMORY
    mdbg_init(i);
    i++;
#   endif

    if(NULL != line) {
      free(line);
    }

    line = repl_readline(prompt);

    if(NULL == line) {
      fprintf(out, "\n"); // C-d shouldn't mess up the shell prompt.
      break;
    }

    if(*line && read_stdin) { // not empty.
      add_history(line); // yay!
    }

    LISP_MDBG("-- START PARSE --\n");

    input = lisp_parse_multi(line, readmore, continuation_prompt);

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
