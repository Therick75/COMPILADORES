%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xml_tree.h"
#include "semantic_analyzer.h"

extern int yylex();
extern void yyerror(const char *s);
extern int yylineno;
extern FILE *yyin;

XMLNode* root = NULL;
SemanticTable semantic_table;
int parse_success = 1;
%}

%union {
    char *str;
    XMLNode *node;
    AttributeList *attr_list;
    Attribute *attr;
}

%token <str> NAME STRING TEXT CDATA_CONTENT XML_DECL
%token TAG_START TAG_END END_TAG_START SELF_CLOSING EQUALS
%token CDATA_START CDATA_END XML_DECL_END

%type <node> document element content_list content
%type <attr_list> attribute_list
%type <attr> attribute
%type <str> start_tag end_tag

%start document

%%

document:
    xml_declaration_opt element {
        root = $2;
        if (semantic_analyze(root, &semantic_table)) {
            printf("Análisis semántico exitoso\n");
        } else {
            printf("Errores en el análisis semántico\n");
            parse_success = 0;
        }
    }
    ;

xml_declaration_opt:
    /* empty */
    | XML_DECL attribute_list XML_DECL_END
    ;

element:
    start_tag attribute_list TAG_END content_list end_tag {
        if (strcmp($1, $5) != 0) {
            fprintf(stderr, "Error: Tag de apertura '%s' no coincide con tag de cierre '%s'\n", $1, $5);
            parse_success = 0;
        }
        $$ = create_element($1, $2, $4);
        free($1);
        free($5);
    }
    | start_tag attribute_list SELF_CLOSING {
        $$ = create_element($1, $2, NULL);
        free($1);
    }
    ;

start_tag:
    TAG_START NAME {
        $$ = $2;
    }
    ;

end_tag:
    END_TAG_START NAME TAG_END {
        $$ = $2;
    }
    ;

attribute_list:
    /* empty */ {
        $$ = NULL;
    }
    | attribute_list attribute {
        $$ = add_attribute($1, $2);
    }
    ;

attribute:
    NAME EQUALS STRING {
        $$ = create_attribute($1, $3);
    }
    ;

content_list:
    /* empty */ {
        $$ = NULL;
    }
    | content_list content {
        $$ = add_content($1, $2);
    }
    ;

content:
    TEXT {
        $$ = create_text_node($1);
    }
    | element {
        $$ = $1;
    }
    | CDATA_START CDATA_CONTENT CDATA_END {
        $$ = create_cdata_node($2);
    }
    ;

%%

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.xml>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error al abrir el archivo");
        return 1;
    }

    init_semantic_table(&semantic_table);

    printf("Analizando archivo XML: %s\n", argv[1]);
    
    if (yyparse() == 0 && parse_success) {
        printf("✓ Análisis exitoso del archivo XML\n");
        printf("✓ Estructura XML válida\n");
        
        // Mostrar estadísticas
        printf("\nEstadísticas del documento:\n");
        printf("- Elementos: %d\n", count_elements(root));
        printf("- Atributos: %d\n", count_attributes(root));
        
        // Modo interactivo para consultas XPath
        printf("\nModo consulta XPath (escriba 'quit' para salir):\n");
        xpath_interactive_mode(root);
        
    } else {
        printf("✗ Error en el análisis del archivo XML\n");
        return 1;
    }

    fclose(yyin);
    free_xml_tree(root);
    free_semantic_table(&semantic_table);
    
    return 0;
}