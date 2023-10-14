    area lab3_3,code
    entry
__main proc
	export __main [weak]
start adr r0,data1
      ldr r1,[r0,#0x00]
	  ldr r2,[r0,#0x01]
	  ldr r3,[r0,#0x02]
	  ldr r4,[r0,#0x03]
      ldrh r5,[r0,#0x00]
	  ldrh r6,[r0,#0x01]
	  ldrh r7,[r0,#0x02]
	  ldrh r8,[r0,#0x03]
	  ;
	  ldr r0,=data2
      str r1,[r0,#0x00]
	  str r2,[r0,#0x01]
	  str r3,[r0,#0x02]
	  str r4,[r0,#0x03]
      strh r5,[r0,#0x00]
	  strh r6,[r0,#0x01]
	  strh r7,[r0,#0x02]
	  strh r8,[r0,#0x03]
      b .
      endp
      align
data1 dcb 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
	  area lab3data,data
data2 space 16
      end