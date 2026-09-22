; ============================================================================
; e01-division-entre-cero.sml
; CASO DE ERROR FATAL: intento de dividir entre cero.
;
; En C, dividir entre cero no devuelve "infinito": el programa muere en seco
; sin ningun mensaje. Por eso el simulador revisa el divisor ANTES de dividir.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1004    READ  04        Lee un numero -> memoria[04]
; 01    +2004    LOAD  04        acumulador <- ese numero
; 02    +3205    DIVIDE 05       Divide entre memoria[05], que vale 0  <-- ERROR
; 03    +4300    HALT            (nunca se alcanza)
; 04    +0000    (dato)          dividendo
; 05    +0000    (dato)          divisor = 0   <-- provoca el error fatal
; ============================================================================
; Datos de prueba  : 8
; Resultado esperado: *** ERROR FATAL: Intento de dividir entre cero ***
;                     seguido del vaciado completo de registros y memoria.
; Ejecutar con     : .\simpletron.exe < pruebas\entradas\e01-division-entre-cero.txt
; ============================================================================