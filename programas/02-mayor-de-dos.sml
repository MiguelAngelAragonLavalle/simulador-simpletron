; ============================================================================
; 02-mayor-de-dos.sml
; Lee dos numeros y escribe el mayor de los dos.
; Origen: Seccion 1, Ejercicio 2. Ejemplo del "if" en SML usando BRANCHNEG.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1009    READ  09        Lee A y lo guarda en memoria[09]
; 01    +1010    READ  10        Lee B y lo guarda en memoria[10]
; 02    +2009    LOAD  09        acumulador <- A
; 03    +3110    SUB   10        acumulador <- A - B
; 04    +4107    BRANCHNEG 07    Si A - B < 0 (o sea A < B) salta a 07
; 05    +1109    WRITE 09        Caso A >= B: imprime A
; 06    +4300    HALT            Fin
; 07    +1110    WRITE 10        Caso A <  B: imprime B
; 08    +4300    HALT            Fin
; 09    +0000    (dato)          Variable A
; 10    +0000    (dato)          Variable B
; ============================================================================
; Datos de prueba : 3 y 9
; Salida esperada : +0009
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\02-mayor-de-dos.txt
; ============================================================================