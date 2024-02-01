'''
 Script to create an asm file that
   contains labels isr0 to isr255 for
   handling interrupts
run with `python3 generateISRs.py`
'''
NUM_ISRs = 256
# https://wiki.osdev.org/Exceptions
# https://users.csc.calpoly.edu/~bellardo/courses/2242/454/24593_APM_v21.pdf
# DF, TS, NP, SS, GP, PF, AC
ISR_WITH_ERROR_CODE = [8,10,11,12,13,14,17]

f = open("src/arch/x86_64/isr.asm", "w")
f.write("extern generic_handler\n")
f.write('''
global isr_table
isr_table:
%assign i 0 
%rep    256 
    dq isr%+i ; use DQ for targeting 64-bit
%assign i i+1 
%endrep

gen_isr:
    ;; save volatile 
    ;; https://os.phil-opp.com/handling-exceptions/#preserved-and-scratch-registers 
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    call generic_handler
    
    ;; pop volatile
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    ;; move error code
    add rsp, 8     
    iretq\n\n'''
)

# write each individual isr
for i in range(NUM_ISRs):
    f.write(f"isr{i}:\n")
    if i not in ISR_WITH_ERROR_CODE:
        f.write(f"    sub rsp, 8\n")
    f.write(f'''    mov rdi, {i}
    jmp gen_isr\n''')
f.close()