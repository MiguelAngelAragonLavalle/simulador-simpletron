# Casos de prueba — versión mejorada

Pruebas de las ocho modificaciones de la Sección 3, todas ejecutadas con
`simpletron_mejorado.exe`. La salida real de cada una está en `salidas/`.

## Cómo reproducir cualquier caso

Se le pasa al simulador el programa que se quiere probar y se redirige su entrada desde el
guión de datos correspondiente:

```bash
.\simpletron_mejorado.exe programas\mejoras\m03-residuo.simp < pruebas\mejoras\entradas\m03-residuo-1.txt
```

También funciona copiando el programa sobre `programa.simp` y ejecutando sin argumentos:

```bash
copy programas\mejoras\m03-residuo.simp programa.simp
.\simpletron_mejorado.exe < pruebas\mejoras\entradas\m03-residuo-1.txt
```

Para capturar un programa por teclado sin borrar `programa.simp`:

```bash
.\simpletron_mejorado.exe -t
```

---

## Mejora 1 — Carga desde archivo

El enunciado pide que funcione en tres escenarios. Los tres están probados.

| Escenario | Prueba | Resultado |
|---|---|---|
| (a) Archivo correcto | cualquiera de los casos de abajo | `*** Se cargaron N palabras desde programa.simp ***` ✅ |
| (b) Archivo inexistente | `salidas/_interactivo.txt` | Pasa a carga por teclado ✅ |
| (c) Archivo con línea inválida | `m01-archivo-invalido` | Informa la línea exacta y se detiene ✅ |

Escenario (b) — al no encontrar el archivo, conserva el modo interactivo:

```
*** No se encontró programa.simp: se cargará por teclado ***

*** Escriba una palabra por posición. ***
*** Escriba el centinela -99999 para terminar la carga. ***

000 ? 001 ? 002 ? 003 ? *** Se terminó de cargar el programa ***
```

Escenario (c) — comportamiento definido ante una línea inválida:

```
*** ERROR FATAL: programa.simp, línea 8: no contiene un número válido ***
```

Se informa **el número de línea** y el programa termina, en vez de ejecutar un programa a medio
cargar.

### Formato del archivo `.simp`

```
; las líneas que empiezan con ; o # son comentarios
+10120   ; 000  READ  120     también se puede comentar después del número
+43000   ; 001  HALT
-99999   ; centinela, opcional
```

---

## Mejora 2 — Memoria de 1000 posiciones

**Programa:** `m02-memoria-1000.simp` · **Entrada:** `100` y `23`

Usa las direcciones **120**, **500** y **999**, imposibles en la versión básica.

```
? ? +00123
```

Resultado obtenido: `+00123` ✅

Las direcciones fuera de 000–999 se rechazan mediante la función `verificarRango`.

---

## Mejora 3 — Residuo (instrucción 34 REMAINDER)

**Programa:** `m03-residuo.simp` — cinco pruebas, incluido el divisor cero.

| # | A | B | Esperado | Obtenido |
|:-:|:-:|:-:|:--:|:--:|
| 1 | 17 | 5 | 2 | `+00002` ✅ |
| 2 | 10 | 2 | 0 | `+00000` ✅ |
| 3 | −17 | 5 | −2 | `-00002` ✅ |
| 4 | 7 | 7 | 0 | `+00000` ✅ |
| 5 | 5 | **0** | error controlado | `*** ERROR FATAL: Intento de calcular el residuo entre cero ***` ✅ |

El caso 3 muestra que el residuo conserva el signo del dividendo, igual que el operador `%` de C.

---

## Mejora 4 — Exponenciación (instrucción 35)

**Programa:** `m04-potencia.simp` — seis pruebas.

| # | Base | Exponente | Esperado | Obtenido |
|:-:|:-:|:-:|:--:|:--:|
| 1 | 2 | 10 | 1024 | `+01024` ✅ |
| 2 | 5 | **0** | 1 | `+00001` ✅ |
| 3 | 7 | **1** | 7 | `+00007` ✅ |
| 4 | 3 | 4 | 81 | `+00081` ✅ |
| 5 | −2 | 3 | −8 | `-00008` ✅ |
| 6 | 2 | **20** | desbordamiento | `*** ERROR FATAL: Desbordamiento del acumulador en una exponenciación ***` ✅ |

El caso 6 es el límite documentado: 2²⁰ = 1 048 576, que no cabe en una palabra de ±99999.

---

## Mejora 5 — Salto de línea (instrucción 12 NEWLINE)

Dos casos que se comparan entre sí. Ambos programas son idénticos salvo por la instrucción 12.

**Con NEWLINE** (`m05-nueva-linea.simp`):

```
Escriba un texto y presione Enter: Simpletron
Simpletron
*** Terminó la ejecución de Simpletron ***
```

**Sin NEWLINE** (`m05-sin-nueva-linea.simp`):

```
Escriba un texto y presione Enter: SimpletronSimpletron*** Terminó la ejecución de Simpletron ***
```

El contraste demuestra que la diferencia la produce realmente la instrucción 12: sin ella las
dos impresiones quedan pegadas en el mismo renglón. ✅

---

## Mejoras 6 y 7 — Entrada y salida de cadenas (instrucciones 13 y 14)

### Formato de almacenamiento

A partir de la dirección base indicada en el operando:

| Posición | Contenido |
|---|---|
| `memoria[base]` | Longitud de la cadena |
| `memoria[base+1]` | Código ASCII del primer carácter (3 dígitos) |
| `memoria[base+2]` | Código ASCII del segundo carácter |
| … | … |

### Pruebas

**Programa:** `m06-m07-cadenas.simp` — lee la cadena, la imprime, y luego imprime la longitud y
los códigos ASCII de los dos primeros caracteres para comprobar cómo quedó guardada.

| # | Cadena | Longitud esperada | Obtenido |
|:-:|---|:--:|---|
| 1 | `Hola` | 4 | `Hola`, `+00004`, `+00072` (H), `+00111` (o) ✅ |
| 2 | `Programacion Avanzada` | 21 | Se imprime completa, incluido el espacio ✅ |
| 3 | `A` | 1 | `A`, `+00001`, `+00065` (A) ✅ |

Salida real del caso 1:

```
Escriba un texto y presione Enter: Hola
+00004
+00072
+00111
```

El `+00072` confirma que la `H` se guardó como su código ASCII de tres dígitos, y el `+00004`
que la longitud quedó en la posición base.

El caso 2 comprueba que se conservan los espacios: por eso `READSTRING` usa `fgets` y no
`scanf("%s")`, que cortaría la cadena en el primer espacio.

Se valida que la cadena quepa en memoria: si la dirección base más la longitud se salen de las
1000 posiciones, se produce un error fatal.

---

## Mejora 8 — Punto flotante (instrucciones 15, 16 y 36)

### Representación elegida

La memoria se declara como arreglo de `double` en lugar de `int`. Un entero es simplemente un
`double` sin parte decimal, de modo que **todos los programas de la versión básica siguen
funcionando igual**.

- `32 DIVIDE` conserva la **división entera** (55 ÷ 7 = 7), por compatibilidad.
- `36 DIVIDEFLOAT` realiza la **división real** (55 ÷ 7 = 7.8571).
- `15 READFLOAT` y `16 WRITEFLOAT` leen e imprimen con decimales.

Las instrucciones deben seguir siendo enteras: intentar ejecutar una palabra con decimales es un
error fatal.

### Pruebas

**Programa:** `m08-flotantes.simp` — ocho pruebas.

| # | A | B | Esperado | Obtenido |
|:-:|:-:|:-:|:--:|:--:|
| 1 | 55 | 7 | 7.8571 | `+7.8571` ✅ |
| 2 | 1 | 3 | 0.3333 | `+0.3333` ✅ |
| 3 | 2.5 | 0.5 | 5.0 | `+5.0000` ✅ |
| 4 | 10 | 4 | 2.5 | `+2.5000` ✅ |
| 5 | −7 | 2 | −3.5 | `-3.5000` ✅ |
| 6 | 0 | 5 | 0 | `+0.0000` ✅ |
| 7 | 3.1416 | 1 | 3.1416 | `+3.1416` ✅ |
| 8 | 5 | **0** | error controlado | `*** ERROR FATAL: Intento de dividir entre cero ***` ✅ |

El caso 1 es el contraste directo con la versión básica: el mismo cálculo que allí daba `7`
aquí da `7.8571`.

### Efecto en el vaciado de memoria

El vaciado detecta automáticamente si hay decimales en memoria:

- Si toda la memoria contiene enteros, imprime compacto: `+00012`
- Si hay al menos un decimal, imprime con dos decimales: `+000007.86`

Así las columnas quedan siempre alineadas sin desperdiciar ancho cuando no hay flotantes.

---

## Resumen

| Mejora | Instrucción nueva | Pruebas | Estado |
|---|---|:--:|:--:|
| 1. Carga desde archivo | — | 3 escenarios | ✅ |
| 2. Memoria de 1000 | — | 1 | ✅ |
| 3. Residuo | 34 REMAINDER | 5 | ✅ |
| 4. Exponenciación | 35 EXPONENT | 6 | ✅ |
| 5. Nueva línea | 12 NEWLINE | 2 | ✅ |
| 6. Entrada de cadenas | 13 READSTRING | 3 | ✅ |
| 7. Salida de cadenas | 14 WRITESTRING | 3 | ✅ |
| 8. Punto flotante | 15, 16, 36 | 8 | ✅ |

**Total: 28 ejecuciones**, todas con el resultado esperado.
