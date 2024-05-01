.data
M:		.asciiz "Ingrese el multiplicando: "            
Q:		.asciiz "Ingrese el multiplicador: "      
Resultado: .asciiz "Resultado: "

.text
.globl __start
__start:
# Init
	# Obtener M
	la $a0, M    			
	li $v0, 4
	syscall
	li $v0, 5
	syscall	
	move $t0, $v0
	
	# Obtener Q
	la $a0, Q     			
	li $v0, 4
	syscall
	li $v0, 5
	syscall	

	move $a1, $v0			# Q
	move $a0, $t0			# M
	li $s0, 0              # A
	li $s3, 0              # Q_1
	li $s4, 32             # n (numero de bits)	
	
	jal algoritmoBoth
	nop
	
	#impresion de texto
	move $t0, $v0
	la $a0, Resultado
	li $v0, 4
	syscall
	
	#imprimir $v0 A
	move $v0, $t0
	jal printBinary
	nop

	#imprimir $v1 Q
	move $v0, $v1
	jal printBinary
	nop
    
	# Finalizar el programa
    li $v0, 10        
    syscall

# Algoritmo de Booth
algoritmoBoth:
	#init
	sw $fp, -4($sp)
	sw $ra, -8($sp)
	sw $s0, -12($sp)
	sw $s3, -16($sp)
	sw $s4, -20($sp)

	move $fp, $sp
	addi $sp, $sp, -20
	
	subi $t0, $s4, 1		# numero de bits - 1
    loop:
		sllv $s5, $a1, $t0
		srlv $s5, $s5, $t0
        andi $s5, $s5, 1   # Q0 = LSB de Q
        bne $s5, $s3, check_sub # Si Q0 != Q_1, ir a check_sub
	
        j shift           # Si Q0 == Q_1, realizar shift
        
    check_sub:
        bne $s5, 0, sub    # Si Q0 == 1, hacer sub
        addu $s0, $s0, $a0 # Q0 == 0, Q_1 == 1, realizar suma A = A + M
		j shift

    sub:
        subu $s0, $s0, $a0 # Q0 == 1, Q_1 == 0, realizar resta A = A - M
		j shift

    shift:
        sllv $s6, $s0, $t0    # Guardamos LSB de A

        srlv $s7, $s0, $t0    # Preparar MSB de A para el shift right de A
		sllv $s7, $s7, $t0    # Establecer MSB en A-1
	
		srl $s0, $s0, 1	    # Shift a la derecha en A
        or $s0, $s0, $s7   # Combinar A y MSB A-1

        srl $a1, $a1, 1    # Shift a la derecha en Q
        or $a1, $a1, $s6   # Establecer nuevo Q

		move $s3, $s5   # Preparar nuevo Q_1
        
        subi $s4, $s4, 1   # Decrementar contador n
        bnez $s4, loop     # Repetir mientras n != 0

    # Guardar y terminar
    move $v0, $s0				#guardar A en $v0
    move $v1, $a1				#guardar Q en $v1

	lw $s4, -20($fp)
	lw $s3, -16($fp)
	lw $s0, -12($fp)
	lw $ra, -8($fp)
	move $sp, $fp
	lw $fp, -4($fp)
	jr $ra

printBinary:
	#init
	sw $fp, -4($sp)
	sw $ra, -8($sp)
	sw $s0, -12($sp)
	sw $s4, -16($sp)

	move $fp, $sp
	addi $sp, $sp, -16

    # Imprimir el número en su representación binaria
	subi $t0, $s4, 1		# numero de bits - 1
    move $t1, $s4        	# Inicializar el contador
	move $s0, $v0		#inicializamos el numero binario

	print_loop:
    	srlv $t2, $s0, $t0  # Extraer el bit más significativo
    	andi $t2, $t2, 1  # Asegurarse de que solo se tome un bit

	    # Imprimir el bit
    	li $v0, 1         # Cargar el código de la llamada al sistema para imprimir entero
    	move $a0, $t2     # Cargar el bit para imprimirlo
    	syscall

    	sll $s0, $s0, 1   # Desplazar el número a la izquierda para obtener el siguiente bit
    	subi $t1, $t1, 1  # Decrementar el contador de bits

    	# Salir del bucle si se han impreso todos los bits
    	bnez $t1, print_loop
	
    # Guardar y terminar

	lw $s4, -16($fp)
	lw $s0, -12($fp)
	lw $ra, -8($fp)
	move $sp, $fp
	lw $fp, -4($fp)

	jr $ra