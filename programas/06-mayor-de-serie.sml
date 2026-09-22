; ============================================================================
; 06-mayor-de-serie.sml
; Lee una serie de numeros e imprime el mayor. El PRIMER numero leido indica
; cuantos numeros se van a procesar en total.
; Origen: Seccion 1.6, ejercicio 4.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1019    READ  19        Lee n = cuantos numeros vienen en total
; 01    +1020    READ  20        Lee el primero y lo toma como mayor provisional
; 02    +2019    LOAD  19        acumulador <- n
; 03    +3121    SUB   21        acumulador <- n - 1
; 04    +2119    STORE 19        n <- n - 1  (ya se leyo uno)
; 05    +4217    BRANCHZERO 17   Si ya no quedan numeros, ir a imprimir
; 06    +1022    READ  22        Lee el siguiente numero -> actual
; 07    +2022    LOAD  22        acumulador <- actual
; 08    +3120    SUB   20        acumulador <- actual - mayor
; 09    +4112    BRANCHNEG 12    Si actual < mayor, NO actualizar el mayor
; 10    +2022    LOAD  22        acumulador <- actual
; 11    +2120    STORE 20        mayor <- actual
; 12    +2019    LOAD  19        acumulador <- n
; 13    +3121    SUB   21        acumulador <- n - 1
; 14    +2119    STORE 19        n <- n - 1
; 15    +4217    BRANCHZERO 17   Si ya no quedan, ir a imprimir
; 16    +4006    BRANCH 06       Si no, leer el siguiente numero
; 17    +1120    WRITE 20        Imprime el mayor encontrado
; 18    +4300    HALT            Fin
; 19    +0000    (dato)          n = cuantos numeros faltan por procesar
; 20    +0000    (dato)          mayor encontrado hasta el momento
; 21    +0001    (dato)          constante 1
; 22    +0000    (dato)          numero actual
; ============================================================================
; Datos de prueba : 5  (y luego)  3 17 8 25 12
; Salida esperada : +0025
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\06-mayor-de-serie.txt
; ============================================================================