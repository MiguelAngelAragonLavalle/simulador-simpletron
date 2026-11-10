/* Evaluacion de expresiones con pilas
   Materia: Programacion Avanzada
   Autor:   Miguel Angel Aragon Lavalle

   Compilar: gcc -Wall -Wextra -std=c11 expresiones.c -o expresiones.exe
   Ejecutar: .\expresiones.exe
             .\expresiones.exe "( 12 + 8 ) * 3"

   Resuelve una expresion infija en tres etapas, cada una con una pila:
     1. Validar el anidamiento de ( ) [ ] { }
     2. Convertir de infija a postfija
     3. Evaluar la postfija

   Los operandos pueden tener uno o mas digitos y admiten punto decimal.    */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX       100
#define MAX_TOK    32
#define MAX_EXPR  300

char  pila[MAX];
int   tope = 0;

float pilaNum[MAX];
int   topeNum = 0;

char  infija[MAX][MAX_TOK];
int   nInfija = 0;

char  postfija[MAX][MAX_TOK];
int   nPostfija = 0;


/* ---------- Primitivas de la pila de caracteres ---------- */

void error(char mensaje[])
{
    printf("\n*** ERROR: %s ***\n", mensaje);
    exit(1);
}

void push(char c)
{
    if (tope >= MAX) {
        error("Pila llena (overflow)");
    }
    pila[tope] = c;
    tope++;
}

char pop(void)
{
    if (tope == 0) {
        error("Pila vacia (underflow)");
    }
    tope--;
    return pila[tope];
}

int isEmpty(void)
{
    return tope == 0;
}

char stackTop(void)
{
    if (tope == 0) {
        error("Pila vacia");
    }
    return pila[tope - 1];
}


/* ---------- Primitivas de la pila de numeros ---------- */

void pushNum(float x)
{
    if (topeNum >= MAX) {
        error("Pila de numeros llena (overflow)");
    }
    pilaNum[topeNum] = x;
    topeNum++;
}

float popNum(void)
{
    if (topeNum == 0) {
        error("Faltan operandos en la expresion (underflow)");
    }
    topeNum--;
    return pilaNum[topeNum];
}


/* ---------- Funciones auxiliares ---------- */

int esDigito(char c)
{
    return c >= '0' && c <= '9';
}

int esOperandoTok(char t[])
{
    return esDigito(t[0]);
}

int esOperadorTok(char t[])
{
    return t[1] == '\0' &&
           (t[0] == '+' || t[0] == '-' || t[0] == '*' || t[0] == '/' || t[0] == '^');
}

int esApertura(char c)
{
    return c == '(' || c == '[' || c == '{';
}

int esCierre(char c)
{
    return c == ')' || c == ']' || c == '}';
}

char cierreDe(char c)
{
    switch (c) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    }
    return ' ';
}

int prioridad(char op)
{
    switch (op) {
    case '^': return 3;
    case '*':
    case '/': return 2;
    case '+':
    case '-': return 1;
    }
    return 0;
}

/* Convierte el texto de un operando a numero. Con un solo digito equivaldria
   a restar '0'; con varios digitos hay que recorrerlos, que es justo lo que
   hace atof(). */
float convierte(char token[])
{
    return (float) atof(token);
}

float potencia(float base, int exponente)
{
    float r = 1;
    int i;

    if (exponente < 0) {
        error("Exponente negativo no soportado");
    }
    for (i = 0; i < exponente; i++) {
        r = r * base;
    }
    return r;
}

float evalua(float opnd1, char sign, float opnd2)
{
    switch (sign) {
    case '+': return opnd1 + opnd2;
    case '-': return opnd1 - opnd2;
    case '*': return opnd1 * opnd2;
    case '/':
        if (opnd2 == 0) {
            error("Division entre cero");
        }
        return opnd1 / opnd2;
    case '^': return potencia(opnd1, (int) opnd2);
    }
    error("Operador no valido");
    return 0;
}


/* ---------- Analisis lexico: partir la expresion en unidades ---------- */

int tokenizar(char expresion[])
{
    int i = 0;
    int n = 0;
    int j;
    int puntos;

    while (expresion[i] != '\0') {

        if (expresion[i] == ' ' || expresion[i] == '\t') {
            i++;
            continue;
        }

        if (n >= MAX) {
            error("La expresion tiene demasiadas unidades");
        }

        if (esDigito(expresion[i])) {
            j = 0;
            puntos = 0;
            while (esDigito(expresion[i]) || expresion[i] == '.') {
                if (expresion[i] == '.') {
                    puntos++;
                    if (puntos > 1) {
                        error("Un numero no puede tener dos puntos decimales");
                    }
                }
                if (j >= MAX_TOK - 1) {
                    error("Numero demasiado largo");
                }
                infija[n][j] = expresion[i];
                j++;
                i++;
            }
            infija[n][j] = '\0';
            n++;
        }
        else {
            infija[n][0] = expresion[i];
            infija[n][1] = '\0';
            n++;
            i++;
        }
    }
    return n;
}


/* ---------- 1. Validacion del anidamiento ---------- */

int valida(char expresion[])
{
    char symb, temp;
    int i;

    tope = 0;
    for (i = 0; expresion[i] != '\0'; i++) {
        symb = expresion[i];

        if (esApertura(symb)) {
            push(symb);
        }
        else if (esCierre(symb)) {
            if (isEmpty()) {
                printf("Sobra un '%c' en la posicion %d.\n", symb, i + 1);
                return 0;
            }
            temp = pop();
            if (cierreDe(temp) != symb) {
                printf("Se abrio '%c' y se cerro con '%c' en la posicion %d.\n",
                       temp, symb, i + 1);
                return 0;
            }
        }
    }

    if (!isEmpty()) {
        printf("Quedaron %d agrupadores sin cerrar.\n", tope);
        return 0;
    }
    return 1;
}

int profundidad(char expresion[])
{
    int actual = 0;
    int maxima = 0;
    int i;

    for (i = 0; expresion[i] != '\0'; i++) {
        if (esApertura(expresion[i])) {
            actual++;
            if (actual > maxima) {
                maxima = actual;
            }
        }
        else if (esCierre(expresion[i])) {
            actual--;
        }
    }
    return maxima;
}


/* ---------- 2. Conversion de infija a postfija ---------- */

int aPostfijo(void)
{
    int i;
    int j = 0;
    char symb;

    /* Vale 1 cuando lo que sigue debe ser un operando o una apertura, y 0
       cuando debe ser un operador o un cierre. Asi se rechazan expresiones
       mal formadas, como las que empiezan con un operador o tienen dos
       operandos seguidos. */
    int esperaOperando = 1;

    tope = 0;
    for (i = 0; i < nInfija; i++) {

        if (esOperandoTok(infija[i])) {
            if (!esperaOperando) {
                error("Faltan operadores: hay dos operandos seguidos");
            }
            strcpy(postfija[j], infija[i]);
            j++;
            esperaOperando = 0;
            continue;
        }

        symb = infija[i][0];

        if (esApertura(symb)) {
            if (!esperaOperando) {
                error("Falta un operador antes del agrupador");
            }
            push(symb);
        }
        else if (esCierre(symb)) {
            if (esperaOperando) {
                error("Falta un operando antes del cierre del agrupador");
            }
            while (!isEmpty() && !esApertura(stackTop())) {
                postfija[j][0] = pop();
                postfija[j][1] = '\0';
                j++;
            }
            if (isEmpty()) {
                error("Agrupadores desbalanceados");
            }
            pop();
            esperaOperando = 0;
        }
        else if (esOperadorTok(infija[i])) {
            if (esperaOperando) {
                error("Falta un operando antes del operador");
            }
            esperaOperando = 1;
            while (!isEmpty() && !esApertura(stackTop()) &&
                   (prioridad(stackTop()) > prioridad(symb) ||
                    (prioridad(stackTop()) == prioridad(symb) && symb != '^'))) {
                postfija[j][0] = pop();
                postfija[j][1] = '\0';
                j++;
            }
            push(symb);
        }
        else {
            printf("\n*** ERROR: unidad no valida '%s' ***\n", infija[i]);
            exit(1);
        }
    }

    if (esperaOperando) {
        error("La expresion esta incompleta: termina en un operador");
    }

    while (!isEmpty()) {
        if (esApertura(stackTop())) {
            error("Falto cerrar un agrupador");
        }
        postfija[j][0] = pop();
        postfija[j][1] = '\0';
        j++;
    }
    return j;
}


/* ---------- 3. Evaluacion de la postfija ---------- */

float evaluar(void)
{
    int i, k;
    float opnd1, opnd2, value;

    topeNum = 0;
    printf("%-8s %-10s %-10s %-10s %s\n", "symb", "opnd1", "opnd2", "value", "pila");

    for (i = 0; i < nPostfija; i++) {

        if (esOperandoTok(postfija[i])) {
            pushNum(convierte(postfija[i]));
            printf("%-8s %-10s %-10s %-10s ", postfija[i], "", "", "");
        }
        else {
            opnd2 = popNum();
            opnd1 = popNum();
            value = evalua(opnd1, postfija[i][0], opnd2);
            pushNum(value);
            printf("%-8s %-10g %-10g %-10g ", postfija[i], opnd1, opnd2, value);
        }

        for (k = 0; k < topeNum; k++) {
            printf("%g", pilaNum[k]);
            if (k < topeNum - 1) {
                printf(",");
            }
        }
        printf("\n");
    }

    value = popNum();
    if (topeNum != 0) {
        error("Sobran operandos en la expresion");
    }
    return value;
}


/* ---------- Programa principal ---------- */

int main(int argc, char *argv[])
{
    char expresion[MAX_EXPR];
    float resultado;
    int i;

    printf("*** Evaluacion de expresiones con pilas ***\n");
    printf("*** Operadores: + - * / ^   y agrupadores ( ) [ ] { } ***\n");
    printf("*** Los operandos pueden tener varios digitos ***\n\n");

    if (argc > 1) {
        strncpy(expresion, argv[1], MAX_EXPR - 1);
        expresion[MAX_EXPR - 1] = '\0';
        printf("Expresion: %s\n", expresion);
    }
    else {
        printf("Escriba la expresion: ");
        if (fgets(expresion, MAX_EXPR, stdin) == NULL) {
            error("No se leyo ninguna expresion");
        }
        for (i = 0; expresion[i] != '\0'; i++) {
            if (expresion[i] == '\n' || expresion[i] == '\r') {
                expresion[i] = '\0';
                break;
            }
        }
    }

    if (expresion[0] == '\0') {
        error("No se escribio ninguna expresion");
    }

    printf("\n--- 1. Validacion de agrupadores ---\n");
    if (!valida(expresion)) {
        error("Los agrupadores no estan bien anidados");
    }
    printf("Agrupadores correctamente anidados.\n");
    printf("Profundidad maxima de anidamiento: %d\n", profundidad(expresion));

    nInfija = tokenizar(expresion);
    nPostfija = aPostfijo();

    printf("\n--- 2. Conversion a postfija ---\n");
    printf("Infija   : ");
    for (i = 0; i < nInfija; i++) {
        printf("%s ", infija[i]);
    }
    printf("\nPostfija : ");
    for (i = 0; i < nPostfija; i++) {
        printf("%s ", postfija[i]);
    }
    printf("\n");

    printf("\n--- 3. Evaluacion ---\n");
    resultado = evaluar();
    printf("\nResultado: %g\n", resultado);

    if (argc == 1) {
        printf("\nPresione Enter para cerrar...");
        getchar();
    }
    return 0;
}
