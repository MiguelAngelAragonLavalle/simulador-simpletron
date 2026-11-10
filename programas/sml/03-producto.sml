; ============================================================================
; 03-producto.sml
; Lee dos numeros e imprime su producto.
; Origen: Seccion 1.6, ejercicio 1.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1007   ; 00  READ  07        Lee A y lo guarda en memoria[07]
+1008   ; 01  READ  08        Lee B y lo guarda en memoria[08]
+2007   ; 02  LOAD  07        acumulador <- A
+3308   ; 03  MULT  08        acumulador <- A * B
+2109   ; 04  STORE 09        memoria[09] <- producto
+1109   ; 05  WRITE 09        Imprime el producto
+4300   ; 06  HALT            Fin
+0000   ; 07  (dato)          Variable A
+0000   ; 08  (dato)          Variable B
+0000   ; 09  (dato)          Producto
; ============================================================================
; Datos de prueba : 6 y 7
; Salida esperada : +0042
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\03-producto.txt
; ============================================================================
