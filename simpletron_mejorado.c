/* ============================================================================
 *  SIMPLETRON MEJORADO
 *  Seccion 3: simulador con las ocho modificaciones solicitadas.
 *
 *  Materia : Programacion Avanzada
 *  Autor   : Miguel Angel Aragon Lavalle
 *  Fecha   : 22/09/2026
 *
 *  Compilar con:
 *      gcc -Wall -Wextra -std=c11 -g simpletron_mejorado.c -o simpletron_mejorado.exe -lm
 *  Ejecutar con:
 *      .\simpletron_mejorado.exe
 *
 *  MEJORAS IMPLEMENTADAS
 *  ---------------------
 *   1. Carga del programa desde el archivo programa.simp (si no existe, se
 *      conserva la carga interactiva por teclado).
 *   2. Memoria ampliada a 1000 posiciones (000 a 999).
 *   3. Nueva instruccion 34 REMAINDER  (residuo / modulo).
 *   4. Nueva instruccion 35 EXPONENTIATION (potencia).
 *   5. Nueva instruccion 12 NEWLINE (salto de linea).
 *   6. Nueva instruccion 13 READSTRING  (lee una cadena y la guarda en ASCII).
 *   7. Nueva instruccion 14 WRITESTRING (imprime una cadena guardada asi).
 *   8. Soporte de punto flotante: 15 READFLOAT, 16 WRITEFLOAT y 36 DIVIDEFLOAT.
 *
 *  DECISIONES DE DISENO (obligatorio documentarlas, lo pide el enunciado)
 *  ---------------------------------------------------------------------
 *  A) TAMANO DE PALABRA. Con 1000 posiciones la direccion ya no cabe en dos
 *     digitos, asi que la palabra crece de 4 a 5 digitos con signo:
 *
 *              + 1 0   0 0 7
 *                └─┬┘   └─┬─┘
 *                 OP     ADDR
 *              2 digitos 3 digitos
 *
 *     La separacion pasa de /100 y %100 a /1000 y %1000. El rango de una
 *     palabra es -99999 a +99999.
 *
 *  B) CENTINELA. Como +9999 ya es una palabra valida, el centinela de fin de
 *     carga pasa a ser -99999, y el rango aceptado al cargar es -99998 a +99999.
 *
 *  C) PUNTO FLOTANTE. La memoria se declara como arreglo de double en vez de
 *     int. Un entero es simplemente un double sin parte decimal, asi que todos
 *     los programas de la version basica siguen funcionando igual. Las
 *     instrucciones existentes conservan su comportamiento entero (32 DIVIDE
 *     sigue truncando); para trabajar con decimales se usan las instrucciones
 *     nuevas 15, 16 y 36. Las instrucciones solo pueden ser numeros enteros:
 *     intentar ejecutar una palabra con decimales es un error fatal.
 *
 *  D) CADENAS. Se guardan a partir de una direccion base asi:
 *
 *        memory[base]       = longitud de la cadena (cuantos caracteres)
 *        memory[base + 1]   = codigo ASCII del caracter 1 (3 digitos)
 *        memory[base + 2]   = codigo ASCII del caracter 2
 *        ...
 *
 *     Es la interpretacion del enunciado: "la primera mitad de la palabra de
 *     esa posicion sera una cuenta del numero de caracteres" y "cada media
 *     palabra subsecuente contendra un caracter ASCII expresado como tres
 *     digitos decimales".
 * ==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* strlen, para medir la cadena leida               */
#include <math.h>       /* pow (potencia) y fmod (residuo con decimales)    */

#ifdef _WIN32
#include <windows.h>    /* SetConsoleOutputCP: para que se vean las tildes  */
#include <io.h>         /* _isatty, _fileno: para saber si hay teclado      */
#endif


/* ----------------------------------------------------------------------------
 * CONSTANTES
 * --------------------------------------------------------------------------*/
#define TAM_MEMORIA        1000        /* MEJORA 2: antes eran 100           */

#define PALABRA_MIN     -99999.0       /* Palabra de 5 digitos con signo     */
#define PALABRA_MAX      99999.0
#define CENTINELA       -99999.0       /* MEJORA: antes era 9999             */
#define CARGA_MIN       -99998.0       /* El -99999 queda como centinela     */

#define ARCHIVO_PROGRAMA "programa.simp"   /* MEJORA 1                       */

#define MAX_LINEA          256         /* Tamano del buffer de lectura       */

/* --- Codigos de operacion --- */
/* Entrada y salida */
#define READ         10
#define WRITE        11
#define NEWLINE      12   /* MEJORA 5 */
#define READSTRING   13   /* MEJORA 6 */
#define WRITESTRING  14   /* MEJORA 7 */
#define READFLOAT    15   /* MEJORA 8 */
#define WRITEFLOAT   16   /* MEJORA 8 */
/* Carga y almacenamiento */
#define LOAD         20
#define STORE        21
/* Aritmetica */
#define ADD          30
#define SUBTRACT     31
#define DIVIDE       32
#define MULTIPLY     33
#define REMAINDER    34   /* MEJORA 3 */
#define EXPONENT     35   /* MEJORA 4 */
#define DIVIDEFLOAT  36   /* MEJORA 8 */
/* Transferencias de control y fin */
#define BRANCH       40
#define BRANCHNEG    41
#define BRANCHZERO   42
#define HALT         43


/* ----------------------------------------------------------------------------
 * LA MAQUINA
 * Igual que en la version basica, pero la memoria y el acumulador ahora son
 * de tipo double para poder guardar decimales (MEJORA 8).
 * --------------------------------------------------------------------------*/
double memory[TAM_MEMORIA];
double accumulator;
int    instructionCounter;
int    instructionRegister;
int    operationCode;
int    operand;


/* ----------------------------------------------------------------------------
 * PROTOTIPOS
 * --------------------------------------------------------------------------*/
void mostrarBienvenida(void);
int  cargarDesdeArchivo(const char *nombreArchivo);
void cargarDesdeTeclado(void);
void ejecutarPrograma(void);
void volcadoDeMemoria(void);
void errorFatal(const char *mensaje);
void limpiarEntrada(void);
void pausarAntesDeSalir(void);
int  leerPalabra(double *destino);
void leerLinea(char *destino, int tamano);
void verificarRango(int direccion, int cuantas, const char *que);


/* ============================================================================
 *  main
 * ==========================================================================*/
int main(int argc, char *argv[])
{
    /* Por omision se carga programa.simp, tal como pide el enunciado. */
    const char *archivo = ARCHIVO_PROGRAMA;
    int forzarTeclado = 0;

    /* argc es cuantos argumentos recibio el programa (el primero, argv[0], es
     * siempre el nombre del propio ejecutable). argv los contiene como texto.
     * Esto permite elegir la fuente del programa SIN tener que renombrar o
     * borrar archivos:
     *
     *     simpletron_mejorado.exe                   -> carga programa.simp
     *     simpletron_mejorado.exe -t                -> captura por teclado
     *     simpletron_mejorado.exe otro.simp         -> carga ese archivo
     *
     * Sin argumentos el comportamiento es exactamente el que pide el
     * enunciado: archivo si existe, teclado si no. */
    if (argc > 1) {
        if (strcmp(argv[1], "-t") == 0) {
            forzarTeclado = 1;
        } else {
            archivo = argv[1];
        }
    }

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    mostrarBienvenida();

    /* MEJORA 1: primero se intenta el archivo. Si no existe, se pide por
     * teclado igual que antes. La funcion devuelve 1 si logro cargar. */
    if (forzarTeclado || !cargarDesdeArchivo(archivo)) {
        cargarDesdeTeclado();
    }

    printf("*** Comienza la ejecución del programa ***\n\n");

    ejecutarPrograma();
    volcadoDeMemoria();

    pausarAntesDeSalir();
    return 0;
}


/* ============================================================================
 *  pausarAntesDeSalir
 *
 *  Resuelve un problema que no es del programa sino de Windows: cuando un
 *  ejecutable de consola se abre con doble clic desde el Explorador, el
 *  sistema destruye la ventana en cuanto el programa termina, asi que el
 *  vaciado de memoria se alcanza a imprimir pero nadie llega a leerlo.
 *
 *  La pausa solo se activa cuando la entrada viene del teclado. _isatty
 *  ("is a teletype") responde si la entrada estandar es una terminal de
 *  verdad; si el programa se ejecuto con la entrada redirigida desde un
 *  archivo (como en los casos de prueba), devuelve falso y no se pausa nada.
 *  Asi la comodidad no estorba a las pruebas automaticas.
 * ==========================================================================*/
void pausarAntesDeSalir(void)
{
#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
        printf("Presione Enter para cerrar esta ventana...");
        fflush(stdout);   /* fuerza a que el mensaje salga ya, sin esperar */
        getchar();
    }
#endif
}


/* ============================================================================
 *  mostrarBienvenida
 * ==========================================================================*/
void mostrarBienvenida(void)
{
    int i;

    printf("*** Bienvenido a Simpletron (versión mejorada) ***\n");
    printf("*** Memoria de %d posiciones, palabras de 5 dígitos ***\n",
           TAM_MEMORIA);
    printf("*** Para interrumpir en cualquier momento: Ctrl + C ***\n");
    printf("*** Uso: simpletron_mejorado.exe [archivo.simp | -t] ***\n");
    printf("***      sin argumentos carga %s; con -t captura por teclado ***\n\n",
           ARCHIVO_PROGRAMA);

    /* La memoria se deja en ceros ANTES de cualquier carga, para que el
     * vaciado nunca muestre valores indeterminados. */
    for (i = 0; i < TAM_MEMORIA; i++) {
        memory[i] = 0.0;
    }
}


/* ============================================================================
 *  limpiarEntrada / leerPalabra / leerLinea
 *  Mismas utilidades de la version basica. leerPalabra ahora entrega un double.
 * ==========================================================================*/
void limpiarEntrada(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descartar */
    }
}

int leerPalabra(double *destino)
{
    int resultado = scanf("%lf", destino);   /* %lf lee un double */

    if (resultado == EOF) {
        return -1;              /* Ya no hay entrada: no reintentar */
    }
    if (resultado != 1) {
        limpiarEntrada();
        return 0;               /* Basura: se puede reintentar */
    }
    limpiarEntrada();
    return 1;
}

/* Lee una linea completa de texto y le quita el salto de linea final.
 * Se usa para READSTRING (MEJORA 6), porque ahi si queremos los espacios
 * que scanf("%s") descartaria. */
void leerLinea(char *destino, int tamano)
{
    if (fgets(destino, tamano, stdin) == NULL) {
        errorFatal("Se acabó la entrada al leer una cadena");
    }
    destino[strcspn(destino, "\r\n")] = '\0';   /* corta en el primer salto */
}


/* ============================================================================
 *  verificarRango: comprueba que una direccion (y las "cuantas" posiciones
 *  que le siguen) existan dentro de la memoria.
 *
 *  Con 1000 posiciones esta comprobacion ya NO es decorativa: el operando
 *  tiene 3 digitos y puede valer hasta 999, asi que una cadena guardada cerca
 *  del final de la memoria si puede desbordarse.
 * ==========================================================================*/
void verificarRango(int direccion, int cuantas, const char *que)
{
    char mensaje[MAX_LINEA];

    if (direccion < 0 || direccion + cuantas >= TAM_MEMORIA) {
        snprintf(mensaje, sizeof mensaje,
                 "%s: la dirección %03d no cabe en la memoria (000 a %03d)",
                 que, direccion, TAM_MEMORIA - 1);
        errorFatal(mensaje);
    }
}


/* ============================================================================
 *  MEJORA 1 - cargarDesdeArchivo
 *
 *  Devuelve 1 si cargo el programa desde programa.simp, 0 si el archivo no
 *  existe (en ese caso main recurre a la carga por teclado).
 *
 *  Formato del archivo (texto plano, una palabra por linea):
 *      # las lineas que empiezan con # o ; son comentarios
 *      +10007        ; se puede comentar despues del numero
 *      +11007
 *      -99999        ; centinela, opcional al final del archivo
 *
 *  Si una linea no contiene un numero valido, se informa el numero de linea
 *  y el programa termina: es preferible a ejecutar un programa a medio cargar.
 * ==========================================================================*/
int cargarDesdeArchivo(const char *nombreArchivo)
{
    FILE *archivo;
    char linea[MAX_LINEA];
    char mensaje[MAX_LINEA];
    char *p;
    char *fin;
    double valor;
    int numeroLinea = 0;
    int posicion = 0;

    /* fopen devuelve NULL si el archivo no se pudo abrir (no existe, por
     * ejemplo). Eso NO es un error: significa "carga por teclado". */
    archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("*** No se encontró %s: se cargará por teclado ***\n\n",
               nombreArchivo);
        return 0;
    }

    printf("*** Cargando el programa desde %s ***\n", nombreArchivo);

    /* fgets lee una linea completa. Devuelve NULL cuando ya no hay mas. */
    while (fgets(linea, sizeof linea, archivo) != NULL) {
        numeroLinea++;

        /* Saltar los espacios y tabuladores del principio */
        p = linea;
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        /* Linea vacia o comentario: se ignora */
        if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#' || *p == ';') {
            continue;
        }

        /* strtod convierte texto a double y deja en "fin" el primer caracter
         * que NO pudo convertir. Si fin quedo igual que p, no habia ningun
         * numero al principio de la linea. */
        valor = strtod(p, &fin);
        if (fin == p) {
            fclose(archivo);
            snprintf(mensaje, sizeof mensaje,
                     "%s, línea %d: no contiene un número válido",
                     nombreArchivo, numeroLinea);
            errorFatal(mensaje);
        }

        /* Lo que venga despues del numero (por ejemplo "; READ 007") se
         * ignora a proposito: permite comentar cada instruccion. */

        if (valor == CENTINELA) {
            break;
        }

        if (valor < CARGA_MIN || valor > PALABRA_MAX) {
            fclose(archivo);
            snprintf(mensaje, sizeof mensaje,
                     "%s, línea %d: el valor %.2f está fuera del rango %.0f a %.0f",
                     nombreArchivo, numeroLinea, valor, CARGA_MIN, PALABRA_MAX);
            errorFatal(mensaje);
        }

        if (posicion >= TAM_MEMORIA) {
            fclose(archivo);
            snprintf(mensaje, sizeof mensaje,
                     "%s: el programa no cabe en %d posiciones de memoria",
                     nombreArchivo, TAM_MEMORIA);
            errorFatal(mensaje);
        }

        memory[posicion] = valor;
        posicion++;
    }

    fclose(archivo);   /* Siempre hay que cerrar lo que se abre */

    printf("*** Se cargaron %d palabras desde %s ***\n",
           posicion, nombreArchivo);
    return 1;
}


/* ============================================================================
 *  cargarDesdeTeclado: la carga interactiva de siempre, adaptada al nuevo
 *  tamano de palabra y de memoria.
 * ==========================================================================*/
void cargarDesdeTeclado(void)
{
    int posicion = 0;
    double palabra;
    int lectura;

    printf("*** Escriba una palabra por posición. ***\n");
    printf("*** Escriba el centinela %.0f para terminar la carga. ***\n\n",
           CENTINELA);

    while (posicion < TAM_MEMORIA) {

        /* %03d: tres digitos ahora que hay 1000 posiciones (000 a 999) */
        printf("%03d ? ", posicion);

        lectura = leerPalabra(&palabra);

        if (lectura == -1) {
            printf("\n*** Se acabó la entrada: se detiene la carga. ***\n");
            break;
        }
        if (lectura == 0) {
            printf("*** Entrada inválida: escriba un número. ***\n");
            continue;
        }
        if (palabra == CENTINELA) {
            break;
        }
        if (palabra < CARGA_MIN || palabra > PALABRA_MAX) {
            printf("*** Valor fuera de rango: debe estar entre %.0f y %.0f. ***\n",
                   CARGA_MIN, PALABRA_MAX);
            continue;
        }

        memory[posicion] = palabra;
        posicion++;
    }

    printf("*** Se terminó de cargar el programa ***\n");
}


/* ============================================================================
 *  ejecutarPrograma: el ciclo de ejecucion, ahora con 19 instrucciones.
 * ==========================================================================*/
void ejecutarPrograma(void)
{
    int continuar = 1;
    int transferenciaDeControl;
    int lectura;
    double valorLeido;
    double palabraActual;
    double divisor;
    double resultado;
    char texto[MAX_LINEA];
    int longitud;
    int i;

    accumulator         = 0.0;
    instructionCounter  = 0;
    instructionRegister = 0;
    operationCode       = 0;
    operand             = 0;

    while (continuar) {

        transferenciaDeControl = 0;

        if (instructionCounter < 0 || instructionCounter >= TAM_MEMORIA) {
            errorFatal("El contador de instrucciones salió de la memoria");
        }

        /* ---------- BUSCAR ---------- */
        palabraActual = memory[instructionCounter];

        /* Una instruccion tiene que ser un numero entero. Si en esta posicion
         * hay un dato con decimales, es que el programa se salio del area de
         * codigo: error fatal. (Comprobacion nueva, nace de la MEJORA 8.) */
        if (palabraActual != (double)(long)palabraActual) {
            errorFatal("Se intentó ejecutar un valor con decimales como instrucción");
        }
        instructionRegister = (int) palabraActual;

        /* ---------- DECODIFICAR ----------
         * Antes era /100 y %100 porque la direccion tenia 2 digitos.
         * Ahora la direccion tiene 3, asi que es /1000 y %1000. */
        operationCode = instructionRegister / 1000;
        operand       = instructionRegister % 1000;

        /* ---------- EJECUTAR ---------- */
        switch (operationCode) {

        /* ================= ENTRADA / SALIDA ================= */

        case READ:          /* 10: lee un entero */
            while (1) {
                printf("? ");
                lectura = leerPalabra(&valorLeido);
                if (lectura == -1) {
                    errorFatal("Se acabó la entrada durante una instrucción READ");
                }
                if (lectura == 0) {
                    printf("*** Entrada inválida: escriba un número entero. ***\n");
                    continue;
                }
                if (valorLeido != (double)(long)valorLeido) {
                    printf("*** READ solo acepta enteros; use 15 (READFLOAT) para decimales. ***\n");
                    continue;
                }
                if (valorLeido < PALABRA_MIN || valorLeido > PALABRA_MAX) {
                    printf("*** Valor fuera de rango: entre %.0f y %.0f. ***\n",
                           PALABRA_MIN, PALABRA_MAX);
                    continue;
                }
                break;
            }
            verificarRango(operand, 0, "READ");
            memory[operand] = valorLeido;
            break;

        case WRITE:         /* 11: imprime un entero */
            verificarRango(operand, 0, "WRITE");
            printf("%+06.0f\n", memory[operand]);
            break;

        /* ---- MEJORA 5: salto de linea ---- */
        case NEWLINE:       /* 12 */
            /* El operando no se usa. Sirve para dar formato a la salida:
             * varias instrucciones WRITE seguidas con NEWLINE entre ellas
             * producen texto en renglones separados. */
            printf("\n");
            break;

        /* ---- MEJORA 6: leer una cadena ---- */
        case READSTRING:    /* 13 */
            /* El mensaje dice explicitamente que se espera texto y que hay que
             * presionar Enter. Un simple "?" hacia parecer que el programa se
             * habia quedado colgado. */
            printf("Escriba un texto y presione Enter: ");
            leerLinea(texto, sizeof texto);
            longitud = (int) strlen(texto);

            /* Se necesitan: 1 posicion para la longitud + una por caracter */
            verificarRango(operand, longitud, "READSTRING");

            memory[operand] = (double) longitud;
            for (i = 0; i < longitud; i++) {
                /* (unsigned char) evita que un byte mayor a 127 se vuelva
                 * negativo al convertirse a int. */
                memory[operand + 1 + i] = (double)(unsigned char) texto[i];
            }
            break;

        /* ---- MEJORA 7: imprimir una cadena ---- */
        case WRITESTRING:   /* 14 */
            verificarRango(operand, 0, "WRITESTRING");
            longitud = (int) memory[operand];

            if (longitud < 0) {
                errorFatal("WRITESTRING: la longitud almacenada es negativa");
            }
            verificarRango(operand, longitud, "WRITESTRING");

            for (i = 0; i < longitud; i++) {
                /* putchar convierte el numero ASCII de vuelta a caracter */
                putchar((int) memory[operand + 1 + i]);
            }
            break;

        /* ---- MEJORA 8: entrada y salida con decimales ---- */
        case READFLOAT:     /* 15 */
            while (1) {
                printf("? ");
                lectura = leerPalabra(&valorLeido);
                if (lectura == -1) {
                    errorFatal("Se acabó la entrada durante una instrucción READFLOAT");
                }
                if (lectura == 0) {
                    printf("*** Entrada inválida: escriba un número. ***\n");
                    continue;
                }
                if (valorLeido < PALABRA_MIN || valorLeido > PALABRA_MAX) {
                    printf("*** Valor fuera de rango: entre %.0f y %.0f. ***\n",
                           PALABRA_MIN, PALABRA_MAX);
                    continue;
                }
                break;
            }
            verificarRango(operand, 0, "READFLOAT");
            memory[operand] = valorLeido;
            break;

        case WRITEFLOAT:    /* 16 */
            verificarRango(operand, 0, "WRITEFLOAT");
            printf("%+.4f\n", memory[operand]);
            break;

        /* ================= CARGA Y ALMACENAMIENTO ================= */

        case LOAD:          /* 20 */
            verificarRango(operand, 0, "LOAD");
            accumulator = memory[operand];
            break;

        case STORE:         /* 21 */
            verificarRango(operand, 0, "STORE");
            memory[operand] = accumulator;
            break;

        /* ================= ARITMETICA ================= */

        case ADD:           /* 30 */
            verificarRango(operand, 0, "ADD");
            accumulator += memory[operand];
            if (accumulator > PALABRA_MAX || accumulator < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una suma");
            }
            break;

        case SUBTRACT:      /* 31 */
            verificarRango(operand, 0, "SUBTRACT");
            accumulator -= memory[operand];
            if (accumulator > PALABRA_MAX || accumulator < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una resta");
            }
            break;

        case DIVIDE:        /* 32: division ENTERA, como en la version basica */
            verificarRango(operand, 0, "DIVIDE");
            divisor = memory[operand];
            if (divisor == 0.0) {
                errorFatal("Intento de dividir entre cero");
            }
            /* (long) trunca hacia cero: 55/7 da 7, igual que antes. */
            accumulator = (double)((long) accumulator / (long) divisor);
            break;

        case MULTIPLY:      /* 33 */
            verificarRango(operand, 0, "MULTIPLY");
            accumulator *= memory[operand];
            if (accumulator > PALABRA_MAX || accumulator < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una multiplicación");
            }
            break;

        /* ---- MEJORA 3: residuo (modulo) ---- */
        case REMAINDER:     /* 34 */
            verificarRango(operand, 0, "REMAINDER");
            divisor = memory[operand];
            if (divisor == 0.0) {
                errorFatal("Intento de calcular el residuo entre cero");
            }
            /* fmod es el equivalente del operador % pero para double.
             * No se puede usar % porque en C solo funciona con enteros. */
            accumulator = fmod(accumulator, divisor);
            break;

        /* ---- MEJORA 4: exponenciacion ---- */
        case EXPONENT:      /* 35 */
            verificarRango(operand, 0, "EXPONENT");
            resultado = pow(accumulator, memory[operand]);

            /* pow puede producir resultados que no son numeros representables:
             * 0 elevado a un exponente negativo es una division entre cero, y
             * una base negativa con exponente fraccionario no tiene resultado
             * real. En ambos casos pow devuelve infinito o NaN.
             * El truco (r != r) detecta NaN: es el unico valor que no es
             * igual a si mismo. */
            if (resultado != resultado) {
                errorFatal("Exponenciación con resultado indefinido");
            }
            if (resultado > PALABRA_MAX || resultado < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una exponenciación");
            }
            accumulator = resultado;
            break;

        /* ---- MEJORA 8: division real ---- */
        case DIVIDEFLOAT:   /* 36 */
            verificarRango(operand, 0, "DIVIDEFLOAT");
            divisor = memory[operand];
            if (divisor == 0.0) {
                errorFatal("Intento de dividir entre cero");
            }
            /* A diferencia de 32, esta NO trunca: 55/7 da 7.8571 */
            accumulator = accumulator / divisor;
            break;

        /* ================= TRANSFERENCIAS DE CONTROL ================= */

        case BRANCH:        /* 40 */
            instructionCounter = operand;
            transferenciaDeControl = 1;
            break;

        case BRANCHNEG:     /* 41 */
            if (accumulator < 0.0) {
                instructionCounter = operand;
                transferenciaDeControl = 1;
            }
            break;

        case BRANCHZERO:    /* 42 */
            if (accumulator == 0.0) {
                instructionCounter = operand;
                transferenciaDeControl = 1;
            }
            break;

        /* ================= FIN ================= */

        case HALT:          /* 43 */
            printf("*** Terminó la ejecución de Simpletron ***\n");
            continuar = 0;
            break;

        default:
            errorFatal("Código de operación no válido");
            break;
        }

        if (!transferenciaDeControl && continuar) {
            instructionCounter++;
        }
    }
}


/* ============================================================================
 *  errorFatal
 * ==========================================================================*/
void errorFatal(const char *mensaje)
{
    printf("\n*** ERROR FATAL: %s ***\n", mensaje);
    printf("*** La ejecución de Simpletron termina de forma anormal ***\n");
    volcadoDeMemoria();
    pausarAntesDeSalir();
    exit(1);
}


/* ============================================================================
 *  volcadoDeMemoria
 *
 *  Con 1000 posiciones el vaciado son 100 renglones de 10 palabras.
 *
 *  Detalle nuevo: como la memoria ahora es de tipo double, el ancho de cada
 *  celda se decide sobre la marcha. Si TODA la memoria contiene enteros, se
 *  imprime compacto (+00000). Si hay aunque sea un decimal, se imprime con
 *  dos decimales (+00000.00). Asi las columnas siempre quedan alineadas y no
 *  se desperdicia ancho cuando no hay flotantes.
 * ==========================================================================*/
void volcadoDeMemoria(void)
{
    int fila;
    int col;
    int i;
    int hayDecimales = 0;

    for (i = 0; i < TAM_MEMORIA; i++) {
        if (memory[i] != (double)(long) memory[i]) {
            hayDecimales = 1;
            break;
        }
    }

    printf("\nREGISTROS:\n");
    if (hayDecimales) {
        printf("%-21s %+010.2f\n", "accumulator", accumulator);
    } else {
        printf("%-21s %+06.0f\n", "accumulator", accumulator);
    }
    printf("%-21s %03d\n",  "instructionCounter", instructionCounter);
    printf("%-21s %+06d\n", "instructionRegister", instructionRegister);
    printf("%-21s %02d\n",  "operationCode", operationCode);
    printf("%-21s %03d\n",  "operand", operand);

    printf("\nMEMORIA:\n");

    printf("%4s", "");
    for (col = 0; col < 10; col++) {
        printf(hayDecimales ? "%11d" : "%7d", col);
    }
    printf("\n");

    for (fila = 0; fila < TAM_MEMORIA; fila += 10) {
        printf("%4d", fila);
        for (col = 0; col < 10; col++) {
            if (hayDecimales) {
                printf(" %+010.2f", memory[fila + col]);
            } else {
                printf(" %+06.0f", memory[fila + col]);
            }
        }
        printf("\n");
    }
    printf("\n");
}
