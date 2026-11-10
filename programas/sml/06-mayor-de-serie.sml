; ============================================================================
; 06-mayor-de-serie.sml
; Lee una serie de numeros e imprime el mayor. El PRIMER numero leido indica
; cuantos numeros se van a procesar en total.
; Origen: Seccion 1.6, ejercicio 4.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1019   ; 00  READ  19        Lee n = cuantos numeros vienen en total
+1020   ; 01  READ  20        Lee el primero y lo toma como mayor provisional
+2019   ; 02  LOAD  19        acumulador <- n
+3121   ; 03  SUB   21        acumulador <- n - 1
+2119   ; 04  STORE 19        n <- n - 1  (ya se leyo uno)
+4217   ; 05  BRANCHZERO 17   Si ya no quedan numeros, ir a imprimir
+1022   ; 06  READ  22        Lee el siguiente numero -> actual
+2022   ; 07  LOAD  22        acumulador <- actual
+3120   ; 08  SUB   20        acumulador <- actual - mayor
+4112   ; 09  BRANCHNEG 12    Si actual < mayor, NO actualizar el mayor
+2022   ; 10  LOAD  22        acumulador <- actual
+2120   ; 11  STORE 20        mayor <- actual
+2019   ; 12  LOAD  19        acumulador <- n
+3121   ; 13  SUB   21        acumulador <- n - 1
+2119   ; 14  STORE 19        n <- n - 1
+4217   ; 15  BRANCHZERO 17   Si ya no quedan, ir a imprimir
+4006   ; 16  BRANCH 06       Si no, leer el siguiente numero
+1120   ; 17  WRITE 20        Imprime el mayor encontrado
+4300   ; 18  HALT            Fin
+0000   ; 19  (dato)          n = cuantos numeros faltan por procesar
+0000   ; 20  (dato)          mayor encontrado hasta el momento
+0001   ; 21  (dato)          constante 1
+0000   ; 22  (dato)          numero actual
; ============================================================================
; Datos de prueba : 5  (y luego)  3 17 8 25 12
; Salida esperada : +0025
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\06-mayor-de-serie.txt
; ============================================================================
