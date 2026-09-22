; ============================================================================
; 03-producto.sml
; Lee dos numeros e imprime su producto.
; Origen: Seccion 1.6, ejercicio 1.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1007    READ  07        Lee A y lo guarda en memoria[07]
; 01    +1008    READ  08        Lee B y lo guarda en memoria[08]
; 02    +2007    LOAD  07        acumulador <- A
; 03    +3308    MULT  08        acumulador <- A * B
; 04    +2109    STORE 09        memoria[09] <- producto
; 05    +1109    WRITE 09        Imprime el producto
; 06    +4300    HALT            Fin
; 07    +0000    (dato)          Variable A
; 08    +0000    (dato)          Variable B
; 09    +0000    (dato)          Producto
; ============================================================================
; Datos de prueba : 6 y 7
; Salida esperada : +0042
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\03-producto.txt
; ============================================================================