#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <stdio.h>

#define TEST_GROUP(name)                        \
  fprintf(stderr, "\nGroup: %s\n", name)
#define TEST_CASE(name)                         \
  fprintf(stderr, "  - %s ", name)
#define TEST_DONE()                             \
  fprintf(stderr, " OK\n")

#define ASSERT_FAILED(thing)                        \
  fprintf(stderr, "E\n");                           \
  fprintf(stderr, "Assertion failed: %s (%s:%d)\n", \
          thing, __FILE__, __LINE__);               \
  abort()
#define ASSERT_SUCCESS(thing)                   \
  fprintf(stderr, ".")

#define ASSERT(exp)                             \
  if(exp) {                                     \
    ASSERT_SUCCESS();                           \
  } else {                                      \
    ASSERT_FAILED(__STRING(exp));               \
  }
#define ASSERT_EQUAL(a, b)                      \
  ASSERT((a) == (b))

#define ASSERT_EQUAL_STRING(a, b)                 \
  if(0 == strcmp(a, b)) {                         \
    ASSERT_SUCCESS();                             \
  } else {                                        \
    char buf[1024];                               \
    sprintf(buf, "'%s' not equal to '%s'", a, b); \
    ASSERT_FAILED(buf);                           \
  }

#endif
