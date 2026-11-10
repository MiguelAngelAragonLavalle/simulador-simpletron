/* ============================================================================
 *  SIMPLETRON - Simulador de la computadora Simpletron
 *  Seccion 2: version base, sin las mejoras de la Seccion 3.
 *
 *  Materia : Programacion Avanzada
 *  Autor   : Miguel Angel Aragon Lavalle
 *  Fecha   : 22/09/2026
 *
 *  Compilar con:
 *      gcc -Wall -Wextra -std=c11 -g simpletron.c -o simpletron.exe
 *  Ejecutar con:
 *      .\simpletron.exe
 *
 *  Basado en: Deitel H.M. y Deitel P.J. (1998). Como programar en C,
 *  1ra Edicion. Mexico. Prentice Hall Latinoamerica.
 * ==========================================================================*/

/* ----------------------------------------------------------------------------
 * ZONA 1: BIBLIOTECAS
 * --------------------------------------------------------------------------*/
#include <stdio.h>      /* printf, scanf, getchar, fflush                     */
#include <stdlib.h>     /* exit (terminar el programa desde cualquier punto)  */

/* Solo en Windows: cambiar la consola a UTF-8 y detectar si hay teclado. */
#ifdef _WIN32
#include <windows.h>    /* SetConsoleOutputCP: para que las tildes se vean    */
#include <io.h>         /* _isatty, _fileno: para saber si hay teclado        */
#endif


/* ----------------------------------------------------------------------------
 * ZONA 2: CONSTANTES
 *
 * Los limites del simulador se definen aqui para no repetir numeros sueltos
 * en el codigo. Si cambia el tamano de la memoria solo se modifica
 * TAM_MEMORIA.
 * --------------------------------------------------------------------------*/
#define TAM_MEMORIA        100   /* La Simpletron tiene 100 palabras: 00 a 99 */

#define PALABRA_MIN      -9999   /* Palabra Simpletron mas pequena posible    */
#define PALABRA_MAX      +9999   /* Palabra Simpletron mas grande posible     */

#define CENTINELA         9999   /* Al teclearlo, termina la fase de carga    */
#define CARGA_MAX         9998   /* Por eso al CARGAR el tope es +9998:       */
                                 /* el 9999 esta reservado como centinela     */

/* Los doce codigos de operacion del SML. */
#define READ       10   /* Lee un valor del teclado y lo guarda en memoria    */
#define WRITE      11   /* Imprime en pantalla una palabra de memoria         */
#define LOAD       20   /* Copia una palabra de memoria al acumulador         */
#define STORE      21   /* Copia el acumulador a una palabra de memoria       */
#define ADD        30   /* Suma al acumulador                                 */
#define SUBTRACT   31   /* Resta al acumulador                                */
#define DIVIDE     32   /* Divide el acumulador                               */
#define MULTIPLY   33   /* Multiplica el acumulador                           */
#define BRANCH     40   /* Salto incondicional                                */
#define BRANCHNEG  41   /* Salta solo si el acumulador es negativo            */
#define BRANCHZERO 42   /* Salta solo si el acumulador es cero                */
#define HALT       43   /* Detiene la ejecucion                               */


/* ----------------------------------------------------------------------------
 * ZONA 3: LA MAQUINA (variables globales)
 *
 * La memoria y los registros de la Simpletron. Son globales porque
 * representan el hardware de la maquina y casi todas las funciones los usan.
 * Los nombres estan en ingles porque asi los pide el enunciado.
 * --------------------------------------------------------------------------*/

int memory[TAM_MEMORIA];   /* Memoria: 100 palabras, posiciones 00..99.
                            * Guarda tanto instrucciones como datos. */

int accumulator;           /* Acumulador: aqui queda el resultado de las
                            * operaciones aritmeticas. */

int instructionCounter;    /* Contador de instrucciones: direccion de la
                            * siguiente instruccion por ejecutar. */

int instructionRegister;   /* Registro de instruccion: la palabra que se
                            * esta ejecutando. Ej: +1009 */

int operationCode;         /* Dos digitos izquierdos: la operacion.
                            * De +1009 -> 10 (READ) */

int operand;               /* Dos digitos derechos: la direccion de memoria.
                            * De +1009 -> 09 */


/* ----------------------------------------------------------------------------
 * ZONA 4: PROTOTIPOS
 * --------------------------------------------------------------------------*/
void mostrarBienvenida(void);
void mostrarInstruccionesDeCaptura(void);
void cargarPrograma(void);
int  cargarDesdeArchivo(char nombreArchivo[]);
void ejecutarPrograma(void);
void volcadoDeMemoria(void);
void errorFatal(const char *mensaje);
void limpiarEntrada(void);
void pausarAntesDeSalir(void);
int  leerPalabra(int *destino);


/* ============================================================================
 *  main: carga el programa SML, lo ejecuta y muestra el vaciado de memoria.
 * ==========================================================================*/
int main(int argc, char *argv[])
{
#ifdef _WIN32
    /* Consola en UTF-8 (codigo 65001) para que se vean las tildes y la ene. */
    SetConsoleOutputCP(65001);
#endif

    mostrarBienvenida();   /* 1. Saludar y explicar como capturar            */

    /* Si se indica un archivo en la linea de comandos, el programa SML se
     * carga desde ahi. Sin argumentos se captura por teclado, tal como lo
     * pide el enunciado de la Seccion 2. La carga desde archivo es la que
     * permite ejecutar directamente lo que produce el compilador de Simple. */
    if (argc > 1) {
        if (!cargarDesdeArchivo(argv[1])) {
            return 1;
        }
    } else {
        cargarPrograma();  /* 2. FASE DE CARGA: llenar memory[] con el SML   */
    }

    ejecutarPrograma();    /* 3. FASE DE EJECUCION: correr ese programa SML  */
    volcadoDeMemoria();    /* 4. Mostrar registros y memoria al terminar     */

    pausarAntesDeSalir();  /* 5. Esperar antes de cerrar la ventana          */

    return 0;
}


/* ============================================================================
 *  cargarDesdeArchivo: llena la memoria con un archivo de codigo SML.
 *
 *  Formato: una palabra por renglon. Las lineas que empiezan con ; o # son
 *  comentarios, y lo que se escriba despues del numero tambien se ignora,
 *  de modo que el archivo puede venir documentado:
 *
 *      +1099   ; 00  input x
 *      +4300   ; 01  end
 *
 *  Devuelve 1 si cargo bien, 0 si hubo algun problema.
 * ==========================================================================*/
int cargarDesdeArchivo(char nombreArchivo[])
{
    FILE *archivo;
    char linea[256];
    char *p;
    char *fin;
    long valor;
    int numeroLinea = 0;
    int posicion = 0;
    int i;

    for (i = 0; i < TAM_MEMORIA; i++) {
        memory[i] = 0;
    }

    archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("*** ERROR: no se encontró el archivo %s ***\n", nombreArchivo);
        return 0;
    }

    printf("*** Cargando el programa desde %s ***\n", nombreArchivo);

    while (fgets(linea, sizeof linea, archivo) != NULL) {
        numeroLinea++;

        p = linea;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#' || *p == ';') {
            continue;
        }

        valor = strtol(p, &fin, 10);
        if (fin == p) {
            printf("*** ERROR: %s, línea %d: no contiene un número válido ***\n",
                   nombreArchivo, numeroLinea);
            fclose(archivo);
            return 0;
        }
        if (valor < PALABRA_MIN || valor > PALABRA_MAX) {
            printf("*** ERROR: %s, línea %d: el valor %ld está fuera de rango ***\n",
                   nombreArchivo, numeroLinea, valor);
            fclose(archivo);
            return 0;
        }
        if (posicion >= TAM_MEMORIA) {
            printf("*** ERROR: %s: el programa no cabe en %d posiciones ***\n",
                   nombreArchivo, TAM_MEMORIA);
            fclose(archivo);
            return 0;
        }

        memory[posicion] = (int) valor;
        posicion++;
    }

    fclose(archivo);
    printf("*** Se cargaron %d palabras desde %s ***\n", posicion, nombreArchivo);
    printf("*** Comienza la ejecución del programa ***\n\n");
    return 1;
}


/* ============================================================================
 *  pausarAntesDeSalir: espera un Enter antes de terminar.
 *
 *  Si el programa se abre con doble clic desde el Explorador, Windows cierra
 *  la ventana en cuanto termina y no se alcanza a leer el vaciado de memoria.
 *
 *  La pausa solo se hace cuando la entrada viene del teclado (_isatty). Si la
 *  entrada esta redirigida desde un archivo, como en los casos de prueba, no
 *  se pausa.
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
 *  mostrarBienvenida: la pantalla inicial que pide el enunciado.
 * ==========================================================================*/
void mostrarBienvenida(void)
{
    printf("*** Bienvenido a Simpletron! ***\n\n");
}


/* ============================================================================
 *  mostrarInstruccionesDeCaptura: solo tiene sentido cuando el programa se va
 *  a teclear. Si se carga desde un archivo, explicar el centinela confundiria.
 * ==========================================================================*/
void mostrarInstruccionesDeCaptura(void)
{
    printf("*** Por favor escriba su programa una instrucción ***\n");
    printf("*** (o palabra de datos) a la vez. Yo mostraré    ***\n");
    printf("*** el número de posición y un signo de           ***\n");
    printf("*** interrogación (?). Después usted escribe la   ***\n");
    printf("*** palabra para esa posición. Escriba el         ***\n");
    printf("*** centinela 9999 para terminar de introducir    ***\n");
    printf("*** su programa. ***\n\n");
}


/* ============================================================================
 *  limpiarEntrada: descarta lo que quede en la linea de entrada.
 *
 *  Si scanf falla (por ejemplo, se escribio "hola" en vez de un numero), el
 *  texto se queda en el buffer y la siguiente lectura volveria a fallar.
 *  Se leen y descartan caracteres hasta el fin de linea o hasta EOF
 *  (Ctrl+Z en Windows).
 * ==========================================================================*/
void limpiarEntrada(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descartar */
    }
}


/* ============================================================================
 *  leerPalabra: pide un entero al usuario de forma segura.
 *
 *  Devuelve:
 *     1  -> se leyo un numero correctamente (queda en *destino)
 *     0  -> el usuario escribio algo que no es un numero (se puede reintentar)
 *    -1  -> ya no hay mas entrada (EOF): reintentar seria un ciclo infinito
 *
 *  Separar el caso 0 del caso -1 permite que quien la llama vuelva a pedir el
 *  dato cuando es invalido, pero se detenga si la entrada ya se acabo.
 * ==========================================================================*/
int leerPalabra(int *destino)
{
    /* scanf devuelve 1 si leyo el numero y EOF si ya no hay entrada. */
    int resultado = scanf("%d", destino);

    if (resultado == EOF) {
        return -1;          /* Ya no hay nada mas que leer */
    }
    if (resultado != 1) {
        limpiarEntrada();   /* Vaciar la basura para no ciclarnos */
        return 0;
    }
    limpiarEntrada();       /* Tirar el Enter y lo que sobre de la linea */
    return 1;
}


/* ============================================================================
 *  cargarPrograma: FASE DE CARGA.
 *
 *  Va llenando memory[00], memory[01], ... con lo que teclea el usuario,
 *  hasta que escriba el centinela 9999 o hasta llenar la memoria.
 *
 *  El enunciado pide dos validaciones:
 *    - que lo tecleado sea un numero,
 *    - que ese numero este entre -9999 y +9998,
 *  y que si falla, se SIGA PIDIENDO la misma posicion hasta que sea valida.
 * ==========================================================================*/
void cargarPrograma(void)
{
    int posicion = 0;   /* Posicion de memoria que se esta llenando */
    int palabra;        /* Valor tecleado por el usuario */
    int lectura;        /* 1 = ok, 0 = basura, -1 = se acabo la entrada */

    mostrarInstruccionesDeCaptura();

    /* Toda la memoria empieza en ceros antes de la carga. */
    for (int i = 0; i < TAM_MEMORIA; i++) {
        memory[i] = 0;
    }

    while (posicion < TAM_MEMORIA) {

        /* Posicion con dos digitos, como en el enunciado: "00 ? ", "01 ? " */
        printf("%02d ? ", posicion);

        lectura = leerPalabra(&palabra);

        /* -- Se acabo la entrada sin centinela -- */
        if (lectura == -1) {
            printf("\n*** Se acabó la entrada: se detiene la carga. ***\n");
            break;
        }

        /* -- Validacion 1: que sea un numero -- */
        if (lectura == 0) {
            printf("*** Entrada inválida: escriba un número entero. ***\n");
            continue;   /* No se avanza: se vuelve a pedir la misma posicion */
        }

        /* -- Fin de la carga -- */
        if (palabra == CENTINELA) {
            break;
        }

        /* -- Validacion 2: que este en el rango permitido -- */
        if (palabra < PALABRA_MIN || palabra > CARGA_MAX) {
            printf("*** Valor fuera de rango: debe estar entre %d y +%d. ***\n",
                   PALABRA_MIN, CARGA_MAX);
            continue;
        }

        /* Valor valido: se guarda y se pasa a la siguiente posicion. */
        memory[posicion] = palabra;
        posicion++;
    }

    /* Si salimos porque se lleno la memoria y no por el centinela, avisamos. */
    if (posicion >= TAM_MEMORIA) {
        printf("*** Memoria llena (%d posiciones). Se inicia la ejecución. ***\n",
               TAM_MEMORIA);
    }

    printf("*** Se terminó de cargar el programa ***\n");
    printf("*** Comienza la ejecución del programa ***\n\n");
}


/* ============================================================================
 *  ejecutarPrograma: FASE DE EJECUCION.
 *
 *  Repite el ciclo de ejecucion de instruccion hasta encontrar un HALT o un
 *  error fatal:
 *
 *      1. BUSCAR      (fetch)  : traer de memoria la instruccion que toca
 *      2. DECODIFICAR (decode) : separarla en codigo de operacion y operando
 *      3. EJECUTAR    (execute): realizar la operacion
 * ==========================================================================*/
void ejecutarPrograma(void)
{
    int continuar = 1;            /* Bandera: 1 = sigue ejecutando, 0 = HALT */
    int transferenciaDeControl;   /* 1 si la instruccion fue un salto        */
    int valorLeido;               /* Auxiliar para la instruccion READ       */
    int lectura;                  /* Resultado de leerPalabra en READ        */

    /* El enunciado pide inicializar TODOS los registros especiales en 0. */
    accumulator         = 0;
    instructionCounter  = 0;   /* La ejecucion siempre empieza en la pos. 00 */
    instructionRegister = 0;
    operationCode       = 0;
    operand             = 0;

    while (continuar) {

        transferenciaDeControl = 0;   /* Suponemos que NO habra salto */

        /* ---------- PASO 0: verificar el contador ----------
         * El contador debe apuntar a una direccion dentro de la memoria. */
        if (instructionCounter < 0 || instructionCounter >= TAM_MEMORIA) {
            errorFatal("El contador de instrucciones salió de la memoria");
        }

        /* ---------- PASO 1: BUSCAR ----------
         * Traer de memoria la palabra que esta en la direccion que marca el
         * contador de instrucciones. */
        instructionRegister = memory[instructionCounter];

        /* ---------- PASO 2: DECODIFICAR ----------
         * Partir la palabra de 4 digitos en sus dos mitades.
         *
         * Con +1009 como ejemplo:
         *   1009 / 100  = 10   <- codigo de operacion (dos digitos izquierdos)
         *   1009 % 100  = 09   <- operando (dos digitos derechos)
         */
        operationCode = instructionRegister / 100;
        operand       = instructionRegister % 100;

        /* ---------- PASO 3: EJECUTAR ----------
         * Cada case corresponde a una instruccion del SML. */
        switch (operationCode) {

        /* ---- OPERACIONES DE ENTRADA / SALIDA ---- */

        case READ:   /* 10: leer del teclado hacia memory[operand] */
            /* Igual que en la fase de carga, el dato se vuelve a pedir hasta
             * que sea un entero dentro del rango de una palabra. */
            while (1) {
                printf("? ");
                lectura = leerPalabra(&valorLeido);

                if (lectura == -1) {
                    /* Sin datos no se puede continuar: error fatal. */
                    errorFatal("Se acabó la entrada durante una instrucción READ");
                }
                if (lectura == 0) {
                    printf("*** Entrada inválida: escriba un número entero. ***\n");
                    continue;
                }
                if (valorLeido < PALABRA_MIN || valorLeido > PALABRA_MAX) {
                    printf("*** Valor fuera de rango: debe estar entre %d y %+d. ***\n",
                           PALABRA_MIN, PALABRA_MAX);
                    continue;
                }
                break;   /* Dato valido: sale del while */
            }
            memory[operand] = valorLeido;
            break;

        case WRITE:  /* 11: imprimir memory[operand] en pantalla */
            /* Formato de palabra Simpletron: signo y 4 digitos (+0004). */
            printf("%+05d\n", memory[operand]);
            break;

        /* ---- OPERACIONES DE CARGA / ALMACENAMIENTO ---- */

        case LOAD:   /* 20: acumulador <- memory[operand] */
            accumulator = memory[operand];
            break;

        case STORE:  /* 21: memory[operand] <- acumulador */
            memory[operand] = accumulator;
            break;

        /* ---- OPERACIONES ARITMETICAS ----
         * Todas dejan el resultado en el acumulador y todas pueden
         * desbordarlo, asi que todas revisan el rango al terminar. */

        case ADD:    /* 30 */
            accumulator = accumulator + memory[operand];
            if (accumulator > PALABRA_MAX || accumulator < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una suma");
            }
            break;

        case SUBTRACT:  /* 31 */
            accumulator = accumulator - memory[operand];
            if (accumulator > PALABRA_MAX || accumulator < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una resta");
            }
            break;

        case DIVIDE:    /* 32 */
            /* El divisor se revisa antes de hacer la division entera. */
            if (memory[operand] == 0) {
                errorFatal("Intento de dividir entre cero");
            }
            accumulator = accumulator / memory[operand];
            break;

        case MULTIPLY:  /* 33 */
            accumulator = accumulator * memory[operand];
            if (accumulator > PALABRA_MAX || accumulator < PALABRA_MIN) {
                errorFatal("Desbordamiento del acumulador en una multiplicación");
            }
            break;

        /* ---- TRANSFERENCIAS DE CONTROL (los "saltos") ----
         * Un salto escribe la direccion destino en el contador de
         * instrucciones; la siguiente vuelta del while busca la instruccion
         * en esa direccion. */

        case BRANCH:     /* 40: salta siempre */
            instructionCounter = operand;
            transferenciaDeControl = 1;
            break;

        case BRANCHNEG:  /* 41: salta solo si el acumulador es negativo */
            if (accumulator < 0) {
                instructionCounter = operand;
                transferenciaDeControl = 1;
            }
            break;

        case BRANCHZERO: /* 42: salta solo si el acumulador es cero */
            if (accumulator == 0) {
                instructionCounter = operand;
                transferenciaDeControl = 1;
            }
            break;

        /* ---- FIN ---- */

        case HALT:   /* 43: detener la maquina */
            printf("*** Terminó la ejecución de Simpletron ***\n");
            continuar = 0;   /* Termina el ciclo de ejecucion */
            break;

        /* ---- CUALQUIER OTRA COSA ----
         * El codigo de operacion no es ninguno de los doce validos. Suele
         * pasar cuando el contador se sale del area de programa y llega a
         * un dato. */
        default:
            errorFatal("Código de operación no válido");
            break;
        }

        /* ---------- PASO 4: avanzar ----------
         * Si la instruccion no fue un salto, se pasa a la siguiente
         * direccion. Si fue un salto, el contador ya tiene la direccion
         * destino. */
        if (!transferenciaDeControl && continuar) {
            instructionCounter++;
        }
    }
}


/* ============================================================================
 *  errorFatal: imprime el mensaje de error y un vaciado completo de la
 *  maquina, como pide el enunciado, para ver el estado en que quedo al
 *  momento de la falla. Despues termina el programa.
 * ==========================================================================*/
void errorFatal(const char *mensaje)
{
    printf("\n*** ERROR FATAL: %s ***\n", mensaje);
    printf("*** La ejecución de Simpletron termina de forma anormal ***\n");

    volcadoDeMemoria();
    pausarAntesDeSalir();

    /* Termina el programa con codigo 1 (terminacion anormal). */
    exit(1);
}


/* ============================================================================
 *  volcadoDeMemoria: el "memory dump" de la figura 5.23.
 *
 *  Imprime los cinco registros y las 100 posiciones de memoria. Sirve para
 *  depurar un programa en SML, porque muestra que quedo en cada posicion.
 * ==========================================================================*/
void volcadoDeMemoria(void)
{
    printf("\nREGISTROS:\n");

    /* Etiquetas alineadas a la izquierda en 21 columnas para que los valores
     * queden en columna. */
    printf("%-21s %+05d\n", "accumulator", accumulator);
    printf("%-21s %02d\n",  "instructionCounter", instructionCounter);
    printf("%-21s %+05d\n", "instructionRegister", instructionRegister);
    printf("%-21s %02d\n",  "operationCode", operationCode);
    printf("%-21s %02d\n",  "operand", operand);

    printf("\nMEMORIA:\n");

    /* --- Encabezado: los numeros de columna 0..9 --- */
    printf("%3s", "");                    /* Hueco para la etiqueta de fila */
    for (int col = 0; col < 10; col++) {
        printf("%6d", col);               /* Cada columna mide 6 caracteres */
    }
    printf("\n");

    /* --- Las 10 filas de 10 palabras cada una ---
     * La fila 0 lleva las posiciones 00..09, la fila 10 las 10..19, etc.
     * Por eso el contador avanza de 10 en 10 (fila += 10). */
    for (int fila = 0; fila < TAM_MEMORIA; fila += 10) {
        printf("%3d", fila);              /* Etiqueta: 0, 10, 20, ... 90 */

        for (int col = 0; col < 10; col++) {
            /* fila + col convierte "fila 20, columna 3" en la posicion 23. */
            printf(" %+05d", memory[fila + col]);
        }
        printf("\n");
    }
    printf("\n");
}
