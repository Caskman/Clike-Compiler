
.text

printint: # arg goes in $a0
	li $v0,1
	syscall
	jr $ra
printchar: # arg goes in $a0
	li $v0,11
	syscall
	jr $ra
printdouble: # arg address goes in $a0
	lwc1 $f12,0($a0)
	lwc1 $f13,4($a0)
	li $v0,3
	syscall
	jr $ra
toint: # arg address goes in $a0, result goes in $v0
	lwc1 $f12,0($a0)
	lwc1 $f13,4($a0)
	floor.w.d $f12,$f12
	cvt.w.d $f12,$f12
	mfc1 $v0,$f12
	jr $ra
todouble: # arg address goes in $a0, result will be in $v0 and $v1
	mtc1 $a0,$f12
	cvt.d.w $f12,$f12
	mfc1 $v0,$f12
	mfc1 $v1,$f13
	jr $ra


# ======================================
# converts an integer to a double, prints the double, 
# converts that double to an integer and prints the new integer
# .text
# main:
# 	subu $sp,$sp,16
# 	sw $ra,4($sp)
# 	sw $fp,0($sp)
# 	addu $fp,$sp,12
# 	# -4 is the local double var

# 	li $a0,5
# 	jal todouble

# 	sw $v0,-4($fp)
# 	sw $v1,0($fp)

# 	addu $a0,$fp,-4
# 	jal printdouble

# 	li $a0,32
# 	jal printchar

# 	addu $a0,$fp,-4
# 	jal toint

# 	move $a0,$v0
# 	jal printint

# 	li $a0,10
# 	jal printchar

# 	lw $fp,0($sp)
# 	lw $ra,4($sp)
# 	addu $sp,$sp,16
# 	jr $ra
# ======================================





# ======================
# prints out the number at dbl
#	
# .text
# main:
# 	subu $sp,$sp,8
# 	sw $ra,-4($sp)
# 	sw $fp,0($sp)
# 	subu $fp,$sp,4

# .data
# 	dbl: .double 51.5
# .text
# 	la $a0,dbl
# 	jal printdouble
# 	li $a0,10
# 	jal printchar

# 	lw $ra,-4($sp)
# 	lw $fp,0($sp)
# 	addu $sp,$sp,8
# 	jr $ra
# ======================

# ======================
# divides a double until it is less than 1.0 and prints out the result of the division each time
# .text
# main:
# 	subu $sp,$sp,48
# 	sw $ra,4($sp)
# 	sw $fp,0($sp)
# 	subu $fp,$sp,44
# 	# -4 is the double i
# 	# -12 is the constant 2.0
# 	# -20 is the temp for holding results of operations
# 	# -28 is the temp for holding the constant 1.0
# 	# -32 is the temp for holding the branch comparison result




# 	# init a local double var
# .data
# 	initdbl0: .double 51.0
# .text
# 	la $t0,initdbl0
# 	lw $t1,0($t0)
# 	sw $t1,-4($fp)
# 	lw $t1,4($t0)
# 	sw $t1,0($fp)

# 	j f0cond # go straight to the condition first
# f0contents:
# 	addu $a0,$fp,-4
# 	jal printdouble
# 	li $a0,10
# 	jal printchar
# 	# post loop

# 	# init a local to be 2.0
# .data
# 	initdbl1: .double 2.0
# .text
# 	la $t0,initdbl1
# 	lw $t1,0($t0)
# 	sw $t1,-12($fp)
# 	lw $t1,4($t0)
# 	sw $t1,-8($fp)

# 	# div two local double vars
# 	# move first var to the first fp reg
# 	lwc1 $f0,-4($fp)
# 	lwc1 $f1,0($fp)
# 	# move second var to the second fp reg
# 	lwc1 $f2,-12($fp)
# 	lwc1 $f3,-8($fp)
# 	# div them
# 	div.d $f0,$f0,$f2
# 	# store the result
# 	swc1 $f0,-20($fp)
# 	swc1 $f1,-16($fp)

# 	# move value from temp to double i
# 	lw $t0,-20($fp)
# 	sw $t0,-4($fp)
# 	lw $t0,-16($fp)
# 	sw $t0,0($fp)

# f0cond:
# 	# compare two fp values
# 	# init the 1.0 var
# .data
# 	initdbl2: .double 1.0
# .text
# 	la $t0,initdbl2
# 	lw $t1,0($t0)
# 	sw $t1,-28($fp)
# 	lw $t1,4($t0)
# 	sw $t1,-24($fp)

# 	# move first var to the first fp reg
# 	lwc1 $f0,-28($fp)
# 	lwc1 $f1,-24($fp)
# 	# move second var to the second fp reg
# 	lwc1 $f2,-4($fp)
# 	lwc1 $f3,0($fp)

# 	c.le.d 0 $f0,$f2
# 	bc1t 0 f0contents

# 	lw $ra,4($sp)
# 	lw $fp,0($sp)
# 	addu $sp,$sp,48
# 	jr $ra
# ======================
