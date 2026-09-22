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
; 00    +1012    READ  12        Lee un numero -> temporal
; 01    +2011    LOAD  11        acumulador <- suma acumulada
; 02    +3012    ADD   12        acumulador <- suma + numero leido
; 03    +2111    STORE 11        suma <- acumulador
; 04    +2013    LOAD  13        acumulador <- contador
; 05    +3114    SUB   14        acumulador <- contador - 1
; 06    +2113    STORE 13        contador <- acumulador
; 07    +4209    BRANCHZERO 09   Si el contador llego a 0, ir a imprimir
; 08    +4000    BRANCH 00       Si no, repetir el ciclo desde el principio
; 09    +1111    WRITE 11        Imprime la suma
; 10    +4300    HALT            Fin
; 11    +0000    (dato)          suma = 0
; 12    +0000    (dato)          numero leido (temporal)
; 13    +0010    (dato)          contador = 10  <- cuantos numeros se leen
; 14    +0001    (dato)          constante 1
; ============================================================================
; Datos de prueba : 1 2 3 4 5 6 7 8 9 10
; Salida esperada : +0055
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\04-suma-de-10.txt
; ============================================================================