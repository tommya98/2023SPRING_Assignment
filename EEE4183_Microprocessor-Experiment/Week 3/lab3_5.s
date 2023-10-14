    area lab3_5,code
    entry
__main proc
	export __main [weak]
start ldr r0,data1
	  ldr r1,=data3
	  str r0,[r1,#4]
	  str r0,[r1,#8]!
	  ldr r0,data2
	  str r0,[r1],#4
      b .
      endp
      align
data1 dcd 0x12345678
data2 dcd 0xbabeface
      area lab3data,data
data3 space 32
      end