#include "semantic_analyzer.h"
#include <ctype.h>

// Inicializar tabla semántica
void init_semantic_table(SemanticTable *table) {
    table->entries = NULL;
    table->total_elements = 0;
    table->total_attributes = 0;
    table->has_root = false;
    table->root_name = NULL;
}

// Liberar memoria de la tabla semántica
void free_semantic_table(SemanticTable *table) {
    SemanticEntry *current = table->entries;
    while (current) {
        SemanticEntry *next = current->next;
        free(current->element_name);
        
        for (int i = 0; i < current->attr_count; i++) {
            free(current->attribute_names[i]);
        }
        free(current->attribute_names);
        free(current);
        current = next;
    }
    
    if (table->root_name) {
        free(table->root_name);
    }
}

// Validar que los nombres de elementos sean válidos
bool validate_element_names(XMLNode *node) {
    if (!node) return true;
    
    if (node->type == NODE_ELEMENT) {
        if (!node->name || strlen(node->name) == 0) {
            printf("Error semántico: Elemento sin nombre\n");
            return false;
        }
        
        // Verificar que el nombre comience con letra o underscore
        if (!isalpha(node->name[0]) && node->name[0] != '_') {
            printf("Error semántico: Nombre de elemento '%s' inválido\n", node->name);
            return false;
        }
        
        // Verificar que contenga solo caracteres válidos
        for (int i = 1; i < strlen(node->name); i++) {
            if (!isalnum(node->name[i]) && node->name[i] != '_' && 
                node->name[i] != '-' && node->name[i] != '.') {
                printf("Error semántico: Nombre de elemento '%s' contiene caracteres inválidos\n", node->name);
                return false;
            }
        }
    }
    
    return validate_element_names(node->children) && validate_element_names(node->next);
}

// Validar que los nombres de atributos sean válidos
bool validate_attribute_names(XMLNode *node) {
    if (!node) return true;
    
    if (node->type == NODE_ELEMENT && node->attributes) {
        Attribute *attr = node->attributes->first;
        while (attr) {
            if (!attr->name || strlen(attr->name) == 0) {
                printf("Error semántico: Atributo sin nombre\n");
                return false;
            }
            
            // Verificar que el nombre comience con letra o underscore
            if (!isalpha(attr->name[0]) && attr->name[0] != '_') {
                printf("Error semántico: Nombre de atributo '%s' inválido\n", attr->name);
                return false;
            }
            
            // Verificar que contenga solo caracteres válidos
            for (int i = 1; i < strlen(attr->name); i++) {
                if (!isalnum(attr->name[i]) && attr->name[i] != '_' && 
                    attr->name[i] != '-' && attr->name[i] != '.') {
                    printf("Error semántico: Nombre de atributo '%s' contiene caracteres inválidos\n", attr->name);
                    return false;
                }
            }
            
            attr = attr->next;
        }
    }
    
    return validate_attribute_names(node->children) && validate_attribute_names(node->next);
}

// Verificar que el XML esté bien formado
bool check_well_formed(XMLNode *root) {
    if (!root) {
        printf("Error semántico: Documento XML vacío\n");
        return false;
    }
    
    if (root->type != NODE_ELEMENT) {
        printf("Error semántico: La raíz debe ser un elemento\n");
        return false;
    }
    
    // Verificar que no haya elementos hermanos a la raíz
    if (root->next) {
        printf("Error semántico: Solo puede haber un elemento raíz\n");
        return false;
    }
    
    return true;
}

// Encontrar o crear entrada en la tabla semántica
SemanticEntry* find_or_create_entry(SemanticTable *table, const char *element_name) {
    SemanticEntry *current = table->entries;
    
    // Buscar entrada existente
    while (current) {
        if (strcmp(current->element_name, element_name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    // Crear nueva entrada
    SemanticEntry *new_entry = (SemanticEntry*)malloc(sizeof(SemanticEntry));
    new_entry->element_name = strdup(element_name);
    new_entry->count = 0;
    new_entry->attribute_names = NULL;
    new_entry->attr_count = 0;
    new_entry->next = table->entries;
    table->entries = new_entry;
    
    return new_entry;
}

// Verificar si un atributo existe en una entrada
bool attribute_exists_in_entry(SemanticEntry *entry, const char *attr_name) {
    for (int i = 0; i < entry->attr_count; i++) {
        if (strcmp(entry->attribute_names[i], attr_name) == 0) {
            return true;
        }
    }
    return false;
}

// Agregar atributo a una entrada
void add_attribute_to_entry(SemanticEntry *entry, const char *attr_name) {
    if (attribute_exists_in_entry(entry, attr_name)) {
        return;
    }
    
    entry->attribute_names = (char**)realloc(entry->attribute_names, 
                                            (entry->attr_count + 1) * sizeof(char*));
    entry->attribute_names[entry->attr_count] = strdup(attr_name);
    entry->attr_count++;
}

// Construir tabla semántica
void build_semantic_table(XMLNode *node, SemanticTable *table) {
    if (!node) return;
    
    if (node->type == NODE_ELEMENT) {
        SemanticEntry *entry = find_or_create_entry(table, node->name);
        entry->count++;
        table->total_elements++;
        
        // Agregar atributos
        if (node->attributes) {
            Attribute *attr = node->attributes->first;
            while (attr) {
                add_attribute_to_entry(entry, attr->name);
                table->total_attributes++;
                attr = attr->next;
            }
        }
        
        build_semantic_table(node->children, table);
    }
    
    build_semantic_table(node->next, table);
}

// Análisis semántico principal
bool semantic_analyze(XMLNode *root, SemanticTable *table) {
    printf("\n=== Iniciando análisis semántico ===\n");
    
    // Verificar que el documento esté bien formado
    if (!check_well_formed(root)) {
        return false;
    }
    
    // Establecer elemento raíz
    table->has_root = true;
    table->root_name = strdup(root->name);
    
    // Validar nombres de elementos
    if (!validate_element_names(root)) {
        return false;
    }
    
    // Validar nombres de atributos
    if (!validate_attribute_names(root)) {
        return false;
    }
    
    // Construir tabla semántica
    build_semantic_table(root, table);
    
    // Imprimir tabla semántica
    print_semantic_table(table);
    
    printf("=== Análisis semántico completado ===\n");
    return true;
}

// Imprimir tabla semántica
void print_semantic_table(SemanticTable *table) {
    printf("\n--- Tabla Semántica ---\n");
    printf("Elemento raíz: %s\n", table->root_name ? table->root_name : "N/A");
    printf("Total de elementos: %d\n", table->total_elements);
    printf("Total de atributos: %d\n", table->total_attributes);
    printf("\nElementos encontrados:\n");
    
    SemanticEntry *current = table->entries;
    while (current) {
        printf("  <%s> - Aparece %d vez(es)\n", current->element_name, current->count);
        if (current->attr_count > 0) {
            printf("    Atributos: ");
            for (int i = 0; i < current->attr_count; i++) {
                printf("%s", current->attribute_names[i]);
                if (i < current->attr_count - 1) printf(", ");
            }
            printf("\n");
        }
        current = current->next;
    }
    printf("--- Fin Tabla Semántica ---\n\n");
}