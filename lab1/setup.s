INITSEG  = 0x9000

entry _start
_start:
    mov ah,#0x03     ; read cursor pos
    xor bh,bh
    int 0x10

    mov cx,#25       ; Print "NOW we are in SETUP"
    mov bx,#0x0007
    mov bp,#msg2 
    mov ax,cs        ; cs: 0x07e0
    mov es,ax
    mov ax,#0x1301
    int 0x10

; Get Cursor Pos
    mov ax,#INITSEG
    mov ds,ax
    mov ah,#0x03
    xor bh,bh
    int 0x10
    mov [0],dx  	; store in 9000:0

; Get Memory Size
    mov ah,#0x88
    int 0x15
    mov [2],ax      ; store in 9000:2

; Get hd0 data
    mov ax,#0x0000
    mov ds,ax       ; modify ds
    lds si,[4*0x41]
    mov ax,#INITSEG
    mov es,ax       
    mov di,#0x0004  ; store in 9000:4
    mov cx,#0x10
    rep
    movsb

! Be Ready to Print
    mov ax,cs       ; 0x07e0
    mov es,ax
    mov ax,#INITSEG ; 9000
    mov ds,ax

; print Cursor Position
    mov cx,#18
    mov bx,#0x0007
    mov bp,#msg_cursor
    mov ax,#0x1301
    int 0x10

    mov dx,[0]  ; pass hex number through register dx to function print_hex
    call    print_hex

; print Memory Size
    mov ah,#0x03
    xor bh,bh
    int 0x10

    mov cx,#14
    mov bx,#0x0007
    mov bp,#msg_memory
    mov ax,#0x1301
    int 0x10

    mov dx,[2]  
    call    print_hex

; print KB
    mov ah,#0x03
    xor bh,bh
    int 0x10

    mov cx,#2
    mov bx,#0x0007
    mov bp,#msg_kb
    mov ax,#0x1301
    int 0x10

; print Cyles
    mov ah,#0x03
    xor bh,bh
    int 0x10
    mov cx,#7
    mov bx,#0x0007
    mov bp,#msg_cyles
    mov ax,#0x1301
    int 0x10
    mov dx,[4]
    call    print_hex

; print Heads
    mov ah,#0x03
    xor bh,bh
    int 0x10
    mov cx,#8
    mov bx,#0x0007
    mov bp,#msg_heads
    mov ax,#0x1301
    int 0x10
    mov dx,[6]
    call    print_hex

; print Secotrs
    mov ah,#0x03
    xor bh,bh
    int 0x10
    mov cx,#10
    mov bx,#0x0007
    mov bp,#msg_sectors
    mov ax,#0x1301
    int 0x10
    mov dx,[12]
    call    print_hex

inf_loop:
    jmp inf_loop

print_hex:
    mov    cx,#4
print_digit:
    rol    dx,#4   ; rotate left
    mov    ax,#0xe0f 
    and    al,dl   ; fetch low 4 bits
    add    al,#0x30    ; 0~9
    cmp    al,#0x3a    
    jl     outp
    add    al,#0x07    ; a~f , add more 0x07
outp:
    int    0x10
    loop   print_digit
    ret

msg2:
    .byte 13,10
    .ascii "NOW we are in SETUP"
    .byte 13,10,13,10
msg_cursor:
    .byte 13,10
    .ascii "Cursor position:"
msg_memory:
    .byte 13,10
    .ascii "Memory Size:"
msg_cyles:
    .byte 13,10
    .ascii "Cyls:"
msg_heads:
    .byte 13,10
    .ascii "Heads:"
msg_sectors:
    .byte 13,10
    .ascii "Sectors:"
msg_kb:
    .ascii "KB"
