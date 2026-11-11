# Requerimientos y dónde se cumplen

Mapa de cada requisito del proyecto final al archivo o carpeta que lo satisface.

## 2.1 Programación en lenguaje de máquina SML

| Requisito | Dónde |
|---|---|
| Entrada/salida: `READ`, `WRITE` | `programas/sml/01-suma.sml` y todos los demás |
| Carga/almacenamiento: `LOAD`, `STORE` | `programas/sml/01-suma.sml` |
| Aritmética: `ADD`, `SUBTRACT`, `DIVIDE`, `MULTIPLY` | `03-producto.sml`, `05-promedio-de-7.sml` |
| Transferencias: `BRANCH`, `BRANCHNEG`, `BRANCHZERO` | `02-mayor-de-dos.sml`, `04-suma-de-10.sml`, `06-mayor-de-serie.sml` |
| Terminación: `HALT` | todos |
| Separación programa / datos | documentada en cada `.sml`; ver `docs/referencia-sml.md` |
| Secuencia, selección y repetición | `01-suma` (secuencia), `02-mayor-de-dos` (selección), `04-suma-de-10` (repetición) |
| Pruebas con resultados esperados | `pruebas/CASOS-DE-PRUEBA.md`, salidas reales en `pruebas/salidas/` |

Los seis programas cubren **las doce instrucciones**; la tabla de cobertura está en
`pruebas/CASOS-DE-PRUEBA.md`.

## 2.2 Simulador de Computadora Simpletron

| Requisito | Dónde |
|---|---|
| Cargar instrucciones en memoria | `src/simpletron/simpletron.c`, `cargarPrograma()` |
| Inicializar registros especiales | `ejecutarPrograma()`, todos en 0 |
| Ciclo: traer, decodificar, ejecutar, actualizar contador | `ejecutarPrograma()` |
| `accumulator`, `instructionCounter`, `instructionRegister`, `operationCode`, `operand` | declarados con esos nombres exactos |
| Vaciado de memoria | `volcadoDeMemoria()` |
| División entre cero | `case DIVIDE` |
| Opcode inválido | `default` del `switch` |
| Desbordamiento del acumulador | tras cada operación aritmética |
| Direcciones inválidas | verificación del contador de instrucciones |
| Valores fuera de rango | validación en la carga y en `READ` |

Casos de error demostrados, cada uno con su salida capturada:

| Caso | Error que demuestra |
|---|---|
| `e01-division-entre-cero` | División entre cero |
| `e02-operacion-invalida` | Código de operación no válido |
| `e03-desbordamiento` | Desbordamiento del acumulador |
| `e05-direccion-invalida` | Contador de instrucciones fuera de la memoria |
| `e04-carga-fuera-de-rango` | Validación de rango **en la fase de carga**: no es un error fatal, el programa continúa |

## 2.3 Modificaciones al simulador

Archivo: `src/simpletron/simpletron_mejorado.c`, Desarrolladas en la rama `mejoras-simpletron`.

| Mejora | Instrucción nueva | Prueba |
|---|---|---|
| 1. Carga desde `programa.simp` | - | 3 escenarios: archivo correcto, inexistente, línea inválida |
| 2. Memoria de 1000 posiciones | - | `m02-memoria-1000.simp` usa las direcciones 120, 500 y 999 |
| 3. Residuo | `34 REMAINDER` | 5 pruebas, incluye divisor cero |
| 4. Exponenciación | `35 EXPONENT` | 6 pruebas, incluye desbordamiento |
| 5. Salto de línea | `12 NEWLINE` | 2 pruebas, con y sin, para contrastar |
| 6. Entrada de cadenas | `13 READSTRING` | 3 pruebas |
| 7. Salida de cadenas | `14 WRITESTRING` | 3 pruebas |
| 8. Punto flotante | `15 READFLOAT`, `16 WRITEFLOAT`, `36 DIVIDEFLOAT` | 8 pruebas |

Opcodes documentados en el encabezado del archivo y en el `README.md`.
Detalle completo: `pruebas/mejoras/CASOS-DE-PRUEBA-MEJORAS.md` (28 ejecuciones).

## 2.4 Evaluación de expresiones

Archivo: `src/expresiones/expresiones.c`, Desarrollado en la rama `evaluacion-expresiones`.

| Requisito | Dónde |
|---|---|
| Validar profundidad de anidamiento | `profundidad()` |
| Verificar balance de símbolos de agrupación | `valida()`, con `( )` `[ ]` `{ }` |
| Convertir infija a postfija | `aPostfijo()` |
| Evaluar postfija | `evaluar()`, con tabla de seguimiento |
| Precedencia y asociatividad | `prioridad()`; `^` asocia a la derecha |
| Operandos de más de un dígito | `tokenizar()` agrupa los dígitos antes de convertir |
| Integrar como base del compilador | el mismo algoritmo compila `let`: `aPostfijo()` y `generarExpresion()` en `src/compilador/compilador.c` |

Pruebas: `pruebas/expresiones/CASOS-DE-PRUEBA-EXPRESIONES.md`.

## 2.5 Lenguaje Simple

Definición completa: `docs/lenguaje-simple.md`.

| Requisito | Dónde |
|---|---|
| Números de línea ascendentes | validado en `compilarEnunciado()` |
| `rem`, `input`, `let`, `print`, `goto`, `if/goto`, `end` | los siete implementados |
| Variables de una sola letra | validado en `ubicacionDe()` |
| Constantes enteras | validado, rango 0 a 9999 |
| Operadores `+ - * /` | `generarExpresion()`; `^` se rechaza con mensaje |
| Condiciones `< <= > >= == !=` | `compilarIf()`, las seis |
| Restricción de minúsculas | validado, salvo el texto de `rem` |
| Reporte de errores sintácticos | 27 mensajes distintos; los de la primera pasada indican la línea del fuente |

Programas de ejemplo: `programas/simple/` (6 correctos y 3 con errores).

## 2.6 Construcción del compilador

Archivo: `src/compilador/compilador.c`, Desarrollado en la rama `compilador-simple`.

| Requisito | Dónde |
|---|---|
| Leer un programa fuente en Simple | `main()` |
| Primera pasada: tabla de símbolos | `symbolTable` de tipo `tableEntry`, con `symbol`, `type`, `location` |
| Registrar líneas, variables y constantes | tipos `'L'`, `'V'`, `'C'` |
| Generar SML preliminar | `generar()` |
| Marcar referencias no resueltas | arreglo `flags[100]`, inicializado en -1 |
| Segunda pasada: resolver referencias | `segundaPasada()` |
| Completar instrucciones inconclusas | suma la ubicación al operando que quedó en `00` |
| Generar archivo final SML | `escribirSalida()` |
| Compilar `input`, `print`, `goto`, `if/goto`, `let`, `end` | los seis, más `rem` |
| Conversión infija a postfija en `let` | `aPostfijo()` y `generarExpresion()` |
| SML ejecutable en el simulador | los 6 programas se ejecutan correctamente |
| Programas de prueba Simple y su SML | `programas/simple/` y `programas/sml/compilados/` |

**Comparación con el libro:** el programa `01-sumatoria.simple` es el ejemplo resuelto del
capítulo. El código que genera el compilador coincide con el de la figura 6.2 y su tabla de
símbolos con la de la figura 6.3. Evidencia en
`evidencias/ejecuciones/compilacion-01-sumatoria.txt`.

## 2.7 Repositorio GitHub

| Requisito | Dónde |
|---|---|
| Organización de carpetas | estructura `src/`, `programas/`, `pruebas/`, `docs/`, `evidencias/` |
| Commits | un commit por componente, en su rama |
| README completo | `README.md` |
| Trabajo local y remoto | las 3 ramas de trabajo publicadas en `origin` |
| Uso de ramas | 3 ramas de trabajo con sus merges a `main` |
| Sin archivos innecesarios | `.gitignore` excluye `*.exe` y temporales |

## 2.8 Actividades de ramas

Evidencia: `evidencias/git-branch/`.

| Requisito | Dónde |
|---|---|
| Creación de ramas locales | `bitacora-git.md`, comandos y salidas reales |
| Cambio entre ramas | `bitacora-git.md` |
| Commits en ramas específicas | `historial-ramas.txt`, grafo del historial |
| Registro ordenado del historial | `historial-ramas.txt` |
