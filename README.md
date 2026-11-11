# Simpletron: simulador, extensiones, evaluador de expresiones y compilador de Simple

Proyecto final de **Programación Avanzada**. Implementa en lenguaje C una computadora virtual
completa y la cadena de herramientas que la acompaña: desde programas escritos en lenguaje de
máquina hasta un compilador que traduce un lenguaje de alto nivel a ese lenguaje de máquina.

**Integrante:** Miguel Angel Aragon Lavalle

**Referencia:** Deitel H.M. y Deitel P.J. (1998). *Cómo programar en C*, 1ra Edición.
México. Prentice Hall Latinoamérica. Usado con propósitos educativos.

---

## Qué hay aquí

Cuatro programas independientes que se encadenan:

```
  programa.simple            un programa en lenguaje Simple
        |
        v  compilador.exe    traduce Simple a SML en dos pasadas
  programa.sml               código de máquina, una instrucción por línea
        |
        v  simpletron.exe    la computadora virtual lo ejecuta
  resultado en pantalla
```

Y dos herramientas más:

- **`expresiones.exe`**: valida, convierte a postfija y evalúa expresiones matemáticas con
  pilas. Su algoritmo es el mismo que usa el compilador para traducir `let`.
- **`simpletron_mejorado.exe`**: el simulador con las ocho extensiones de la Sección 3:
  memoria de 1000 posiciones, carga desde archivo, cadenas, punto flotante, residuo,
  potencia y salto de línea.

---

## Compilación

Desde la **raíz del repositorio**, con GCC (en Windows se instala con MinGW-w64):

```bash
gcc -Wall -Wextra -std=c11 src/simpletron/simpletron.c -o simpletron.exe
```

```bash
gcc -Wall -Wextra -std=c11 src/simpletron/simpletron_mejorado.c -o simpletron_mejorado.exe
```

```bash
gcc -Wall -Wextra -std=c11 src/expresiones/expresiones.c -o expresiones.exe
```

```bash
gcc -Wall -Wextra -std=c11 src/compilador/compilador.c -o compilador.exe
```

Los cuatro compilan **sin errores y sin advertencias** con `-Wall -Wextra`. No necesitan
bibliotecas externas.

Para comprobar que GCC está disponible:

```bash
gcc --version
```

---

## Ejecución

### El camino completo: de Simple a resultado

```bash
.\compilador.exe programas\simple\01-sumatoria.simple programas\sml\compilados\01-sumatoria.sml
```

```bash
.\simpletron.exe programas\sml\compilados\01-sumatoria.sml
```

El programa pedirá un número con `?`. Con `5` responde `+0015`, que es 1+2+3+4+5.

Para no teclear los datos a mano:

```bash
.\simpletron.exe programas\sml\compilados\01-sumatoria.sml < pruebas\compilador\entradas\01-sumatoria.txt
```

Ese `<` es redirección de entrada del intérprete de comandos: el programa sigue creyendo que
lee del teclado, pero el sistema le entrega el contenido del archivo.

### El simulador por teclado (Sección 2)

Sin argumentos captura el programa SML instrucción por instrucción, terminando con el
centinela `9999`:

```bash
.\simpletron.exe
```

```
00 ? +1007
01 ? +1008
02 ? +2007
03 ? +3008
04 ? +2109
05 ? +1109
06 ? +4300
07 ? 9999
*** Se terminó de cargar el programa ***
```

### Los programas SML de ejemplo

Cada archivo de `programas/sml/` es a la vez **documentación y programa ejecutable**: la columna
de la izquierda es la palabra SML y lo que sigue a `;` es el comentario, que el cargador ignora.

```
+1007   ; 00  READ  07        Lee A y lo guarda en memoria[07]
+1008   ; 01  READ  08        Lee B y lo guarda en memoria[08]
+2007   ; 02  LOAD  07        acumulador <- A
```

Por eso se pueden correr de dos maneras:

```bash
.\simpletron.exe programas\sml\01-suma.sml < pruebas\datos\01-suma.txt
```

```bash
.\simpletron.exe < pruebas\entradas\01-suma.txt
```

La primera carga el programa del archivo y solo teclea los **datos**. La segunda simula la
captura completa por teclado: el guion de `pruebas/entradas/` trae el programa, el centinela
`9999` y los datos.

### El simulador mejorado (Sección 3)

```bash
.\simpletron_mejorado.exe
```

Carga automáticamente `programa.simp`. También acepta argumentos:

| Comando | Qué hace |
|---|---|
| `.\simpletron_mejorado.exe` | Carga `programa.simp`; si no existe, pide por teclado |
| `.\simpletron_mejorado.exe -t` | Fuerza la captura por teclado |
| `.\simpletron_mejorado.exe otro.simp` | Carga el archivo indicado |

### El evaluador de expresiones

```bash
.\expresiones.exe "( 12 + 8 ) * 3"
```

```
--- 1. Validacion de agrupadores ---
Agrupadores correctamente anidados.
Profundidad maxima de anidamiento: 1

--- 2. Conversion a postfija ---
Infija   : ( 12 + 8 ) * 3
Postfija : 12 8 + 3 *

--- 3. Evaluacion ---
symb     opnd1      opnd2      value      pila
12                                        12
8                                         12,8
+        12         8          20         20
3                                         20,3
*        20         3          60         60

Resultado: 60
```

---

## Ejemplo de entrada y salida completo

**Entrada:** `programas/simple/01-sumatoria.simple`

```
5 rem sumatoria de 1 hasta x
10 input x
15 rem probar y == x
20 if y == x goto 60
25 rem incrementar y
30 let y = y + 1
35 rem sumar y a total
40 let t = t + y
45 rem iterar
50 goto 20
55 rem exhibir resultado
60 print t
99 end
```

**Salida del compilador:** tabla de símbolos y código generado.

```
--- Tabla de simbolos ---
Simbolo    Tipo   Ubicacion
5          L      00
10         L      00
x          V      99
15         L      01
20         L      01
y          V      98
25         L      04
30         L      04
1          C      97
35         L      09
40         L      09
t          V      95
45         L      14
50         L      14
55         L      15
60         L      15
99         L      16

--- Segunda pasada: referencias hacia adelante ---
  posicion 03  ->  linea 60 esta en 15  ->  +4215

--- Codigo SML generado ---
00  +1099   input x
01  +2098   condicion y == x
02  +3199   restar
03  +4215   saltar si igual
04  +2098   cargar y
05  +3097   sumar 1
06  +2196   guardar en temporal 96
07  +2096   cargar temporal 96
08  +2198   asignar a y
09  +2095   cargar t
10  +3098   sumar y
11  +2194   guardar en temporal 94
12  +2094   cargar temporal 94
13  +2195   asignar a t
14  +4001   goto 20
15  +1195   print t
16  +4300   end
```

**Ejecución en el simulador** con `x = 5`:

```
*** Cargando el programa desde programas\sml\compilados\01-sumatoria.sml ***
*** Se cargaron 100 palabras desde programas\sml\compilados\01-sumatoria.sml ***
*** Comienza la ejecución del programa ***

? 5
+0015
*** Terminó la ejecución de Simpletron ***

REGISTROS:
accumulator           +0000
instructionCounter    16
instructionRegister   +4300
operationCode         43
operand               00

MEMORIA:
        0     1     2     3     4     5     6     7     8     9
  0 +1099 +2098 +3199 +4215 +2098 +3097 +2196 +2096 +2198 +2095
 10 +3098 +2194 +2094 +2195 +4001 +1195 +4300 +0000 +0000 +0000
 ...
 90 +0000 +0000 +0000 +0000 +0015 +0015 +0005 +0001 +0005 +0005
```

El vaciado deja ver todo el estado final de la máquina:

| Posición | Valor | Qué es |
|:--:|:--:|---|
| 00-16 | - | El programa SML |
| 94 | `+0015` | Temporal del último `t + y` |
| 95 | `+0015` | La variable `t`, el resultado |
| 96 | `+0005` | Temporal del último `y + 1` |
| 97 | `+0001` | La constante `1` |
| 98 | `+0005` | La variable `y`, que llegó a valer 5 |
| 99 | `+0005` | La variable `x`, lo que se tecleó |

El acumulador terminó en `+0000` justamente porque el ciclo acabó cuando `y - x` dio cero: ése
es el valor que hizo saltar al `BRANCHZERO`.

Este programa es el ejemplo resuelto del capítulo del compilador. El código generado coincide
con el de la figura 6.2 del libro y la tabla de símbolos con la de la figura 6.3.

---

## Estructura del repositorio

```
.
├── README.md
├── programa.simp                   programa que carga el simulador mejorado al iniciar
├── docs/
│   ├── requerimientos.md           cada requisito y dónde se cumple
│   ├── diseno.md                   decisiones de diseño y su porqué
│   ├── lenguaje-simple.md          definición y gramática del lenguaje Simple
│   └── referencia-sml.md           las doce instrucciones de SML, explicadas
├── src/
│   ├── simpletron/
│   │   ├── simpletron.c            simulador base (Sección 2)
│   │   └── simpletron_mejorado.c   simulador con las 8 extensiones (Sección 3)
│   ├── expresiones/
│   │   └── expresiones.c           validación, conversión y evaluación con pilas
│   └── compilador/
│       └── compilador.c            compilador de Simple a SML
├── programas/
│   ├── sml/                        programas en lenguaje de máquina, ejecutables y documentados
│   │   ├── 01-suma.sml ... 06-mayor-de-serie.sml
│   │   ├── e01 ... e05               casos de error
│   │   ├── mejoras/                programas .simp del simulador mejorado
│   │   └── compilados/             SML producido por el compilador
│   └── simple/                     programas en lenguaje Simple
├── pruebas/
│   ├── CASOS-DE-PRUEBA.md          simulador base
│   ├── expresiones/                evaluador de expresiones
│   ├── compilador/                 compilador
│   ├── mejoras/                    extensiones del simulador
│   ├── entradas/                   guiones de entrada
│   └── salidas/                    salidas reales capturadas
└── evidencias/
    ├── git-branch/                 bitácora de ramas y grafo del historial
    └── ejecuciones/                salidas completas de cada herramienta
```

---

## Las doce instrucciones de SML

Cada instrucción es una palabra decimal de cuatro dígitos con signo: los dos de la izquierda
son el **código de operación** y los dos de la derecha la **dirección**.

```
    + 1 0 0 9
      --- ---
       OP  ADDR
```

| OP | Nombre | Efecto |
|:--:|---|---|
| 10 | READ | Lee del teclado hacia `memory[ADDR]` |
| 11 | WRITE | Imprime `memory[ADDR]` |
| 20 | LOAD | `accumulator <- memory[ADDR]` |
| 21 | STORE | `memory[ADDR] <- accumulator` |
| 30 | ADD | `accumulator <- accumulator + memory[ADDR]` |
| 31 | SUBTRACT | `accumulator <- accumulator - memory[ADDR]` |
| 32 | DIVIDE | `accumulator <- accumulator / memory[ADDR]` |
| 33 | MULTIPLY | `accumulator <- accumulator * memory[ADDR]` |
| 40 | BRANCH | Salta a `ADDR` |
| 41 | BRANCHNEG | Salta si el acumulador es negativo |
| 42 | BRANCHZERO | Salta si el acumulador es cero |
| 43 | HALT | Detiene la ejecución |

Explicación detallada en [`docs/referencia-sml.md`](docs/referencia-sml.md).

### Instrucciones añadidas en el simulador mejorado

| OP | Nombre | Efecto |
|:--:|---|---|
| 12 | NEWLINE | Imprime un salto de línea |
| 13 | READSTRING | Lee texto y lo guarda como códigos ASCII desde `ADDR` |
| 14 | WRITESTRING | Imprime la cadena guardada en `ADDR` |
| 15 | READFLOAT | Lee un número con decimales |
| 16 | WRITEFLOAT | Imprime con decimales |
| 34 | REMAINDER | Residuo (módulo) |
| 35 | EXPONENT | Potencia |
| 36 | DIVIDEFLOAT | División real, sin truncar |

---

## El lenguaje Simple

| Comando | Ejemplo | Qué hace |
|---|---|---|
| `rem` | `10 rem comentario` | Comentario, no genera código |
| `input` | `20 input x` | Lee un entero |
| `let` | `30 let y = ( a + b ) * 2` | Evalúa y asigna |
| `print` | `40 print t` | Muestra el valor |
| `goto` | `50 goto 20` | Salta |
| `if/goto` | `60 if y == x goto 90` | Salta si se cumple la condición |
| `end` | `99 end` | Termina |

Variables de una sola letra minúscula, constantes enteras, operadores `+ - * /` con
paréntesis, y los seis relacionales `< <= > >= == !=`.

Definición completa en [`docs/lenguaje-simple.md`](docs/lenguaje-simple.md).

---

## Pruebas

| Componente | Casos | Documento |
|---|:--:|---|
| Programas SML, por teclado | 12 | [`pruebas/CASOS-DE-PRUEBA.md`](pruebas/CASOS-DE-PRUEBA.md) |
| Programas SML, cargados de archivo | 12 | `pruebas/salidas-archivo/` |
| Extensiones del simulador | 28 | [`pruebas/mejoras/CASOS-DE-PRUEBA-MEJORAS.md`](pruebas/mejoras/CASOS-DE-PRUEBA-MEJORAS.md) |
| Evaluación de expresiones | 15 | [`pruebas/expresiones/CASOS-DE-PRUEBA-EXPRESIONES.md`](pruebas/expresiones/CASOS-DE-PRUEBA-EXPRESIONES.md) |
| Compilador | 9 | [`pruebas/compilador/CASOS-DE-PRUEBA-COMPILADOR.md`](pruebas/compilador/CASOS-DE-PRUEBA-COMPILADOR.md) |

**76 ejecuciones** en total, todas con la salida real guardada en `salidas/` y en
`evidencias/ejecuciones/`.

### Errores detectados

**Simulador:** división entre cero, código de operación no válido, desbordamiento del
acumulador, contador de instrucciones fuera de memoria, entrada agotada, valores fuera del
rango -9999 a +9998 durante la carga.

**Expresiones:** agrupadores sin cerrar, cierre que no corresponde, división entre cero,
operandos faltantes, *overflow* y *underflow* de la pila.

**Compilador:** variable en mayúscula o de más de una letra, comando no reconocido, enunciado
sin número de línea, número de línea repetido, `goto` a una línea inexistente, constante
fuera de rango, formato incorrecto de cada comando, programa más grande que la memoria.

---

## Ramas

Cada componente se desarrolló en su propia rama y se integró a `main`:

| Rama | Contenido |
|---|---|
| `main` | Simulador base, documentación e integración |
| `mejoras-simpletron` | Las ocho extensiones de la Sección 3 |
| `evaluacion-expresiones` | Validación, conversión y evaluación con pilas |
| `compilador-simple` | Compilador de Simple a SML |

Bitácora de comandos y grafo del historial en
[`evidencias/git-branch/`](evidencias/git-branch/).

---

## Notas

- `DIVIDE` realiza **división entera**: 55 / 7 da 7. El simulador mejorado añade `DIVIDEFLOAT`
  (opcode 36) para división real.
- El compilador genera SML de 4 dígitos para la máquina de 100 posiciones, que es la que
  describe el capítulo del compilador. El simulador mejorado usa palabras de 5 dígitos y 1000
  posiciones, por lo que son formatos distintos a propósito; el porqué está en
  [`docs/diseno.md`](docs/diseno.md).
- Los ejecutables no se versionan: se generan al compilar y están excluidos en `.gitignore`.
