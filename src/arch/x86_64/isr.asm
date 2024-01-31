extern generic_handler

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
global isr32
global isr33
global isr34
global isr35
global isr36
global isr37
global isr38
global isr39
global isr40
global isr41
global isr42
global isr43
global isr44
global isr45
global isr46
global isr47
global isr48
global isr49
global isr50
global isr51
global isr52
global isr53
global isr54
global isr55
global isr56
global isr57
global isr58
global isr59
global isr60
global isr61
global isr62
global isr63
global isr64
global isr65
global isr66
global isr67
global isr68
global isr69
global isr70
global isr71
global isr72
global isr73
global isr74
global isr75
global isr76
global isr77
global isr78
global isr79
global isr80
global isr81
global isr82
global isr83
global isr84
global isr85
global isr86
global isr87
global isr88
global isr89
global isr90
global isr91
global isr92
global isr93
global isr94
global isr95
global isr96
global isr97
global isr98
global isr99
global isr100
global isr101
global isr102
global isr103
global isr104
global isr105
global isr106
global isr107
global isr108
global isr109
global isr110
global isr111
global isr112
global isr113
global isr114
global isr115
global isr116
global isr117
global isr118
global isr119
global isr120
global isr121
global isr122
global isr123
global isr124
global isr125
global isr126
global isr127
global isr128
global isr129
global isr130
global isr131
global isr132
global isr133
global isr134
global isr135
global isr136
global isr137
global isr138
global isr139
global isr140
global isr141
global isr142
global isr143
global isr144
global isr145
global isr146
global isr147
global isr148
global isr149
global isr150
global isr151
global isr152
global isr153
global isr154
global isr155
global isr156
global isr157
global isr158
global isr159
global isr160
global isr161
global isr162
global isr163
global isr164
global isr165
global isr166
global isr167
global isr168
global isr169
global isr170
global isr171
global isr172
global isr173
global isr174
global isr175
global isr176
global isr177
global isr178
global isr179
global isr180
global isr181
global isr182
global isr183
global isr184
global isr185
global isr186
global isr187
global isr188
global isr189
global isr190
global isr191
global isr192
global isr193
global isr194
global isr195
global isr196
global isr197
global isr198
global isr199
global isr200
global isr201
global isr202
global isr203
global isr204
global isr205
global isr206
global isr207
global isr208
global isr209
global isr210
global isr211
global isr212
global isr213
global isr214
global isr215
global isr216
global isr217
global isr218
global isr219
global isr220
global isr221
global isr222
global isr223
global isr224
global isr225
global isr226
global isr227
global isr228
global isr229
global isr230
global isr231
global isr232
global isr233
global isr234
global isr235
global isr236
global isr237
global isr238
global isr239
global isr240
global isr241
global isr242
global isr243
global isr244
global isr245
global isr246
global isr247
global isr248
global isr249
global isr250
global isr251
global isr252
global isr253
global isr254
global isr255

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
    iretq

isr0:
    sub rsp, 8
    mov rdi, 0
    jmp gen_isr
isr1:
    sub rsp, 8
    mov rdi, 1
    jmp gen_isr
isr2:
    sub rsp, 8
    mov rdi, 2
    jmp gen_isr
isr3:
    sub rsp, 8
    mov rdi, 3
    jmp gen_isr
isr4:
    sub rsp, 8
    mov rdi, 4
    jmp gen_isr
isr5:
    sub rsp, 8
    mov rdi, 5
    jmp gen_isr
isr6:
    sub rsp, 8
    mov rdi, 6
    jmp gen_isr
isr7:
    sub rsp, 8
    mov rdi, 7
    jmp gen_isr
isr8:
    mov rdi, 8
    jmp gen_isr
isr9:
    sub rsp, 8
    mov rdi, 9
    jmp gen_isr
isr10:
    mov rdi, 10
    jmp gen_isr
isr11:
    mov rdi, 11
    jmp gen_isr
isr12:
    mov rdi, 12
    jmp gen_isr
isr13:
    mov rdi, 13
    jmp gen_isr
isr14:
    mov rdi, 14
    jmp gen_isr
isr15:
    sub rsp, 8
    mov rdi, 15
    jmp gen_isr
isr16:
    sub rsp, 8
    mov rdi, 16
    jmp gen_isr
isr17:
    mov rdi, 17
    jmp gen_isr
isr18:
    sub rsp, 8
    mov rdi, 18
    jmp gen_isr
isr19:
    sub rsp, 8
    mov rdi, 19
    jmp gen_isr
isr20:
    sub rsp, 8
    mov rdi, 20
    jmp gen_isr
isr21:
    sub rsp, 8
    mov rdi, 21
    jmp gen_isr
isr22:
    sub rsp, 8
    mov rdi, 22
    jmp gen_isr
isr23:
    sub rsp, 8
    mov rdi, 23
    jmp gen_isr
isr24:
    sub rsp, 8
    mov rdi, 24
    jmp gen_isr
isr25:
    sub rsp, 8
    mov rdi, 25
    jmp gen_isr
isr26:
    sub rsp, 8
    mov rdi, 26
    jmp gen_isr
isr27:
    sub rsp, 8
    mov rdi, 27
    jmp gen_isr
isr28:
    sub rsp, 8
    mov rdi, 28
    jmp gen_isr
isr29:
    sub rsp, 8
    mov rdi, 29
    jmp gen_isr
isr30:
    sub rsp, 8
    mov rdi, 30
    jmp gen_isr
isr31:
    sub rsp, 8
    mov rdi, 31
    jmp gen_isr
isr32:
    sub rsp, 8
    mov rdi, 32
    jmp gen_isr
isr33:
    sub rsp, 8
    mov rdi, 33
    jmp gen_isr
isr34:
    sub rsp, 8
    mov rdi, 34
    jmp gen_isr
isr35:
    sub rsp, 8
    mov rdi, 35
    jmp gen_isr
isr36:
    sub rsp, 8
    mov rdi, 36
    jmp gen_isr
isr37:
    sub rsp, 8
    mov rdi, 37
    jmp gen_isr
isr38:
    sub rsp, 8
    mov rdi, 38
    jmp gen_isr
isr39:
    sub rsp, 8
    mov rdi, 39
    jmp gen_isr
isr40:
    sub rsp, 8
    mov rdi, 40
    jmp gen_isr
isr41:
    sub rsp, 8
    mov rdi, 41
    jmp gen_isr
isr42:
    sub rsp, 8
    mov rdi, 42
    jmp gen_isr
isr43:
    sub rsp, 8
    mov rdi, 43
    jmp gen_isr
isr44:
    sub rsp, 8
    mov rdi, 44
    jmp gen_isr
isr45:
    sub rsp, 8
    mov rdi, 45
    jmp gen_isr
isr46:
    sub rsp, 8
    mov rdi, 46
    jmp gen_isr
isr47:
    sub rsp, 8
    mov rdi, 47
    jmp gen_isr
isr48:
    sub rsp, 8
    mov rdi, 48
    jmp gen_isr
isr49:
    sub rsp, 8
    mov rdi, 49
    jmp gen_isr
isr50:
    sub rsp, 8
    mov rdi, 50
    jmp gen_isr
isr51:
    sub rsp, 8
    mov rdi, 51
    jmp gen_isr
isr52:
    sub rsp, 8
    mov rdi, 52
    jmp gen_isr
isr53:
    sub rsp, 8
    mov rdi, 53
    jmp gen_isr
isr54:
    sub rsp, 8
    mov rdi, 54
    jmp gen_isr
isr55:
    sub rsp, 8
    mov rdi, 55
    jmp gen_isr
isr56:
    sub rsp, 8
    mov rdi, 56
    jmp gen_isr
isr57:
    sub rsp, 8
    mov rdi, 57
    jmp gen_isr
isr58:
    sub rsp, 8
    mov rdi, 58
    jmp gen_isr
isr59:
    sub rsp, 8
    mov rdi, 59
    jmp gen_isr
isr60:
    sub rsp, 8
    mov rdi, 60
    jmp gen_isr
isr61:
    sub rsp, 8
    mov rdi, 61
    jmp gen_isr
isr62:
    sub rsp, 8
    mov rdi, 62
    jmp gen_isr
isr63:
    sub rsp, 8
    mov rdi, 63
    jmp gen_isr
isr64:
    sub rsp, 8
    mov rdi, 64
    jmp gen_isr
isr65:
    sub rsp, 8
    mov rdi, 65
    jmp gen_isr
isr66:
    sub rsp, 8
    mov rdi, 66
    jmp gen_isr
isr67:
    sub rsp, 8
    mov rdi, 67
    jmp gen_isr
isr68:
    sub rsp, 8
    mov rdi, 68
    jmp gen_isr
isr69:
    sub rsp, 8
    mov rdi, 69
    jmp gen_isr
isr70:
    sub rsp, 8
    mov rdi, 70
    jmp gen_isr
isr71:
    sub rsp, 8
    mov rdi, 71
    jmp gen_isr
isr72:
    sub rsp, 8
    mov rdi, 72
    jmp gen_isr
isr73:
    sub rsp, 8
    mov rdi, 73
    jmp gen_isr
isr74:
    sub rsp, 8
    mov rdi, 74
    jmp gen_isr
isr75:
    sub rsp, 8
    mov rdi, 75
    jmp gen_isr
isr76:
    sub rsp, 8
    mov rdi, 76
    jmp gen_isr
isr77:
    sub rsp, 8
    mov rdi, 77
    jmp gen_isr
isr78:
    sub rsp, 8
    mov rdi, 78
    jmp gen_isr
isr79:
    sub rsp, 8
    mov rdi, 79
    jmp gen_isr
isr80:
    sub rsp, 8
    mov rdi, 80
    jmp gen_isr
isr81:
    sub rsp, 8
    mov rdi, 81
    jmp gen_isr
isr82:
    sub rsp, 8
    mov rdi, 82
    jmp gen_isr
isr83:
    sub rsp, 8
    mov rdi, 83
    jmp gen_isr
isr84:
    sub rsp, 8
    mov rdi, 84
    jmp gen_isr
isr85:
    sub rsp, 8
    mov rdi, 85
    jmp gen_isr
isr86:
    sub rsp, 8
    mov rdi, 86
    jmp gen_isr
isr87:
    sub rsp, 8
    mov rdi, 87
    jmp gen_isr
isr88:
    sub rsp, 8
    mov rdi, 88
    jmp gen_isr
isr89:
    sub rsp, 8
    mov rdi, 89
    jmp gen_isr
isr90:
    sub rsp, 8
    mov rdi, 90
    jmp gen_isr
isr91:
    sub rsp, 8
    mov rdi, 91
    jmp gen_isr
isr92:
    sub rsp, 8
    mov rdi, 92
    jmp gen_isr
isr93:
    sub rsp, 8
    mov rdi, 93
    jmp gen_isr
isr94:
    sub rsp, 8
    mov rdi, 94
    jmp gen_isr
isr95:
    sub rsp, 8
    mov rdi, 95
    jmp gen_isr
isr96:
    sub rsp, 8
    mov rdi, 96
    jmp gen_isr
isr97:
    sub rsp, 8
    mov rdi, 97
    jmp gen_isr
isr98:
    sub rsp, 8
    mov rdi, 98
    jmp gen_isr
isr99:
    sub rsp, 8
    mov rdi, 99
    jmp gen_isr
isr100:
    sub rsp, 8
    mov rdi, 100
    jmp gen_isr
isr101:
    sub rsp, 8
    mov rdi, 101
    jmp gen_isr
isr102:
    sub rsp, 8
    mov rdi, 102
    jmp gen_isr
isr103:
    sub rsp, 8
    mov rdi, 103
    jmp gen_isr
isr104:
    sub rsp, 8
    mov rdi, 104
    jmp gen_isr
isr105:
    sub rsp, 8
    mov rdi, 105
    jmp gen_isr
isr106:
    sub rsp, 8
    mov rdi, 106
    jmp gen_isr
isr107:
    sub rsp, 8
    mov rdi, 107
    jmp gen_isr
isr108:
    sub rsp, 8
    mov rdi, 108
    jmp gen_isr
isr109:
    sub rsp, 8
    mov rdi, 109
    jmp gen_isr
isr110:
    sub rsp, 8
    mov rdi, 110
    jmp gen_isr
isr111:
    sub rsp, 8
    mov rdi, 111
    jmp gen_isr
isr112:
    sub rsp, 8
    mov rdi, 112
    jmp gen_isr
isr113:
    sub rsp, 8
    mov rdi, 113
    jmp gen_isr
isr114:
    sub rsp, 8
    mov rdi, 114
    jmp gen_isr
isr115:
    sub rsp, 8
    mov rdi, 115
    jmp gen_isr
isr116:
    sub rsp, 8
    mov rdi, 116
    jmp gen_isr
isr117:
    sub rsp, 8
    mov rdi, 117
    jmp gen_isr
isr118:
    sub rsp, 8
    mov rdi, 118
    jmp gen_isr
isr119:
    sub rsp, 8
    mov rdi, 119
    jmp gen_isr
isr120:
    sub rsp, 8
    mov rdi, 120
    jmp gen_isr
isr121:
    sub rsp, 8
    mov rdi, 121
    jmp gen_isr
isr122:
    sub rsp, 8
    mov rdi, 122
    jmp gen_isr
isr123:
    sub rsp, 8
    mov rdi, 123
    jmp gen_isr
isr124:
    sub rsp, 8
    mov rdi, 124
    jmp gen_isr
isr125:
    sub rsp, 8
    mov rdi, 125
    jmp gen_isr
isr126:
    sub rsp, 8
    mov rdi, 126
    jmp gen_isr
isr127:
    sub rsp, 8
    mov rdi, 127
    jmp gen_isr
isr128:
    sub rsp, 8
    mov rdi, 128
    jmp gen_isr
isr129:
    sub rsp, 8
    mov rdi, 129
    jmp gen_isr
isr130:
    sub rsp, 8
    mov rdi, 130
    jmp gen_isr
isr131:
    sub rsp, 8
    mov rdi, 131
    jmp gen_isr
isr132:
    sub rsp, 8
    mov rdi, 132
    jmp gen_isr
isr133:
    sub rsp, 8
    mov rdi, 133
    jmp gen_isr
isr134:
    sub rsp, 8
    mov rdi, 134
    jmp gen_isr
isr135:
    sub rsp, 8
    mov rdi, 135
    jmp gen_isr
isr136:
    sub rsp, 8
    mov rdi, 136
    jmp gen_isr
isr137:
    sub rsp, 8
    mov rdi, 137
    jmp gen_isr
isr138:
    sub rsp, 8
    mov rdi, 138
    jmp gen_isr
isr139:
    sub rsp, 8
    mov rdi, 139
    jmp gen_isr
isr140:
    sub rsp, 8
    mov rdi, 140
    jmp gen_isr
isr141:
    sub rsp, 8
    mov rdi, 141
    jmp gen_isr
isr142:
    sub rsp, 8
    mov rdi, 142
    jmp gen_isr
isr143:
    sub rsp, 8
    mov rdi, 143
    jmp gen_isr
isr144:
    sub rsp, 8
    mov rdi, 144
    jmp gen_isr
isr145:
    sub rsp, 8
    mov rdi, 145
    jmp gen_isr
isr146:
    sub rsp, 8
    mov rdi, 146
    jmp gen_isr
isr147:
    sub rsp, 8
    mov rdi, 147
    jmp gen_isr
isr148:
    sub rsp, 8
    mov rdi, 148
    jmp gen_isr
isr149:
    sub rsp, 8
    mov rdi, 149
    jmp gen_isr
isr150:
    sub rsp, 8
    mov rdi, 150
    jmp gen_isr
isr151:
    sub rsp, 8
    mov rdi, 151
    jmp gen_isr
isr152:
    sub rsp, 8
    mov rdi, 152
    jmp gen_isr
isr153:
    sub rsp, 8
    mov rdi, 153
    jmp gen_isr
isr154:
    sub rsp, 8
    mov rdi, 154
    jmp gen_isr
isr155:
    sub rsp, 8
    mov rdi, 155
    jmp gen_isr
isr156:
    sub rsp, 8
    mov rdi, 156
    jmp gen_isr
isr157:
    sub rsp, 8
    mov rdi, 157
    jmp gen_isr
isr158:
    sub rsp, 8
    mov rdi, 158
    jmp gen_isr
isr159:
    sub rsp, 8
    mov rdi, 159
    jmp gen_isr
isr160:
    sub rsp, 8
    mov rdi, 160
    jmp gen_isr
isr161:
    sub rsp, 8
    mov rdi, 161
    jmp gen_isr
isr162:
    sub rsp, 8
    mov rdi, 162
    jmp gen_isr
isr163:
    sub rsp, 8
    mov rdi, 163
    jmp gen_isr
isr164:
    sub rsp, 8
    mov rdi, 164
    jmp gen_isr
isr165:
    sub rsp, 8
    mov rdi, 165
    jmp gen_isr
isr166:
    sub rsp, 8
    mov rdi, 166
    jmp gen_isr
isr167:
    sub rsp, 8
    mov rdi, 167
    jmp gen_isr
isr168:
    sub rsp, 8
    mov rdi, 168
    jmp gen_isr
isr169:
    sub rsp, 8
    mov rdi, 169
    jmp gen_isr
isr170:
    sub rsp, 8
    mov rdi, 170
    jmp gen_isr
isr171:
    sub rsp, 8
    mov rdi, 171
    jmp gen_isr
isr172:
    sub rsp, 8
    mov rdi, 172
    jmp gen_isr
isr173:
    sub rsp, 8
    mov rdi, 173
    jmp gen_isr
isr174:
    sub rsp, 8
    mov rdi, 174
    jmp gen_isr
isr175:
    sub rsp, 8
    mov rdi, 175
    jmp gen_isr
isr176:
    sub rsp, 8
    mov rdi, 176
    jmp gen_isr
isr177:
    sub rsp, 8
    mov rdi, 177
    jmp gen_isr
isr178:
    sub rsp, 8
    mov rdi, 178
    jmp gen_isr
isr179:
    sub rsp, 8
    mov rdi, 179
    jmp gen_isr
isr180:
    sub rsp, 8
    mov rdi, 180
    jmp gen_isr
isr181:
    sub rsp, 8
    mov rdi, 181
    jmp gen_isr
isr182:
    sub rsp, 8
    mov rdi, 182
    jmp gen_isr
isr183:
    sub rsp, 8
    mov rdi, 183
    jmp gen_isr
isr184:
    sub rsp, 8
    mov rdi, 184
    jmp gen_isr
isr185:
    sub rsp, 8
    mov rdi, 185
    jmp gen_isr
isr186:
    sub rsp, 8
    mov rdi, 186
    jmp gen_isr
isr187:
    sub rsp, 8
    mov rdi, 187
    jmp gen_isr
isr188:
    sub rsp, 8
    mov rdi, 188
    jmp gen_isr
isr189:
    sub rsp, 8
    mov rdi, 189
    jmp gen_isr
isr190:
    sub rsp, 8
    mov rdi, 190
    jmp gen_isr
isr191:
    sub rsp, 8
    mov rdi, 191
    jmp gen_isr
isr192:
    sub rsp, 8
    mov rdi, 192
    jmp gen_isr
isr193:
    sub rsp, 8
    mov rdi, 193
    jmp gen_isr
isr194:
    sub rsp, 8
    mov rdi, 194
    jmp gen_isr
isr195:
    sub rsp, 8
    mov rdi, 195
    jmp gen_isr
isr196:
    sub rsp, 8
    mov rdi, 196
    jmp gen_isr
isr197:
    sub rsp, 8
    mov rdi, 197
    jmp gen_isr
isr198:
    sub rsp, 8
    mov rdi, 198
    jmp gen_isr
isr199:
    sub rsp, 8
    mov rdi, 199
    jmp gen_isr
isr200:
    sub rsp, 8
    mov rdi, 200
    jmp gen_isr
isr201:
    sub rsp, 8
    mov rdi, 201
    jmp gen_isr
isr202:
    sub rsp, 8
    mov rdi, 202
    jmp gen_isr
isr203:
    sub rsp, 8
    mov rdi, 203
    jmp gen_isr
isr204:
    sub rsp, 8
    mov rdi, 204
    jmp gen_isr
isr205:
    sub rsp, 8
    mov rdi, 205
    jmp gen_isr
isr206:
    sub rsp, 8
    mov rdi, 206
    jmp gen_isr
isr207:
    sub rsp, 8
    mov rdi, 207
    jmp gen_isr
isr208:
    sub rsp, 8
    mov rdi, 208
    jmp gen_isr
isr209:
    sub rsp, 8
    mov rdi, 209
    jmp gen_isr
isr210:
    sub rsp, 8
    mov rdi, 210
    jmp gen_isr
isr211:
    sub rsp, 8
    mov rdi, 211
    jmp gen_isr
isr212:
    sub rsp, 8
    mov rdi, 212
    jmp gen_isr
isr213:
    sub rsp, 8
    mov rdi, 213
    jmp gen_isr
isr214:
    sub rsp, 8
    mov rdi, 214
    jmp gen_isr
isr215:
    sub rsp, 8
    mov rdi, 215
    jmp gen_isr
isr216:
    sub rsp, 8
    mov rdi, 216
    jmp gen_isr
isr217:
    sub rsp, 8
    mov rdi, 217
    jmp gen_isr
isr218:
    sub rsp, 8
    mov rdi, 218
    jmp gen_isr
isr219:
    sub rsp, 8
    mov rdi, 219
    jmp gen_isr
isr220:
    sub rsp, 8
    mov rdi, 220
    jmp gen_isr
isr221:
    sub rsp, 8
    mov rdi, 221
    jmp gen_isr
isr222:
    sub rsp, 8
    mov rdi, 222
    jmp gen_isr
isr223:
    sub rsp, 8
    mov rdi, 223
    jmp gen_isr
isr224:
    sub rsp, 8
    mov rdi, 224
    jmp gen_isr
isr225:
    sub rsp, 8
    mov rdi, 225
    jmp gen_isr
isr226:
    sub rsp, 8
    mov rdi, 226
    jmp gen_isr
isr227:
    sub rsp, 8
    mov rdi, 227
    jmp gen_isr
isr228:
    sub rsp, 8
    mov rdi, 228
    jmp gen_isr
isr229:
    sub rsp, 8
    mov rdi, 229
    jmp gen_isr
isr230:
    sub rsp, 8
    mov rdi, 230
    jmp gen_isr
isr231:
    sub rsp, 8
    mov rdi, 231
    jmp gen_isr
isr232:
    sub rsp, 8
    mov rdi, 232
    jmp gen_isr
isr233:
    sub rsp, 8
    mov rdi, 233
    jmp gen_isr
isr234:
    sub rsp, 8
    mov rdi, 234
    jmp gen_isr
isr235:
    sub rsp, 8
    mov rdi, 235
    jmp gen_isr
isr236:
    sub rsp, 8
    mov rdi, 236
    jmp gen_isr
isr237:
    sub rsp, 8
    mov rdi, 237
    jmp gen_isr
isr238:
    sub rsp, 8
    mov rdi, 238
    jmp gen_isr
isr239:
    sub rsp, 8
    mov rdi, 239
    jmp gen_isr
isr240:
    sub rsp, 8
    mov rdi, 240
    jmp gen_isr
isr241:
    sub rsp, 8
    mov rdi, 241
    jmp gen_isr
isr242:
    sub rsp, 8
    mov rdi, 242
    jmp gen_isr
isr243:
    sub rsp, 8
    mov rdi, 243
    jmp gen_isr
isr244:
    sub rsp, 8
    mov rdi, 244
    jmp gen_isr
isr245:
    sub rsp, 8
    mov rdi, 245
    jmp gen_isr
isr246:
    sub rsp, 8
    mov rdi, 246
    jmp gen_isr
isr247:
    sub rsp, 8
    mov rdi, 247
    jmp gen_isr
isr248:
    sub rsp, 8
    mov rdi, 248
    jmp gen_isr
isr249:
    sub rsp, 8
    mov rdi, 249
    jmp gen_isr
isr250:
    sub rsp, 8
    mov rdi, 250
    jmp gen_isr
isr251:
    sub rsp, 8
    mov rdi, 251
    jmp gen_isr
isr252:
    sub rsp, 8
    mov rdi, 252
    jmp gen_isr
isr253:
    sub rsp, 8
    mov rdi, 253
    jmp gen_isr
isr254:
    sub rsp, 8
    mov rdi, 254
    jmp gen_isr
isr255:
    sub rsp, 8
    mov rdi, 255
    jmp gen_isr
