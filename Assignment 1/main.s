      area projcode, code
      entry
__main proc
      export __main [weak]
	  extern subfunc
start ldrh r0,num1
	  ldrh r1,num2
	  add r0,r1
	  ldr r1,=sum
	  str r0,[r1]
	  ldr r1,=sumascii
	  push {r0,r1}
	  bl subfunc
here  b here
      endp
	  align
num1  dcw 999
num2  dcw 999
	  align
	  area projdata, data
sum   dcd 0
sumascii space 4
      end