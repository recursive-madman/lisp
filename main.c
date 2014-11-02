
#include <stdio.h>
#include <stdlib.h>
#include <readline/history.h>

#include "lisp.h"

char history_path[256];

void save_history() {
  write_history(history_path);
}

int main(int argc, char **argv) {
# ifdef LISP_DEBUG_MEMORY
  mdbg_init(0);
# endif
  using_history();
  snprintf(history_path, 256, "%s/.lisp_history", getenv("HOME"));
  read_history(history_path);
  atexit(save_history);
  LispContext *ctx = lisp_context_create(NULL);
  lisp_install_functions(ctx);
  lisp_repl(ctx, stdin, stdout);
# ifdef LISP_DEBUG_MEMORY
  mdbg_done();
# endif
  return 0;
}
