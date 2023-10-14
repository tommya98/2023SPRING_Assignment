STACK_BASE EQU 0x20000100
      area lab6_5,code
      entry
__main proc
      export __main [weak]
start ldr r0, data1
	  ldr sp, =STACK_BASE
	  bl fibo
stop  b stop
      endp 
fibo  proc
	  push {r1, lr}
	  cmp r0, #1
	  blt return0
	  beq return1
	  mov r1,r0
      sub r0,r1,#1
      bl fibo
	  push {r0}
	  sub r0,r1,#2
	  bl fibo
	  pop {r1}
	  add r0,r1
	  pop {r1, pc}  
return0 mov r0, #0
	  pop {r1, pc}
return1 mov r0, #1
	  pop {r1, pc}
	  align
data1 dcd 3
      end