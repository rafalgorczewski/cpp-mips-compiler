.data
		temp_KcbVSNs4AuOGQjxv: .word 0
		temp_GFgMn4xpwY3hUIzm: .word 0
		a: .word 0

.text
		li $t0, 2
		li $t1, 1
		sub $t0, $t0, $t1
		sw $t0, temp_GFgMn4xpwY3hUIzm
		lw $t0, temp_GFgMn4xpwY3hUIzm
		li $t1, 6
		add $t0, $t0, $t1
		sw $t0, temp_KcbVSNs4AuOGQjxv
		sw $t0, a
