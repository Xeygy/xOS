extern syscall_handler

;; code for saving registers in syscall
global syscall_isr
syscall_isr:
    sub rsp, 8
    push rdi
    push rax
    push rcx
    push rdx
    push rsi
    push r8
    push r9
    push r10
    push r11
    call syscall_handler
    
    ;; pop volatile
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdx
    pop rcx
    pop rax
    pop rdi
    ;; move error code
    add rsp, 8     
    iretq