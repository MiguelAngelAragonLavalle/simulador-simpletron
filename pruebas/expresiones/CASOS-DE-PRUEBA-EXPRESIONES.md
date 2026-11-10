# Casos de prueba: Evaluación de expresiones

Quince casos ejecutados con `expresiones.exe`. La salida completa de cada uno está en
`salidas/`.

## Cómo reproducir

```bash
.\expresiones.exe "( 12 + 8 ) * 3"
```

También funciona sin argumentos, pidiendo la expresión por teclado.

## Casos correctos

| # | Expresión | Postfija | Resultado | Qué comprueba |
|:--:|---|---|:--:|---|
| x01 | `( 12 + 8 ) * 3` | `12 8 + 3 *` | 60 | **Operandos de dos dígitos** y paréntesis |
| x02 | `100 / 4 + 25` | `100 4 / 25 +` | 50 | Operando de **tres dígitos** |
| x03 | `7 + 3 * 20 - 100 / 5` | `7 3 20 * + 100 5 / -` | 47 | Precedencia: `*` y `/` antes que `+` y `-` |
| x04 | `144 / 12 / 3` | `144 12 / 3 /` | 4 | Asociatividad **izquierda** de `/` |
| x05 | `2 ^ 3 ^ 2` | `2 3 2 ^ ^` | 512 | Asociatividad **derecha** de `^` |
| x06 | `{ [ 15 - 5 ] * 12 } + 480` | `15 5 - 12 * 480 +` | 600 | Los tres tipos de agrupador |
| x07 | `( ( 6 - ( 2 + 3 ) ) * ( 3 + ( 8 / 2 ) ) ) ^ 2 + 3` | `6 2 3 + - 3 8 2 / + * 2 ^ 3 +` | 52 | Ejemplo completo de clase |
| x08 | `2.5 * 4` | `2.5 4 *` | 10 | Operando con punto decimal |

Los casos x04 y x05 son los que delatan una conversión mal hecha:

- Si `144 / 12 / 3` diera **36**, la asociatividad izquierda estaría mal.
- Si `2 ^ 3 ^ 2` diera **64**, la asociatividad derecha estaría mal.

El caso x07 es la expresión que se resolvió en clase. La postfija y la tabla de seguimiento
coinciden con las del pizarrón:

```
symb     opnd1      opnd2      value      pila
6                                         6
2                                         6,2
3                                         6,2,3
+        2          3          5          6,5
-        6          5          1          1
3                                         1,3
8                                         1,3,8
2                                         1,3,8,2
/        8          2          4          1,3,4
+        3          4          7          1,7
*        1          7          7          7
2                                         7,2
^        7          2          49         49
3                                         49,3
+        49         3          52         52

Resultado: 52
```

## Casos de error

| # | Expresión | Mensaje |
|:--:|---|---|
| e01 | `( 12 + 8` | Quedaron 1 agrupadores sin cerrar |
| e02 | `[ 3 + 4 )` | Se abrió `[` y se cerró con `)` en la posición 9 |
| e03 | `12 / ( 5 - 5 )` | División entre cero |
| e04 | `5 + * 3` | Falta un operando antes del operador |
| e05 | `+ 2 3` | Falta un operando antes del operador |
| e06 | `2 3` | Faltan operadores: hay dos operandos seguidos |
| e07 | `5 +` | La expresión está incompleta: termina en un operador |

Los casos e04 a e07 se detectan durante la conversión a postfija: `aPostfijo()` revisa que
operandos y operadores se vayan alternando, así que una expresión mal formada se rechaza sin
llegar a la evaluación. Además, las primitivas de la pila vigilan su *underflow*: si al evaluar
faltara un operando, `popNum()` lo reporta con *Faltan operandos en la expresion (underflow)*.

## Validación de anidamiento

Además del balance, el programa reporta la **profundidad máxima de anidamiento**, que es la
cantidad de ámbitos abiertos al mismo tiempo:

| Expresión | Profundidad |
|---|:--:|
| `7 + 3 * 20` | 0 |
| `( 12 + 8 ) * 3` | 1 |
| `{ [ 15 - 5 ] * 12 } + 480` | 2 |
| `( ( 6 - ( 2 + 3 ) ) * ( 3 + ( 8 / 2 ) ) ) ^ 2 + 3` | 3 |

## Operandos de más de un dígito

El programa hace primero un **análisis léxico**: agrupa los dígitos consecutivos en una sola
unidad antes de convertir. Por eso `convierte()` usa `atof()` sobre la unidad completa en lugar
de `car - '0'`, que solo sirve para un carácter.

```
Infija   : ( 12 + 8 ) * 3
Postfija : 12 8 + 3 *
```

El `12` se conserva como un solo operando; no se parte en `1` y `2`.
