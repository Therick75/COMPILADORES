%{
#define _GNU_SOURCE  // Para que strdup esté habilitado en algunos sistemas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xml_tree.h"
#include <unistd.h>   // fileno, isatty (algunos compiladores de Windows lo simulan con io.h)
#include "parser.tab.h"

extern int yylineno;
int column = 1;

void count_column() {
    int i;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            column = 1;
        } else {
            column++;
        }
    }
}

void yyerror(const char *s);
%}

%option noyywrap
%option yylineno

/* Estados para manejar contenido dentro de tags */
%x INSIDE_TAG
%x INSIDE_COMMENT
%x INSIDE_CDATA

%%

"<!--"                      { BEGIN(INSIDE_COMMENT); }
<INSIDE_COMMENT>"-->"       { BEGIN(INITIAL); }
<INSIDE_COMMENT>.|\n        { /* Ignorar contenido del comentario */ }

"<![CDATA["                 { BEGIN(INSIDE_CDATA); return CDATA_START; }
<INSIDE_CDATA>"]]>"         { BEGIN(INITIAL); return CDATA_END; }
<INSIDE_CDATA>[^]]+         { yylval.str = strdup(yytext); return CDATA_CONTENT; }

"<?"[a-zA-Z][a-zA-Z0-9]*    { yylval.str = strdup(yytext + 2); return XML_DECL; }
"?>"                        { return XML_DECL_END; }

"</"                        { BEGIN(INSIDE_TAG); return END_TAG_START; }
"<"                         { BEGIN(INSIDE_TAG); return TAG_START; }

<INSIDE_TAG>">"             { BEGIN(INITIAL); return TAG_END; }
<INSIDE_TAG>"/>"            { BEGIN(INITIAL); return SELF_CLOSING; }
<INSIDE_TAG>"="             { return EQUALS; }
<INSIDE_TAG>\"[^\"]*\"      { 
                              yylval.str = strdup(yytext + 1);
                              yylval.str[strlen(yylval.str) - 1] = '\0';
                              return STRING; 
                            }
<INSIDE_TAG>'[^']*'         { 
                              yylval.str = strdup(yytext + 1);
                              yylval.str[strlen(yylval.str) - 1] = '\0';
                              return STRING; 
                            }

<INSIDE_TAG>[a-zA-Z_][a-zA-Z0-9_\-\.]*  { 
                              yylval.str = strdup(yytext); 
                              return NAME; 
                            }

<INSIDE_TAG>[ \t\r\n]+      { count_column(); /* Ignorar espacios */ }

[^<]+                       { 
                              yylval.str = strdup(yytext);
                              return TEXT; 
                            }

[ \t\r\n]+                  { count_column(); /* Ignorar espacios */ }

.                           { 
                              printf("Caracter no reconocido: %c\n", *yytext);
                              return *yytext;
                            }

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error en línea %d, columna %d: %s\n", yylineno, column, s);
}