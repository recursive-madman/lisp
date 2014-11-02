
#include "test-helper.h"
#include "../lisp.h"

#define ASSERT_NUMBER(exp, n)                   \
  ASSERT(exp);                                  \
  ASSERT_EQUAL(LISP_NUMBER, exp->type);         \
  ASSERT_EQUAL(n, exp->value.number)

#define ASSERT_SYMBOL(exp, s)                     \
  ASSERT(exp);                                    \
  ASSERT_EQUAL(LISP_SYMBOL, exp->type);           \
  ASSERT_EQUAL_STRING(s, exp->value.symbol)


int main(int argc, char **argv) {
  LispExpression *exp;

  LispContext *ctx = lisp_context_create(NULL);

# define EVAL_PARSE(string)                     \
  exp = lisp_evaluate(lisp_parse(string), ctx)

  TEST_GROUP("Evaluator -- simple expressions");

  TEST_CASE("Number evaluates to itself");
  EVAL_PARSE("123");
  ASSERT_NUMBER(exp, 123);
  TEST_DONE();

  TEST_CASE("Quoted number evalutes to number");
  EVAL_PARSE("'123");
  ASSERT_NUMBER(exp, 123);
  TEST_DONE();

  TEST_CASE("Double-quoted number evalutes to quoted number");
  EVAL_PARSE("''123");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_QUOTE, exp->type);
  ASSERT_NUMBER(exp->value.quoted, 123);
  TEST_DONE();

  TEST_CASE("Quoted symbol evaluates to symbol");
  EVAL_PARSE("'foo");
  ASSERT_SYMBOL(exp, "foo");
  TEST_DONE();


  lisp_context_set(ctx, make_lisp_symbol("foo"), make_lisp_symbol("bar"));

  TEST_GROUP("Evaluator -- resolution of symbols");

  TEST_CASE("A symbol resolves to it's set value");
  EVAL_PARSE("foo");
  ASSERT_SYMBOL(exp, "bar");
  TEST_DONE();

  // switch to derived context
  LispContext *tmp = ctx;
  ctx = lisp_context_create(tmp);

  TEST_CASE("Symbols are resolved from the parent table");
  EVAL_PARSE("foo");
  ASSERT_SYMBOL(exp, "bar");
  TEST_DONE();

  lisp_context_set(ctx, make_lisp_symbol("foo"), make_lisp_number(42));
  
  TEST_CASE("The local table overrules the parent table");
  EVAL_PARSE("foo");
  ASSERT_NUMBER(exp, 42);
  TEST_DONE();

  // switch back to old context
  LispContext *tmp2 = ctx;
  ctx = tmp;

  TEST_CASE("Values from the local table don't taint the parent table");
  EVAL_PARSE("foo");
  ASSERT_SYMBOL(exp, "bar");
  TEST_DONE();

  // we don't need this any more.
  lisp_context_destroy(tmp2);

  return 0;
}
