
#include <stdio.h>

#include "lisp.h"

int main(int argc, char **argv) {
# ifdef LISP_DEBUG_MEMORY
  mdbg_init(0);
# endif
  LispContext *ctx = lisp_context_create(NULL);
  lisp_install_functions(ctx);
  lisp_repl(ctx, stdin, stdout);
# ifdef LISP_DEBUG_MEMORY
  mdbg_done();
# endif
  return 0;
}
