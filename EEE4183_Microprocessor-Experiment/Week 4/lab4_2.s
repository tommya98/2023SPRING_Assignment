      area lab4_2,code
      entry
__main proc
      export __main [weak]
strat
      ldr r0,data3
      asrs r1,r0,#0x3
;	  ldr r0,data3			;ldr r1,data4			;ldr r3,data1			;ldr r4,data2
;	  adds r2,r0,r1			;adc r5,r3,r4
      ldr r0,data2
      mov r1,#0x02
      mul r2,r0,r1
;
      ldr r2,data1
      umull r3,r4,r0,r2
;	  ;ldr r0,data5			;mov r1,#0x02			;sdiv r2,r0,r1			;udiv r2,r0,r1

      b .
      endp
      align
data1 dcd 0x67000005
data2 dcd 0x41000000
data3 dcd 0xb7000005
data4 dcd 0xb1234567
data5 dcd 0xa0000000
      end