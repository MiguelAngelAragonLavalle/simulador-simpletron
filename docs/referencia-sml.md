# Referencia de SML (Simpletron Machine Language)

## La palabra Simpletron

Toda la información de la Simpletron se maneja en **palabras**: números decimales con signo de
cuatro dígitos, como `+0007`, `-0001`, `+1234` o `-3364`.

El rango de una palabra es **−9999 a +9999**. Durante la carga el máximo es **+9998**, porque
`+9999` está reservado como centinela de fin de carga.

Cada posición de memoria puede contener tres cosas distintas:

- una **instrucción**,
- un **dato** que usa el programa,
- o una posición **no utilizada**.

La máquina no distingue entre ellas: es responsabilidad del programador saber qué hay en cada
dirección. Ésa es la esencia del modelo de Von Neumann, donde instrucciones y datos comparten
la misma memoria.

## Formato de una instrucción

```
    + 1 0 0 9
      └─┬┘ └┬┘
       OP  ADDR
```

| Parte | Dígitos | Significado |
|---|---|---|
| **OP** | los 2 de la izquierda | Qué operación realizar |
| **ADDR** | los 2 de la derecha | Sobre qué dirección de memoria (00 a 99) |

Se separan con aritmética entera:

```
1009 / 100  =  10   (la división entera descarta los dos últimos dígitos)
1009 % 100  =   9   (el residuo deja exactamente los dos últimos dígitos)
```

Se supone que el signo de una **instrucción** siempre es positivo. El signo de un **dato** puede
ser positivo o negativo.

---

## Las doce operaciones

### Entrada y salida

#### `10` — READ

Detiene temporalmente la ejecución, muestra `?`, lee una palabra desde el teclado y la almacena
en `memory[ADDR]`.

```
+1007   ->   lee un valor y lo guarda en memory[07]
```

#### `11` — WRITE

Imprime en pantalla la palabra con signo almacenada en `memory[ADDR]`.

```
+1109   ->   imprime memory[09]
```

### Carga y almacenamiento

#### `20` — LOAD

Copia al acumulador la palabra almacenada en `memory[ADDR]`.

```
accumulator ← memory[ADDR]
```

#### `21` — STORE

Copia el contenido actual del acumulador a `memory[ADDR]`.

```
memory[ADDR] ← accumulator
```

Nota: `STORE` **no modifica el acumulador**; el valor sigue disponible después de guardarlo.

### Aritmética

Todas dejan su resultado en el acumulador y todas pueden desbordarlo.

| OP | Nombre | Efecto |
|:--:|---|---|
| 30 | ADD | `accumulator ← accumulator + memory[ADDR]` |
| 31 | SUBTRACT | `accumulator ← accumulator − memory[ADDR]` |
| 32 | DIVIDE | `accumulator ← accumulator ÷ memory[ADDR]` |
| 33 | MULTIPLY | `accumulator ← accumulator × memory[ADDR]` |

Dos advertencias importantes:

- **`DIVIDE` es división entera.** `55 ÷ 7` da `7`, no `7.857`. La Simpletron no maneja
  decimales.
- **`DIVIDE` entre cero es un error fatal.** El simulador lo verifica antes de operar.

### Transferencias de control

Son el equivalente del `if`, el `while` y el `for` de C. Todas funcionan de la misma manera:
escriben una dirección distinta en el contador de instrucciones.

#### `40` — BRANCH

Salto incondicional: la ejecución continúa en `ADDR`.

```
+4017   ->   instructionCounter ← 17
```

#### `41` — BRANCHNEG

Salta a `ADDR` **solo si el acumulador es negativo**. Si no, continúa con la instrucción
siguiente.

#### `42` — BRANCHZERO

Salta a `ADDR` **solo si el acumulador es cero**. Si no, continúa con la instrucción siguiente.

### Terminación

#### `43` — HALT

Detiene por completo la ejecución. Los dos dígitos de `ADDR` no tienen efecto; por convención se
escribe `+4300`.

---

## Cómo se escriben las estructuras de control

SML no tiene `if` ni `while`. Se construyen combinando una resta con un salto condicional.

### El `if`

```c
if (A < B) { imprimir B; } else { imprimir A; }
```

```
02  +2009    LOAD      09     acumulador ← A
03  +3110    SUBTRACT  10     acumulador ← A − B
04  +4107    BRANCHNEG 07     si A − B < 0 (A < B) ir a 07
05  +1109    WRITE     09     caso A >= B
06  +4300    HALT
07  +1110    WRITE     10     caso A < B
08  +4300    HALT
```

La clave: para comparar dos números se **restan** y se examina el signo del resultado.

### El ciclo controlado por contador

```c
for (i = 10; i > 0; i--) { ... }
```

```
04  +2013    LOAD       13    acumulador ← contador
05  +3114    SUBTRACT   14    acumulador ← contador − 1
06  +2113    STORE      13    contador ← acumulador
07  +4209    BRANCHZERO 09    si llegó a 0, salir del ciclo
08  +4000    BRANCH     00    si no, repetir
```

---

## Organización de la memoria

La Simpletron tiene 100 posiciones (00 a 99) y no distingue automáticamente entre programa y
datos. La convención que se sigue es:

| Zona | Contenido |
|---|---|
| A partir de 00 | **Área de programa**: las instrucciones, en orden de ejecución |
| Después del programa | **Área de datos**: variables, contadores y constantes |

Dos reglas para no tener errores difíciles de encontrar:

1. Que el programa **no pise** sus propias variables.
2. Que ninguna variable ocupe una dirección donde hay instrucciones.

Si el contador de instrucciones llega al área de datos (por ejemplo, porque falta un `HALT`),
la máquina intentará ejecutar un número como si fuera una instrucción, y lo más probable es que
se produzca el error fatal *código de operación no válido*.

---

## Tabla resumen

| OP | Nombre | Efecto |
|:--:|---|---|
| 10 | READ | `memory[ADDR] ←` teclado |
| 11 | WRITE | pantalla `← memory[ADDR]` |
| 20 | LOAD | `accumulator ← memory[ADDR]` |
| 21 | STORE | `memory[ADDR] ← accumulator` |
| 30 | ADD | `accumulator ← accumulator + memory[ADDR]` |
| 31 | SUBTRACT | `accumulator ← accumulator − memory[ADDR]` |
| 32 | DIVIDE | `accumulator ← accumulator ÷ memory[ADDR]` |
| 33 | MULTIPLY | `accumulator ← accumulator × memory[ADDR]` |
| 40 | BRANCH | `instructionCounter ← ADDR` |
| 41 | BRANCHNEG | si `accumulator < 0`, `instructionCounter ← ADDR` |
| 42 | BRANCHZERO | si `accumulator == 0`, `instructionCounter ← ADDR` |
| 43 | HALT | detiene la ejecución |
