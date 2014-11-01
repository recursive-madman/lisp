#ifndef DECLARATION_H
#define DECLARATION_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// declaration macros

#define DeclareThing(what, name, block)          \
  typedef what _ ## name name;                   \
  what _ ## name block

#define DeclareType(name, block)                \
  DeclareThing(struct, name, block)

#define DeclareUnion(name, block)                \
  DeclareThing(union, name, block)


#define ArrayAttribute(type, name)              \
  type * name;                                  \
  int n_ ## name


// helper macros for implementation

#define Allocate(type, name)                     \
  type *name = calloc(1, sizeof(type));          \
  if(NULL == name) {                             \
    perror("calloc()");                          \
    abort();                                     \
  }

#define Destroy(type, name)                     \
  memset(name, 0, sizeof(type));                \
  free(name);                                   \
  name = NULL

#define AllocateType(type) calloc(1, sizeof(type))

#endif
