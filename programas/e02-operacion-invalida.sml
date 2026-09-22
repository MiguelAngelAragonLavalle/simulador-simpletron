; ============================================================================
; e02-operacion-invalida.sml
; CASO DE ERROR FATAL: codigo de operacion que no existe.
;
; Los unicos codigos validos son 10, 11, 20, 21, 30, 31, 32, 33, 40, 41, 42
; y 43. La palabra +9900 tiene codigo de operacion 99, que no es ninguno de
; ellos, asi que cae en el "default" del switch.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1003    READ  03        Lee un numero -> memoria[03]
; 01    +9900    ????            Codigo de operacion 99: NO EXISTE  <-- ERROR
; 02    +4300    HALT            (nunca se alcanza)
; 03    +0000    (dato)          numero leido
; ============================================================================
; Datos de prueba  : 5
; Resultado esperado: *** ERROR FATAL: Codigo de operacion no valido ***
;                     seguido del vaciado completo.
; Ejecutar con     : .\simpletron.exe < pruebas\entradas\e02-operacion-invalida.txt
; ============================================================================