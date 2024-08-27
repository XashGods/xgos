org 0x0
bits 16

%define ENDL 0x0D, 0x0A

start:
	; print message
	mov si, test_message
	call puts

.halt:
	cli
	hlt

; ds:si = string
puts:
	; registers
	push si
	push ax

.loop:
	lodsb

	or al, al		; check for null terminator
	jz .done

	mov ah, 0x0E	; teletype output
	mov bh, 0		; page number
	int 0x10

	jmp .loop

.done:
	pop ax
	pop si
	ret

; boot sector
test_message: db 'Hello, World!', ENDL, 0