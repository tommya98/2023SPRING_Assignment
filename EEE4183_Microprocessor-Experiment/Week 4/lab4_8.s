      area lab4_8,code
      entry
__main proc
      export __main [weak]
strat ldr r0,data1
	  ldr r1,data2
	  ldr r2,data3
	  ldr r3,data4
	  umull r4,r5,r1,r3
	  umlal r5,r6,r0,r3
	  umlal r5,r6,r1,r2
	  umlal r6,r7,r0,r2
      b .
      endp
      align
data1 dcd 0x01234567
data2 dcd 0x89abcdef
data3 dcd 0x19981024
data4 dcd 0xbabeface
      end