
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "lisp.h"

char history_path[256];

void save_history() {
  write_history(history_path);
}

int main(int argc, char **argv) {
  FILE *input = stdin;
  if(argc > 1) {
    input = fopen(argv[1], "r");
    if(NULL == input) {
      fprintf(stderr, "Failed to open '%s': %s\n", argv[1], strerror(errno));
      abort();
    }
  }
# ifdef LISP_DEBUG_MEMORY
  mdbg_init(0);
# endif
  if(input == stdin) {
    using_history();
    snprintf(history_path, 256, "%s/.lisp_history", getenv("HOME"));
    read_history(history_path);
    atexit(save_history);
  }
  rl_variable_bind("blink-matching-paren", "yes");
  LispContext *ctx = lisp_context_create(NULL);
  lisp_install_functions(ctx);
  lisp_repl(ctx, input, stdout);
# ifdef LISP_DEBUG_MEMORY
  mdbg_done();
# endif
  return 0;
}
