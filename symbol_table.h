
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "lisp.h"

#include <stdint.h>

typedef struct _SymbolTable SymbolTable;

SymbolTable *symbol_table_create(uint32_t size);
LispExpression *symbol_table_add(SymbolTable *tab, char *key, LispExpression *value);
LispExpression *symbol_table_lookup(SymbolTable *tab, char *key);

#endif
