
.text

printint: # arg goes in $a0
	li $v0,1
	syscall
	jr $ra

printchar: # arg goes in $a0
	li $v0,11
	syscall
	jr $ra

printdouble: # arg goes in $f12
	li $v0,3
	syscall
	jr $ra

toint: # arg goes in $f12
	floor.w.d $f12,$f12
	cvt.w.d $f12,$f12
	mfc1 $v0,$f12
	jr $ra

todouble: # arg goes in $a0, result will be in $f12
	mtc1 $a0,$f12
	cvt.d.w $f12,$f12
	jr $ra

main:
	subu $sp,$sp,8
	sw $ra,4($sp)
	sw $fp,0($sp)
	subu $fp,$sp,4

	li $t1,0 # $t1 is i
beg_int:	
	addi $a0,$t1,0
	jal printint
	li $a0,10
	jal printchar
	addi $t1,$t1,1
	slti $t2,$t1,10
	beqz $t2,endint
	j beg_int	
endint:

# do double division

.data 
	initdbl: .double 50.0
	divdbl: .double 1.5
	zerodbl: .double 0.0
	onedbl: .double 1.0
.text
	l.d $f0,zerodbl
	l.d $f4,divdbl
	l.d $f2,initdbl
	l.d $f6,onedbl
	# li $t2,0 # $t2 is i
begindbl:
	add.d $f12,$f0,$f2
	jal printdouble
	li $a0,32
	jal printchar
	add.d $f12,$f0,$f2
	jal toint
	move $a0,$v0
	jal printint
	li $a0,10
	jal printchar
	div.d $f2,$f2,$f4
	c.le.d 0 $f6,$f2
	bc1f 0 enddbl
	# addi $t2,$t2,1
	# slti $t3,$t2,5
	# beqz $t3,enddbl
	j begindbl
enddbl:
	
.data
	initcooldbl: .double 0.015625
.text
	l.d $f2,initcooldbl
	li $t1,2
	j coolcond
begincool:
	# convert $t1 to a double and multiply it to $f2
	move $a0,$t1
	jal todouble
	mov.d $f4,$f12
	mul.d $f2,$f2,$f4
	# print it all out
	mov.d $f12,$f2
	jal printdouble
	li $a0,32
	jal printchar
	move $a0,$t1
	jal printint
	li $a0,10
	jal printchar
	# post loop assignment
	addi $t1,$t1,1
coolcond:
	slti $t2,$t1,10
	beqz $t2,endcool
	j begincool
endcool:
	lw $ra,4($sp)
	lw $fp,0($sp)
	addu $sp,$sp,8
	jr $ra
