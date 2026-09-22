; ============================================================================
; 01-suma.sml
; Lee dos numeros enteros con signo y escribe su suma.
; Origen: Seccion 1, Ejemplo 1 del documento de lenguaje maquina.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1007    READ  07        Lee A y lo guarda en memoria[07]
; 01    +1008    READ  08        Lee B y lo guarda en memoria[08]
; 02    +2007    LOAD  07        acumulador <- A
; 03    +3008    ADD   08        acumulador <- A + B
; 04    +2109    STORE 09        memoria[09] <- acumulador   (C = A + B)
; 05    +1109    WRITE 09        Imprime C
; 06    +4300    HALT            Fin de la ejecucion
; 07    +0000    (dato)          Variable A
; 08    +0000    (dato)          Variable B
; 09    +0000    (dato)          Variable C (resultado)
; ============================================================================
; Datos de prueba : 5 y 7
; Salida esperada : +0012
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\01-suma.txt
; ============================================================================