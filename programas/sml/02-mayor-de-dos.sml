; ============================================================================
; 02-mayor-de-dos.sml
; Lee dos numeros y escribe el mayor de los dos.
; Origen: Seccion 1, Ejercicio 2. Ejemplo del "if" en SML usando BRANCHNEG.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1009   ; 00  READ  09        Lee A y lo guarda en memoria[09]
+1010   ; 01  READ  10        Lee B y lo guarda en memoria[10]
+2009   ; 02  LOAD  09        acumulador <- A
+3110   ; 03  SUB   10        acumulador <- A - B
+4107   ; 04  BRANCHNEG 07    Si A - B < 0 (o sea A < B) salta a 07
+1109   ; 05  WRITE 09        Caso A >= B: imprime A
+4300   ; 06  HALT            Fin
+1110   ; 07  WRITE 10        Caso A <  B: imprime B
+4300   ; 08  HALT            Fin
+0000   ; 09  (dato)          Variable A
+0000   ; 10  (dato)          Variable B
; ============================================================================
; Datos de prueba : 3 y 9
; Salida esperada : +0009
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\02-mayor-de-dos.txt
; ============================================================================
