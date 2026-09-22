; ============================================================================
; e04-carga-fuera-de-rango.sml
; CASO DE VALIDACION EN LA FASE DE CARGA (no es un error fatal).
;
; Este caso NO prueba la ejecucion, sino la carga. Se teclean a proposito
; tres entradas invalidas antes del programa real:
;
;    -12000   fuera de rango (es menor que -9999)
;     99999   fuera de rango (es mayor que +9998)
;     abc     no es un numero
;
; El simulador debe rechazar las tres SIN avanzar de posicion de memoria:
; despues de cada rechazo vuelve a pedir la direccion 00. Solo cuando recibe
; un valor valido guarda y avanza a la 01.
;
; Programa real que se carga despues de los rechazos:
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
; 00    +1003    READ  03        Lee un numero -> memoria[03]
; 01    +1103    WRITE 03        Lo imprime de vuelta
; 02    +4300    HALT            Fin
; 03    +0000    (dato)          numero leido
; ============================================================================
; Datos de prueba : 7
; Salida esperada : tres mensajes de rechazo durante la carga y luego +0007
; Ejecutar con    : .\simpletron.exe < pruebas\entradas\e04-carga-fuera-de-rango.txt
; ============================================================================