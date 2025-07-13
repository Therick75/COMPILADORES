#include "xml_tree.h"
#include <ctype.h>

// Crear un elemento XML
XMLNode* create_element(char *name, AttributeList *attrs, XMLNode *children) {
    XMLNode *node = (XMLNode*)malloc(sizeof(XMLNode));
    node->type = NODE_ELEMENT;
    node->name = strdup(name);
    node->content = NULL;
    node->attributes = attrs;
    node->children = children;
    node->next = NULL;
    node->parent = NULL;
    
    // Establecer el padre de los hijos
    XMLNode *child = children;
    while (child) {
        child->parent = node;
        child = child->next;
    }
    
    return node;
}

// Crear un nodo de texto
XMLNode* create_text_node(char *text) {
    XMLNode *node = (XMLNode*)malloc(sizeof(XMLNode));
    node->type = NODE_TEXT;
    node->name = NULL;
    node->content = strdup(text);
    node->attributes = NULL;
    node->children = NULL;
    node->next = NULL;
    node->parent = NULL;
    return node;
}

// Crear un nodo CDATA
XMLNode* create_cdata_node(char *data) {
    XMLNode *node = (XMLNode*)malloc(sizeof(XMLNode));
    node->type = NODE_CDATA;
    node->name = NULL;
    node->content = strdup(data);
    node->attributes = NULL;
    node->children = NULL;
    node->next = NULL;
    node->parent = NULL;
    return node;
}

// Crear un atributo
Attribute* create_attribute(char *name, char *value) {
    Attribute *attr = (Attribute*)malloc(sizeof(Attribute));
    attr->name = strdup(name);
    attr->value = strdup(value);
    attr->next = NULL;
    return attr;
}

// Agregar atributo a la lista
AttributeList* add_attribute(AttributeList *list, Attribute *attr) {
    if (!list) {
        list = (AttributeList*)malloc(sizeof(AttributeList));
        list->first = NULL;
        list->last = NULL;
        list->count = 0;
    }
    
    if (!list->first) {
        list->first = attr;
        list->last = attr;
    } else {
        list->last->next = attr;
        list->last = attr;
    }
    list->count++;
    return list;
}

// Agregar contenido a la lista
XMLNode* add_content(XMLNode *list, XMLNode *node) {
    if (!list) {
        return node;
    }
    
    XMLNode *current = list;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
    return list;
}

// Imprimir el árbol XML
void print_xml_tree(XMLNode *node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    switch (node->type) {
        case NODE_ELEMENT:
            printf("<%s", node->name);
            if (node->attributes) {
                Attribute *attr = node->attributes->first;
                while (attr) {
                    printf(" %s=\"%s\"", attr->name, attr->value);
                    attr = attr->next;
                }
            }
            printf(">\n");
            
            print_xml_tree(node->children, depth + 1);
            
            for (int i = 0; i < depth; i++) printf("  ");
            printf("</%s>\n", node->name);
            break;
            
        case NODE_TEXT:
            printf("TEXT: %s\n", node->content);
            break;
            
        case NODE_CDATA:
            printf("CDATA: %s\n", node->content);
            break;
    }
    
    print_xml_tree(node->next, depth);
}

// Liberar memoria del árbol
void free_xml_tree(XMLNode *node) {
    if (!node) return;
    
    free_xml_tree(node->children);
    free_xml_tree(node->next);
    
    if (node->name) free(node->name);
    if (node->content) free(node->content);
    
    if (node->attributes) {
        Attribute *attr = node->attributes->first;
        while (attr) {
            Attribute *next = attr->next;
            free(attr->name);
            free(attr->value);
            free(attr);
            attr = next;
        }
        free(node->attributes);
    }
    
    free(node);
}

// Contar elementos
int count_elements(XMLNode *node) {
    if (!node) return 0;
    
    int count = 0;
    if (node->type == NODE_ELEMENT) {
        count = 1;
        count += count_elements(node->children);
    }
    count += count_elements(node->next);
    return count;
}

// Contar atributos
int count_attributes(XMLNode *node) {
    if (!node) return 0;
    
    int count = 0;
    if (node->type == NODE_ELEMENT && node->attributes) {
        count = node->attributes->count;
        count += count_attributes(node->children);
    }
    count += count_attributes(node->next);
    return count;
}

// Funciones auxiliares para XPath
XMLNode* find_elements_by_name(XMLNode *node, const char *name) {
    if (!node) return NULL;
    
    XMLNode *result = NULL;
    XMLNode *last_result = NULL;
    
    if (node->type == NODE_ELEMENT && strcmp(node->name, name) == 0) {
        result = (XMLNode*)malloc(sizeof(XMLNode));
        memcpy(result, node, sizeof(XMLNode));
        result->next = NULL;
        last_result = result;
    }
    
    // Buscar en hijos
    XMLNode *child_results = find_elements_by_name(node->children, name);
    if (child_results) {
        if (result) {
            last_result->next = child_results;
        } else {
            result = child_results;
        }
        while (last_result && last_result->next) {
            last_result = last_result->next;
        }
    }
    
    // Buscar en hermanos
    XMLNode *sibling_results = find_elements_by_name(node->next, name);
    if (sibling_results) {
        if (result) {
            last_result->next = sibling_results;
        } else {
            result = sibling_results;
        }
    }
    
    return result;
}

// Implementación básica de XPath
XMLNode* xpath_query(XMLNode *root, const char *xpath) {
    if (!root || !xpath) return NULL;
    
    // Implementación básica para consultas simples
    if (xpath[0] == '/') {
        // Consulta absoluta
        if (xpath[1] == '/') {
            // Búsqueda en todo el documento
            return find_elements_by_name(root, xpath + 2);
        } else {
            // Búsqueda desde la raíz
            return find_elements_by_name(root, xpath + 1);
        }
    } else {
        // Consulta relativa
        return find_elements_by_name(root, xpath);
    }
}

// Imprimir resultados de XPath
void print_xpath_results(XMLNode *results) {
    XMLNode *current = results;
    int count = 0;
    
    while (current) {
        count++;
        printf("Resultado %d:\n", count);
        print_xml_tree(current, 1);
        current = current->next;
    }
    
    if (count == 0) {
        printf("No se encontraron resultados.\n");
    } else {
        printf("Total de resultados: %d\n", count);
    }
}

// Modo interactivo para consultas XPath
void xpath_interactive_mode(XMLNode *root) {
    char xpath[256];
    
    while (1) {
        printf("XPath> ");
        if (!fgets(xpath, sizeof(xpath), stdin)) {
            break;
        }
        
        // Remover salto de línea
        xpath[strcspn(xpath, "\n")] = '\0';
        
        if (strcmp(xpath, "quit") == 0) {
            break;
        }
        
        if (strlen(xpath) == 0) {
            continue;
        }
        
        XMLNode *results = xpath_query(root, xpath);
        print_xpath_results(results);
        
        // Liberar resultados
        free_xml_tree(results);
    }
}