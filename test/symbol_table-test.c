
#include "test-helper.h"
#include "../lisp.h"

typedef uint32_t hash_t;

typedef struct _SymbolTableEntry SymbolTableEntry;
struct _SymbolTable {
  SymbolTableEntry **entries;
  hash_t size;
  hash_t mask;
};

int main(int argc, char **argv) {
  TEST_GROUP("Symbol table");

  TEST_CASE("Create table");
  SymbolTable *tab = symbol_table_create(32);
  TEST_DONE();

  TEST_CASE("Verify size");
  ASSERT_EQUAL(32, tab->size);
  TEST_DONE();

  TEST_CASE("Verify mask");
  ASSERT_EQUAL(0x0F, tab->mask);
  TEST_DONE();

  TEST_CASE("Add a value as 'foo'");
  LispExpression *value = make_lisp_number(42);
  ASSERT_EQUAL(NULL, symbol_table_add(tab, "foo", value));
  TEST_DONE();

  TEST_CASE("Lookup the value of 'foo'");
  ASSERT_EQUAL(value, symbol_table_lookup(tab, "foo"));
  TEST_DONE();

  TEST_CASE("Add another value as 'foo'");
  LispExpression *other_value = make_lisp_number(23);
  ASSERT_EQUAL(value, symbol_table_add(tab, "foo", other_value));
  TEST_DONE();

  TEST_CASE("Lookup the value of 'foo'");
  ASSERT_EQUAL(other_value, symbol_table_lookup(tab, "foo"));
  TEST_DONE();

  TEST_CASE("Add a value as 'bar'");
  LispExpression *third_value = make_lisp_number(77);
  ASSERT_EQUAL(NULL, symbol_table_add(tab, "bar", third_value));
  TEST_DONE();

  TEST_CASE("Lookup the value of 'bar'");
  ASSERT_EQUAL(third_value, symbol_table_lookup(tab, "bar"));
  TEST_DONE();

  return 0;
}
