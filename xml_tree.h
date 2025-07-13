#ifndef XML_TREE_H
#define XML_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tipos de nodos
typedef enum {
    NODE_ELEMENT,
    NODE_TEXT,
    NODE_CDATA
} NodeType;

// Estructura para atributos
typedef struct Attribute {
    char *name;
    char *value;
    struct Attribute *next;
} Attribute;

typedef struct AttributeList {
    Attribute *first;
    Attribute *last;
    int count;
} AttributeList;

// Estructura para nodos XML
typedef struct XMLNode {
    NodeType type;
    char *name;           // Para elementos
    char *content;        // Para texto y CDATA
    AttributeList *attributes;
    struct XMLNode *children;
    struct XMLNode *next;
    struct XMLNode *parent;
} XMLNode;

// Funciones para crear nodos
XMLNode* create_element(char *name, AttributeList *attrs, XMLNode *children);
XMLNode* create_text_node(char *text);
XMLNode* create_cdata_node(char *data);

// Funciones para atributos
Attribute* create_attribute(char *name, char *value);
AttributeList* add_attribute(AttributeList *list, Attribute *attr);

// Funciones para contenido
XMLNode* add_content(XMLNode *list, XMLNode *node);

// Funciones de utilidad
void print_xml_tree(XMLNode *node, int depth);
void free_xml_tree(XMLNode *node);
int count_elements(XMLNode *node);
int count_attributes(XMLNode *node);

// Funciones para XPath
void xpath_interactive_mode(XMLNode *root);
XMLNode* xpath_query(XMLNode *root, const char *xpath);
void print_xpath_results(XMLNode *results);

#endif