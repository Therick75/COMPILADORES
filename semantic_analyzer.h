#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "xml_tree.h"
#include <stdbool.h>

// Estructura para la tabla semántica
typedef struct SemanticEntry {
    char *element_name;
    int count;
    char **attribute_names;
    int attr_count;
    struct SemanticEntry *next;
} SemanticEntry;

typedef struct SemanticTable {
    SemanticEntry *entries;
    int total_elements;
    int total_attributes;
    bool has_root;
    char *root_name;
} SemanticTable;

// Funciones del analizador semántico
void init_semantic_table(SemanticTable *table);
void free_semantic_table(SemanticTable *table);
bool semantic_analyze(XMLNode *root, SemanticTable *table);
void print_semantic_table(SemanticTable *table);

// Funciones de validación
bool validate_xml_structure(XMLNode *root);
bool validate_element_names(XMLNode *node);
bool validate_attribute_names(XMLNode *node);
bool check_well_formed(XMLNode *root);

// Funciones auxiliares
SemanticEntry* find_or_create_entry(SemanticTable *table, const char *element_name);
void add_attribute_to_entry(SemanticEntry *entry, const char *attr_name);
bool attribute_exists_in_entry(SemanticEntry *entry, const char *attr_name);

#endif