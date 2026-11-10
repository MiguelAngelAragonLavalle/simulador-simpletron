/* Compilador de Simple a SML
   Materia: Programacion Avanzada
   Autor:   Miguel Angel Aragon Lavalle

   Compilar: gcc -Wall -Wextra -std=c11 compilador.c -o compilador.exe
   Ejecutar: .\compilador.exe programa.simple salida.sml

   Traduce un programa escrito en el lenguaje Simple a codigo SML ejecutable
   por el simulador de Simpletron. Trabaja en dos pasadas:

     Primera pasada  construye la tabla de simbolos, genera las instrucciones
                     SML y marca en el arreglo flags las referencias hacia
                     adelante que todavia no se pueden resolver.
     Segunda pasada  resuelve esas referencias y escribe el archivo .sml     */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_MEMORIA   100
#define MAX_SIMBOLOS  200
#define MAX_LINEA     300
#define MAX_TOKENS     64
#define MAX_PILA      100
#define MAX_NOTA      160
#define MAX_NOMBRE     40

#define READ        10
#define WRITE       11
#define LOAD        20
#define STORE       21
#define ADD         30
#define SUBTRACT    31
#define DIVIDE      32
#define MULTIPLY    33
#define BRANCH      40
#define BRANCHNEG   41
#define BRANCHZERO  42
#define HALT        43

typedef struct {
    int  symbol;
    char type;
    int  location;
} tableEntry;

tableEntry symbolTable[MAX_SIMBOLOS];
int numSimbolos = 0;

int memoria[TAM_MEMORIA];
int flags[TAM_MEMORIA];
char comentario[TAM_MEMORIA][MAX_NOTA];

int contadorInstrucciones = 0;
int siguienteDato = TAM_MEMORIA - 1;
int lineaFuente = 0;
int ultimaLinea = -1;


/* ---------- Errores ---------- */

void error(char mensaje[])
{
    printf("\n*** ERROR en la linea %d del fuente: %s ***\n", lineaFuente, mensaje);
    exit(1);
}


/* ---------- Tabla de simbolos ---------- */

int buscarSimbolo(int symbol, char type)
{
    int i;

    for (i = 0; i < numSimbolos; i++) {
        if (symbolTable[i].symbol == symbol && symbolTable[i].type == type) {
            return symbolTable[i].location;
        }
    }
    return -1;
}

void insertarSimbolo(int symbol, char type, int location)
{
    if (numSimbolos >= MAX_SIMBOLOS) {
        error("Tabla de simbolos llena");
    }
    symbolTable[numSimbolos].symbol   = symbol;
    symbolTable[numSimbolos].type     = type;
    symbolTable[numSimbolos].location = location;
    numSimbolos++;
}

int nuevaCelda(void)
{
    if (siguienteDato < contadorInstrucciones) {
        error("La memoria de Simpletron se agoto: el programa es demasiado grande");
    }
    return siguienteDato--;
}

/* Devuelve un nombre legible para una posicion de memoria: la letra de la
   variable, el valor de la constante, o "temporal NN" si es una celda que el
   compilador reservo para un resultado intermedio. Sirve para que los
   comentarios del codigo generado se lean como el programa original. */
void nombreDe(int loc, char destino[])
{
    int i;

    for (i = 0; i < numSimbolos; i++) {
        if (symbolTable[i].location == loc) {
            if (symbolTable[i].type == 'V') {
                snprintf(destino, MAX_NOMBRE, "%c", (char) symbolTable[i].symbol);
                return;
            }
            if (symbolTable[i].type == 'C') {
                snprintf(destino, MAX_NOMBRE, "%d", symbolTable[i].symbol);
                return;
            }
        }
    }
    snprintf(destino, MAX_NOMBRE, "temporal %02d", loc);
}

/* Devuelve la posicion de memoria de una variable o constante.
   Si todavia no existe, le asigna una celda desde el final de la memoria. */
int ubicacionDe(char token[])
{
    int loc;
    int valor;
    int i;

    if (isalpha((unsigned char) token[0])) {
        if (strlen(token) != 1 || !islower((unsigned char) token[0])) {
            error("Las variables deben ser una sola letra minuscula");
        }
        loc = buscarSimbolo(token[0], 'V');
        if (loc == -1) {
            loc = nuevaCelda();
            insertarSimbolo(token[0], 'V', loc);
            snprintf(comentario[loc], MAX_NOTA, "variable %c", token[0]);
        }
        return loc;
    }

    /* Una constante debe ser solo digitos, con un maximo de cuatro, para que
       quepa en una palabra de Simpletron (0 a 9999). */
    for (i = 0; token[i] != '\0'; i++) {
        if (!isdigit((unsigned char) token[i])) {
            error("Operando no valido: se esperaba una variable o una constante entera");
        }
    }
    if (strlen(token) > 4) {
        error("Las constantes deben tener como maximo 4 digitos");
    }

    valor = atoi(token);
    if (valor < 0 || valor > 9999) {
        error("Las constantes deben estar entre 0 y 9999");
    }
    loc = buscarSimbolo(valor, 'C');
    if (loc == -1) {
        loc = nuevaCelda();
        insertarSimbolo(valor, 'C', loc);
        memoria[loc] = valor;
        snprintf(comentario[loc], MAX_NOTA, "constante %d", valor);
    }
    return loc;
}


/* ---------- Generacion de codigo ---------- */

void generar(int opcode, int operando, char nota[])
{
    if (contadorInstrucciones > siguienteDato) {
        error("La memoria de Simpletron se agoto: demasiadas instrucciones");
    }
    memoria[contadorInstrucciones] = opcode * 100 + operando;
    if (nota != NULL) {
        snprintf(comentario[contadorInstrucciones], MAX_NOTA, "%s", nota);
    }
    contadorInstrucciones++;
}

/* Genera una bifurcacion. Si la linea destino ya se conoce, la resuelve de
   inmediato; si no, deja el operando en 00 y anota la linea en flags. */
void generarBifurcacion(int opcode, int lineaDestino, char nota[])
{
    int loc = buscarSimbolo(lineaDestino, 'L');

    if (loc != -1) {
        generar(opcode, loc, nota);
    } else {
        flags[contadorInstrucciones] = lineaDestino;
        generar(opcode, 0, nota);
    }
}


/* ---------- Conversion de infija a postfija (sobre tokens) ---------- */

int prioridad(char op[])
{
    if (strcmp(op, "^") == 0) return 3;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 0;
}

int esOperador(char t[])
{
    return strlen(t) == 1 &&
           (t[0] == '+' || t[0] == '-' || t[0] == '*' || t[0] == '/' || t[0] == '^');
}

int aPostfijo(char infija[][32], int n, char postfija[][32])
{
    char pila[MAX_PILA][32];
    int tope = 0;
    int i;
    int j = 0;

    for (i = 0; i < n; i++) {
        if (strcmp(infija[i], "(") == 0) {
            strcpy(pila[tope], infija[i]);
            tope++;
        }
        else if (strcmp(infija[i], ")") == 0) {
            while (tope > 0 && strcmp(pila[tope - 1], "(") != 0) {
                tope--;
                strcpy(postfija[j], pila[tope]);
                j++;
            }
            if (tope == 0) {
                error("Parentesis desbalanceados en la expresion");
            }
            tope--;
        }
        else if (esOperador(infija[i])) {
            while (tope > 0 && strcmp(pila[tope - 1], "(") != 0 &&
                   (prioridad(pila[tope - 1]) > prioridad(infija[i]) ||
                    (prioridad(pila[tope - 1]) == prioridad(infija[i]) &&
                     strcmp(infija[i], "^") != 0))) {
                tope--;
                strcpy(postfija[j], pila[tope]);
                j++;
            }
            strcpy(pila[tope], infija[i]);
            tope++;
        }
        else {
            strcpy(postfija[j], infija[i]);
            j++;
        }
    }

    while (tope > 0) {
        tope--;
        if (strcmp(pila[tope], "(") == 0) {
            error("Falto cerrar un parentesis en la expresion");
        }
        strcpy(postfija[j], pila[tope]);
        j++;
    }
    return j;
}

/* Recorre la expresion postfija generando codigo SML. En lugar de calcular,
   apila POSICIONES DE MEMORIA: al encontrar un operador saca las dos de
   arriba, emite LOAD / operacion / STORE en una celda temporal y apila esa
   celda. Al terminar queda una sola posicion: la del resultado. */
int generarExpresion(char postfija[][32], int n)
{
    int pila[MAX_PILA];
    int tope = 0;
    int i;
    int loc1, loc2, temp;
    int opcode;
    char nota[MAX_NOTA];
    char nombre1[MAX_NOMBRE];
    char nombre2[MAX_NOMBRE];
    char verbo[20];

    for (i = 0; i < n; i++) {
        if (!esOperador(postfija[i])) {
            if (tope >= MAX_PILA) {
                error("Expresion demasiado compleja");
            }
            pila[tope] = ubicacionDe(postfija[i]);
            tope++;
        }
        else {
            if (tope < 2) {
                error("Expresion mal formada: faltan operandos");
            }
            tope--;
            loc2 = pila[tope];
            tope--;
            loc1 = pila[tope];

            switch (postfija[i][0]) {
            case '+': opcode = ADD;      strcpy(verbo, "sumar");          break;
            case '-': opcode = SUBTRACT; strcpy(verbo, "restar");         break;
            case '*': opcode = MULTIPLY; strcpy(verbo, "multiplicar por"); break;
            case '/': opcode = DIVIDE;   strcpy(verbo, "dividir entre");   break;
            default:
                error("El operador ^ no tiene equivalente en SML basico");
                opcode = ADD;
                strcpy(verbo, "sumar");
                break;
            }

            nombreDe(loc1, nombre1);
            nombreDe(loc2, nombre2);

            snprintf(nota, MAX_NOTA, "cargar %s", nombre1);
            generar(LOAD, loc1, nota);
            snprintf(nota, MAX_NOTA, "%s %s", verbo, nombre2);
            generar(opcode, loc2, nota);

            temp = nuevaCelda();
            snprintf(comentario[temp], MAX_NOTA, "temporal");
            snprintf(nota, MAX_NOTA, "guardar en temporal %02d", temp);
            generar(STORE, temp, nota);

            pila[tope] = temp;
            tope++;
        }
    }

    if (tope != 1) {
        error("Expresion mal formada: sobran operandos");
    }
    return pila[0];
}


/* ---------- Compilacion de cada enunciado ---------- */

void compilarLet(char tokens[][32], int nTokens)
{
    char infija[MAX_TOKENS][32];
    char postfija[MAX_TOKENS][32];
    int n = 0;
    int i;
    int locResultado;
    int locDestino;
    char nota[MAX_NOTA];
    char nombreRes[MAX_NOMBRE];

    if (nTokens < 5 || strcmp(tokens[3], "=") != 0) {
        error("Formato correcto: let v = expresion");
    }

    locDestino = ubicacionDe(tokens[2]);

    for (i = 4; i < nTokens; i++) {
        strcpy(infija[n], tokens[i]);
        n++;
    }

    n = aPostfijo(infija, n, postfija);
    locResultado = generarExpresion(postfija, n);

    nombreDe(locResultado, nombreRes);
    snprintf(nota, MAX_NOTA, "cargar %s", nombreRes);
    generar(LOAD, locResultado, nota);
    snprintf(nota, MAX_NOTA, "asignar a %s", tokens[2]);
    generar(STORE, locDestino, nota);
}

void compilarIf(char tokens[][32], int nTokens)
{
    int loc1, loc2;
    int lineaDestino;
    char nota[MAX_NOTA];

    if (nTokens != 7 || strcmp(tokens[5], "goto") != 0) {
        error("Formato correcto: if a <op> b goto linea");
    }

    lineaDestino = atoi(tokens[6]);
    loc1 = ubicacionDe(tokens[2]);
    loc2 = ubicacionDe(tokens[4]);

    snprintf(nota, MAX_NOTA, "condicion %s %s %s", tokens[2], tokens[3], tokens[4]);

    if (strcmp(tokens[3], "==") == 0) {
        generar(LOAD, loc1, nota);
        generar(SUBTRACT, loc2, "restar");
        generarBifurcacion(BRANCHZERO, lineaDestino, "saltar si igual");
    }
    else if (strcmp(tokens[3], "!=") == 0) {
        generar(LOAD, loc1, nota);
        generar(SUBTRACT, loc2, "restar");
        generar(BRANCHZERO, contadorInstrucciones + 2, "si igual, no saltar");
        generarBifurcacion(BRANCH, lineaDestino, "saltar si distinto");
    }
    else if (strcmp(tokens[3], "<") == 0) {
        generar(LOAD, loc1, nota);
        generar(SUBTRACT, loc2, "restar");
        generarBifurcacion(BRANCHNEG, lineaDestino, "saltar si menor");
    }
    else if (strcmp(tokens[3], ">") == 0) {
        generar(LOAD, loc2, nota);
        generar(SUBTRACT, loc1, "restar al reves");
        generarBifurcacion(BRANCHNEG, lineaDestino, "saltar si mayor");
    }
    else if (strcmp(tokens[3], "<=") == 0) {
        generar(LOAD, loc1, nota);
        generar(SUBTRACT, loc2, "restar");
        generarBifurcacion(BRANCHNEG, lineaDestino, "saltar si menor");
        generarBifurcacion(BRANCHZERO, lineaDestino, "o si igual");
    }
    else if (strcmp(tokens[3], ">=") == 0) {
        generar(LOAD, loc2, nota);
        generar(SUBTRACT, loc1, "restar al reves");
        generarBifurcacion(BRANCHNEG, lineaDestino, "saltar si mayor");
        generarBifurcacion(BRANCHZERO, lineaDestino, "o si igual");
    }
    else {
        error("Operador relacional no valido (use < <= > >= == !=)");
    }
}

void compilarEnunciado(char linea[])
{
    char copia[MAX_LINEA];
    char tokens[MAX_TOKENS][32];
    int nTokens = 0;
    char *p;
    int numeroLinea;
    char nota[MAX_NOTA];
    int i;

    strcpy(copia, linea);

    p = strtok(copia, " \t\r\n");
    while (p != NULL && nTokens < MAX_TOKENS) {
        if (strlen(p) > 31) {
            error("Unidad lexicografica demasiado larga");
        }
        strcpy(tokens[nTokens], p);
        nTokens++;
        p = strtok(NULL, " \t\r\n");
    }

    if (nTokens == 0) {
        return;
    }

    for (i = 0; tokens[0][i] != '\0'; i++) {
        if (!isdigit((unsigned char) tokens[0][i])) {
            error("Todo enunciado debe empezar con un numero de linea");
        }
    }
    numeroLinea = atoi(tokens[0]);

    if (buscarSimbolo(numeroLinea, 'L') != -1) {
        error("Numero de linea repetido");
    }
    if (numeroLinea <= ultimaLinea) {
        error("Los numeros de linea deben ir en orden ascendente");
    }
    ultimaLinea = numeroLinea;
    insertarSimbolo(numeroLinea, 'L', contadorInstrucciones);

    if (nTokens < 2) {
        error("Falta el comando despues del numero de linea");
    }

    for (i = 0; tokens[1][i] != '\0'; i++) {
        if (isupper((unsigned char) tokens[1][i])) {
            error("Los comandos deben escribirse en minusculas");
        }
    }

    if (strcmp(tokens[1], "rem") == 0) {
        return;
    }
    else if (strcmp(tokens[1], "input") == 0) {
        if (nTokens != 3) {
            error("Formato correcto: input variable");
        }
        snprintf(nota, MAX_NOTA, "input %s", tokens[2]);
        generar(READ, ubicacionDe(tokens[2]), nota);
    }
    else if (strcmp(tokens[1], "print") == 0) {
        if (nTokens != 3) {
            error("Formato correcto: print variable");
        }
        snprintf(nota, MAX_NOTA, "print %s", tokens[2]);
        generar(WRITE, ubicacionDe(tokens[2]), nota);
    }
    else if (strcmp(tokens[1], "goto") == 0) {
        if (nTokens != 3) {
            error("Formato correcto: goto linea");
        }
        snprintf(nota, MAX_NOTA, "goto %s", tokens[2]);
        generarBifurcacion(BRANCH, atoi(tokens[2]), nota);
    }
    else if (strcmp(tokens[1], "if") == 0) {
        compilarIf(tokens, nTokens);
    }
    else if (strcmp(tokens[1], "let") == 0) {
        compilarLet(tokens, nTokens);
    }
    else if (strcmp(tokens[1], "end") == 0) {
        generar(HALT, 0, "end");
    }
    else {
        error("Comando no reconocido");
    }
}


/* ---------- Segunda pasada ---------- */

int segundaPasada(void)
{
    int i;
    int loc;
    int resueltas = 0;

    for (i = 0; i < TAM_MEMORIA; i++) {
        if (flags[i] != -1) {
            loc = buscarSimbolo(flags[i], 'L');
            if (loc == -1) {
                printf("\n*** ERROR: la linea %d no existe en el programa ***\n", flags[i]);
                exit(1);
            }
            memoria[i] = memoria[i] + loc;
            printf("  posicion %02d  ->  linea %d esta en %02d  ->  %+05d\n",
                   i, flags[i], loc, memoria[i]);
            resueltas++;
        }
    }
    return resueltas;
}


/* ---------- Reportes ---------- */

void mostrarTablaDeSimbolos(void)
{
    int i;

    printf("\n--- Tabla de simbolos ---\n");
    printf("%-10s %-6s %s\n", "Simbolo", "Tipo", "Ubicacion");
    for (i = 0; i < numSimbolos; i++) {
        if (symbolTable[i].type == 'V') {
            printf("%-10c %-6c %02d\n", symbolTable[i].symbol,
                   symbolTable[i].type, symbolTable[i].location);
        } else {
            printf("%-10d %-6c %02d\n", symbolTable[i].symbol,
                   symbolTable[i].type, symbolTable[i].location);
        }
    }
}

void escribirSalida(char nombreArchivo[])
{
    FILE *salida;
    int i;

    salida = fopen(nombreArchivo, "w");
    if (salida == NULL) {
        error("No se pudo crear el archivo de salida");
    }

    fprintf(salida, "; Codigo SML generado por el compilador de Simple\n");
    fprintf(salida, "; Una instruccion por linea. Lo que sigue a ; es comentario.\n");
    fprintf(salida, ";\n");

    for (i = 0; i < TAM_MEMORIA; i++) {
        fprintf(salida, "%+05d   ; %02d", memoria[i], i);
        if (comentario[i][0] != '\0') {
            fprintf(salida, "  %s", comentario[i]);
        }
        fprintf(salida, "\n");
    }

    fclose(salida);
}


/* ---------- Programa principal ---------- */

int main(int argc, char *argv[])
{
    FILE *fuente;
    char linea[MAX_LINEA];
    char nombreSalida[MAX_LINEA];
    int i;
    int resueltas;

    if (argc < 2) {
        printf("Uso: compilador.exe programa.simple [salida.sml]\n");
        return 1;
    }

    if (argc >= 3) {
        strcpy(nombreSalida, argv[2]);
    } else {
        strcpy(nombreSalida, "salida.sml");
    }

    for (i = 0; i < TAM_MEMORIA; i++) {
        memoria[i] = 0;
        flags[i] = -1;
        comentario[i][0] = '\0';
    }

    fuente = fopen(argv[1], "r");
    if (fuente == NULL) {
        printf("*** ERROR: no se encontro el archivo %s ***\n", argv[1]);
        return 1;
    }

    printf("*** Compilador de Simple a SML ***\n");
    printf("Fuente : %s\n", argv[1]);
    printf("Salida : %s\n", nombreSalida);

    printf("\n--- Primera pasada ---\n");
    while (fgets(linea, MAX_LINEA, fuente) != NULL) {
        lineaFuente++;
        compilarEnunciado(linea);
    }
    fclose(fuente);
    printf("Se generaron %d instrucciones SML.\n", contadorInstrucciones);
    printf("Los datos ocupan de la posicion %02d a la 99.\n", siguienteDato + 1);

    mostrarTablaDeSimbolos();

    printf("\n--- Segunda pasada: referencias hacia adelante ---\n");
    resueltas = segundaPasada();
    if (resueltas == 0) {
        printf("  (no habia referencias pendientes)\n");
    } else {
        printf("Se resolvieron %d referencias.\n", resueltas);
    }

    escribirSalida(nombreSalida);

    printf("\n--- Codigo SML generado ---\n");
    for (i = 0; i < contadorInstrucciones; i++) {
        printf("%02d  %+05d   %s\n", i, memoria[i], comentario[i]);
    }

    printf("\n*** Compilacion terminada. Ejecute: simpletron.exe %s ***\n",
           nombreSalida);
    return 0;
}
