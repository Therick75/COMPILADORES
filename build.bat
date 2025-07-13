@echo off
REM Script de construcción para el compilador XML
REM Requiere Flex, Bison y GCC instalados

echo === Compilador XML - Script de Construcción ===
echo.

REM Verificar que las herramientas estén disponibles
echo Verificando herramientas...
flex --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Flex no está instalado o no está en el PATH
    echo Instale WinFlexBison o añada Flex al PATH del sistema
    pause
    exit /b 1
)

bison --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Bison no está instalado o no está en el PATH
    echo Instale Bison o añada al PATH del sistema
    pause
    exit /b 1
)

gcc --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: GCC no está instalado o no está en el PATH
    echo Instale MinGW-w64 o añada GCC al PATH del sistema
    pause
    exit /b 1
)

echo ✓ Todas las herramientas están disponibles
echo.

REM Limpiar archivos previos
echo Limpiando archivos anteriores...
if exist lex.yy.c del lex.yy.c
if exist parser.tab.c del parser.tab.c
if exist parser.tab.h del parser.tab.h
if exist *.o del *.o
if exist xml_compiler.exe del xml_compiler.exe
echo ✓ Archivos limpiados
echo.

REM Generar analizador sintáctico
echo Generando analizador sintáctico...
bison -d parser.y
if %errorlevel% neq 0 (
    echo ERROR: Fallo al generar el analizador sintáctico
    pause
    exit /b 1
)
echo ✓ Analizador sintáctico generado (parser.tab.c, parser.tab.h)

REM Generar analizador léxico
echo Generando analizador léxico...
flex lexer.l
if %errorlevel% neq 0 (
    echo ERROR: Fallo al generar el analizador léxico
    pause
    exit /b 1
)
echo ✓ Analizador léxico generado (lex.yy.c)

REM Compilar archivos objeto
echo.
echo Compilando archivos objeto...

echo Compilando parser.tab.c...
gcc -Wall -Wextra -g -std=c99 -c parser.tab.c -o parser.tab.o
if %errorlevel% neq 0 (
    echo ERROR: Fallo al compilar parser.tab.c
    pause
    exit /b 1
)

echo Compilando lex.yy.c...
gcc -Wall -Wextra -g -std=c99 -c lex.yy.c -o lex.yy.o
if %errorlevel% neq 0 (
    echo ERROR: Fallo al compilar lex.yy.c
    pause
    exit /b 1
)

echo Compilando xml_tree.c...
gcc -Wall -Wextra -g -std=c99 -c xml_tree.c -o xml_tree.o
if %errorlevel% neq 0 (
    echo ERROR: Fallo al compilar xml_tree.c
    pause
    exit /b 1
)

echo Compilando semantic_analyzer.c...
gcc -Wall -Wextra -g -std=c99 -c semantic_analyzer.c -o semantic_analyzer.o
if %errorlevel% neq 0 (
    echo ERROR: Fallo al compilar semantic_analyzer.c
    pause
    exit /b 1
)

echo Compilando xpath_engine.c...
gcc -Wall -Wextra -g -std=c99 -c xpath_engine.c -o xpath_engine.o
if %errorlevel% neq 0 (
    echo ERROR: Fallo al compilar xpath_engine.c
    pause
    exit /b 1
)

echo ✓ Todos los archivos objeto compilados

REM Enlazar archivos objeto en un ejecutable
echo Enlazando archivos objeto...
gcc -o xml_compiler.exe parser.tab.o lex.yy.o xml_tree.o semantic_analyzer.o xpath_engine.o
if %errorlevel% neq 0 (
    echo ERROR: Fallo al enlazar xml_compiler.exe
    pause
    exit /b 1
)

echo ✓ Compilación completa: xml_compiler.exe generado correctamente.
