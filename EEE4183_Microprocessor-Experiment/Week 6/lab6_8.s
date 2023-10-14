STACK_BASE EQU 0x20000100
MY_BUFFER EQU 0x20000128
      area lab6_8,code
      entry
__main proc
      export __main [weak]
start ldr sp,=STACK_BASE
      mov r0,#0
      bl setj
	  bl sub1
stop  b stop
      endp
setj proc
	 push{r0,lr}
	 mov r0,sp
	 ldr sp,=MY_BUFFER
	 push{r0-r8,lr}
	 mov sp,r0
     pop{r0,pc}
	 endp 
longj proc
	  ldr sp,=MY_BUFFER
	  sub sp,#0x28
	  pop{r0-r8,pc} 
	  endp 
sub1  proc
	  cmp r0,#5
	  beq longj
      add r0,#1
	  bl sub2
      endp  
sub2  proc
	  cmp r0,#5
	  beq longj
      add r0,#1
	  bl sub3
      endp  
sub3  proc
	  cmp r0,#5
	  beq longj
      add r0,#1
	  bl sub1
      endp		  
      end