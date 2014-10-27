
#include <stdio.h>

#include "lisp.h"

int main(int argc, char **argv) {
  LispContext *ctx = lisp_context_create();
  lisp_install_functions(ctx);
  lisp_repl(ctx, stdin, stdout);
}
