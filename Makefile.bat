# Makefile para compilador XML con Flex y Bison en Windows

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
FLEX = flex
BISON = bison

# Archivos fuente
SOURCES = parser.tab.c lex.yy.c xml_tree.c semantic_analyzer.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = xml_compiler.exe

# Archivos de entrada
LEXER_SRC = lexer.l
PARSER_SRC = parser.y

# Archivos generados
LEXER_OUTPUT = lex.yy.c
PARSER_OUTPUT = parser.tab.c parser.tab.h

# Regla principal
all: $(EXECUTABLE)

# Compilar el ejecutable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# Generar el analizador léxico
$(LEXER_OUTPUT): $(LEXER_SRC)
	$(FLEX) $(LEXER_SRC)

# Generar el analizador sintáctico
$(PARSER_OUTPUT): $(PARSER_SRC)
	$(BISON) -d $(PARSER_SRC)

# Compilar objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencias especiales
parser.tab.o: parser.tab.c parser.tab.h xml_tree.h semantic_analyzer.h
lex.yy.o: lex.yy.c parser.tab.h
xml_tree.o: xml_tree.c xml_tree.h
semantic_analyzer.o: semantic_analyzer.c semantic_analyzer.h xml_tree.h

# Asegurar que los archivos generados existan antes de compilar
parser.tab.c: $(PARSER_SRC)
lex.yy.c: $(LEXER_SRC) parser.tab.h

# Limpiar archivos generados
clean:
	del /f *.o $(LEXER_OUTPUT) $(PARSER_OUTPUT) $(EXECUTABLE) 2>nul || true

# Limpiar todo incluyendo archivos de backup
distclean: clean
	del /f *~ *.bak 2>nul || true

# Reglas de ayuda
help:
	@echo Uso del Makefile:
	@echo   make all      - Compilar todo el proyecto
	@echo   make clean    - Limpiar archivos generados
	@echo   make distclean - Limpiar todo
	@echo   make help     - Mostrar esta ayuda
	@echo.
	@echo Ejemplo de uso:
	@echo   xml_compiler.exe archivo.xml

# Regla para generar archivos de prueba
test-files:
	@echo Generando archivos de prueba...
	@echo ^<?xml version="1.0" encoding="UTF-8"?^> > test1.xml
	@echo ^<root^> >> test1.xml
	@echo   ^<element1 attr="value1"^>Contenido 1^</element1^> >> test1.xml
	@echo   ^<element2 attr="value2"^>Contenido 2^</element2^> >> test1.xml
	@echo ^</root^> >> test1.xml
	@echo.
	@echo ^<?xml version="1.0"?^> > test2.xml
	@echo ^<biblioteca^> >> test2.xml
	@echo   ^<libro id="1" genero="ficcion"^> >> test2.xml
	@echo     ^<titulo^>El Quijote^</titulo^> >> test2.xml
	@echo     ^<autor^>Miguel de Cervantes^</autor^> >> test2.xml
	@echo   ^</libro^> >> test2.xml
	@echo   ^<libro id="2" genero="terror"^> >> test2.xml
	@echo     ^<titulo^>Dracula^</titulo^> >> test2.xml
	@echo     ^<autor^>Bram Stoker^</autor^> >> test2.xml
	@echo   ^</libro^> >> test2.xml
	@echo ^</biblioteca^> >> test2.xml
	@echo.
	@echo Archivos de prueba generados: test1.xml, test2.xml

.PHONY: all clean distclean help test-files