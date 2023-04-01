      area lab4_5,code
      entry
__main proc
      export __main [weak]
strat ldr r0,data1
      mov r1,r0
	  bfc r1,#23,#9
	  orr r1,#0x00800000
	  add r2,r1,r1
	  lsr r2,#1
	  bfc r2,#23,#9
	  mov r3,r0
	  bfc r3,#0,#23
	  add r3,#0x00800000
	  orr r4,r3,r2
      b .
      endp
      align
data1 dcd 0x40F40000 ;7.625
	  end