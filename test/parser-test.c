
#include "test-helper.h"

#include "../lisp.h"

int main(int argc, char **argv) {
  LispExpression *exp;

  TEST_GROUP("Parser");

  TEST_CASE("Number");
  exp = lisp_parse("712");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_NUMBER, exp->type);
  ASSERT_EQUAL(712, exp->value.number);
  TEST_DONE();


  TEST_CASE("Symbol");
  exp = lisp_parse("foobar");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_SYMBOL, exp->type);
  ASSERT_EQUAL_STRING("foobar", exp->value.symbol);
  TEST_DONE();

  TEST_CASE("Simple string");
  exp = lisp_parse("\"foo bar\"");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_STRING, exp->type);
  ASSERT_EQUAL_STRING("foo bar", exp->value.string);
  TEST_DONE();

  TEST_CASE("String with escaped double quotes");
  exp = lisp_parse("\"bla \\\"blubb\\\" bla\"");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_STRING, exp->type);
  ASSERT_EQUAL_STRING("bla \"blubb\" bla", exp->value.string);
  TEST_DONE();

  TEST_CASE("String with escaped backslash");
  exp = lisp_parse("\"foo: \\\\\"");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_STRING, exp->type);
  ASSERT_EQUAL_STRING("foo: \\", exp->value.string);
  TEST_DONE();

  TEST_CASE("Quoted symbol");
  exp = lisp_parse("'foo");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_QUOTE, exp->type);
  ASSERT(exp->value.quoted);
  ASSERT_EQUAL(LISP_SYMBOL, exp->value.quoted->type);
  ASSERT_EQUAL_STRING("foo", exp->value.quoted->value.symbol);
  TEST_DONE();

  TEST_CASE("Quoted number");
  exp = lisp_parse("'123");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_QUOTE, exp->type);
  ASSERT(exp->value.quoted);
  ASSERT_EQUAL(LISP_NUMBER, exp->value.quoted->type);
  ASSERT_EQUAL(123, exp->value.quoted->value.number);
  TEST_DONE();

  TEST_CASE("Empty list is nil");
  exp = lisp_parse("()");
  ASSERT_EQUAL(NULL, exp);
  TEST_DONE();

  TEST_CASE("Quoted empty list is nil");
  exp = lisp_parse("'()");
  ASSERT_EQUAL(NULL, exp);
  TEST_DONE();

  TEST_CASE("List of two elements");
  exp = lisp_parse("(a b)");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_CONS, exp->type);
  ASSERT(CAR(exp));
  ASSERT_EQUAL(LISP_SYMBOL, CAR(exp)->type);
  ASSERT_EQUAL_STRING("a", CAR(exp)->value.symbol);
  ASSERT(CDR(exp));
  ASSERT_EQUAL(LISP_CONS, CDR(exp)->type);
  ASSERT(CADR(exp));
  ASSERT_EQUAL(LISP_SYMBOL, CADR(exp)->type);
  ASSERT_EQUAL_STRING("b", CADR(exp)->value.symbol);
  ASSERT_EQUAL(NULL, CDDR(exp));
  TEST_DONE();

  TEST_CASE("List of three elements, with nil in between");
  exp = lisp_parse("(a '() b)");
  ASSERT(exp);
  ASSERT_EQUAL(LISP_CONS, exp->type);
  ASSERT(CAR(exp));
  ASSERT_EQUAL(LISP_SYMBOL, CAR(exp)->type);
  ASSERT_EQUAL_STRING("a", CAR(exp)->value.symbol);
  ASSERT(CDR(exp));
  ASSERT_EQUAL(LISP_CONS, CDR(exp)->type);
  ASSERT_EQUAL(NULL, CADR(exp));
  ASSERT(CDDR(exp));
  ASSERT_EQUAL(LISP_SYMBOL, CAR(CDDR(exp))->type);
  ASSERT_EQUAL_STRING("b", CAR(CDDR(exp))->value.symbol);
  ASSERT_EQUAL(NULL, CDR(CDDR(exp)));
  TEST_DONE();

  TEST_GROUP("Multiline parser");

  TEST_CASE("Parsing a partial list");

  char *some_context = "blabla";
  int x = 0;
  char *continue_parsing(void *read_context) {
    ASSERT_EQUAL(read_context, some_context);
    if(x) {
      return NULL;
    } else {
      x = 1;
      return " bar)";
    }
  }
  exp = lisp_parse_multi("(foo ", continue_parsing, some_context);
  ASSERT(exp);
  ASSERT_EQUAL(LISP_CONS, exp->type);
  ASSERT_SYMBOL(CAR(exp), "foo");
  ASSERT(CDR(exp));
  ASSERT_SYMBOL(CADR(exp), "bar");
  ASSERT_EQUAL(NULL, CDDR(exp));
  TEST_DONE();

  return 0;
}
