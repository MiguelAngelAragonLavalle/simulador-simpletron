# El lenguaje Simple

Simple es un lenguaje de alto nivel mínimo, parecido a las primeras versiones de BASIC. Cada
programa escrito en Simple se traduce a SML con el compilador de este proyecto y el resultado
se ejecuta en el simulador de Simpletron.

## Reglas generales

| Regla | Detalle |
|---|---|
| Números de línea | Todo enunciado empieza con un número de línea entero, en **orden ascendente** y sin repetirse |
| Un enunciado por línea | No se admiten dos comandos en el mismo renglón |
| Variables | Una **sola letra minúscula**: `a` ... `z` |
| Constantes | Enteras, de 0 a 9999 |
| Minúsculas | Comandos y variables van en minúsculas; la única excepción es el texto de un `rem` |
| Separación | Los elementos se separan con espacios. En las expresiones, **operandos y operadores deben ir separados por espacios** |

## Comandos

| Comando | Sintaxis | Qué hace |
|---|---|---|
| `rem` | `10 rem texto libre` | Comentario. No genera código |
| `input` | `20 input x` | Lee un entero del teclado y lo guarda en la variable |
| `let` | `30 let x = expresión` | Evalúa la expresión y asigna el resultado a la variable |
| `print` | `40 print x` | Muestra el valor de la variable |
| `goto` | `50 goto 20` | Salta al número de línea indicado |
| `if/goto` | `60 if x < y goto 90` | Si la condición se cumple, salta a esa línea |
| `end` | `99 end` | Termina la ejecución |

## Expresiones

Se admiten en el lado derecho de un `let`:

- Operandos: variables de una letra y constantes enteras
- Operadores: `+` `-` `*` `/`
- Paréntesis para agrupar
- Precedencia normal: primero `*` y `/`, después `+` y `-`; los paréntesis mandan

```
40 let r = ( a + b ) * 2 - a / 2
```

> **Nota sobre `^`:** el lenguaje contempla la potencia, pero SML básico no tiene una
> instrucción de exponenciación, así que el compilador la rechaza con un mensaje claro en lugar
> de generar código incorrecto. La potencia sí está disponible en el evaluador de expresiones
> (`expresiones.exe`) y en el simulador mejorado (opcode 35).

## Condiciones

En `if/goto` se admiten los seis operadores relacionales:

| Operador | Significado |
|---|---|
| `<` | menor que |
| `<=` | menor o igual |
| `>` | mayor que |
| `>=` | mayor o igual |
| `==` | igual |
| `!=` | distinto |

Ninguno existe en SML, así que todos se simulan restando los dos operandos y examinando el
signo del resultado. El detalle de cada traducción está en [`diseno.md`](diseno.md).

## Gramática

```
programa    ::= enunciado { enunciado }
enunciado   ::= numlinea comando
numlinea    ::= entero
comando     ::= "rem" texto
              | "input" variable
              | "let" variable "=" expresion
              | "print" variable
              | "goto" numlinea
              | "if" operando relacional operando "goto" numlinea
              | "end"
expresion   ::= termino { ("+" | "-") termino }
termino     ::= factor { ("*" | "/") factor }
factor      ::= operando | "(" expresion ")"
operando    ::= variable | constante
variable    ::= "a" | "b" | ... | "z"
constante   ::= entero sin signo de 0 a 9999
relacional  ::= "<" | "<=" | ">" | ">=" | "==" | "!="
```

## Programa de ejemplo

Suma todos los enteros de 1 hasta el número que se teclee:

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

Con `x = 5` imprime `+0015`.

## Errores que detecta el compilador

| Error | Mensaje |
|---|---|
| Variable en mayúscula o de más de una letra | *Las variables deben ser una sola letra minuscula* |
| Comando en mayúsculas | *Los comandos deben escribirse en minusculas* |
| Comando inexistente | *Comando no reconocido* |
| Enunciado sin número de línea | *Todo enunciado debe empezar con un numero de linea* |
| Número de línea repetido | *Numero de linea repetido* |
| Números de línea fuera de orden | *Los numeros de linea deben ir en orden ascendente* |
| Constante de más de 4 dígitos | *Las constantes deben tener como maximo 4 digitos* |
| `goto` a una línea que no existe | *La linea N no existe en el programa* |
| Constante fuera de rango | *Las constantes deben estar entre 0 y 9999* |
| Formato incorrecto de `let`, `if`, `input`, `print`, `goto` | *Formato correcto: ...* |
| Programa más grande que la memoria | *La memoria de Simpletron se agoto* |

Los errores que se detectan en la **primera pasada** indican el número de línea del archivo
fuente donde está el problema:

```
*** ERROR en la linea 2 del fuente: Las variables deben ser una sola letra minuscula ***
```

Los que se detectan en la **segunda pasada** no pueden indicarlo, porque para entonces el
archivo ya se leyó completo y el problema no pertenece a una línea concreta sino al programa en
su conjunto:

```
*** ERROR: la linea 500 no existe en el programa ***
```

Es el caso de `goto` a una línea inexistente, que solo se descubre al intentar resolver las
referencias pendientes.
