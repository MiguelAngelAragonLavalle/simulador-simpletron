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
 *
 * Un #include le dice al compilador: "pega aqui las declaraciones de estas
 * funciones para que yo pueda usarlas". Sin esto, printf y scanf no existen
 * para el compilador.
 * --------------------------------------------------------------------------*/
#include <stdio.h>      /* printf, scanf, getchar, fflush                     */
#include <stdlib.h>     /* exit (terminar el programa desde cualquier punto)  */

#ifdef _WIN32
#include <windows.h>    /* SetConsoleOutputCP: para que las tildes se vean    */
#include <io.h>         /* _isatty, _fileno: para saber si hay teclado        */
#endif
/* El #ifdef _WIN32 significa "solo si estamos compilando en Windows".
 * Se llama compilacion condicional: el compilador incluye o descarta ese
 * bloque segun la plataforma. Asi el programa sigue siendo C estandar. */


/* ----------------------------------------------------------------------------
 * ZONA 2: CONSTANTES
 *
 * #define crea un nombre para un valor fijo. El compilador sustituye el nombre
 * por el valor antes de compilar. Ventaja: si manana la memoria crece a 1000
 * posiciones (eso es justo la mejora 2 de la Seccion 3), solo cambias UNA
 * linea aqui y todo el programa se adapta. Por eso NUNCA escribimos el numero
 * 100 suelto en el codigo: lo llamamos TAM_MEMORIA.
 * --------------------------------------------------------------------------*/
#define TAM_MEMORIA        100   /* La Simpletron tiene 100 palabras: 00 a 99 */

#define PALABRA_MIN      -9999   /* Palabra Simpletron mas pequena posible    */
#define PALABRA_MAX      +9999   /* Palabra Simpletron mas grande posible     */

#define CENTINELA         9999   /* Al teclearlo, termina la fase de carga    */
#define CARGA_MAX         9998   /* Por eso al CARGAR el tope es +9998:       */
                                 /* el 9999 esta reservado como centinela     */

/* Los doce codigos de operacion del SML.
 * Darles nombre en vez de escribir "case 10:" hace el switch legible. */
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
 * Normalmente las variables globales son mala practica. Aqui son la eleccion
 * correcta, y vale la pena entender por que: estas variables NO son variables
 * del programa en C, son LAS PIEZAS FISICAS de la computadora que estamos
 * simulando. La memoria y los registros de una computadora existen siempre,
 * son unicos y todas las partes de la maquina los ven. Hacerlos globales es
 * modelar fielmente el hardware.
 *
 * El enunciado pide explicitamente estos cinco nombres, por eso estan en
 * ingles; los comentarios van en espanol.
 * --------------------------------------------------------------------------*/

int memory[TAM_MEMORIA];   /* LA MEMORIA: 100 cajones numerados 00..99.
                            * Cada cajon guarda UNA palabra Simpletron.
                            * En el modelo de Von Neumann, instrucciones y
                            * datos viven en esta misma memoria; lo que
                            * distingue a una de otro es COMO se usa. */

int accumulator;           /* EL ACUMULADOR: el registro donde la maquina
                            * hace todas sus cuentas. Toda operacion
                            * aritmetica deja aqui su resultado. */

int instructionCounter;    /* EL CONTADOR DE INSTRUCCIONES (IP o PC):
                            * guarda la DIRECCION de la siguiente instruccion
                            * por ejecutar. Empieza en 00 y normalmente
                            * avanza de uno en uno. */

int instructionRegister;   /* EL REGISTRO DE INSTRUCCION: copia de la palabra
                            * que acabamos de traer de memoria y que estamos
                            * ejecutando en este momento. Ej: +1009 */

int operationCode;         /* Los DOS DIGITOS IZQUIERDOS de esa palabra: QUE
                            * hay que hacer. De +1009 -> 10 (READ) */

int operand;               /* Los DOS DIGITOS DERECHOS: SOBRE QUE direccion
                            * de memoria hay que hacerlo. De +1009 -> 09 */


/* ----------------------------------------------------------------------------
 * ZONA 4: PROTOTIPOS
 *
 * Un prototipo le avisa al compilador "esta funcion existe y se usa asi".
 * C lee el archivo de arriba hacia abajo: si main() llama a cargarPrograma()
 * y esa funcion esta escrita mas abajo, sin el prototipo el compilador se
 * queja. Con los prototipos aqui arriba podemos ordenar las funciones como
 * queramos.
 * --------------------------------------------------------------------------*/
void mostrarBienvenida(void);
void cargarPrograma(void);
void ejecutarPrograma(void);
void volcadoDeMemoria(void);
void errorFatal(const char *mensaje);
void limpiarEntrada(void);
void pausarAntesDeSalir(void);
int  leerPalabra(int *destino);


/* ============================================================================
 *  main: el director de orquesta.
 *  Nota lo corto que es. Esa es la idea: main solo dice QUE pasa y en que
 *  orden; el COMO esta repartido en funciones pequenas. Un programa donde
 *  main tiene 300 lineas es un programa dificil de corregir.
 * ==========================================================================*/
int main(void)
{
#ifdef _WIN32
    /* La consola de Windows usa por omision una tabla de caracteres vieja
     * (pagina de codigos 850) que no conoce las tildes ni la ene. Por eso en
     * muchos simuladores se lee "termin%" en vez de "termino". Esta linea
     * cambia la consola a UTF-8 (codigo 65001) y el problema desaparece. */
    SetConsoleOutputCP(65001);
#endif

    mostrarBienvenida();   /* 1. Saludar y explicar como capturar            */
    cargarPrograma();      /* 2. FASE DE CARGA: llenar memory[] con el SML   */
    ejecutarPrograma();    /* 3. FASE DE EJECUCION: correr ese programa SML  */
    volcadoDeMemoria();    /* 4. Mostrar registros y memoria al terminar     */

    pausarAntesDeSalir();  /* 5. Esperar antes de cerrar la ventana          */

    return 0;              /* 0 le dice a Windows "termine sin problemas"    */
}


/* ============================================================================
 *  pausarAntesDeSalir
 *
 *  Resuelve un problema que no es del programa sino de Windows: cuando un
 *  ejecutable de consola se abre con doble clic desde el Explorador, el
 *  sistema destruye la ventana en cuanto el programa termina. El vaciado de
 *  memoria alcanza a imprimirse, pero nadie llega a leerlo.
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
 *  mostrarBienvenida: la pantalla inicial que pide el enunciado.
 * ==========================================================================*/
void mostrarBienvenida(void)
{
    printf("*** Bienvenido a Simpletron! ***\n");
    printf("*** Por favor escriba su programa una instrucción ***\n");
    printf("*** (o palabra de datos) a la vez. Yo mostraré    ***\n");
    printf("*** el número de posición y un signo de           ***\n");
    printf("*** interrogación (?). Después usted escribe la   ***\n");
    printf("*** palabra para esa posición. Escriba el         ***\n");
    printf("*** centinela 9999 para terminar de introducir    ***\n");
    printf("*** su programa. ***\n\n");
}


/* ============================================================================
 *  limpiarEntrada: tira a la basura lo que quedo escrito en el teclado.
 *
 *  Esto arregla un problema clasico de C que a casi todos nos muerde alguna
 *  vez. Cuando escribes "hola" y scanf esperaba un numero, scanf falla PERO
 *  la palabra "hola" se queda guardada en el buffer de entrada. Si vuelves a
 *  llamar a scanf, vuelve a leer "hola", vuelve a fallar... y el programa se
 *  cicla infinitamente escupiendo el mismo mensaje de error.
 *
 *  La solucion es leer caracter por caracter hasta el fin de linea y tirarlos.
 *  EOF significa "End Of File": si el usuario cierra la entrada (Ctrl+Z en
 *  Windows), tambien hay que parar o el ciclo no terminaria nunca.
 * ==========================================================================*/
void limpiarEntrada(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Cuerpo vacio a proposito: todo el trabajo lo hace la condicion.
         * Solo queremos consumir y descartar caracteres. */
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
 *  Ese tercer caso es importante. Si quien nos llama insiste "hasta que el
 *  dato sea valido" y la entrada ya se acabo, scanf devolvera EOF para
 *  siempre y el programa se quedaria colgado imprimiendo el mismo error.
 *  Distinguir "basura" de "ya no hay nada" evita ese cuelgue.
 *
 *  El asterisco de "int *destino" significa que no recibimos una copia del
 *  valor, sino la DIRECCION de una variable de quien nos llamo. Asi podemos
 *  modificar SU variable. Es lo mismo que hace scanf cuando le escribes &x:
 *  le estas pasando la direccion de x para que la pueda rellenar.
 * ==========================================================================*/
int leerPalabra(int *destino)
{
    /* scanf devuelve CUANTOS datos logro leer correctamente. Le pedimos uno
     * (%d), asi que 1 es exito. Devuelve EOF si ya no queda entrada. */
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
    int posicion = 0;   /* En que cajon de memoria vamos a guardar */
    int palabra;        /* Lo que acaba de teclear el usuario */
    int lectura;        /* 1 = ok, 0 = basura, -1 = se acabo la entrada */

    /* Dejar toda la memoria en ceros antes de empezar.
     * Sin esto, memory[] contendria basura: valores impredecibles que quedaron
     * en esa zona de la RAM. Una computadora de verdad tambien arranca con su
     * memoria en un estado conocido. */
    for (int i = 0; i < TAM_MEMORIA; i++) {
        memory[i] = 0;
    }

    while (posicion < TAM_MEMORIA) {

        /* El formato "%02d" imprime el numero con 2 digitos rellenando con
         * ceros: 0 se ve como "00", 7 como "07". Asi la pantalla queda como
         * la del enunciado: "00 ? ", "01 ? ", ... */
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
            continue;   /* continue = "salta al siguiente giro del while".
                         * Como NO incrementamos posicion, se vuelve a pedir
                         * exactamente la misma direccion. */
        }

        /* -- Fin de la carga -- */
        if (palabra == CENTINELA) {
            break;      /* break = "sal del while ahora mismo" */
        }

        /* -- Validacion 2: que este en el rango permitido -- */
        if (palabra < PALABRA_MIN || palabra > CARGA_MAX) {
            printf("*** Valor fuera de rango: debe estar entre %d y +%d. ***\n",
                   PALABRA_MIN, CARGA_MAX);
            continue;
        }

        /* Todo bien: guardamos y avanzamos al siguiente cajon. */
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
 *  ejecutarPrograma: FASE DE EJECUCION. El corazon del simulador.
 *
 *  Aqui vive el CICLO DE EJECUCION DE INSTRUCCION, que toda computadora real
 *  repite millones de veces por segundo y que tiene tres pasos:
 *
 *      1. BUSCAR   (fetch)   : traer de memoria la instruccion que toca
 *      2. DECODIFICAR (decode): partirla en "que hacer" y "sobre que"
 *      3. EJECUTAR (execute) : hacerlo
 *
 *  ...y repetir, hasta que aparezca un HALT o un error fatal.
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

        /* ---------- PASO 0: seguridad ----------
         * Antes de traer nada, verificamos que el contador apunte a una
         * direccion que exista. Sin esto, un programa mal escrito podria
         * pedirnos memory[150] y leeriamos memoria ajena: en C eso no da
         * error automatico, simplemente produce basura o revienta. */
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
         *   1009 / 100  = 10   <- division ENTERA: tira los dos ultimos
         *                          digitos y deja los dos primeros (el QUE)
         *   1009 % 100  = 09   <- residuo: deja exactamente los dos ultimos
         *                          digitos (el DONDE)
         *
         * El operador % (modulo) da el sobrante de una division entera.
         * 1009 entre 100 da 10 de cociente y sobran 9. */
        operationCode = instructionRegister / 100;
        operand       = instructionRegister % 100;

        /* ---------- PASO 3: EJECUTAR ----------
         * El switch es la "unidad de control" de nuestra computadora: compara
         * operationCode contra cada caso y ejecuta el que coincida.
         *
         * OJO con el break: en C, si olvidas el break al final de un case, la
         * ejecucion SIGUE hacia el case de abajo (se llama fall-through) y el
         * programa hace cosas rarisimas. Es el error mas comun en un switch. */
        switch (operationCode) {

        /* ---- OPERACIONES DE ENTRADA / SALIDA ---- */

        case READ:   /* 10: leer del teclado hacia memory[operand] */
            /* Aplicamos el mismo criterio que en la fase de carga: si el dato
             * no sirve, insistimos hasta que sea valido. Antes este caso
             * guardaba un 0 cuando la entrada era basura, y eso es peor que
             * un error: el programa SML seguiria corriendo con un dato falso
             * y daria un resultado incorrecto sin avisar. */
            while (1) {
                printf("? ");
                lectura = leerPalabra(&valorLeido);

                if (lectura == -1) {
                    /* No hay mas datos y la maquina los necesita: no hay
                     * forma de continuar, asi que es un error fatal. */
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
                break;   /* OJO: este break sale del while de arriba,
                          * NO del switch. Un break siempre rompe el ciclo o
                          * switch MAS CERCANO que lo contiene. */
            }
            memory[operand] = valorLeido;
            break;       /* Este otro break si es el del switch. */

        case WRITE:  /* 11: imprimir memory[operand] en pantalla */
            /* "%+05d" se lee asi:
             *    +  -> muestra siempre el signo, aunque sea positivo
             *    0  -> rellena con ceros a la izquierda
             *    5  -> ancho total de 5 caracteres (signo + 4 digitos)
             * Resultado: el 4 se ve como +0004, que es el formato de palabra
             * Simpletron. */
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
            /* Dividir entre cero en C no da "infinito": el programa se
             * MUERE ahi mismo, sin mensaje. Por eso hay que revisarlo ANTES
             * de hacer la division. Es el error fatal mas importante. */
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
         * Simular un salto es sorprendentemente simple: basta con escribir
         * otra direccion en el contador de instrucciones. La proxima vuelta
         * del while buscara la instruccion ahi y no en la siguiente linea.
         * Esto es, por debajo, lo que hacen el if, el while y el for de C. */

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
            continuar = 0;   /* Apagamos la bandera: el while se acaba */
            break;

        /* ---- CUALQUIER OTRA COSA ----
         * default atrapa todo lo que no coincidio con ningun case. Si llegamos
         * aqui, el "codigo de operacion" no es ninguno de los doce validos:
         * casi siempre significa que el contador de instrucciones se salio del
         * area de programa y esta intentando ejecutar un DATO como si fuera
         * una instruccion. */
        default:
            errorFatal("Código de operación no válido");
            break;
        }

        /* ---------- PASO 4: avanzar ----------
         * Si la instruccion NO fue un salto, pasamos a la siguiente direccion.
         * Si SI fue un salto, el contador ya apunta a donde debe y tocarlo
         * aqui arruinaria el salto. */
        if (!transferenciaDeControl && continuar) {
            instructionCounter++;
        }
    }
}


/* ============================================================================
 *  errorFatal: que hacer cuando algo sale irremediablemente mal.
 *
 *  El enunciado lo pide claro: imprimir un mensaje Y un vaciado completo, para
 *  que el usuario pueda ver el estado exacto de la maquina en el instante de
 *  la falla y descubrir que lo causo. Es, literalmente, la caja negra de un
 *  avion.
 *
 *  "const char *mensaje" recibe un texto. El const promete que esta funcion
 *  no va a modificarlo.
 * ==========================================================================*/
void errorFatal(const char *mensaje)
{
    printf("\n*** ERROR FATAL: %s ***\n", mensaje);
    printf("*** La ejecución de Simpletron termina de forma anormal ***\n");

    volcadoDeMemoria();
    pausarAntesDeSalir();

    /* exit() corta el programa de inmediato desde donde sea que estemos, sin
     * volver a main. Es lo correcto aqui: despues de un error fatal la maquina
     * quedo en un estado invalido y seguir ejecutando no tendria sentido.
     * El 1 le indica al sistema operativo "termine mal". */
    exit(1);
}


/* ============================================================================
 *  volcadoDeMemoria: el "memory dump" de la figura 5.23.
 *
 *  Imprime una fotografia completa de la maquina: sus cinco registros y sus
 *  100 posiciones de memoria. Es la herramienta principal para depurar un
 *  programa en SML, porque deja ver exactamente que quedo en cada cajon.
 * ==========================================================================*/
void volcadoDeMemoria(void)
{
    printf("\nREGISTROS:\n");

    /* "%-21s" alinea el texto a la IZQUIERDA en 21 espacios (el guion es lo
     * que invierte la alineacion). Asi todas las etiquetas ocupan el mismo
     * ancho y los valores quedan en columna, sin usar tabuladores. */
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
