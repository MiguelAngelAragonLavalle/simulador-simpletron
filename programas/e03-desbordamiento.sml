; ============================================================================
; e03-desbordamiento.sml
; CASO DE ERROR FATAL: desbordamiento del acumulador.
;
; Una palabra Simpletron solo puede guardar de -9999 a +9999. Si una operacion
; aritmetica produce un resultado mas grande, no cabe y es un error fatal.
; Aqui 9000 + 9000 = 18000, que se sale del rango.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1005    READ  05        Lee A -> memoria[05]
; 01    +1006    READ  06        Lee B -> memoria[06]
; 02    +2005    LOAD  05        acumulador <- A
; 03    +3006    ADD   06        acumulador <- A + B   <-- 18000 no cabe: ERROR
; 04    +4300    HALT            (nunca se alcanza)
; 05    +0000    (dato)          Variable A
; 06    +0000    (dato)          Variable B
; ============================================================================
; Datos de prueba  : 9000 y 9000
; Resultado esperado: *** ERROR FATAL: Desbordamiento del acumulador en una
;                     suma ***, seguido del vaciado completo.
; Ejecutar con     : .\simpletron.exe < pruebas\entradas\e03-desbordamiento.txt
; ============================================================================