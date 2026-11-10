; ============================================================================
; 04-suma-de-10.sml
; Lee 10 numeros con un ciclo controlado por contador e imprime su suma.
; Origen: Seccion 1.6, ejercicio 2.
;
; El "ciclo" se construye con BRANCHZERO + BRANCH: no existe while en SML,
; se arma restando 1 a un contador y saltando segun el resultado.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+1012   ; 00  READ  12        Lee un numero -> temporal
+2011   ; 01  LOAD  11        acumulador <- suma acumulada
+3012   ; 02  ADD   12        acumulador <- suma + numero leido
+2111   ; 03  STORE 11        suma <- acumulador
+2013   ; 04  LOAD  13        acumulador <- contador
+3114   ; 05  SUB   14        acumulador <- contador - 1
+2113   ; 06  STORE 13        contador <- acumulador
+4209   ; 07  BRANCHZERO 09   Si el contador llego a 0, ir a imprimir
+4000   ; 08  BRANCH 00       Si no, repetir el ciclo desde el principio
+1111   ; 09  WRITE 11        Imprime la suma
+4300   ; 10  HALT            Fin
+0000   ; 11  (dato)          suma = 0
+0000   ; 12  (dato)          numero leido (temporal)
+0010   ; 13  (dato)          contador = 10  <- cuantos numeros se leen
+0001   ; 14  (dato)          constante 1
; ============================================================================
; Datos de prueba : 1 2 3 4 5 6 7 8 9 10
; Salida esperada : +0055
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\04-suma-de-10.txt
; ============================================================================
