# Diseño y decisiones

Documento de las decisiones relevantes del proyecto y del porqué de cada una.

## 1. Arquitectura general

El proyecto son **cuatro programas independientes** que se encadenan:

```
  programa.simple
        |
        v  compilador.exe        traduce Simple a SML (dos pasadas)
  programa.sml
        |
        v  simpletron.exe        ejecuta el SML
  resultado en pantalla
```

```
  expresiones.exe     resuelve expresiones infijas con pilas (componente que
                      alimenta la compilación de let)

  simpletron_mejorado.exe   el simulador con las ocho extensiones de la Sección 3
```

Se mantuvieron separados a propósito: cada uno cumple su propio enunciado sin que las
decisiones de uno contaminen al otro.

## 2. Por qué dos simuladores y no uno

| | `simpletron.c` | `simpletron_mejorado.c` |
|---|---|---|
| Memoria | 100 posiciones | 1000 posiciones |
| Palabra | 4 dígitos (-9999 a +9999) | 5 dígitos (-99999 a +99999) |
| Instrucciones | 12 | 20 (12 base + 8 nuevas) |
| Centinela | `9999` | `-99999` |

La Sección 2 exige **100 posiciones y palabra de 4 dígitos**; la mejora 2 de la Sección 3 pide
**1000 posiciones**, lo que obliga a que la dirección crezca a 3 dígitos y la palabra a 5. Son
requisitos incompatibles entre sí, así que modificar un único archivo habría roto uno de los dos
enunciados. Con dos programas, cada uno cumple el suyo exactamente.

El compilador genera SML de 4 dígitos para la máquina de 100 posiciones, que es la que
describe el capítulo del compilador.

## 3. El simulador básico y la carga desde archivo

El enunciado de la Sección 2 pide captura por teclado con centinela `9999`. El capítulo del
compilador dice, en cambio: *"Es preciso modificar ese programa de modo que lea de un archivo"*.

Solución: **un argumento opcional en la línea de comandos**.

```
simpletron.exe                   captura por teclado, con centinela 9999
simpletron.exe programa.sml      carga el archivo que produce el compilador
```

Sin argumentos el comportamiento es idéntico al que pide la Sección 2, así que no se rompe nada;
con argumento se habilita lo que necesita el compilador.

## 4. Formato del archivo SML

El compilador escribe las 100 posiciones de memoria, una por renglón, con comentario:

```
+1099   ; 00  input x
+2098   ; 01  condicion y == x
...
+0001   ; 97  constante 1
```

Se escriben **las 100** y no solo las instrucciones porque las constantes viven en el área de
datos (al final de la memoria) y deben llegar inicializadas. Si solo se escribiera el área de
programa, una constante como el `1` de `let y = y + 1` valdría 0 al ejecutarse.

El cargador ignora las líneas que empiezan con `;` o `#` y todo lo que siga al número, de modo
que el archivo puede venir documentado sin dejar de ser ejecutable.

## 5. El compilador: dos pasadas

### Primera pasada

Lee cada enunciado, lo parte en unidades lexicográficas con `strtok()` y:

1. Mete el número de línea en la tabla de símbolos como tipo `L`, con la posición SML actual.
2. Según el comando, genera las instrucciones SML correspondientes.
3. Si un `goto` o un `if/goto` apunta a una línea que **todavía no se ha visto**, deja el
   operando en `00` y anota la línea pendiente en el arreglo `flags`.

### Tabla de símbolos

```c
typedef struct {
    int  symbol;    /* número de línea, letra de la variable, o la constante */
    char type;      /* 'L' línea, 'V' variable, 'C' constante */
    int  location;  /* posición de memoria 00 a 99 */
} tableEntry;
```

Las instrucciones se asignan **desde la 00 hacia adelante** y las variables, constantes y
temporales **desde la 99 hacia atrás**. Si ambos punteros se cruzan, el programa no cabe y se
reporta el error.

### Segunda pasada

Recorre `flags`; por cada elemento distinto de `-1` busca ese número de línea en la tabla de
símbolos y le suma la ubicación al operando que había quedado en `00`.

```
posicion 03  ->  linea 60 esta en 15  ->  +4215
```

## 6. Compilación de `let`: el "gancho" del postfijo

La expresión se convierte de infija a postfija y después se recorre, pero **en vez de calcular,
se apilan posiciones de memoria**.

Al encontrar un operador se sacan las dos posiciones del tope y se emite:

```
LOAD   <posición del operando izquierdo>
<op>   <posición del operando derecho>
STORE  <celda temporal nueva>
```

y se apila la celda temporal. Al terminar queda una sola posición: la del resultado.

`let y = y + 1` produce exactamente:

```
04 +2098   cargar y   (98)
05 +3097   sumar 1    (97)
06 +2196   guardar en temporal 96
07 +2096   cargar temporal 96
08 +2198   asignar a y
```

Cada subexpresión consume una celda temporal nueva. Como se asignan **en tiempo de compilación**,
un ciclo no consume memoria adicional al ejecutarse.

## 7. Compilación de `if/goto`

SML no tiene comparaciones. Las seis se simulan restando y examinando el signo:

| Condición | Código SML generado |
|---|---|
| `a == b` | `LOAD a`, `SUBTRACT b`, `BRANCHZERO destino` |
| `a != b` | `LOAD a`, `SUBTRACT b`, `BRANCHZERO (esta posición + 2)`, `BRANCH destino` |
| `a < b` | `LOAD a`, `SUBTRACT b`, `BRANCHNEG destino` |
| `a > b` | `LOAD b`, `SUBTRACT a`, `BRANCHNEG destino` |
| `a <= b` | `LOAD a`, `SUBTRACT b`, `BRANCHNEG destino`, `BRANCHZERO destino` |
| `a >= b` | `LOAD b`, `SUBTRACT a`, `BRANCHNEG destino`, `BRANCHZERO destino` |

`>` y `>=` invierten el orden de la resta en lugar de necesitar una instrucción nueva.
`!=` es el único que requiere dos bifurcaciones encadenadas: salta **sobre** el salto cuando la
resta da cero.

En `<=` y `>=` las **dos** bifurcaciones pueden ser referencias hacia adelante, así que ambas se
marcan en `flags` por separado.

## 8. Evaluación de expresiones: asociatividad

En la conversión a postfija, la condición que decide si se saca un operador de la pila es:

```c
prioridad(tope) > prioridad(actual) ||
(prioridad(tope) == prioridad(actual) && actual != '^')
```

Ese `actual != '^'` es lo que hace que la potencia asocie **a la derecha**:

- `2 ^ 3 ^ 2` = `2 ^ (3 ^ 2)` = **512**, no `(2 ^ 3) ^ 2` = 64
- `9 - 4 - 3` = `(9 - 4) - 3` = **2**, no `9 - (4 - 3)` = 8

## 9. Operandos de más de un dígito

El evaluador hace primero un **análisis léxico**: `tokenizar()` recorre la expresión y agrupa
los dígitos consecutivos (y el punto decimal, si lo hay) en una sola unidad antes de convertir
y evaluar. Así `12` se toma como un solo operando y no como `1` y `2`.

Por eso `convierte()` usa `atof(token)` sobre la unidad completa en lugar de `car - '0'`, que
solo sirve para un dígito, tal como sugiere la presentación de clase.

## 10. Dos pilas y no una

En C un arreglo guarda un solo tipo de dato, y las tres etapas necesitan cosas distintas:

| Pila | Guarda | Se usa en |
|---|---|---|
| `pila[]` de `char` | paréntesis y operadores | validación y conversión |
| `pilaNum[]` de `float` | resultados parciales | evaluación |

En el compilador la segunda pila guarda **posiciones de memoria** (`int`) en lugar de valores:
ése es justamente el "gancho" del punto 6.

## 11. Manejo de errores

Las primitivas de pila vigilan sus dos casos extremos, como pide la teoría:

- **overflow**: meter en una pila llena
- **underflow**: sacar de una pila vacía

El simulador detecta división entre cero, código de operación inválido, desbordamiento del
acumulador, contador fuera de memoria, entrada agotada y valores fuera de rango. En todos los
casos imprime el mensaje **y el vaciado completo**, que es lo que permite localizar la causa.

## 12. Detalles de presentación

- `SetConsoleOutputCP(65001)` dentro de `#ifdef _WIN32` para que la consola de Windows muestre
  los acentos. Con la página de códigos por defecto se leería `termin%` en lugar de `terminó`.
- Una pausa al final, activa solo cuando la entrada viene del teclado (`_isatty`), para que la
  ventana no se cierre de golpe al abrir el `.exe` con doble clic. Con la entrada redirigida
  desde un archivo no se activa, así que no estorba a las pruebas automáticas.
