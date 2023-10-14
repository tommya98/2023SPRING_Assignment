 	area lab5_6,code
 	entry
__main proc
 	export __main [weak]
start ldr r0,=data2
	  adr r1,data1
	  ldm r1,{r2-r6}
	  stm r0,{r2-r6}
	  mov r1, #5
	  
loop1 mov r2,#0
	  mov r3,#0
loop2 add r4,r2,#1
      cmp r4,r1
	  beq check
	  ldr r5,[r0,r2,lsl #2]
	  ldr r6,[r0,r4,lsl #2]
	  cmp r5,r6
	  strgt r6,[r0,r2,lsl #2]
	  strgt r5,[r0,r4,lsl #2]
	  addgt r3,#1
	  mov r2,r4
	  b loop2
check cmp r3,#0
	  subgt	r1,#1
	  bgt loop1

		b .
		align
data1 dcd 0x00000007, 0x00000004, 0x00000005, 0x00000001, 0x00000003
 	endp
	align
	area lab5data,data
data2 space 16
	end