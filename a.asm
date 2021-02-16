.data
		temp_cTFLVwShMNbaJ3H5: .word 0
		z: .word 0
		temp_fc7mPkIvdpBeKOyu: .word 0
		x: .word 0

.text
		li $t0, 3
		sw $t0, x
		lw $t0, x
		li $t1, 2
		mul $t0, $t0, $t1
		sw $t0, temp_fc7mPkIvdpBeKOyu
		li $t0, 5
		lw $t1, temp_fc7mPkIvdpBeKOyu
		add $t0, $t0, $t1
		sw $t0, temp_cTFLVwShMNbaJ3H5
		lw $t0, temp_cTFLVwShMNbaJ3H5
		sw $t0, z
		li $v0, 1
		lw $a0, x
		syscall
		li $v0, 1
		lw $a0, z
		syscall
