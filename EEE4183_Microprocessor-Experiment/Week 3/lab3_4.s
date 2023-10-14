    area lab3_4,code,align=7
    entry
__main proc
	export __main [weak]
	  adr r0,data1
	  ldm r0,{r1,r2}
	  ldr r3,=data2
	  stm r3,{r1,r2}
	  ;
	  ldrex r4,[r0]
	  strex r5,r4,[r3]
	  b .
	  endp
	  align
data1 dcd 0x01234567, 0x89abcdef
      area lab3data,data
data2 space 16
      end