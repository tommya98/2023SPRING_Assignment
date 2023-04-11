STACK_BASE EQU 0x20000100
ARRAY_BASE EQU 0x20000100
ARRAY_SIZE EQU 0x8
      area lab6_4_1, code
      entry
__main proc
      export __main [weak]
	  extern fill_array
	  extern fill_pointer
start ldr sp,=STACK_BASE
      ldr r2,=ARRAY_BASE
      ldr r3,=ARRAY_SIZE
      ldr r4,=0x5555aaaa
      push {r2-r4}
      bl fill_array
      add sp,#12
;
      ldr r4,=0x77337733
      push {r2-r4}
      bl fill_pointer
      add sp,#12
stop  b stop
      endp
      end