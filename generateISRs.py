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

f = open("isr.asm", "w")
f.write("extern generic_handler\n\n")
for i in range(NUM_ISRs):
    f.write(f'global isr{i}\n')
f.write('''
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
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    ;; move error code
    add rsp, 8     
    iretq\n\n'''
)

for i in range(NUM_ISRs):
    f.write(f"isr{i}:\n")
    if i not in ISR_WITH_ERROR_CODE:
        f.write(f"    sub rsp, 8\n")
    f.write(f'''    mov rdi, {i}
    jmp gen_isr\n''')
f.close()


# generate header for each of the interrupt labels
f = open("isr.h", "w")
f.write('''#ifndef ISR_H
#define ISR_H\n''')
for i in range(NUM_ISRs):
    f.write(f"extern void* isr{i};\n")
f.write("#endif\n")
f.close()