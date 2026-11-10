# Casos de prueba: Compilador de Simple a SML

Nueve casos: seis programas que compilan y se ejecutan correctamente, y tres que deben ser
rechazados con un mensaje claro.

## Cómo reproducir

```bash
.\compilador.exe programas\simple\01-sumatoria.simple programas\sml\compilados\01-sumatoria.sml
```

```bash
.\simpletron.exe programas\sml\compilados\01-sumatoria.sml
```

Para pasarle los datos sin teclearlos:

```bash
.\simpletron.exe programas\sml\compilados\01-sumatoria.sml < pruebas\compilador\entradas\01-sumatoria.txt
```

## Programas que compilan y se ejecutan

| # | Programa | Entrada | Instrucciones SML | Salida | Qué ejercita |
|:--:|---|---|:--:|:--:|---|
| 1 | `01-sumatoria.simple` | 5 | 17 | `+0015` | Ejemplo del capítulo: `input`, `if/goto`, `let`, `goto`, `print`, `end` |
| 2 | `02-mayor-de-dos.simple` | 3, 9 | 10 | `+0009` | `if >= goto` y salto incondicional |
| 3 | `03-promedio.simple` | 10, 20, 30 | 18 | `+0020` | `let` con dos operadores y división |
| 4 | `04-tabla.simple` | 3 | 19 | 3, 6, 9 ... 30 | Ciclo con `if >` y `goto`; diez `print` |
| 5 | `05-expresion.simple` | 10, 4 | 18 | `+0023` | `let` con paréntesis y precedencia mixta |
| 6 | `06-factorial.simple` | 5 | 21 | `+0120` | Ciclo con multiplicación acumulada |

Las salidas reales están en `salidas/` y las evidencias completas de compilación en
`../../evidencias/ejecuciones/`.

## Verificación contra el libro

El caso 1 es el ejemplo resuelto del capítulo del compilador. El código generado coincide con
el de la figura 6.2:

```
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

Y la tabla de símbolos coincide con la de la figura 6.3:

| Símbolo | Tipo | Ubicación |
|---|:--:|:--:|
| 5 | L | 00 |
| 10 | L | 00 |
| x | V | 99 |
| 15 | L | 01 |
| 20 | L | 01 |
| y | V | 98 |
| 25 | L | 04 |
| 30 | L | 04 |
| 1 | C | 97 |
| 35 | L | 09 |
| 40 | L | 09 |
| t | V | 95 |
| 45 | L | 14 |
| 50 | L | 14 |
| 55 | L | 15 |
| 60 | L | 15 |
| 99 | L | 16 |

La posición 03 es la única referencia hacia adelante del programa: el `if y == x goto 60` apunta
a una línea que todavía no existía al compilarla. Queda marcada en `flags` y la segunda pasada
la resuelve:

```
--- Segunda pasada: referencias hacia adelante ---
  posicion 03  ->  linea 60 esta en 15  ->  +4215
Se resolvieron 1 referencias.
```

## Referencias hacia adelante por programa

| Programa | Referencias pendientes tras la primera pasada |
|---|:--:|
| `01-sumatoria` | 1 (`if ... goto 60`) |
| `02-mayor-de-dos` | 3 (`if a >= b goto 70` genera **dos** bifurcaciones, más `goto 80`) |
| `04-tabla` | 1 (`if i > 10 goto 90`) |
| `06-factorial` | 1 (`if i > n goto 90`) |
| `03-promedio`, `05-expresion` | 0 (sin saltos hacia adelante) |

El caso de `02-mayor-de-dos` muestra por qué `>=` y `<=` necesitan marcar **dos** posiciones en
`flags`: se traducen a `BRANCHNEG destino` seguido de `BRANCHZERO destino`, y ambas apuntan a la
misma línea todavía desconocida.

## Casos que deben fallar

| # | Programa | Error | Mensaje |
|:--:|---|---|---|
| e01 | `e01-mayuscula.simple` | `input X` con mayúscula | *ERROR en la linea 2 del fuente: Las variables deben ser una sola letra minuscula* |
| e02 | `e02-linea-inexistente.simple` | `goto 500` a una línea que no existe | *ERROR: la linea 500 no existe en el programa* |
| e03 | `e03-comando-invalido.simple` | `imprime a` no es un comando | *ERROR en la linea 2 del fuente: Comando no reconocido* |

En el caso e02 el error **no se detecta en la primera pasada** sino en la segunda, cuando se
intenta resolver la referencia pendiente y el número de línea no está en la tabla de símbolos.

## Uso de la memoria

| Programa | Instrucciones (desde 00) | Datos (hasta 99) | Libre |
|---|:--:|:--:|:--:|
| `01-sumatoria` | 00-16 | 94-99 | 77 |
| `02-mayor-de-dos` | 00-09 | 98-99 | 88 |
| `03-promedio` | 00-17 | 91-99 | 73 |
| `04-tabla` | 00-18 | 93-99 | 74 |
| `05-expresion` | 00-17 | 92-99 | 74 |
| `06-factorial` | 00-20 | 94-99 | 73 |

Las instrucciones crecen hacia adelante desde la 00 y los datos hacia atrás desde la 99. Si los
dos punteros se cruzaran, el compilador reportaría que el programa no cabe en la memoria.
