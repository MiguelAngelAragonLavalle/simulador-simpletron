; ============================================================================
; e01-division-entre-cero.sml
; CASO DE ERROR FATAL: intento de dividir entre cero.
;
; En C, dividir entre cero no devuelve "infinito": el programa muere en seco
; sin ningun mensaje. Por eso el simulador revisa el divisor ANTES de dividir.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1004   ; 00  READ  04        Lee un numero -> memoria[04]
+2004   ; 01  LOAD  04        acumulador <- ese numero
+3205   ; 02  DIVIDE 05       Divide entre memoria[05], que vale 0  <-- ERROR
+4300   ; 03  HALT            (nunca se alcanza)
+0000   ; 04  (dato)          dividendo
+0000   ; 05  (dato)          divisor = 0   <-- provoca el error fatal
; ============================================================================
; Datos de prueba  : 8
; Resultado esperado: *** ERROR FATAL: Intento de dividir entre cero ***
;                     seguido del vaciado completo de registros y memoria.
; Ejecutar con     : .\simpletron.exe < pruebas\entradas\e01-division-entre-cero.txt
; ============================================================================
