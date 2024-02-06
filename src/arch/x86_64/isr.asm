extern generic_handler

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
    pop rsi
    pop rdx
    pop rcx
    pop rax
    pop rdi
    ;; move error code
    add rsp, 8     
    iretq

isr0:
    sub rsp, 8
    push rdi
    mov rdi, 0
    jmp gen_isr
isr1:
    sub rsp, 8
    push rdi
    mov rdi, 1
    jmp gen_isr
isr2:
    sub rsp, 8
    push rdi
    mov rdi, 2
    jmp gen_isr
isr3:
    sub rsp, 8
    push rdi
    mov rdi, 3
    jmp gen_isr
isr4:
    sub rsp, 8
    push rdi
    mov rdi, 4
    jmp gen_isr
isr5:
    sub rsp, 8
    push rdi
    mov rdi, 5
    jmp gen_isr
isr6:
    sub rsp, 8
    push rdi
    mov rdi, 6
    jmp gen_isr
isr7:
    sub rsp, 8
    push rdi
    mov rdi, 7
    jmp gen_isr
isr8:
    push rdi
    mov rdi, 8
    jmp gen_isr
isr9:
    sub rsp, 8
    push rdi
    mov rdi, 9
    jmp gen_isr
isr10:
    push rdi
    mov rdi, 10
    jmp gen_isr
isr11:
    push rdi
    mov rdi, 11
    jmp gen_isr
isr12:
    push rdi
    mov rdi, 12
    jmp gen_isr
isr13:
    push rdi
    mov rdi, 13
    jmp gen_isr
isr14:
    push rdi
    mov rdi, 14
    jmp gen_isr
isr15:
    sub rsp, 8
    push rdi
    mov rdi, 15
    jmp gen_isr
isr16:
    sub rsp, 8
    push rdi
    mov rdi, 16
    jmp gen_isr
isr17:
    push rdi
    mov rdi, 17
    jmp gen_isr
isr18:
    sub rsp, 8
    push rdi
    mov rdi, 18
    jmp gen_isr
isr19:
    sub rsp, 8
    push rdi
    mov rdi, 19
    jmp gen_isr
isr20:
    sub rsp, 8
    push rdi
    mov rdi, 20
    jmp gen_isr
isr21:
    sub rsp, 8
    push rdi
    mov rdi, 21
    jmp gen_isr
isr22:
    sub rsp, 8
    push rdi
    mov rdi, 22
    jmp gen_isr
isr23:
    sub rsp, 8
    push rdi
    mov rdi, 23
    jmp gen_isr
isr24:
    sub rsp, 8
    push rdi
    mov rdi, 24
    jmp gen_isr
isr25:
    sub rsp, 8
    push rdi
    mov rdi, 25
    jmp gen_isr
isr26:
    sub rsp, 8
    push rdi
    mov rdi, 26
    jmp gen_isr
isr27:
    sub rsp, 8
    push rdi
    mov rdi, 27
    jmp gen_isr
isr28:
    sub rsp, 8
    push rdi
    mov rdi, 28
    jmp gen_isr
isr29:
    sub rsp, 8
    push rdi
    mov rdi, 29
    jmp gen_isr
isr30:
    sub rsp, 8
    push rdi
    mov rdi, 30
    jmp gen_isr
isr31:
    sub rsp, 8
    push rdi
    mov rdi, 31
    jmp gen_isr
isr32:
    sub rsp, 8
    push rdi
    mov rdi, 32
    jmp gen_isr
isr33:
    sub rsp, 8
    push rdi
    mov rdi, 33
    jmp gen_isr
isr34:
    sub rsp, 8
    push rdi
    mov rdi, 34
    jmp gen_isr
isr35:
    sub rsp, 8
    push rdi
    mov rdi, 35
    jmp gen_isr
isr36:
    sub rsp, 8
    push rdi
    mov rdi, 36
    jmp gen_isr
isr37:
    sub rsp, 8
    push rdi
    mov rdi, 37
    jmp gen_isr
isr38:
    sub rsp, 8
    push rdi
    mov rdi, 38
    jmp gen_isr
isr39:
    sub rsp, 8
    push rdi
    mov rdi, 39
    jmp gen_isr
isr40:
    sub rsp, 8
    push rdi
    mov rdi, 40
    jmp gen_isr
isr41:
    sub rsp, 8
    push rdi
    mov rdi, 41
    jmp gen_isr
isr42:
    sub rsp, 8
    push rdi
    mov rdi, 42
    jmp gen_isr
isr43:
    sub rsp, 8
    push rdi
    mov rdi, 43
    jmp gen_isr
isr44:
    sub rsp, 8
    push rdi
    mov rdi, 44
    jmp gen_isr
isr45:
    sub rsp, 8
    push rdi
    mov rdi, 45
    jmp gen_isr
isr46:
    sub rsp, 8
    push rdi
    mov rdi, 46
    jmp gen_isr
isr47:
    sub rsp, 8
    push rdi
    mov rdi, 47
    jmp gen_isr
isr48:
    sub rsp, 8
    push rdi
    mov rdi, 48
    jmp gen_isr
isr49:
    sub rsp, 8
    push rdi
    mov rdi, 49
    jmp gen_isr
isr50:
    sub rsp, 8
    push rdi
    mov rdi, 50
    jmp gen_isr
isr51:
    sub rsp, 8
    push rdi
    mov rdi, 51
    jmp gen_isr
isr52:
    sub rsp, 8
    push rdi
    mov rdi, 52
    jmp gen_isr
isr53:
    sub rsp, 8
    push rdi
    mov rdi, 53
    jmp gen_isr
isr54:
    sub rsp, 8
    push rdi
    mov rdi, 54
    jmp gen_isr
isr55:
    sub rsp, 8
    push rdi
    mov rdi, 55
    jmp gen_isr
isr56:
    sub rsp, 8
    push rdi
    mov rdi, 56
    jmp gen_isr
isr57:
    sub rsp, 8
    push rdi
    mov rdi, 57
    jmp gen_isr
isr58:
    sub rsp, 8
    push rdi
    mov rdi, 58
    jmp gen_isr
isr59:
    sub rsp, 8
    push rdi
    mov rdi, 59
    jmp gen_isr
isr60:
    sub rsp, 8
    push rdi
    mov rdi, 60
    jmp gen_isr
isr61:
    sub rsp, 8
    push rdi
    mov rdi, 61
    jmp gen_isr
isr62:
    sub rsp, 8
    push rdi
    mov rdi, 62
    jmp gen_isr
isr63:
    sub rsp, 8
    push rdi
    mov rdi, 63
    jmp gen_isr
isr64:
    sub rsp, 8
    push rdi
    mov rdi, 64
    jmp gen_isr
isr65:
    sub rsp, 8
    push rdi
    mov rdi, 65
    jmp gen_isr
isr66:
    sub rsp, 8
    push rdi
    mov rdi, 66
    jmp gen_isr
isr67:
    sub rsp, 8
    push rdi
    mov rdi, 67
    jmp gen_isr
isr68:
    sub rsp, 8
    push rdi
    mov rdi, 68
    jmp gen_isr
isr69:
    sub rsp, 8
    push rdi
    mov rdi, 69
    jmp gen_isr
isr70:
    sub rsp, 8
    push rdi
    mov rdi, 70
    jmp gen_isr
isr71:
    sub rsp, 8
    push rdi
    mov rdi, 71
    jmp gen_isr
isr72:
    sub rsp, 8
    push rdi
    mov rdi, 72
    jmp gen_isr
isr73:
    sub rsp, 8
    push rdi
    mov rdi, 73
    jmp gen_isr
isr74:
    sub rsp, 8
    push rdi
    mov rdi, 74
    jmp gen_isr
isr75:
    sub rsp, 8
    push rdi
    mov rdi, 75
    jmp gen_isr
isr76:
    sub rsp, 8
    push rdi
    mov rdi, 76
    jmp gen_isr
isr77:
    sub rsp, 8
    push rdi
    mov rdi, 77
    jmp gen_isr
isr78:
    sub rsp, 8
    push rdi
    mov rdi, 78
    jmp gen_isr
isr79:
    sub rsp, 8
    push rdi
    mov rdi, 79
    jmp gen_isr
isr80:
    sub rsp, 8
    push rdi
    mov rdi, 80
    jmp gen_isr
isr81:
    sub rsp, 8
    push rdi
    mov rdi, 81
    jmp gen_isr
isr82:
    sub rsp, 8
    push rdi
    mov rdi, 82
    jmp gen_isr
isr83:
    sub rsp, 8
    push rdi
    mov rdi, 83
    jmp gen_isr
isr84:
    sub rsp, 8
    push rdi
    mov rdi, 84
    jmp gen_isr
isr85:
    sub rsp, 8
    push rdi
    mov rdi, 85
    jmp gen_isr
isr86:
    sub rsp, 8
    push rdi
    mov rdi, 86
    jmp gen_isr
isr87:
    sub rsp, 8
    push rdi
    mov rdi, 87
    jmp gen_isr
isr88:
    sub rsp, 8
    push rdi
    mov rdi, 88
    jmp gen_isr
isr89:
    sub rsp, 8
    push rdi
    mov rdi, 89
    jmp gen_isr
isr90:
    sub rsp, 8
    push rdi
    mov rdi, 90
    jmp gen_isr
isr91:
    sub rsp, 8
    push rdi
    mov rdi, 91
    jmp gen_isr
isr92:
    sub rsp, 8
    push rdi
    mov rdi, 92
    jmp gen_isr
isr93:
    sub rsp, 8
    push rdi
    mov rdi, 93
    jmp gen_isr
isr94:
    sub rsp, 8
    push rdi
    mov rdi, 94
    jmp gen_isr
isr95:
    sub rsp, 8
    push rdi
    mov rdi, 95
    jmp gen_isr
isr96:
    sub rsp, 8
    push rdi
    mov rdi, 96
    jmp gen_isr
isr97:
    sub rsp, 8
    push rdi
    mov rdi, 97
    jmp gen_isr
isr98:
    sub rsp, 8
    push rdi
    mov rdi, 98
    jmp gen_isr
isr99:
    sub rsp, 8
    push rdi
    mov rdi, 99
    jmp gen_isr
isr100:
    sub rsp, 8
    push rdi
    mov rdi, 100
    jmp gen_isr
isr101:
    sub rsp, 8
    push rdi
    mov rdi, 101
    jmp gen_isr
isr102:
    sub rsp, 8
    push rdi
    mov rdi, 102
    jmp gen_isr
isr103:
    sub rsp, 8
    push rdi
    mov rdi, 103
    jmp gen_isr
isr104:
    sub rsp, 8
    push rdi
    mov rdi, 104
    jmp gen_isr
isr105:
    sub rsp, 8
    push rdi
    mov rdi, 105
    jmp gen_isr
isr106:
    sub rsp, 8
    push rdi
    mov rdi, 106
    jmp gen_isr
isr107:
    sub rsp, 8
    push rdi
    mov rdi, 107
    jmp gen_isr
isr108:
    sub rsp, 8
    push rdi
    mov rdi, 108
    jmp gen_isr
isr109:
    sub rsp, 8
    push rdi
    mov rdi, 109
    jmp gen_isr
isr110:
    sub rsp, 8
    push rdi
    mov rdi, 110
    jmp gen_isr
isr111:
    sub rsp, 8
    push rdi
    mov rdi, 111
    jmp gen_isr
isr112:
    sub rsp, 8
    push rdi
    mov rdi, 112
    jmp gen_isr
isr113:
    sub rsp, 8
    push rdi
    mov rdi, 113
    jmp gen_isr
isr114:
    sub rsp, 8
    push rdi
    mov rdi, 114
    jmp gen_isr
isr115:
    sub rsp, 8
    push rdi
    mov rdi, 115
    jmp gen_isr
isr116:
    sub rsp, 8
    push rdi
    mov rdi, 116
    jmp gen_isr
isr117:
    sub rsp, 8
    push rdi
    mov rdi, 117
    jmp gen_isr
isr118:
    sub rsp, 8
    push rdi
    mov rdi, 118
    jmp gen_isr
isr119:
    sub rsp, 8
    push rdi
    mov rdi, 119
    jmp gen_isr
isr120:
    sub rsp, 8
    push rdi
    mov rdi, 120
    jmp gen_isr
isr121:
    sub rsp, 8
    push rdi
    mov rdi, 121
    jmp gen_isr
isr122:
    sub rsp, 8
    push rdi
    mov rdi, 122
    jmp gen_isr
isr123:
    sub rsp, 8
    push rdi
    mov rdi, 123
    jmp gen_isr
isr124:
    sub rsp, 8
    push rdi
    mov rdi, 124
    jmp gen_isr
isr125:
    sub rsp, 8
    push rdi
    mov rdi, 125
    jmp gen_isr
isr126:
    sub rsp, 8
    push rdi
    mov rdi, 126
    jmp gen_isr
isr127:
    sub rsp, 8
    push rdi
    mov rdi, 127
    jmp gen_isr
isr128:
    sub rsp, 8
    push rdi
    mov rdi, 128
    jmp gen_isr
isr129:
    sub rsp, 8
    push rdi
    mov rdi, 129
    jmp gen_isr
isr130:
    sub rsp, 8
    push rdi
    mov rdi, 130
    jmp gen_isr
isr131:
    sub rsp, 8
    push rdi
    mov rdi, 131
    jmp gen_isr
isr132:
    sub rsp, 8
    push rdi
    mov rdi, 132
    jmp gen_isr
isr133:
    sub rsp, 8
    push rdi
    mov rdi, 133
    jmp gen_isr
isr134:
    sub rsp, 8
    push rdi
    mov rdi, 134
    jmp gen_isr
isr135:
    sub rsp, 8
    push rdi
    mov rdi, 135
    jmp gen_isr
isr136:
    sub rsp, 8
    push rdi
    mov rdi, 136
    jmp gen_isr
isr137:
    sub rsp, 8
    push rdi
    mov rdi, 137
    jmp gen_isr
isr138:
    sub rsp, 8
    push rdi
    mov rdi, 138
    jmp gen_isr
isr139:
    sub rsp, 8
    push rdi
    mov rdi, 139
    jmp gen_isr
isr140:
    sub rsp, 8
    push rdi
    mov rdi, 140
    jmp gen_isr
isr141:
    sub rsp, 8
    push rdi
    mov rdi, 141
    jmp gen_isr
isr142:
    sub rsp, 8
    push rdi
    mov rdi, 142
    jmp gen_isr
isr143:
    sub rsp, 8
    push rdi
    mov rdi, 143
    jmp gen_isr
isr144:
    sub rsp, 8
    push rdi
    mov rdi, 144
    jmp gen_isr
isr145:
    sub rsp, 8
    push rdi
    mov rdi, 145
    jmp gen_isr
isr146:
    sub rsp, 8
    push rdi
    mov rdi, 146
    jmp gen_isr
isr147:
    sub rsp, 8
    push rdi
    mov rdi, 147
    jmp gen_isr
isr148:
    sub rsp, 8
    push rdi
    mov rdi, 148
    jmp gen_isr
isr149:
    sub rsp, 8
    push rdi
    mov rdi, 149
    jmp gen_isr
isr150:
    sub rsp, 8
    push rdi
    mov rdi, 150
    jmp gen_isr
isr151:
    sub rsp, 8
    push rdi
    mov rdi, 151
    jmp gen_isr
isr152:
    sub rsp, 8
    push rdi
    mov rdi, 152
    jmp gen_isr
isr153:
    sub rsp, 8
    push rdi
    mov rdi, 153
    jmp gen_isr
isr154:
    sub rsp, 8
    push rdi
    mov rdi, 154
    jmp gen_isr
isr155:
    sub rsp, 8
    push rdi
    mov rdi, 155
    jmp gen_isr
isr156:
    sub rsp, 8
    push rdi
    mov rdi, 156
    jmp gen_isr
isr157:
    sub rsp, 8
    push rdi
    mov rdi, 157
    jmp gen_isr
isr158:
    sub rsp, 8
    push rdi
    mov rdi, 158
    jmp gen_isr
isr159:
    sub rsp, 8
    push rdi
    mov rdi, 159
    jmp gen_isr
isr160:
    sub rsp, 8
    push rdi
    mov rdi, 160
    jmp gen_isr
isr161:
    sub rsp, 8
    push rdi
    mov rdi, 161
    jmp gen_isr
isr162:
    sub rsp, 8
    push rdi
    mov rdi, 162
    jmp gen_isr
isr163:
    sub rsp, 8
    push rdi
    mov rdi, 163
    jmp gen_isr
isr164:
    sub rsp, 8
    push rdi
    mov rdi, 164
    jmp gen_isr
isr165:
    sub rsp, 8
    push rdi
    mov rdi, 165
    jmp gen_isr
isr166:
    sub rsp, 8
    push rdi
    mov rdi, 166
    jmp gen_isr
isr167:
    sub rsp, 8
    push rdi
    mov rdi, 167
    jmp gen_isr
isr168:
    sub rsp, 8
    push rdi
    mov rdi, 168
    jmp gen_isr
isr169:
    sub rsp, 8
    push rdi
    mov rdi, 169
    jmp gen_isr
isr170:
    sub rsp, 8
    push rdi
    mov rdi, 170
    jmp gen_isr
isr171:
    sub rsp, 8
    push rdi
    mov rdi, 171
    jmp gen_isr
isr172:
    sub rsp, 8
    push rdi
    mov rdi, 172
    jmp gen_isr
isr173:
    sub rsp, 8
    push rdi
    mov rdi, 173
    jmp gen_isr
isr174:
    sub rsp, 8
    push rdi
    mov rdi, 174
    jmp gen_isr
isr175:
    sub rsp, 8
    push rdi
    mov rdi, 175
    jmp gen_isr
isr176:
    sub rsp, 8
    push rdi
    mov rdi, 176
    jmp gen_isr
isr177:
    sub rsp, 8
    push rdi
    mov rdi, 177
    jmp gen_isr
isr178:
    sub rsp, 8
    push rdi
    mov rdi, 178
    jmp gen_isr
isr179:
    sub rsp, 8
    push rdi
    mov rdi, 179
    jmp gen_isr
isr180:
    sub rsp, 8
    push rdi
    mov rdi, 180
    jmp gen_isr
isr181:
    sub rsp, 8
    push rdi
    mov rdi, 181
    jmp gen_isr
isr182:
    sub rsp, 8
    push rdi
    mov rdi, 182
    jmp gen_isr
isr183:
    sub rsp, 8
    push rdi
    mov rdi, 183
    jmp gen_isr
isr184:
    sub rsp, 8
    push rdi
    mov rdi, 184
    jmp gen_isr
isr185:
    sub rsp, 8
    push rdi
    mov rdi, 185
    jmp gen_isr
isr186:
    sub rsp, 8
    push rdi
    mov rdi, 186
    jmp gen_isr
isr187:
    sub rsp, 8
    push rdi
    mov rdi, 187
    jmp gen_isr
isr188:
    sub rsp, 8
    push rdi
    mov rdi, 188
    jmp gen_isr
isr189:
    sub rsp, 8
    push rdi
    mov rdi, 189
    jmp gen_isr
isr190:
    sub rsp, 8
    push rdi
    mov rdi, 190
    jmp gen_isr
isr191:
    sub rsp, 8
    push rdi
    mov rdi, 191
    jmp gen_isr
isr192:
    sub rsp, 8
    push rdi
    mov rdi, 192
    jmp gen_isr
isr193:
    sub rsp, 8
    push rdi
    mov rdi, 193
    jmp gen_isr
isr194:
    sub rsp, 8
    push rdi
    mov rdi, 194
    jmp gen_isr
isr195:
    sub rsp, 8
    push rdi
    mov rdi, 195
    jmp gen_isr
isr196:
    sub rsp, 8
    push rdi
    mov rdi, 196
    jmp gen_isr
isr197:
    sub rsp, 8
    push rdi
    mov rdi, 197
    jmp gen_isr
isr198:
    sub rsp, 8
    push rdi
    mov rdi, 198
    jmp gen_isr
isr199:
    sub rsp, 8
    push rdi
    mov rdi, 199
    jmp gen_isr
isr200:
    sub rsp, 8
    push rdi
    mov rdi, 200
    jmp gen_isr
isr201:
    sub rsp, 8
    push rdi
    mov rdi, 201
    jmp gen_isr
isr202:
    sub rsp, 8
    push rdi
    mov rdi, 202
    jmp gen_isr
isr203:
    sub rsp, 8
    push rdi
    mov rdi, 203
    jmp gen_isr
isr204:
    sub rsp, 8
    push rdi
    mov rdi, 204
    jmp gen_isr
isr205:
    sub rsp, 8
    push rdi
    mov rdi, 205
    jmp gen_isr
isr206:
    sub rsp, 8
    push rdi
    mov rdi, 206
    jmp gen_isr
isr207:
    sub rsp, 8
    push rdi
    mov rdi, 207
    jmp gen_isr
isr208:
    sub rsp, 8
    push rdi
    mov rdi, 208
    jmp gen_isr
isr209:
    sub rsp, 8
    push rdi
    mov rdi, 209
    jmp gen_isr
isr210:
    sub rsp, 8
    push rdi
    mov rdi, 210
    jmp gen_isr
isr211:
    sub rsp, 8
    push rdi
    mov rdi, 211
    jmp gen_isr
isr212:
    sub rsp, 8
    push rdi
    mov rdi, 212
    jmp gen_isr
isr213:
    sub rsp, 8
    push rdi
    mov rdi, 213
    jmp gen_isr
isr214:
    sub rsp, 8
    push rdi
    mov rdi, 214
    jmp gen_isr
isr215:
    sub rsp, 8
    push rdi
    mov rdi, 215
    jmp gen_isr
isr216:
    sub rsp, 8
    push rdi
    mov rdi, 216
    jmp gen_isr
isr217:
    sub rsp, 8
    push rdi
    mov rdi, 217
    jmp gen_isr
isr218:
    sub rsp, 8
    push rdi
    mov rdi, 218
    jmp gen_isr
isr219:
    sub rsp, 8
    push rdi
    mov rdi, 219
    jmp gen_isr
isr220:
    sub rsp, 8
    push rdi
    mov rdi, 220
    jmp gen_isr
isr221:
    sub rsp, 8
    push rdi
    mov rdi, 221
    jmp gen_isr
isr222:
    sub rsp, 8
    push rdi
    mov rdi, 222
    jmp gen_isr
isr223:
    sub rsp, 8
    push rdi
    mov rdi, 223
    jmp gen_isr
isr224:
    sub rsp, 8
    push rdi
    mov rdi, 224
    jmp gen_isr
isr225:
    sub rsp, 8
    push rdi
    mov rdi, 225
    jmp gen_isr
isr226:
    sub rsp, 8
    push rdi
    mov rdi, 226
    jmp gen_isr
isr227:
    sub rsp, 8
    push rdi
    mov rdi, 227
    jmp gen_isr
isr228:
    sub rsp, 8
    push rdi
    mov rdi, 228
    jmp gen_isr
isr229:
    sub rsp, 8
    push rdi
    mov rdi, 229
    jmp gen_isr
isr230:
    sub rsp, 8
    push rdi
    mov rdi, 230
    jmp gen_isr
isr231:
    sub rsp, 8
    push rdi
    mov rdi, 231
    jmp gen_isr
isr232:
    sub rsp, 8
    push rdi
    mov rdi, 232
    jmp gen_isr
isr233:
    sub rsp, 8
    push rdi
    mov rdi, 233
    jmp gen_isr
isr234:
    sub rsp, 8
    push rdi
    mov rdi, 234
    jmp gen_isr
isr235:
    sub rsp, 8
    push rdi
    mov rdi, 235
    jmp gen_isr
isr236:
    sub rsp, 8
    push rdi
    mov rdi, 236
    jmp gen_isr
isr237:
    sub rsp, 8
    push rdi
    mov rdi, 237
    jmp gen_isr
isr238:
    sub rsp, 8
    push rdi
    mov rdi, 238
    jmp gen_isr
isr239:
    sub rsp, 8
    push rdi
    mov rdi, 239
    jmp gen_isr
isr240:
    sub rsp, 8
    push rdi
    mov rdi, 240
    jmp gen_isr
isr241:
    sub rsp, 8
    push rdi
    mov rdi, 241
    jmp gen_isr
isr242:
    sub rsp, 8
    push rdi
    mov rdi, 242
    jmp gen_isr
isr243:
    sub rsp, 8
    push rdi
    mov rdi, 243
    jmp gen_isr
isr244:
    sub rsp, 8
    push rdi
    mov rdi, 244
    jmp gen_isr
isr245:
    sub rsp, 8
    push rdi
    mov rdi, 245
    jmp gen_isr
isr246:
    sub rsp, 8
    push rdi
    mov rdi, 246
    jmp gen_isr
isr247:
    sub rsp, 8
    push rdi
    mov rdi, 247
    jmp gen_isr
isr248:
    sub rsp, 8
    push rdi
    mov rdi, 248
    jmp gen_isr
isr249:
    sub rsp, 8
    push rdi
    mov rdi, 249
    jmp gen_isr
isr250:
    sub rsp, 8
    push rdi
    mov rdi, 250
    jmp gen_isr
isr251:
    sub rsp, 8
    push rdi
    mov rdi, 251
    jmp gen_isr
isr252:
    sub rsp, 8
    push rdi
    mov rdi, 252
    jmp gen_isr
isr253:
    sub rsp, 8
    push rdi
    mov rdi, 253
    jmp gen_isr
isr254:
    sub rsp, 8
    push rdi
    mov rdi, 254
    jmp gen_isr
isr255:
    sub rsp, 8
    push rdi
    mov rdi, 255
    jmp gen_isr
