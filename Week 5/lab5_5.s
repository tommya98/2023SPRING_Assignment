 	area lab5_5,code
 	entry
__main proc
 	export __main [weak]
start   
		ldr r0, =0x12
		ldr r1, =0x33
		ldr r2, =0xEE
		sub r3, r0, r1
		; Danger Section
		bl func1
		b.w func2
		space 4096

func1	ldr.n r7, =0xE7FE
		bx lr
		ltorg
func2	ldr r0, =0xAA
		ldr r1, =0xBB
		ldr r2, =0xCC
		sub r3, r0, r1
		b .
 	endp	
	align	
dummy1 dcd 0xFFFFEEEE
dummy2 dcd 0x12341234
dummy3 dcd 0xABCABCDD
	end