# Pointer_Analysis (Wave Propagation + extensión flow-sensitive)

Este repositorio contiene un prototipo de análisis estático de punteros (alias/points-to).  
El análisis se basa en restricciones de inclusión (estilo Andersen) resueltas mediante el algoritmo **Wave Propagation (WP)**, y una extensión **sensible al flujo** que mantiene estados **IN/OUT** por sentencia.


- El análisis trabaja sobre un subconjunto de instrucciones de punteros (asignaciones base, simple, load/store).

- En la extensión flow-sensitive, el análisis se evalúa sentencia por sentencia y combina estados en estructuras de control (IF/WHILE) mediante JOIN.

## Requisitos

- `gcc`
- `flex`
- `bison`
- Algun programa para visualizar `.dot`

En Debian/Ubuntu:
```bash
sudo apt-get install gcc flex bison
```

## Compilacion del Parser
1. Ir al directorio del parser:
```bash
cd Parser
```
2. Ejecutar el script de build:
```bash
sh script
```

Esto genera el ejecutable parser.

## Ejecucion
Ejecutar el parser con un programa de entrada
```bash
./parser Examples/<archivo_de_ejemplo>
```

Ejemplo:
```bash
./parser Examples/ejemplo1.txt
```

Durante la ejecucion se imprime el proceso del analisis y, se generan grafos en formato `.dot`.

## Menu de estados (State Menu)
El prototipo flow-sensitive guarda el estado IN/OUT del grafo de restricciones por cada sentencia procesada.
Al finalizar, se ofrece un menú interactivo para exportar los estados en formato `.dot`.

### Que guarda cada estado?

Para cada sentencia con identificador id se guarda:

- IN: grafo antes de ejecutar la sentencia
- OUT: grafo después de ejecutar la sentencia y ejecutar WP sobre el estado resultante
- Statement: texto descriptivo de la sentencia (ej. A = &X, *A = B, if (...), etc.)

### Comandos del Menun

Al entrar al menú se vera algo así:

- `l` : listar los IDs disponibles
- `<id>` : seleccionar un ID para inspeccionar
- `q` : salir del menú

Ejemplo:

-> l
| ID | IN | OUT | Statement |
|:---|:---|:---|:----------|
| 1  | Si | Si  | A = &X    |
| 2  | Si | Si  | B = &Y    |
| 3  | Si | Si  | if (...)  |

### Exportar un grafo IN / OUT
Luego de seleccionar un ID, podés elegir:

- `i` : exportar solo IN
- `o` : exportar solo OUT
- `b` : exportar both (IN y OUT)
- `c` : cancelar

## Contacto
Autor: Giovanni Buchieri
Departamento de Computación, Universidad Nacional de Río Cuarto (UNRC)
