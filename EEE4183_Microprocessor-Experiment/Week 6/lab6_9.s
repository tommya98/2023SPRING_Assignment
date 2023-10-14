QUEUE_BASE EQU 0x20000100
      area lab6_9,code
      entry
__main proc
      export __main [weak]
start ldr r7,=QUEUE_BASE
      ldr r8,=QUEUE_BASE
      adr r0,input
	  ldr r1,=data
      bl enque
	  bl enque
	  bl enque
	  bl deque
	  bl enque
	  bl deque
	  bl deque
	  bl deque
stop  b stop
      endp
enque proc
	  ldr r2,[r0],#4
	  stmia r7!,{r2}
	  bx lr
	  endp
deque proc
	  ldmia r8!,{r2}
	  str r2,[r1],#4
	  bx lr
	  endp
	  align
input dcd 0x12345678, 0x23456789, 0x3456789a, 0x456789ab
	  area lab6data,data
data  space 16
      end