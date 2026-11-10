; ============================================================================
; e05-direccion-invalida.sml
; CASO DE ERROR FATAL: el contador de instrucciones sale de la memoria.
;
; El programa no tiene HALT. El contador avanza instruccion tras instruccion
; hasta pasar de la posicion 99, que es la ultima que existe. En ese momento
; el simulador detiene la ejecucion en vez de leer memoria que no le pertenece.
;
; Se carga con las 100 posiciones llenas de +0000, que el simulador interpreta
; como codigo de operacion 00. Para que el error que aparezca sea el de la
; direccion y no el de operacion invalida, se rellena con BRANCH a la
; siguiente posicion, de modo que el contador avance limpiamente hasta el
; final de la memoria.
; ============================================================================
; Pos   Palabra  Instruccion     Significado
; ----  -------  --------------  ------------------------------------------
+4001   ; 00  BRANCH 01       Salta a la siguiente
+4002   ; 01  BRANCH 02
+4003   ; 02  BRANCH 03
+4004   ; 03  BRANCH 04
+4005   ; 04  BRANCH 05
+4006   ; 05  BRANCH 06
+4007   ; 06  BRANCH 07
+4008   ; 07  BRANCH 08
+4009   ; 08  BRANCH 09
+4010   ; 09  BRANCH 10
+4011   ; 10  BRANCH 11
+4012   ; 11  BRANCH 12
+4013   ; 12  BRANCH 13
+4014   ; 13  BRANCH 14
+4015   ; 14  BRANCH 15
+4016   ; 15  BRANCH 16
+4017   ; 16  BRANCH 17
+4018   ; 17  BRANCH 18
+4019   ; 18  BRANCH 19
+4020   ; 19  BRANCH 20
+4021   ; 20  BRANCH 21
+4022   ; 21  BRANCH 22
+4023   ; 22  BRANCH 23
+4024   ; 23  BRANCH 24
+4025   ; 24  BRANCH 25
+4026   ; 25  BRANCH 26
+4027   ; 26  BRANCH 27
+4028   ; 27  BRANCH 28
+4029   ; 28  BRANCH 29
+4030   ; 29  BRANCH 30
+4031   ; 30  BRANCH 31
+4032   ; 31  BRANCH 32
+4033   ; 32  BRANCH 33
+4034   ; 33  BRANCH 34
+4035   ; 34  BRANCH 35
+4036   ; 35  BRANCH 36
+4037   ; 36  BRANCH 37
+4038   ; 37  BRANCH 38
+4039   ; 38  BRANCH 39
+4040   ; 39  BRANCH 40
+4041   ; 40  BRANCH 41
+4042   ; 41  BRANCH 42
+4043   ; 42  BRANCH 43
+4044   ; 43  BRANCH 44
+4045   ; 44  BRANCH 45
+4046   ; 45  BRANCH 46
+4047   ; 46  BRANCH 47
+4048   ; 47  BRANCH 48
+4049   ; 48  BRANCH 49
+4050   ; 49  BRANCH 50
+4051   ; 50  BRANCH 51
+4052   ; 51  BRANCH 52
+4053   ; 52  BRANCH 53
+4054   ; 53  BRANCH 54
+4055   ; 54  BRANCH 55
+4056   ; 55  BRANCH 56
+4057   ; 56  BRANCH 57
+4058   ; 57  BRANCH 58
+4059   ; 58  BRANCH 59
+4060   ; 59  BRANCH 60
+4061   ; 60  BRANCH 61
+4062   ; 61  BRANCH 62
+4063   ; 62  BRANCH 63
+4064   ; 63  BRANCH 64
+4065   ; 64  BRANCH 65
+4066   ; 65  BRANCH 66
+4067   ; 66  BRANCH 67
+4068   ; 67  BRANCH 68
+4069   ; 68  BRANCH 69
+4070   ; 69  BRANCH 70
+4071   ; 70  BRANCH 71
+4072   ; 71  BRANCH 72
+4073   ; 72  BRANCH 73
+4074   ; 73  BRANCH 74
+4075   ; 74  BRANCH 75
+4076   ; 75  BRANCH 76
+4077   ; 76  BRANCH 77
+4078   ; 77  BRANCH 78
+4079   ; 78  BRANCH 79
+4080   ; 79  BRANCH 80
+4081   ; 80  BRANCH 81
+4082   ; 81  BRANCH 82
+4083   ; 82  BRANCH 83
+4084   ; 83  BRANCH 84
+4085   ; 84  BRANCH 85
+4086   ; 85  BRANCH 86
+4087   ; 86  BRANCH 87
+4088   ; 87  BRANCH 88
+4089   ; 88  BRANCH 89
+4090   ; 89  BRANCH 90
+4091   ; 90  BRANCH 91
+4092   ; 91  BRANCH 92
+4093   ; 92  BRANCH 93
+4094   ; 93  BRANCH 94
+4095   ; 94  BRANCH 95
+4096   ; 95  BRANCH 96
+4097   ; 96  BRANCH 97
+4098   ; 97  BRANCH 98
+4099   ; 98  BRANCH 99
+2000   ; 99  LOAD 00         Ultima posicion: al terminar, el contador
;                            pasa a 100, que ya no existe  <-- ERROR
; ============================================================================
; Datos de prueba  : ninguno
; Resultado esperado: *** ERROR FATAL: El contador de instrucciones salio de
;                     la memoria ***, seguido del vaciado completo.
; Ejecutar con     : .\simpletron.exe programas\sml\e05-direccion-invalida.sml
; ============================================================================
