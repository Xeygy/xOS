extern generic_handler

global isr0

gen_isr:
    ;; save volatile 
    ;; https://os.phil-opp.com/handling-exceptions/#preserved-and-scratch-registers 
    push $rax
    push $rcx
    push $rdx
    push $rsi
    push $rdi
    push $r8
    push $r9
    push $r10
    push $r11
    call generic_handler
    
    ;; pop volatile
    push $r11
    push $r10
    push $r9
    push $r8
    push $rdi
    push $rsi
    push $rdx
    push $rcx
    push $rax
    ;; move error code
    add $rsp, 8     
    iretq

isr0:
    sub $rsp, 8
    mov $rdi, 0
    jmp gen_isr
    