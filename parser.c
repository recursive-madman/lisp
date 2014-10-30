
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
# define ADVANCE(ctx) {                         \
    _ADVANCE(ctx);                              \
    ctx->col++;                                 \
  }
# define ADVANCE_LINE(ctx) {                    \
    _ADVANCE(ctx);                              \
    ctx->line++;                                \
    ctx->col = 0;                               \
  }

#define ASSERT_NO_EOF(ctx) {                                    \
    if(*ctx->source == 0) {                                     \
      ParseError("Unexpected EOF at %d:%d (called from %s:%d)", \
                 ctx->line, ctx->col, __FILE__, __LINE__);      \
    }                                                           \
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
  if(*ctx->source == ')') {
    return NULL;
  }
  LispExpression *left, *right;
  left = lisp_parse_expression(ctx);
  SKIP_WHITESPACE(ctx);
  ASSERT_NO_EOF(ctx);
  right = (*ctx->source == ')') ? NULL :
    lisp_parse_list(ctx);
  return make_lisp_cons(left, right);
}

#define StringParseSetup(name)                  \
  int name ## _prealloc = 8, name ## _len = 0;  \
  char *name = malloc(name ## _prealloc)
#define StringParseChar(name, c) name[name ## _len++] = c
#define StringParseCheck(name)                  \
  if(name ## _len == name ## _prealloc) {       \
    name ## _prealloc *= 2;                     \
      name = realloc(name, name ## _prealloc);  \
  }
#define StringParseDone(name)                   \
  name = realloc(name, name ## _len + 1);       \
  name[name ## _len] = 0

LispExpression *lisp_parse_string(LispParseContext *ctx) {
  int escape = 0;
  StringParseSetup(string);
  while(*ctx->source != '"' || escape) {
    ASSERT_NO_EOF(ctx);
    StringParseChar(string, *ctx->source);
    escape = (*ctx->source == '\\' && !escape) ? 1 : 0;
    if(*ctx->source == '\n') ADVANCE_LINE(ctx) else ADVANCE(ctx);
    StringParseCheck(string);
  }
  ADVANCE(ctx);
  StringParseDone(string);
  return make_lisp_string(string);
}

LispExpression *lisp_parse_number(LispParseContext *ctx) {
  int number = 0;
  while(*ctx->source >= '0' && *ctx->source <= '9') {
    number *= 10;
    number += *ctx->source - '0';
    ADVANCE(ctx);
  }
  if(*ctx->source && *ctx->source != ' ' &&
     *ctx->source != '\n' && *ctx->source != ')') {
    ParseError("Unexpected symbol '%c' (0x%x), expected one of ' ', '\\n', ')', EOF\n",
               *ctx->source, *ctx->source);
  }
  return make_lisp_number(number);
}

LispExpression *lisp_parse_symbol(LispParseContext *ctx) {
  StringParseSetup(symbol);
  ;
  for(char c = *ctx->source; c != 0 && c != ')' && c != ' ' && c != '\n';
      c = *ctx->source) {
    StringParseChar(symbol, c);
    StringParseCheck(symbol);
    ADVANCE(ctx);
  }
  StringParseDone(symbol);
  return make_lisp_symbol(symbol);
}

LispExpression *lisp_parse_expression(LispParseContext *ctx) {
  char c = *ctx->source;
  switch(c) {
  case 0:
    return NULL;
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
