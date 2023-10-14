      area lab4_7,code
      entry
__main proc
      export __main [weak]
strat ldr r0,data1
      ldr r1,data2
	  ldr r2,data3
	  mla r3,r0,r1,r2
	  ldr r4,data4
	  ldr r5,data5
	  umlal r4,r5,r1,r2
      b .
      endp
      align
data1 dcd 0x00004000
data2 dcd 0x00000003
data3 dcd 0x00006000
data4 dcd 0x00001234
data5 dcd 0x00005678
      end