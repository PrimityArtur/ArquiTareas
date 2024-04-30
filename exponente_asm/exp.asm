## Fibonacci.asm

# SECCION DE INSTRUCCIONES (.text)
.text
.globl __start

__start:

la $a0, ExpBase
li $v0, 4
syscall
li $v0, 5
syscall
addi $t0, $v0, 0

la $a0, ExpExp
li $v0, 4
syscall
li $v0, 5
syscall
addi $a1, $v0, 0
addi $a0, $t0, 0

jal exp
nop

#Imprimir resultados
addi $t0, $v0, 0
la $a0, ExpResult
li $v0, 4
syscall
add $a0, $t0, 0
li $v0, 1
syscall

# Fin del programa
li $v0, 10
syscall


exp:
	#start
	sw $fp, -4($sp)
	move $fp, $sp
	addi $sp, $sp, -12
	sw $ra, -8($fp)
	sw $s0, -12($fp)
	
	#Exp
	li $s0, 0

	# ($t0 = $a1 < 0) expEnd
	slti $t0, $a1, 0
	beq $t0, 1, expExpNegative

	# $a0 == 0 = expEnd
	beqz $a0, expEnd
	
	li $s0, 1

	# $a1 == 0 = expEnd		
	beqz $a1, expEnd	
	
	add $s0, $a0, 0
	expLoop: addi $t0, $t0, 1	
		beq $t0, $a1, expEnd
		
		mul $s0, $s0, $a0
 
		j expLoop	

	#exponente negativo
	expExpNegative:
		la $a0, ExpNegative
		li $v0, 4
		syscall
		j expEnd
	#end
	expEnd:
		add $v0, $s0, 0

		lw $s0, -12($fp)
		lw $ra, -8($fp)
		move $sp, $fp
		lw $fp, -4($fp)
	jr $ra
	



# SECCION DE VARIABLES EN MEMORIA (.data)
.data
ExpBase: .asciiz "Ingresar base: "
ExpExp: .asciiz "Ingresar exp: "
ExpResult: .asciiz "El resultado es: "
ExpNegative: .asciiz "Error: el exponente es negativo \n"