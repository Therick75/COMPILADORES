# Compilador XML con Análisis Léxico, Sintáctico y Semántico

Este proyecto implementa un compilador completo para documentos XML que incluye análisis léxico, sintáctico y semántico, además de soporte para consultas XPath.

## Características

- **Análisis Léxico**: Reconoce tokens XML (elementos, atributos, texto, CDATA, comentarios)
- **Análisis Sintáctico**: Valida la estructura correcta del XML
- **Análisis Semántico**: Verifica reglas semánticas y construye tabla de símbolos
- **Consultas XPath**: Soporte para búsquedas básicas y avanzadas
- **Validación**: Verifica que el XML esté bien formado

## Archivos del Proyecto

### Archivos Principales
- `lexer.l` - Analizador léxico (Flex)
- `parser.y` - Analizador sintáctico (Bison)
- `xml_tree.h/c` - Estructura de datos para el árbol XML
- `semantic_analyzer.h/c` - Analizador semántico y tabla de símbolos
- `xpath_engine.c` - Motor de consultas XPath extendido

### Archivos de Construcción
- `Makefile` - Archivo de construcción para Windows
- `README.md` - Este archivo de documentación

## Requisitos

- **Windows** con MinGW o similar
- **Flex** (Analizador léxico)
- **Bison** (Analizador sintáctico)
- **GCC** (Compilador C)

## Instalación

1. Instalar MinGW-w64 con Flex y Bison:
```bash
# Descargar desde: https://www.mingw-w64.org/
# O usar chocolatey:
choco install mingw
choco install winflexbison
```

2. Clonar o descargar los archivos del proyecto

3. Compilar el proyecto:
```bash
make all
```

## Uso

### Compilación
```bash
make all
```

### Generar archivos de prueba
```bash
make test-files
```

### Ejecutar el compilador
```bash
xml_compiler.exe archivo.xml
```

### Ejemplo de uso
```bash
xml_compiler.exe test1.xml
```

## Funcionalidades

### 1. Análisis Léxico
- Reconoce elementos XML: `<elemento>`, `</elemento>`
- Detecta atributos: `nombre="valor"`
- Maneja texto y CDATA
- Procesa comentarios XML
- Reconoce declaraciones XML

### 2. Análisis Sintáctico
- Valida estructura XML jerárquica
- Verifica correspondencia de tags de apertura/cierre
- Maneja elementos auto-cerrados
- Detecta errores de sintaxis

### 3. Análisis Semántico
- Verifica que el XML esté bien formado
- Valida nombres de elementos y atributos
- Construye tabla de símbolos con estadísticas
- Detecta elementos duplicados
- Verifica unicidad del elemento raíz

### 4. Consultas XPath
El compilador soporta las siguientes consultas XPath:

#### Consultas Básicas
- `/elemento` - Buscar desde la raíz
- `//elemento` - Buscar en todo el documento
- `elemento` - Buscar relativo al contexto actual

#### Consultas con Predicados
- `elemento[@atributo='valor']` - Buscar por atributo
- `elemento[1]` - Buscar por posición
- `//*[@atributo='valor']` - Buscar cualquier elemento con atributo

#### Ejemplos de Consultas
```
XPath> /root
XPath> //libro
XPath> /biblioteca/libro[1]
XPath> //libro[@id='1']
XPath> //*[@genero='ficcion']
```

## Estructura de Salida

### Análisis Exitoso
```
Analizando archivo XML: test1.xml
✓ Análisis exitoso del archivo XML
✓ Estructura XML válida

=== Iniciando análisis semántico ===
--- Tabla Semántica ---
Elemento raíz: root
Total de elementos: 3
Total de atributos: 2

Elementos encontrados:
  <root> - Aparece 1 vez(es)
  <element1> - Aparece 1 vez(es)
    Atributos: attr
  <element2> - Aparece 1 vez(es)
    Atributos: attr
--- Fin Tabla Semántica ---

=== Análisis semántico completado ===

Estadísticas del documento:
- Elementos: 3
- Atributos: 2

Modo consulta XPath (escriba 'quit' para salir):
XPath> 
```

### Errores Detectados
- Errores léxicos: caracteres no válidos
- Errores sintácticos: estructura XML incorrecta
- Errores semánticos: nombres inválidos, múltiples raíces

## Ejemplos de Archivos XML

### Archivo Simple (test1.xml)
```xml
<?xml version="1.0" encoding="UTF-8"?>
<root>
  <element1 attr="value1">Contenido 1</element1>
  <element2 attr="value2">Contenido 2</element2>
</root>
```

### Archivo Complejo (test2.xml)
```xml
<?xml version="1.0"?>
<biblioteca>
  <libro id="1" genero="ficcion">
    <titulo>El Quijote</titulo>
    <autor>Miguel de Cervantes</autor>
  </libro>
  <libro id="2" genero="terror">
    <titulo>Dracula</titulo>
    <autor>Bram Stoker</autor>
  </libro>
</biblioteca>
```

## Comandos Make

- `make all` - Compilar todo el proyecto
- `make clean` - Limpiar archivos generados
- `make test-files` - Generar archivos de prueba
- `make help` - Mostrar ayuda

## Estructura del Proyecto

```
xml-compiler/
├── lexer.l                 # Analizador léxico
├── parser.y                # Analizador sintáctico  
├── xml_tree.h              # Definiciones del árbol XML
├── xml_tree.c              # Implementación del árbol XML
├── semantic_analyzer.h     # Definiciones del análisis semántico
├── semantic_analyzer.c     # Implementación del análisis semántico
├── xpath_engine.c          # Motor de consultas XPath
├── Makefile               # Archivo de construcción
└── README.md              # Documentación
```

## Limitaciones

- XPath implementa un subconjunto básico de la especificación completa
- No soporta espacios de nombres XML
- No valida contra DTD o XML Schema
- El análisis semántico es básico (no incluye validación de contenido)

## Extensiones Futuras

- Soporte completo de XPath 2.0
- Validación contra DTD/XSD
- Soporte para espacios de nombres
- Generación de código para diferentes lenguajes
- Optimización de consultas XPath

## Troubleshooting

### Error: "flex: command not found"
Instalar WinFlexBison o añadir Flex al PATH del sistema.

### Error: "bison: command not found"  
Instalar Bison o usar la versión incluida en MinGW.

### Error de compilación con GCC
Verificar que MinGW esté instalado y en el PATH.

## Contribuir

1. Fork el proyecto
2. Crear branch para nueva funcionalidad
3. Hacer commit de los cambios
4. Push al branch
5. Crear Pull Request

## Licencia

Este proyecto está disponible bajo la licencia MIT.