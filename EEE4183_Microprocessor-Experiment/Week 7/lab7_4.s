NVIC_ISER0 equ 0xE000E100
NVIC_ISER1 equ 0xE000E104

	export EXTI0_IRQHandler
	export EXTI1_IRQHandler
	
	
; 메인 루프에서 인터럽트 14 호출
        area lab7_additional_2, code
        entry
__main proc
        export __main [weak]
			
	; 전역 인터럽트 활성화
	CPSIE I

	; 인터럽트 13 및 14 활성화
	LDR R0, =NVIC_ISER0 ; NVIC의 ISER 레지스터 주소를 R0에 로드
	MOV R1, #0
	MOV R1, #(1 << 6)   ; 인터럽트 13의 비트 마스크 설정
	ORR R1, R1, #(1 << 7) ; 인터럽트 14의 비트 마스크를 OR 연산으로 추가
	STR R1, [R0]          ; ISER 레지스터에 활성화할 인터럽트 비트마스크를 쓰기

	; NVIC컨트롤러에서 EXTI Line1의 우선순위를 8로 변경
	LDR R0, =0xE000E404
	MOV R1, #0x80000000
	STR R1, [R0]

    ; 인터럽트 14를 소프트웨어로 호출
    LDR R0, =0xE000EF00 ; NVIC의 STIR 레지스터 주소를 R0에 로드
    MOV R1, #7         ; 인터럽트 14의 번호를 R1에 로드
    STR R1, [R0]        ; STIR 레지스터에 인터럽트 번호를 쓰기

    B .         ; 무한 루프
	endp
		
; 인터럽트 14 핸들러
EXTI1_IRQHandler proc

    ; bit 3를 설정
	LDR R5, =0x2200000C
	MOV R4, #1
	
    ; 인터럽트 13 호출
    LDR R0, =0xE000E200 ; NVIC의 STIR 레지스터 주소를 R0에 로드
    MOV R1, #(1<<6)     ; 인터럽트 13의 번호를 R1에 로드
    STR R1, [R0]        ; STIR 레지스터에 인터럽트 번호를 쓰기

	STR R4, [R5]

    BX LR ; 인터럽트 핸들러 종료
	endp
		 
		 
	
; 인터럽트 13 핸들러
EXTI0_IRQHandler proc
    ; bit 4를 설정
    LDR R0, =0x20000000 ; 데이터 주소를 R0에 로드
    LDR R1, [R0]        ; R0 주소의 데이터를 R1에 로드
    ORR R1, R1, #(1 << 4) ; R1의 bit 4를 set
    STR R1, [R0]        ; R1의 데이터를 R0 주소에 저장

    BX LR               ; 인터럽트 핸들러 종료
	endp