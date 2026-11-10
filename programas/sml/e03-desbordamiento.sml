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
+1005   ; 00  READ  05        Lee A -> memoria[05]
+1006   ; 01  READ  06        Lee B -> memoria[06]
+2005   ; 02  LOAD  05        acumulador <- A
+3006   ; 03  ADD   06        acumulador <- A + B   <-- 18000 no cabe: ERROR
+4300   ; 04  HALT            (nunca se alcanza)
+0000   ; 05  (dato)          Variable A
+0000   ; 06  (dato)          Variable B
; ============================================================================
; Datos de prueba  : 9000 y 9000
; Resultado esperado: *** ERROR FATAL: Desbordamiento del acumulador en una
;                     suma ***, seguido del vaciado completo.
; Ejecutar con     : .\simpletron.exe < pruebas\entradas\e03-desbordamiento.txt
; ============================================================================
