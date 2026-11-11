# Evidencias

Salidas reales capturadas al ejecutar cada herramienta del proyecto, y el registro del trabajo
con ramas.

## `git-branch/`

| Archivo | Contenido |
|---|---|
| `bitacora-git.md` | Las actividades del laboratorio de ramas, explicadas con los comandos y sus salidas |
| `captura-1-ramas.png` | Captura de `git status`, `git branch -v` y `git branch -r` |
| `captura-2-cambio-de-rama.png` | Captura del cambio entre ramas con `git checkout` y el commit de cada una |
| `captura-3-historial.png` | Captura de `git log --graph --oneline --all --decorate` |
| `comandos-git.txt` | Transcripción de la sesión de Git: los comandos y sus salidas |
| `historial-ramas.txt` | Grafo del historial (`git log --graph`) y lista de ramas (`git branch -v`) |

## `ejecuciones/`

### Compilación de Simple a SML

| Archivo | Contenido |
|---|---|
| `compilacion-01-sumatoria.txt` | Tabla de símbolos, segunda pasada y código SML del ejemplo del capítulo |
| `compilacion-02-mayor-de-dos.txt` ... `compilacion-06-factorial.txt` | Lo mismo para los demás programas |

El primero es el ejemplo del libro: el código generado y la tabla de símbolos coinciden con la
figura 6.2 y la figura 6.3.

### Ejecución del código compilado

| Archivo | Contenido |
|---|---|
| `ejecucion-01-sumatoria.txt` ... `ejecucion-06-factorial.txt` | Salida del simulador ejecutando cada programa compilado, con su vaciado de memoria completo |

### Errores de compilación

| Archivo | Error que demuestra |
|---|---|
| `error-e01-mayuscula.txt` | Variable en mayúscula |
| `error-e02-linea-inexistente.txt` | `goto` a una línea que no existe (se detecta en la **segunda** pasada) |
| `error-e03-comando-invalido.txt` | Comando no reconocido |

### Simuladores

| Archivo | Contenido |
|---|---|
| `simulador-01-suma.txt` | Simulador base ejecutando un programa SML tecleado |
| `simulador-carga-archivo.txt` | Simulador base cargando el programa desde un archivo `.sml` |
| `simulador-error-fatal.txt` | Error fatal de división entre cero, con el vaciado completo |
| `simulador-direccion-invalida.txt` | Error fatal por salir de la memoria, con el vaciado completo |
| `simulador-mejorado-residuo.txt` | Simulador mejorado cargando un `.simp` y usando la instrucción 34 |

## Cómo se generaron

Todas las salidas se capturaron redirigiendo la salida estándar de los programas, sin editarlas:

```bash
.\simpletron.exe programas\sml\compilados\01-sumatoria.sml < pruebas\compilador\entradas\01-sumatoria.txt > evidencias\ejecuciones\ejecucion-01-sumatoria.txt
```

Las pruebas por componente, con su resultado esperado y el obtenido, están en `pruebas/`.
