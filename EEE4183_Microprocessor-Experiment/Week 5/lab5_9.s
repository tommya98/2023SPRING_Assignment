    area lab5_9,code
    entry
__main proc
	export __main [weak]
start bl func1
      b .
func1 sub lr,#1
	  bx lr
      endp
      end