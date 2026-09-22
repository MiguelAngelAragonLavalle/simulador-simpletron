# Simulador de la computadora Simpletron

Simulador por software, escrito en lenguaje C, de la computadora **Simpletron** y de su
lenguaje de máquina **SML** (*Simpletron Machine Language*).

El simulador convierte la computadora donde se ejecuta en una Simpletron: administra una
memoria de 100 palabras, implementa las doce operaciones de SML, realiza el ciclo completo de
búsqueda–decodificación–ejecución, valida las entradas y detecta errores fatales, mostrando en
todos los casos un vaciado completo de registros y memoria.

> Materia: Programación Avanzada
> Referencia: Deitel H.M. y Deitel P.J. (1998). *Cómo programar en C*, 1ra Edición.
> México. Prentice Hall Latinoamérica. Usado con propósitos educativos.

---

## Las dos versiones

El repositorio contiene **dos simuladores independientes**, cada uno con su propio código fuente
y sus propias pruebas.

| | Versión básica | Versión mejorada |
|---|---|---|
| Archivo | `simpletron.c` | `simpletron_mejorado.c` |
| Corresponde a | Sección 2 | Sección 3 |
| Memoria | **100** posiciones (00–99) | 1000 posiciones (000–999) |
| Palabra | 4 dígitos con signo (±9999) | 5 dígitos con signo (±99999) |
| Instrucciones | 12 | 19 |
| Carga del programa | Solo por teclado | Desde `programa.simp`, o por teclado si no existe |
| Centinela | `9999` | `-99999` |
| Tipo de dato | Enteros | Enteros y punto flotante |

La versión básica **no fue modificada** al construir la mejorada: son dos programas separados,
de modo que cada uno cumple exactamente con su propio enunciado.

Si lo que se busca es el simulador de la Sección 2, es `simpletron.c`. Las mejoras están
documentadas más abajo, en su propia sección.

---

## Contenido del repositorio

```
simulador-simpletron/
├── README.md                     Este documento
├── simpletron.c                  VERSIÓN BÁSICA (Sección 2)
├── simpletron_mejorado.c         VERSIÓN MEJORADA (Sección 3)
├── programa.simp                 Programa que carga la versión mejorada al iniciar
├── docs/
│   └── referencia-sml.md         Las doce operaciones de SML, explicadas
├── programas/                    Programas SML de la versión básica, documentados
│   ├── 01-suma.sml … 06-mayor-de-serie.sml
│   ├── e01-division-entre-cero.sml … e04-carga-fuera-de-rango.sml
│   └── mejoras/                  Programas .simp de la versión mejorada
│       ├── demo.simp
│       ├── m02-memoria-1000.simp … m08-flotantes.simp
│       └── m01-archivo-invalido.simp
└── pruebas/
    ├── CASOS-DE-PRUEBA.md        Casos de la versión básica, con su resultado
    ├── entradas/                 Guiones de entrada (lo que se teclea)
    ├── salidas/                  Evidencia: salida real capturada
    └── mejoras/
        ├── CASOS-DE-PRUEBA-MEJORAS.md
        ├── entradas/
        └── salidas/
```

---

## Requisitos

- **Windows** (el simulador también compila en cualquier sistema con un compilador de C)
- **GCC** (se instala en Windows mediante **MinGW-w64**)

Verificar que el compilador esté disponible:

```bash
gcc --version
```

Si el comando no se reconoce, falta agregar la carpeta `bin` de MinGW-w64 al `PATH` del
sistema.

---

## Compilación

Desde la carpeta del proyecto:

```bash
gcc -Wall -Wextra -std=c11 -g simpletron.c -o simpletron.exe
```

| Opción | Para qué sirve |
|---|---|
| `-Wall -Wextra` | Activa todas las advertencias del compilador |
| `-std=c11` | Compila con el estándar C11 |
| `-g` | Incluye información de depuración |
| `-o simpletron.exe` | Nombre del ejecutable generado |

El programa compila **sin errores y sin advertencias** con esas opciones.

---

## Ejecución

```bash
.\simpletron.exe
```

El simulador trabaja en dos fases.

### Fase 1 — Carga del programa

Muestra cada posición de memoria seguida de un signo de interrogación. El usuario escribe una
palabra SML por posición. Para terminar la carga se escribe el centinela **9999**.

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

Durante la carga se valida cada palabra: debe ser un número entero dentro del rango
**−9999 a +9998**. Si no lo es, se muestra un mensaje y se vuelve a pedir **la misma posición**.

### Fase 2 — Ejecución

El simulador ejecuta el programa desde la posición 00. Cuando una instrucción `READ` necesita
un dato, lo pide con `?`. Al terminar (o al detectar un error fatal) imprime el vaciado
completo de la máquina.

```
? 5
? 7
+0012
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
 ...
```

### Ejecución automática de un caso de prueba

Para no teclear el programa a mano, se puede redirigir la entrada desde un archivo con el
operador `<`. Esto es una función del intérprete de comandos, no del simulador: el programa
sigue leyendo "del teclado", solo que el sistema operativo le entrega el contenido del archivo.

```bash
.\simpletron.exe < pruebas\entradas\01-suma.txt
```

Para guardar la salida en un archivo:

```bash
.\simpletron.exe < pruebas\entradas\01-suma.txt > salida.txt
```

---

## Las doce operaciones de SML

Cada instrucción es una palabra decimal de cuatro dígitos con signo. Los dos dígitos de la
izquierda son el **código de operación (OP)** y los dos de la derecha son la **dirección u
operando (ADDR)**.

```
    + 1 0 0 9
      └─┬┘ └┬┘
       OP  ADDR
       10   09
```

| OP | Nombre | Efecto |
|:--:|---|---|
| 10 | READ | Lee una palabra del teclado y la guarda en `memory[ADDR]` |
| 11 | WRITE | Imprime en pantalla `memory[ADDR]` |
| 20 | LOAD | `accumulator ← memory[ADDR]` |
| 21 | STORE | `memory[ADDR] ← accumulator` |
| 30 | ADD | `accumulator ← accumulator + memory[ADDR]` |
| 31 | SUBTRACT | `accumulator ← accumulator − memory[ADDR]` |
| 32 | DIVIDE | `accumulator ← accumulator ÷ memory[ADDR]` |
| 33 | MULTIPLY | `accumulator ← accumulator × memory[ADDR]` |
| 40 | BRANCH | Salta a `ADDR` incondicionalmente |
| 41 | BRANCHNEG | Salta a `ADDR` si el acumulador es negativo |
| 42 | BRANCHZERO | Salta a `ADDR` si el acumulador es cero |
| 43 | HALT | Detiene la ejecución |

La explicación detallada de cada una está en [`docs/referencia-sml.md`](docs/referencia-sml.md).

---

## Arquitectura del simulador

### Registros

Los cinco registros especiales se inicializan en 0 al comenzar la ejecución.

| Registro | Función |
|---|---|
| `accumulator` | Registro donde se realizan todas las operaciones aritméticas |
| `instructionCounter` | Dirección de la siguiente instrucción por ejecutar (IP/PC) |
| `instructionRegister` | Palabra que se está ejecutando en este momento |
| `operationCode` | Los dos dígitos izquierdos del registro de instrucción |
| `operand` | Los dos dígitos derechos del registro de instrucción |

### Ciclo de ejecución de instrucción

```
    ┌──────────────────────────────────────────────────┐
    │ 1. BUSCAR        traer la instrucción de memoria │
    │ 2. DECODIFICAR   separarla en OP y ADDR          │
    │ 3. EJECUTAR      realizar la operación           │
    │ 4. AVANZAR       incrementar el contador         │
    └────────────────────┬─────────────────────────────┘
                         └──── repetir hasta HALT ────┘
```

La decodificación usa aritmética entera:

```c
instructionRegister = memory[instructionCounter];
operationCode       = instructionRegister / 100;   /* dos dígitos izquierdos */
operand             = instructionRegister % 100;   /* dos dígitos derechos   */
```

El paso 4 se omite cuando la instrucción fue una transferencia de control (`BRANCH`,
`BRANCHNEG`, `BRANCHZERO`), porque en ese caso el contador ya apunta al destino del salto.

### Organización del código

| Función | Responsabilidad |
|---|---|
| `main` | Coordina las cuatro etapas del programa |
| `mostrarBienvenida` | Pantalla inicial con las instrucciones de captura |
| `cargarPrograma` | Fase de carga y validación de las palabras |
| `ejecutarPrograma` | Ciclo de búsqueda, decodificación y ejecución |
| `volcadoDeMemoria` | Impresión formateada de registros y memoria |
| `errorFatal` | Mensaje de error, vaciado y terminación |
| `leerPalabra` | Lectura segura de un entero desde la entrada |
| `limpiarEntrada` | Descarta entrada sobrante para evitar ciclos infinitos |

---

## Errores detectados

### Validación durante la carga

| Situación | Comportamiento |
|---|---|
| Valor fuera del rango −9999 a +9998 | Se rechaza y se vuelve a pedir la misma posición |
| Entrada que no es un número | Se rechaza y se vuelve a pedir la misma posición |
| Memoria llena sin recibir el centinela | Se avisa y comienza la ejecución |

### Errores fatales durante la ejecución

Al detectar cualquiera de estos, el simulador imprime el mensaje de error, realiza el **vaciado
completo de registros y memoria** y termina con código de salida 1.

| Error | Cuándo ocurre |
|---|---|
| División entre cero | `DIVIDE` con `memory[ADDR]` igual a 0 |
| Código de operación no válido | El OP no es ninguno de los doce |
| Desbordamiento del acumulador | Una operación aritmética produce un resultado fuera de ±9999 |
| Contador fuera de la memoria | El contador de instrucciones apunta fuera de 00–99 |
| Entrada agotada durante `READ` | Se pide un dato y ya no hay nada que leer |
| Valor fuera de rango en `READ` | Se rechaza y se vuelve a pedir el dato |

---

## Casos de prueba

Se incluyen **diez casos**: seis de ejecución correcta, tres de error fatal y uno de validación
de entradas. Cada caso tiene su programa SML documentado en `programas/`, su guión de entrada
en `pruebas/entradas/` y la salida real capturada en `pruebas/salidas/`.

| Caso | Qué prueba | Resultado esperado |
|---|---|:--:|
| 01-suma | READ, LOAD, ADD, STORE, WRITE, HALT | `+0012` |
| 02-mayor-de-dos | SUBTRACT y BRANCHNEG (el `if` de SML) | `+0009` |
| 03-producto | MULTIPLY | `+0042` |
| 04-suma-de-10 | Ciclo con BRANCHZERO y BRANCH | `+0055` |
| 05-promedio-de-7 | Ciclo + DIVIDE, con datos negativos | `+0007` |
| 06-mayor-de-serie | Ciclo con condición anidada | `+0025` |
| e01-division-entre-cero | Error fatal: división entre cero | ERROR FATAL |
| e02-operacion-invalida | Error fatal: código de operación 99 | ERROR FATAL |
| e03-desbordamiento | Error fatal: 9000 + 9000 se sale de ±9999 | ERROR FATAL |
| e04-carga-fuera-de-rango | Validación en la fase de carga | Rechaza y continúa |

El detalle completo, con la salida obtenida de cada uno, está en
[`pruebas/CASOS-DE-PRUEBA.md`](pruebas/CASOS-DE-PRUEBA.md).

Para reproducir todos de una vez, desde la carpeta del proyecto:

```bash
for %f in (pruebas\entradas\*.txt) do @(echo === %~nf & simpletron.exe < "%f" > "pruebas\salidas\%~nf.txt")
```

---

## Versión mejorada (Sección 3)

Las ocho modificaciones solicitadas están implementadas en `simpletron_mejorado.c`.

### Compilación y ejecución

```bash
gcc -Wall -Wextra -std=c11 -g simpletron_mejorado.c -o simpletron_mejorado.exe -lm
```

```bash
.\simpletron_mejorado.exe
```

La opción `-lm` enlaza la biblioteca matemática, necesaria para `pow` y `fmod`. También compila
sin errores ni advertencias.

Al iniciar busca el archivo **`programa.simp`** en la carpeta actual. Si lo encuentra, carga el
programa automáticamente; si no, pide las instrucciones por teclado igual que la versión básica.

### Las ocho mejoras

| # | Mejora | Cómo se implementó |
|:-:|---|---|
| 1 | Carga desde archivo | Lee `programa.simp`; si no existe, carga interactiva |
| 2 | Memoria de 1000 posiciones | `TAM_MEMORIA` pasa a 1000; la dirección crece a 3 dígitos |
| 3 | Residuo | Nueva instrucción **34 REMAINDER** |
| 4 | Exponenciación | Nueva instrucción **35 EXPONENT** |
| 5 | Salto de línea | Nueva instrucción **12 NEWLINE** |
| 6 | Entrada de cadenas | Nueva instrucción **13 READSTRING** |
| 7 | Salida de cadenas | Nueva instrucción **14 WRITESTRING** |
| 8 | Punto flotante | Memoria de tipo `double` + **15 READFLOAT**, **16 WRITEFLOAT**, **36 DIVIDEFLOAT** |

### Decisiones de diseño

**Tamaño de palabra.** Con 1000 posiciones la dirección ya no cabe en dos dígitos, así que la
palabra crece de 4 a 5 dígitos con signo:

```
    + 1 0   0 0 7
      └─┬┘   └─┬─┘
       OP     ADDR
    2 dígitos 3 dígitos
```

La decodificación pasa de `/100` y `%100` a **`/1000` y `%1000`**.

**Centinela.** Como `+9999` ya es una palabra válida, el centinela de fin de carga pasa a ser
**`-99999`**, y el rango aceptado al cargar es de −99998 a +99999.

**Punto flotante.** La memoria se declara como arreglo de `double`. Un entero es un `double` sin
parte decimal, así que los programas de la versión básica siguen funcionando igual.
`32 DIVIDE` conserva la división entera por compatibilidad; para división real se usa
`36 DIVIDEFLOAT`.

**Cadenas.** Se guardan a partir de una dirección base: `memoria[base]` contiene la longitud y
cada posición siguiente el código ASCII de un carácter, expresado en tres dígitos.

### Instrucciones nuevas

| OP | Nombre | Efecto |
|:--:|---|---|
| 12 | NEWLINE | Imprime un salto de línea (el operando no se usa) |
| 13 | READSTRING | Lee una línea de texto y la guarda desde `ADDR` en formato ASCII |
| 14 | WRITESTRING | Imprime la cadena guardada a partir de `ADDR` |
| 15 | READFLOAT | Lee un número con decimales en `memory[ADDR]` |
| 16 | WRITEFLOAT | Imprime `memory[ADDR]` con decimales |
| 34 | REMAINDER | `accumulator ← accumulator mod memory[ADDR]` |
| 35 | EXPONENT | `accumulator ← accumulator ^ memory[ADDR]` |
| 36 | DIVIDEFLOAT | División real, sin truncar |

### Formato del archivo `programa.simp`

Texto plano, una palabra por línea. Las líneas que empiezan con `;` o `#` son comentarios, y lo
que se escriba después del número también se ignora, de modo que cada instrucción puede
comentarse:

```
; m02-memoria-1000.simp
+10120   ; 000  READ  120     Lee A en la dirección 120
+10500   ; 001  READ  500     Lee B en la dirección 500
+20120   ; 002  LOAD  120
+30500   ; 003  ADD   500
+21999   ; 004  STORE 999
+11999   ; 005  WRITE 999
+43000   ; 006  HALT
-99999   ; centinela (opcional)
```

Si una línea no contiene un número válido, el simulador informa **el número de línea** y se
detiene, en lugar de ejecutar un programa a medio cargar.

### Cómo elegir de dónde se carga el programa

Sin argumentos, el comportamiento es el que pide el enunciado: carga `programa.simp` y, si el
archivo no existe, pasa a la captura por teclado. Además admite dos argumentos opcionales para
no tener que renombrar ni copiar archivos:

| Comando | Qué hace |
|---|---|
| `.\simpletron_mejorado.exe` | Carga `programa.simp`; si no existe, pide por teclado |
| `.\simpletron_mejorado.exe -t` | Fuerza la captura por teclado, exista o no el archivo |
| `.\simpletron_mejorado.exe otro.simp` | Carga el archivo que se indique |

Ejemplos:

```bash
.\simpletron_mejorado.exe -t
```

```bash
.\simpletron_mejorado.exe programas\mejoras\m03-residuo.simp
```

También sigue funcionando la forma manual, copiando el programa encima:

```bash
copy programas\mejoras\m03-residuo.simp programa.simp
```

### Cómo termina el simulador

Hay dos finales distintos y conviene no confundirlos:

| Qué se quiere terminar | Versión básica | Versión mejorada |
|---|---|---|
| La **carga** del programa | teclear `9999` | teclear **`-99999`** |
| La **ejecución** | instrucción `+4300` (HALT) | instrucción `+43000` (HALT) |
| Interrumpir a la fuerza | `Ctrl + C` | `Ctrl + C` |

El centinela solo se pide cuando la carga es por teclado. Si existe `programa.simp`, el
programa se carga desde el archivo y el simulador nunca pregunta por él.

La ejecución no se termina tecleando nada: se detiene cuando el programa SML llega a su
instrucción `HALT`, que es responsabilidad de quien escribe el programa.

### Pruebas de las mejoras

**28 ejecuciones**, todas con el resultado esperado. El detalle completo está en
[`pruebas/mejoras/CASOS-DE-PRUEBA-MEJORAS.md`](pruebas/mejoras/CASOS-DE-PRUEBA-MEJORAS.md).

| Mejora | Pruebas | Incluye |
|---|:--:|---|
| 1. Carga desde archivo | 3 | archivo correcto, archivo inexistente, línea inválida |
| 2. Memoria de 1000 | 1 | direcciones 120, 500 y 999 |
| 3. Residuo | 5 | incluye divisor cero |
| 4. Exponenciación | 6 | exponente 0, exponente 1, base negativa, desbordamiento |
| 5. Nueva línea | 2 | con y sin la instrucción, para contrastar |
| 6 y 7. Cadenas | 3 | cadena con espacios, cadena de un carácter |
| 8. Punto flotante | 8 | precisión, negativos, cero, división entre cero |

Para ejecutar un caso concreto se copia su programa y se redirige la entrada:

```bash
copy programas\mejoras\m03-residuo.simp programa.simp
```

```bash
.\simpletron_mejorado.exe < pruebas\mejoras\entradas\m03-residuo-1.txt
```

---

## Notas de implementación

- La memoria se inicializa completamente en cero antes de la carga, para que el vaciado no
  muestre valores indeterminados.
- `DIVIDE` realiza **división entera**: 55 ÷ 7 da 7, no 7.857.
- El simulador llama a `SetConsoleOutputCP(65001)` al iniciar, únicamente en Windows, para que
  la consola muestre correctamente los acentos y la eñe. La llamada está dentro de un
  `#ifdef _WIN32`, de modo que el código sigue siendo C estándar.
- Los archivos de `programas/` son **documentación**, no son leídos por el simulador: esta
  versión carga los programas desde el teclado, tal como lo especifica el enunciado.
