      area lab5_8,code
      entry
__main proc
      export __main [weak]
start ldr r0,data1
	  mov r2,#10000
	  mov r8,#0
	  udiv r1,r0,r2
	  sub r1,#1
year1 cmp r1,#400
	  blt year2
      subs r1,#400
	  addmi r1,#400
	  b year1
year2 cmp r1,#300
      addge r8,#1
	  subge r1,#300
	  bge year3
	  cmp r1,#200
	  addge r8,#3
	  subge r1,#200
	  bge year3
	  cmp r1,#100
	  addge r8,#5
	  subge r1,#100
year3 add r8,r1
	  mov r2,#4
      udiv r3,r1,r2
	  add r8,r3
	  
      mov r2,#100
	  udiv r1,r0,r2
	  mov r2,#10000
	  udiv r3,r0,r2
	  mov r2,#100
	  mul r3,r2
	  sub r1,r3 ;r1 month
	  adr r4,table
	  sub r2,r1,#1
	  tbb [r4,r2]
jan   add r8,#0
	  b day
feb   add r8,#3
	  b day	 
mar   add r8,#3
	  b leap
apr   add r8,#6
	  b leap
may   add r8,#8
	  b leap
jun   add r8,#11
	  b leap
jul   add r8,#13
	  b leap
aug   add r8,#16
	  b leap
sep   add r8,#19
	  b leap
oct   add r8,#21
	  b leap
nov   add r8,#24
	  b leap
dec   add r8,#26
	  b leap
leap  mov r2,#10000
	  udiv r3,r0,r2
loop  mov r2,#4
	  udiv r4,r3,r2
	  mul r4,r2
	  cmp r4,r3
	  bne day
	  add r8,#1
	  mov r2,#100
	  udiv r4,r3,r2
	  mul r4,r2
	  cmp r4,r3
	  bne day
	  sub r8,#1
	  mov r2,#400
	  udiv r4,r3,r2
	  mul r4,r2
	  cmp r4,r3
	  bne day
	  add r8,#1
day   mov r2,#100
	  mul r1,r2
	  mov r2,#10000
	  udiv r3,r0,r2	  
	  mul r3,r2
	  sub r0,r1
	  sub r0,r3
	  add r8,r0
dodiv mov r2,#7
	  udiv r7,r8,r2
	  mul r7,r2
	  sub r8,r7
	  b .
	  align
table dcb 0
      dcb ((feb - jan)/2)
	  dcb ((mar - jan)/2)
	  dcb ((apr - jan)/2)
	  dcb ((may - jan)/2)
	  dcb ((jun - jan)/2)
	  dcb ((jul - jan)/2)
	  dcb ((aug - jan)/2)
	  dcb ((sep - jan)/2)
	  dcb ((oct - jan)/2)
	  dcb ((nov - jan)/2)
	  dcb ((dec - jan)/2)
      endp
data1 dcd 20230410
      end