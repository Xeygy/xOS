;; adapted from pnico's cpe453

global swap_rfiles
swap_rfiles:
	;; void swap_rfiles(rfile *old, rfile  *new)
	;;
	;; "old" will be in rdi
	;; "new" will be in rsi
	;;
	push rbp		;; set up a frame pointer
	mov rbp, rsp
	
	;; save the old context (if old != NULL)
	cmp	rdi, 0
	je load

	mov [rdi], rax	;; store rax into old->rax so we can use it

	;; Now store the Floating Point State
	;; lea rax, [rdi+128]	;; get the address
	;;fxsave rax

	mov [rdi+8], rbx	;; now the rest of the registers
	mov [rdi+16], rcx	;; etc.
	mov [rdi+24], rdx
	mov [rdi+32], rsi
	mov [rdi+40], rdi
	mov [rdi+48], rbp	
	mov [rdi+56], rsp
	mov [rdi+64], r8
	mov [rdi+72], r9
	mov [rdi+80], r10
	mov [rdi+88], r11
	mov [rdi+96], r12
	mov [rdi+104], r13
	mov [rdi+112], r14
	mov [rdi+120], r15

	pushfq
	pop QWORD [rdi+128]

	;; load the new one (if new != NULL)
load:	cmp rsi, 0
	je done

	;; First restore the Floating Point State
	;;lea rax, [rsi+128]	;; get the address
	;;fxrstor (rax)
	
	mov  rax, [rsi]	;; retreive rax from new->rax
	mov  rbx, [rsi+8]	;; etc.
	mov  rcx, [rsi+16]
	mov  rdx, [rsi+24]
	mov  rdi, [rsi+40]
	mov  rbp, [rsi+48]
	mov  rsp, [rsi+56]
	mov  r8, [rsi+64] 
	mov  r9, [rsi+72] 
	mov  r10, [rsi+80]
	mov  r11, [rsi+88]
	mov  r12, [rsi+96]
	mov r13, [rsi+104]
	mov r14, [rsi+112]
	mov r15, [rsi+120]
	push QWORD [rsi+128]
	popfq

	mov  rsi, [rsi+32]	;; must do rsi since, last it's our pointer

done:	leave
	ret
	