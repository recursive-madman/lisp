
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdint.h>

typedef struct _LispExpression LispExpression;
typedef struct _LispContext LispContext;
typedef void(*LispCallback)(LispExpression *args, LispContext *ctx);

typedef struct _SymbolTable SymbolTable;

SymbolTable *symbol_table_create(uint32_t size);
void symbol_table_destroy(SymbolTable *tab, LispCallback cb, LispContext *ctx);
LispExpression *symbol_table_add(SymbolTable *tab, char *key, LispExpression *value);
LispExpression *symbol_table_lookup(SymbolTable *tab, char *key, int *found);

#endif
