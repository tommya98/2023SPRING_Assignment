SCB_SHCSR equ 0xE000ED24
SCB_CCR equ 0xE000ED14
      export UsageFault_Handler
      area lab7_3, code
      entry
__main proc
      export __main [weak]
	  ldr r0,=SCB_SHCSR
	  ldr r1,=0x00040000
	  str r1,[r0]
	  ldr r0,=SCB_CCR
	  ldr r1,=0x00000010
	  str r1,[r0]
	  
	  mov r0,#10
	  mov r1,#0
	  udiv r0,r1
	  b .
	  endp
UsageFault_Handler proc
	  mov r0,#0
	  bx lr
      endp
      end