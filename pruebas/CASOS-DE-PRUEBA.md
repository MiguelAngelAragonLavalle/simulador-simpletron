# Casos de prueba

Diez casos que ejercitan las doce operaciones de SML, las validaciones de entrada y los errores
fatales. Todos fueron ejecutados y su salida real está guardada en `salidas/`.

**Cómo reproducir cualquier caso** (desde la carpeta del proyecto, ya compilado):

```bash
.\simpletron.exe < pruebas\entradas\01-suma.txt
```

**Cómo leer los archivos:**

| Carpeta | Contiene |
|---|---|
| `../programas/*.sml` | El programa SML documentado línea por línea |
| `entradas/*.txt` | Exactamente lo que se teclea: el programa, el centinela 9999 y los datos |
| `salidas/*.txt` | La salida real capturada al ejecutar ese caso |

---

## Resumen

| # | Caso | Operaciones ejercitadas | Esperado | Obtenido | Código de salida |
|:-:|---|---|:--:|:--:|:--:|
| 1 | `01-suma` | READ, LOAD, ADD, STORE, WRITE, HALT | `+0012` | `+0012` ✅ | 0 |
| 2 | `02-mayor-de-dos` | SUBTRACT, BRANCHNEG | `+0009` | `+0009` ✅ | 0 |
| 3 | `03-producto` | MULTIPLY | `+0042` | `+0042` ✅ | 0 |
| 4 | `04-suma-de-10` | BRANCHZERO, BRANCH (ciclo) | `+0055` | `+0055` ✅ | 0 |
| 5 | `05-promedio-de-7` | DIVIDE + ciclo, con negativos | `+0007` | `+0007` ✅ | 0 |
| 6 | `06-mayor-de-serie` | Ciclo con condición anidada | `+0025` | `+0025` ✅ | 0 |
| 7 | `e01-division-entre-cero` | Error fatal | ERROR FATAL | ERROR FATAL ✅ | 1 |
| 8 | `e02-operacion-invalida` | Error fatal | ERROR FATAL | ERROR FATAL ✅ | 1 |
| 9 | `e03-desbordamiento` | Error fatal | ERROR FATAL | ERROR FATAL ✅ | 1 |
| 10 | `e04-carga-fuera-de-rango` | Validación en la carga | Rechaza 3 y continúa | Rechaza 3 y continúa ✅ | 0 |

Las doce operaciones quedan cubiertas entre los casos 1 a 6:

| Operación | Caso donde se prueba |
|---|---|
| 10 READ | 1, 2, 3, 4, 5, 6 |
| 11 WRITE | 1, 2, 3, 4, 5, 6 |
| 20 LOAD | 1, 2, 3, 4, 5, 6 |
| 21 STORE | 1, 3, 4, 5, 6 |
| 30 ADD | 1, 4, 5 |
| 31 SUBTRACT | 2, 4, 5, 6 |
| 32 DIVIDE | 5 |
| 33 MULTIPLY | 3 |
| 40 BRANCH | 4, 5, 6 |
| 41 BRANCHNEG | 2, 6 |
| 42 BRANCHZERO | 4, 5, 6 |
| 43 HALT | 1, 2, 3, 4, 5, 6 |

---

## Caso 1 — Suma de dos números

**Programa:** `programas/01-suma.sml` · **Entrada:** `5` y `7` · **Esperado:** `+0012`

Salida de la fase de ejecución:

```
? ? +0012
*** Terminó la ejecución de Simpletron ***

REGISTROS:
accumulator           +0012
instructionCounter    06
instructionRegister   +4300
operationCode         43
operand               00

MEMORIA:
        0     1     2     3     4     5     6     7     8     9
  0 +1007 +1008 +2007 +3008 +2109 +1109 +4300 +0005 +0007 +0012
 10 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000
```

El renglón 0 del vaciado muestra el modelo de Von Neumann en acción: las posiciones 00 a 06
contienen el programa y las 07, 08 y 09 contienen los datos (A = 5, B = 7, C = 12), todo en la
misma memoria.

---

## Caso 2 — El mayor de dos números

**Programa:** `programas/02-mayor-de-dos.sml` · **Entrada:** `3` y `9` · **Esperado:** `+0009`

Se calcula `A − B = 3 − 9 = −6`. Como el resultado es negativo, `BRANCHNEG` transfiere el
control a la posición 07, que imprime B. Es la traducción a SML de:

```c
if (A < B) printf("%d", B); else printf("%d", A);
```

---

## Caso 3 — Producto de dos números

**Programa:** `programas/03-producto.sml` · **Entrada:** `6` y `7` · **Esperado:** `+0042`

Prueba `MULTIPLY` (33) y su verificación de desbordamiento (que aquí no se dispara).

---

## Caso 4 — Suma de diez números

**Programa:** `programas/04-suma-de-10.sml` · **Entrada:** `1 2 3 4 5 6 7 8 9 10` ·
**Esperado:** `+0055`

En SML no existe `while`. El ciclo se construye restando 1 a un contador y decidiendo con
`BRANCHZERO` si continuar o salir, con un `BRANCH` incondicional que regresa al inicio:

```
07 +4209    BRANCHZERO 09    si el contador llegó a 0 -> salir del ciclo
08 +4000    BRANCH     00    si no -> repetir
```

---

## Caso 5 — Promedio de siete números

**Programa:** `programas/05-promedio-de-7.sml` ·
**Entrada:** `10 −5 20 −15 30 −25 40` · **Esperado:** `+0007`

La suma es 55 y hay 7 números. `DIVIDE` hace **división entera**, así que 55 ÷ 7 da **7** y no
7.857: la parte fraccionaria se descarta porque la Simpletron solo maneja enteros.

Incluye datos negativos, como pide el enunciado del ejercicio.

---

## Caso 6 — El mayor de una serie

**Programa:** `programas/06-mayor-de-serie.sml` ·
**Entrada:** `5` y luego `3 17 8 25 12` · **Esperado:** `+0025`

El primer valor leído indica cuántos números se van a procesar. El programa guarda el primero
como mayor provisional y, con cada nuevo número, lo compara restando y usando `BRANCHNEG` para
decidir si actualiza el máximo.

---

## Caso 7 — Error fatal: división entre cero

**Programa:** `programas/e01-division-entre-cero.sml` · **Entrada:** `8`

```
*** ERROR FATAL: Intento de dividir entre cero ***
*** La ejecución de Simpletron termina de forma anormal ***

REGISTROS:
accumulator           +0008
instructionCounter    02
instructionRegister   +3205
operationCode         32
operand               05
```

El vaciado indica con precisión dónde falló: **posición 02**, ejecutando **+3205**, es decir
intentando dividir entre `memory[05]`, que vale 0. Con esa información se localiza el error en
el programa SML.

En C, dividir entre cero no produce "infinito" ni un mensaje: el programa termina de forma
abrupta. Por eso el simulador revisa el divisor **antes** de realizar la división.

---

## Caso 8 — Error fatal: código de operación no válido

**Programa:** `programas/e02-operacion-invalida.sml` · **Entrada:** `5`

```
*** ERROR FATAL: Código de operación no válido ***

REGISTROS:
accumulator           +0000
instructionCounter    01
instructionRegister   +9900
operationCode         99
operand               00
```

La palabra `+9900` tiene código de operación **99**, que no corresponde a ninguna de las doce
instrucciones válidas, por lo que cae en el `default` del `switch`.

Este mismo error aparece cuando un programa sin `HALT` sigue avanzando y llega al área de
datos: intenta ejecutar un número como si fuera una instrucción.

---

## Caso 9 — Error fatal: desbordamiento del acumulador

**Programa:** `programas/e03-desbordamiento.sml` · **Entrada:** `9000` y `9000`

```
*** ERROR FATAL: Desbordamiento del acumulador en una suma ***

REGISTROS:
accumulator           +18000
instructionCounter    03
instructionRegister   +3006
operationCode         30
operand               06
```

Una palabra Simpletron solo admite valores de −9999 a +9999. La suma 9000 + 9000 = 18000 no
cabe. El vaciado muestra el valor desbordado (`+18000`) precisamente para que se vea la causa.

La misma verificación se aplica a `ADD`, `SUBTRACT` y `MULTIPLY`.

---

## Caso 10 — Validación de entradas durante la carga

**Programa:** `programas/e04-carga-fuera-de-rango.sml`

Se teclean a propósito tres valores inválidos antes del programa real:

| Se teclea | Por qué se rechaza |
|---|---|
| `-12000` | Menor que −9999 |
| `99999` | Mayor que +9998 |
| `abc` | No es un número |

Salida obtenida durante la carga:

```
00 ? *** Valor fuera de rango: debe estar entre -9999 y +9998. ***
00 ? *** Valor fuera de rango: debe estar entre -9999 y +9998. ***
00 ? *** Entrada inválida: escriba un número entero. ***
00 ? 01 ? 02 ? 03 ? 04 ? *** Se terminó de cargar el programa ***
```

Obsérvese que la posición **no avanza**: después de cada rechazo se vuelve a pedir la
dirección 00. Solo cuando llega un valor válido se guarda y se pasa a la 01.

Terminada la carga, el programa se ejecuta normalmente e imprime `+0007`, lo que confirma que
ninguna de las entradas rechazadas quedó almacenada en memoria.

---

## Verificación de compilación

```
gcc -Wall -Wextra -std=c11 -g simpletron.c -o simpletron.exe
```

Compila **sin errores y sin advertencias**.
