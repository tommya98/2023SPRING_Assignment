	  area projcode, code
	  export subfunc
subfunc proc
	  pop{r0,r1}
	  mov r7,r1
	  ldr r1,=1000
      udiv r1,r0,r1
	  cmp r1,#0
	  beq b1
	  ldr r2,=0x31
	  strb r2,[r7]
	  add r7,#1
	  sub r0,#1000
	  ldr r1,=100
	  udiv r1,r0,r1
	  b b11
b1	  ldr r1,=100
	  udiv r1,r0,r1
	  cmp r1,#0
	  beq b2
b11	  ldr r2,=0x30
	  add r2,r1
	  strb r2,[r7]
	  add r7,#1
	  ldr r2,=100
	  mul r1,r2
	  sub r0,r1
	  ldr r1,=10
	  udiv r1,r0,r1
	  b b22
b2    ldr r1,=10
	  udiv r1,r0,r1
	  cmp r1,#0
	  beq b3
b22	  ldr r2,=0x30
	  add r2,r1
	  strb r2,[r7]
	  add r7,#1
	  ldr r2,=10
	  mul r1,r2
	  sub r0,r1
b3    ldr r2,=0x30
	  add r2,r0
	  strb r2,[r7]
fin	  bx lr
      endp
	  end