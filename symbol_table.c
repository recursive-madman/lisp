
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "symbol_table.h"

typedef uint32_t hash_t;

typedef struct _SymbolTableEntry SymbolTableEntry;

struct _SymbolTableEntry {
  hash_t hash;
  char *key;
  LispExpression *value;
  SymbolTableEntry *next;
};

struct _SymbolTable {
  SymbolTableEntry **entries;
  hash_t size;
  hash_t mask;
};

// djb hash
hash_t string_hash (char *key) {
  int len = strlen(key);
  hash_t h = 0;
  for(int i = 0; i < len; i++) {
    h = 33 * h + key[i];
  }
  return h;
}

SymbolTable *symbol_table_create(uint32_t size) {
  SymbolTable *tab = malloc(sizeof(SymbolTable));

  int bits = (int)ceil(log2((double)size));
  tab->size = 1 << bits;
  tab->entries = calloc(tab->size, sizeof(SymbolTableEntry*));
  tab->mask = 0;
  for(int i = 0; i < (bits-1); i++) {
    tab->mask |= 1 << i;
  }
  return tab;
}

void symbol_table_destroy(SymbolTable *tab, LispCallback cb, LispContext *ctx) {
  for(int i = 0; i < tab->size; i++) {
    for(SymbolTableEntry *entry = tab->entries[i], *next = NULL;
        entry != NULL; entry = next) {
      cb(entry->value, ctx);
      free(entry->key);
      next = entry->next;
      free(entry);
    }
  }
  free(tab->entries);
  free(tab);
}

SymbolTableEntry *symbol_table_lookup_entry(SymbolTable *tab, char *key,
                                            hash_t hash, hash_t bucket) {
  for(SymbolTableEntry *entry = tab->entries[bucket];
      NULL != entry; entry = entry->next) {
    if(entry->hash == hash && 0 == strcmp(entry->key, key)) {
      return entry;
    }
  }
  return NULL;
}

LispExpression *symbol_table_add(SymbolTable *tab, char *key,
                                 LispExpression *value) {
  hash_t hash = string_hash(key);
  hash_t bucket = hash & tab->mask;
  SymbolTableEntry *entry;
  entry = symbol_table_lookup_entry(tab, key, hash, bucket);
  LispExpression *previous_value = NULL;
  if(NULL == entry) {
    entry = malloc(sizeof(SymbolTableEntry));
    entry->hash = hash;
    entry->key = strdup(key);
    entry->value = value;
  } else {
    previous_value = entry->value;
    entry->value = value;
  }
  entry->next = tab->entries[bucket];
  tab->entries[bucket] = entry;
  return previous_value;
}

LispExpression *symbol_table_lookup(SymbolTable *tab, char *key, int *found) {
  hash_t hash = string_hash(key);
  hash_t bucket = hash & tab->mask;
  SymbolTableEntry *entry;
  entry = symbol_table_lookup_entry(tab, key, hash, bucket);
  if(NULL != entry) {
    *found = 1;
    return entry->value;
  } else {
    *found = 0;
    return NULL;
  }
}
