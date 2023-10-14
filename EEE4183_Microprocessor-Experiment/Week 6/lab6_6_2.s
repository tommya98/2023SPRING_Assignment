	  area lab6_6,code
add   proc
	  export add
	  push {lr}
      add r0,r1
      pop {pc}
      endp
	    
fibo  proc
	  export fibo
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
	  end