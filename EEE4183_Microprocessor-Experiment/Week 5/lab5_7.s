    area lab5_7,code
    entry
__main proc
	export __main [weak]
start mov r0,#0
	  cmp r0,#0
	  beq j1
	  mov r1,#0
j1    mov r1,#1
	  cbz r0,j2
	  mov r2,#0
j2    mov r2,#1
	  mov r0,#5
	  cmp r0,#0
	  bne j3
	  mov r3,#0
j3    mov r3,#1
	  cbnz r0,j4
	  mov r4,#0
j4    mov r4,#1 	  
      b .
      endp
      end