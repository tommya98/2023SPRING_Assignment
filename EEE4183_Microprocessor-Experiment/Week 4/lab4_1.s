      area lab4_1,code
      entry
__main proc
      export __main [weak]
strat ldrb r0,data1
      ldrb r1,data2
      ldrb r3,mask1
      ldrb r4,mask2
      ands r5,r0,r3
;     tst r0,r3
      orr r0,r0,#0x08
      and r5,r0,r4
;
      eor r6,r0,r3
;     eors r6,r0,#2_10101111
      teq r0,#2_10101111
      eor r6,r0,r0
;
      ldr r7,=ramdata
      strb r0,[r7,#0x6]
;     ;ldr r2,[r7,#0x6]             ;and r6,r2,r4			;strb r6,[r7,#0x6]
;	  ;ldr r8,=0x22000000           ;mov r9,#0x0			;str r9,[r8,#0xCC]
      ldr r0,data3
      bic r6,r0,#0x0f0
;
      ldr r5,data3
      ssat r6,#12,r5				;mrs r7,apsr			;bic r7,#0x08000000			;msr apsr,r7
      b .
      endp
      align
data1 dcb 2_10110011                ;data1 dcb '8'
data2 dcb 2_10100101
      align
data3 dcd 0x00011234
mask1 dcb 2_00010000
mask2 dcb 2_11110111
      align
      area lab4data,data
ramdata space 16
      end