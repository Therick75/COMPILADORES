#include "xml_tree.h"
#include <ctype.h>
#include <string.h>

// Estructura para resultados de XPath
typedef struct XPathResult {
    XMLNode **nodes;
    int count;
    int capacity;
} XPathResult;

// Inicializar resultado XPath
XPathResult* init_xpath_result() {
    XPathResult *result = (XPathResult*)malloc(sizeof(XPathResult));
    result->nodes = NULL;
    result->count = 0;
    result->capacity = 0;
    return result;
}

// Agregar nodo al resultado
void add_to_result(XPathResult *result, XMLNode *node) {
    if (result->count >= result->capacity) {
        result->capacity = result->capacity == 0 ? 10 : result->capacity * 2;
        result->nodes = (XMLNode**)realloc(result->nodes, result->capacity * sizeof(XMLNode*));
    }
    result->nodes[result->count++] = node;
}

// Liberar resultado XPath
void free_xpath_result(XPathResult *result) {
    if (result) {
        if (result->nodes) free(result->nodes);
        free(result);
    }
}

// Buscar por nombre de elemento
void find_by_element_name(XMLNode *node, const char *name, XPathResult *result) {
    if (!node) return;
    
    if (node->type == NODE_ELEMENT && strcmp(node->name, name) == 0) {
        add_to_result(result, node);
    }
    
    find_by_element_name(node->children, name, result);
    find_by_element_name(node->next, name, result);
}

// Buscar por atributo
void find_by_attribute(XMLNode *node, const char *attr_name, const char *attr_value, XPathResult *result) {
    if (!node) return;
    
    if (node->type == NODE_ELEMENT && node->attributes) {
        Attribute *attr = node->attributes->first;
        while (attr) {
            if (strcmp(attr->name, attr_name) == 0) {
                if (!attr_value || strcmp(attr->value, attr_value) == 0) {
                    add_to_result(result, node);
                    break;
                }
            }
            attr = attr->next;
        }
    }
    
    find_by_attribute(node->children, attr_name, attr_value, result);
    find_by_attribute(node->next, attr_name, attr_value, result);
}

// Buscar hijos directos
void find_direct_children(XMLNode *parent, const char *name, XPathResult *result) {
    if (!parent || !parent->children) return;
    
    XMLNode *child = parent->children;
    while (child) {
        if (child->type == NODE_ELEMENT && strcmp(child->name, name) == 0) {
            add_to_result(result, child);
        }
        child = child->next;
    }
}

// Buscar por posición
void find_by_position(XMLNode *parent, const char *name, int position, XPathResult *result) {
    if (!parent || !parent->children) return;
    
    XMLNode *child = parent->children;
    int current_pos = 1;
    
    while (child) {
        if (child->type == NODE_ELEMENT && strcmp(child->name, name) == 0) {
            if (current_pos == position) {
                add_to_result(result, child);
                return;
            }
            current_pos++;
        }
        child = child->next;
    }
}

// Buscar por texto contenido
void find_by_text_content(XMLNode *node, const char *text, XPathResult *result) {
    if (!node) return;
    
    if (node->type == NODE_TEXT && strstr(node->content, text)) {
        if (node->parent) {
            add_to_result(result, node->parent);
        }
    }
    
    find_by_text_content(node->children, text, result);
    find_by_text_content(node->next, text, result);
}

// Parser mejorado de XPath
XPathResult* xpath_query_extended(XMLNode *root, const char *xpath) {
    XPathResult *result = init_xpath_result();
    
    if (!root || !xpath) return result;
    
    char *xpath_copy = strdup(xpath);
    char *token = strtok(xpath_copy, "/");
    XMLNode *current_context = root;
    
    // Manejar consulta absoluta
    if (xpath[0] == '/') {
        if (xpath[1] == '/') {
            // Búsqueda descendiente (//)
            token = strtok(NULL, "/");
            if (token) {
                find_by_element_name(root, token, result);
            }
            free(xpath_copy);
            return result;
        }
        // Búsqueda desde raíz (/)
        token = strtok(NULL, "/");
    }
    
    while (token) {
        XPathResult *temp_result = init_xpath_result();
        
        // Verificar si es una consulta con predicado
        if (strstr(token, "[@")) {
            // Consulta con atributo: element[@attr='value']
            char *element_name = strtok(token, "[");
            char *predicate = strtok(NULL, "]");
            
            if (predicate && predicate[0] == '@') {
                char *attr_part = predicate + 1;
                char *attr_name = strtok(attr_part, "=");
                char *attr_value = strtok(NULL, "'\"");
                
                if (attr_value) {
                    // Remover comillas si existen
                    if (attr_value[0] == '\'' || attr_value[0] == '"') {
                        attr_value++;
                    }
                    int len = strlen(attr_value);
                    if (len > 0 && (attr_value[len-1] == '\'' || attr_value[len-1] == '"')) {
                        attr_value[len-1] = '\0';
                    }
                }
                
                find_by_attribute(current_context, attr_name, attr_value, temp_result);
            }
        } else if (strstr(token, "[") && strstr(token, "]")) {
            // Consulta con posición: element[1]
            char *element_name = strtok(token, "[");
            char *position_str = strtok(NULL, "]");
            int position = atoi(position_str);
            
            find_by_position(current_context, element_name, position, temp_result);
        } else if (strstr(token, "text()")) {
            // Consulta de texto: contains(text(), 'value')
            // Implementación básica
            find_by_element_name(current_context, "text", temp_result);
        } else {
            // Consulta simple de elemento
            if (strcmp(token, "*") == 0) {
                // Todos los elementos
                find_by_element_name(current_context, "*", temp_result);
            } else {
                find_by_element_name(current_context, token, temp_result);
            }
        }
        
        // Actualizar contexto para próxima iteración
        if (temp_result->count > 0) {
            // Por simplicidad, usar el primer resultado como contexto
            current_context = temp_result->nodes[0];
        }
        
        // Combinar resultados
        for (int i = 0; i < temp_result->count; i++) {
            add_to_result(result, temp_result->nodes[i]);
        }
        
        free_xpath_result(temp_result);
        token = strtok(NULL, "/");
    }
    
    free(xpath_copy);
    return result;
}

// Funciones auxiliares para XPath
void print_xpath_results_extended(XPathResult *result) {
    if (!result || result->count == 0) {
        printf("No se encontraron resultados.\n");
        return;
    }
    
    printf("Encontrados %d resultado(s):\n", result->count);
    for (int i = 0; i < result->count; i++) {
        XMLNode *node = result->nodes[i];
        printf("\n--- Resultado %d ---\n", i + 1);
        
        if (node->type == NODE_ELEMENT) {
            printf("Elemento: <%s", node->name);
            
            if (node->attributes) {
                Attribute *attr = node->attributes->first;
                while (attr) {
                    printf(" %s=\"%s\"", attr->name, attr->value);
                    attr = attr->next;
                }
            }
            printf(">\n");
            
            // Mostrar contenido de texto si existe
            XMLNode *child = node->children;
            while (child) {
                if (child->type == NODE_TEXT) {
                    printf("Contenido: %s\n", child->content);
                }
                child = child->next;
            }
            
            // Mostrar ruta del elemento
            printf("Ruta: ");
            XMLNode *parent = node->parent;
            char path[1000] = "";
            char temp[100];
            
            // Construir ruta hacia arriba
            while (parent) {
                snprintf(temp, sizeof(temp), "/%s", parent->name);
                memmove(path + strlen(temp), path, strlen(path) + 1);
                memcpy(path, temp, strlen(temp));
                parent = parent->parent;
            }
            printf("%s/%s\n", path, node->name);
        }
    }
}

// Modo interactivo extendido para XPath
void xpath_interactive_mode_extended(XMLNode *root) {
    char xpath[512];
    
    printf("\nModo consulta XPath extendido\n");
    printf("Comandos disponibles:\n");
    printf("  /elemento          - Buscar elemento desde raíz\n");
    printf("  //elemento         - Buscar elemento en todo el documento\n");
    printf("  elemento[@attr='valor'] - Buscar por atributo\n");
    printf("  elemento[1]        - Buscar por posición\n");
    printf("  help               - Mostrar ayuda\n");
    printf("  quit               - Salir\n\n");
    
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
        
        if (strcmp(xpath, "help") == 0) {
            printf("Ejemplos de consultas XPath:\n");
            printf("  /root              - Elemento raíz\n");
            printf("  //libro            - Todos los libros\n");
            printf("  /root/libro[1]     - Primer libro\n");
            printf("  //libro[@id='1']   - Libro con id='1'\n");
            printf("  //*[@genero='ficcion'] - Elementos con genero='ficcion'\n");
            continue;
        }
        
        if (strlen(xpath) == 0) {
            continue;
        }
        
        XPathResult *results = xpath_query_extended(root, xpath);
        print_xpath_results_extended(results);
        free_xpath_result(results);
    }
}