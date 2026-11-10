; ============================================================================
; 05-promedio-de-7.sml
; Lee 7 numeros (positivos y negativos) e imprime su promedio.
; Origen: Seccion 1.6, ejercicio 3.
;
; OJO: Simpletron trabaja con enteros, asi que DIVIDE hace division entera.
; Con suma = 55 y 7 numeros, 55 / 7 da 7 (se descarta el 0.857).
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1015   ; 00  READ  15        Lee un numero -> temporal
+2014   ; 01  LOAD  14        acumulador <- suma acumulada
+3015   ; 02  ADD   15        acumulador <- suma + numero leido
+2114   ; 03  STORE 14        suma <- acumulador
+2016   ; 04  LOAD  16        acumulador <- contador
+3117   ; 05  SUB   17        acumulador <- contador - 1
+2116   ; 06  STORE 16        contador <- acumulador
+4209   ; 07  BRANCHZERO 09   Si ya se leyeron los 7, salir del ciclo
+4000   ; 08  BRANCH 00       Si no, repetir
+2014   ; 09  LOAD  14        acumulador <- suma total
+3218   ; 10  DIVIDE 18       acumulador <- suma / 7
+2119   ; 11  STORE 19        promedio <- acumulador
+1119   ; 12  WRITE 19        Imprime el promedio
+4300   ; 13  HALT            Fin
+0000   ; 14  (dato)          suma = 0
+0000   ; 15  (dato)          numero leido (temporal)
+0007   ; 16  (dato)          contador = 7
+0001   ; 17  (dato)          constante 1
+0007   ; 18  (dato)          constante 7 (el divisor)
+0000   ; 19  (dato)          promedio
; ============================================================================
; Datos de prueba : 10 -5 20 -15 30 -25 40   (suma = 55)
; Salida esperada : +0007
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\05-promedio-de-7.txt
; ============================================================================
