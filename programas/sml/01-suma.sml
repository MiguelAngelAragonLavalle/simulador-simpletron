; ============================================================================
; 01-suma.sml
; Lee dos numeros enteros con signo y escribe su suma.
; Origen: Seccion 1, Ejemplo 1 del documento de lenguaje maquina.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1007   ; 00  READ  07        Lee A y lo guarda en memoria[07]
+1008   ; 01  READ  08        Lee B y lo guarda en memoria[08]
+2007   ; 02  LOAD  07        acumulador <- A
+3008   ; 03  ADD   08        acumulador <- A + B
+2109   ; 04  STORE 09        memoria[09] <- acumulador   (C = A + B)
+1109   ; 05  WRITE 09        Imprime C
+4300   ; 06  HALT            Fin de la ejecucion
+0000   ; 07  (dato)          Variable A
+0000   ; 08  (dato)          Variable B
+0000   ; 09  (dato)          Variable C (resultado)
; ============================================================================
; Datos de prueba : 5 y 7
; Salida esperada : +0012
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\01-suma.txt
; ============================================================================
