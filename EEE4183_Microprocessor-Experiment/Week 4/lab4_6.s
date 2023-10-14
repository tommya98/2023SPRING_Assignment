      area lab4_6,code
      entry
__main proc
      export __main [weak]
strat ldr r0,data1
      ldr r1,data2
      ldr r3,data3
      ldr r4,data4
	  adds r2,r0,r1
	  adds r5,r3,r4
      b .
      endp
      align
data1 dcd 0x67000000
data2 dcd 0x41000000
data3 dcd 0xB1234567
data4 dcd 0xA0000000
      end