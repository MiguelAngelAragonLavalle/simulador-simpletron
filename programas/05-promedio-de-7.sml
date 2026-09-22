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
; 00    +1015    READ  15        Lee un numero -> temporal
; 01    +2014    LOAD  14        acumulador <- suma acumulada
; 02    +3015    ADD   15        acumulador <- suma + numero leido
; 03    +2114    STORE 14        suma <- acumulador
; 04    +2016    LOAD  16        acumulador <- contador
; 05    +3117    SUB   17        acumulador <- contador - 1
; 06    +2116    STORE 16        contador <- acumulador
; 07    +4209    BRANCHZERO 09   Si ya se leyeron los 7, salir del ciclo
; 08    +4000    BRANCH 00       Si no, repetir
; 09    +2014    LOAD  14        acumulador <- suma total
; 10    +3218    DIVIDE 18       acumulador <- suma / 7
; 11    +2119    STORE 19        promedio <- acumulador
; 12    +1119    WRITE 19        Imprime el promedio
; 13    +4300    HALT            Fin
; 14    +0000    (dato)          suma = 0
; 15    +0000    (dato)          numero leido (temporal)
; 16    +0007    (dato)          contador = 7
; 17    +0001    (dato)          constante 1
; 18    +0007    (dato)          constante 7 (el divisor)
; 19    +0000    (dato)          promedio
; ============================================================================
; Datos de prueba : 10 -5 20 -15 30 -25 40   (suma = 55)
; Salida esperada : +0007
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\05-promedio-de-7.txt
; ============================================================================