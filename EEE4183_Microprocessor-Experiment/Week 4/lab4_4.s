      area lab4_4,code
      entry
__main proc
      export __main [weak]
strat ldr r0,data1
      ldr r1,data2
	  add r2,r0,r1
	  sub r3,r0,r1
	  smull r4,r5,r0,r1
	  lsl r5,#16
	  lsr r4,#16
	  orr r6,r4,r5
      b .
      endp
      align
data1 dcd 0x0007A000 ;7.625
data2 dcd 0x00004000 ;0.25
	  end