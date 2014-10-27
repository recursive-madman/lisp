
#include "lisp.h"

DeclareType(LispParseContext, {
    char *source;
    int line;
    int col;
  });

#define ParseError(...)                         \
  fputs("Parse Error: ", stderr);               \
  fprintf(stderr, __VA_ARGS__);                 \
  fputs("\n", stderr);                          \
  abort()

# define _ADVANCE(ctx)                          \
  ctx->source++
# define ADVANCE(ctx)                           \
  _ADVANCE(ctx);                                \
  ctx->col++
# define ADVANCE_LINE(ctx)                      \
  _ADVANCE(ctx);                                \
  ctx->line++;                                  \
  ctx->col = 0

#define ASSERT_NO_EOF(ctx) {                                      \
    if(*ctx->source == 0) {                                       \
      ParseError("Unexpected EOF at %d:%d (called from %s:%d)",   \
                 ctx->line, ctx->col, __FILE__, __LINE__);        \
    }                                                             \
  }

#define SKIP_WHITESPACE(ctx) {                  \
    for(;;) {                                   \
      ASSERT_NO_EOF(ctx);                       \
      char c = *ctx->source;                    \
      if(c == ' ') {                            \
        ADVANCE(ctx);                           \
      } else if(c == '\n') {                    \
        ADVANCE_LINE(ctx);                      \
      } else {                                  \
        break;                                  \
      }                                         \
    }                                           \
  }

LispExpression *lisp_parse_expression(LispParseContext *ctx);

LispExpression *lisp_parse_list(LispParseContext *ctx) {
  LispExpression *left, *right;
  left = lisp_parse_expression(ctx);
  SKIP_WHITESPACE(ctx);
  ASSERT_NO_EOF(ctx);
  right = (*ctx->source == ')') ? make_lisp_nil() :
    lisp_parse_list(ctx);
  return make_lisp_cons(left, right);
}

LispExpression *lisp_parse_string(LispParseContext *ctx) {
  int escape = 0;
  int prealloc = 8;
  LispString string;
  string.ptr = malloc(prealloc);
  while(*ctx->source != '"' || escape) {
    string.ptr[string.len++] = *ctx->source;
    escape = (*ctx->source == '\\' && !escape) ? 1 : 0;
    ADVANCE(ctx);
    if(string.len == prealloc) {
      prealloc *= 2;
      string.ptr = realloc(string.ptr, prealloc);
    }
  }
  string.ptr = realloc(string.ptr, string.len + 1);
  string.ptr[string.len] = 0;
  return make_lisp_string(string);
}

LispExpression *lisp_parse_number(LispParseContext *ctx) {
  int number = 0;
  while(*ctx->source >= '0' && *ctx->source <= '9') {
    number *= 10;
    number += *ctx->source - '0';
    ADVANCE(ctx);
  }
  return make_lisp_number(number);
}

LispExpression *lisp_parse_symbol(LispParseContext *ctx) {
  int prealloc = 8;
  char *symbol = malloc(prealloc);
  int len = 0;
  char c;
  for(;;){
    c = *ctx->source;
    if(c == '\n') {
      ADVANCE_LINE(ctx);
    } else if(c == ' ') {
      ADVANCE(ctx);
    } else if(c == 0 || c == ')') {
      // don't advance. caller needs to see this.
    } else {
      symbol[len++] = c;
      if(len == prealloc) {
        prealloc *= 2;
        symbol = realloc(symbol, prealloc);
      }
      ADVANCE(ctx);
      continue;
    }
    symbol = realloc(symbol, len + 1);
    symbol[len] = 0;
    return make_lisp_symbol(symbol);
  }
}

LispExpression *lisp_parse_expression(LispParseContext *ctx) {
  char c = *ctx->source;
  switch(c) {
  case 0:
    return make_lisp_nil();
  case '\n':
    ADVANCE_LINE(ctx);
    break;
  case ' ':
    ADVANCE(ctx);
    break;
  case '\'':
    ADVANCE(ctx);
    return make_lisp_quote(lisp_parse_expression(ctx));
  case '(':
    ADVANCE(ctx);
    return lisp_parse_list(ctx);
  case '"':
    ADVANCE(ctx);
    return lisp_parse_string(ctx);
  default:
    if(c >= '0' && c <= '9') {
      return lisp_parse_number(ctx);
    } else {
      return lisp_parse_symbol(ctx);
    }
  }
  // reached by whitespace.
  return lisp_parse_expression(ctx);
}

LispExpression *lisp_parse(char *source) {
  LispParseContext ctx = { source, 0, 0 };
  return lisp_parse_expression(&ctx);
}
