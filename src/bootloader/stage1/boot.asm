org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

; FAT12 header
jmp short start
nop

bdb_oem: db 'MSWIN4.1' 							; 8 bytes
bdb_bytes_per_sector: dw 512
bdb_sectors_per_cluster: db 1
bdb_reserved_sectors: dw 1
bdb_fat_count: db 2
bdb_dir_entries_count: dw 0E0h
bdb_total_sectors: dw 2880 						; 2880 * 512 = 1.44MB
bdb_media_descriptor_type: db 0F0h 				; F0 = 3.5" 1.44MB floppy
bdb_sectors_per_fat: dw 9
bdb_sectors_per_track: dw 18
bdb_heads: dw 2
bdb_hidden_sectors: dd 0
bdb_large_sector_count: dd 0

; extended boot record
ebr_drive_number: db 0 							; 0x00 for floppy, 0x80 for hard drive
				  db 0 							; reserved
ebr_signature: db 29h
ebr_volume_id: db 12h, 34h, 56h, 78h 			; serial number, doesn't matter
ebr_volume_label: db 'XASHGODS OS' 				; should be 11 bytes, not more or less
ebr_system_id: db 'FAT12   ' 					; should be 8 bytes, not more or less

start:
	mov ax, 0									; ds/es cant be directly written to
	mov ds, ax
	mov es, ax

	; setup
	mov ss, ax
	mov sp, 0x7C00								; stack grows down

	; support for different bioses
	; some of them start at 07C0:0000, some at 0000:7C00
	; so we need make sure that we are at expected location
	push es
	push word .after
	retf

.after:
	; try to read something from disk
	; bios should set dl to drive number
	mov [ebr_drive_number], dl

	; mov ax, 1 ; LBA=1, 2. sector
	; mov cl, 1 ; read 1 sector
	; mov bx, 0x7E00 ; read to 0x7E00, (data should be after bootloader)
	; call disk_read

	; print message
	mov si, msg_loading
	call puts

	; read drive params
	push es
	mov ah, 08h
	int 13h
	jc floppy_error
	pop es

	and cl, 0x3F 								; 6 bits for sector number
	xor ch, ch
	mov [bdb_sectors_per_track], cx 			; sector

	inc dh
	mov [bdb_heads], dh 						; head

	; read fat root directory
	mov ax, [bdb_sectors_per_fat] 				; lba of root = reserved + fats * sectors_per_fat
	mov bl, [bdb_fat_count]
	xor bh, bh
	mul bx 										; ax = (fats * sectors_per_fat)
	add ax, [bdb_reserved_sectors] 				; ax = lba of root
	push ax

	; size of root directory
	mov ax, [bdb_sectors_per_fat]
	shl ax, 5 									; ax *= 32
	xor dx, dx 									; dx = 0
	div word [bdb_bytes_per_sector] 			; ax = (sectors_per_fat * 32) / bytes_per_sector

	test dx, dx 								; if dx != 0, add 1
	jz .root_dir_after
	inc ax 										; division remainder != 0, add 1

.root_dir_after:
	mov cl, al 									; cl = number of sectors to read
	pop ax 										; ax = lba of root
	mov dl, [ebr_drive_number]
	mov bx, buffer
	call disk_read

	; search kernel.bin
	xor bx, bx
	mov di, buffer

.search_kernel:
	mov si, file_kernel_bin
	mov cx, 11 									; length of file name
	push di
	repe cmpsb
	pop di
	je .found_kernel

	add di, 32 									; next entry
	inc bx
	cmp bx, [bdb_dir_entries_count]
	jb .search_kernel

	; kernel not found
	jmp kernel_not_found

.found_kernel:
	; di = kernel entry
	mov ax, [di+26] 							; first logical cluster
	mov [kernel_cluster], ax

	; load fat from disk to buffer
	mov ax, [bdb_reserved_sectors]
	mov bx, buffer
	mov cl, [bdb_sectors_per_fat]
	mov dl, [ebr_drive_number]
	call disk_read

	; read kernel from disk
	mov bx, KERNEL_LOAD_SEGMENT
	mov es, bx
	mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
	; :( hardcoded values, ... idk other way
	mov ax, [kernel_cluster]
	add ax, 31									; first cluster = (kernel_cluster - 2) * sectors_per_cluster + start_sector
												; start_sector = reserved + fats * root dir size = 1 + 18 + 134 = 33

	mov cl, 1
	mov dl, [ebr_drive_number]
	call disk_read

	add bx, [bdb_bytes_per_sector]

	; next cluster
	mov ax, [kernel_cluster]
	mov cx, 3
	mul cx
	mov cx, 2
	div cx 										; ax = index of fat entry, dx = cluster mod 2

	mov si, buffer
	add si, ax
	mov ax, [ds:si]

	or dx, dx
	jz .even

.odd:
	shr ax, 4
	jmp .next_cluster_after

.even:
	and ax, 0x0FFF

.next_cluster_after:
	cmp ax, 0xFF8 								; end of cluster chain
	jae .read_finish

	mov [kernel_cluster], ax
	jmp .load_kernel_loop

.read_finish:
	; jump to kernel
	mov dl, [ebr_drive_number]

	mov ax, KERNEL_LOAD_SEGMENT 				; set segment registers
	mov ds, ax
	mov es, ax

	jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

	jmp wait_interaction_for_reboot 			; should never reach here

	cli 										; disable interrupts for forcing cpu to halt
	hlt

; error handlers
floppy_error:
	mov si, msg_read_error
	call puts
	jmp wait_interaction_for_reboot

kernel_not_found:
	mov si, msg_kernel_not_found
	call puts
	jmp wait_interaction_for_reboot

wait_interaction_for_reboot:
	mov ah, 0
	int 16h 									; wait for key press
	jmp 0FFFFh:0 								; reboot a.k.a jump to beginning of bios memory
	hlt

.halt:
	cli 										; disable interrupts for forcing cpu to halt
	hlt

; ds:si = string
puts:
	; registers
	push si
	push ax

.loop:
	lodsb

	or al, al									; check for null terminator
	jz .done

	mov ah, 0x0E								; teletype output
	mov bh, 0									; page number
	int 0x10

	jmp .loop

.done:
	pop ax
	pop si
	ret

; disk routines

; LBA address to CHS
; params: 
; 	ax: LBA address
; returns:
; 	cx [bits 0-5]: sector num
; 	cx [bits 6-16]: cylinder
; 	dh: head
lba_to_chs:

	push ax
	push dx

	xor dx, dx									; dx = 0
	div word [bdb_sectors_per_track]			; ax = LBA / sectors_per_track
												; dx = LBA % sectors_per_track

	inc dx										; dx = (LBA % sectors_per_track + 1) = sector
	mov cx, dx									; cx = sector

	xor dx, dx									; dx = 0
	div word [bdb_heads]						; ax = LBA / (sectors_per_track * heads) = cylinder
												; dx = (LBA / (sectors_per_track) % heads = head
	
	mov dh, dl									; dh = head
	mov ch, al									; ch = cylinder (low 8 bits)
	shl ah, 6
	or ch, ah									; upper 2 bits of cylinder to ch

	pop ax
	mov dl, al									; dl restoring
	pop ax

	ret

; read sector
; params:
; 	ax: LBA address
; 	cl: number of sectors to read (1-128)
; 	dl: drive number
; 	es:bx: range for writing readed data from disk  
disk_read:

	; registers
	push ax
	push bx
	push cx
	push dx
	push di

	push cx 									; temp save cl
	call lba_to_chs 							; get chs
	pop ax 										; AL = cl

	mov ah, 2 									; read sectors
	mov di, 3 									; retry counter

.retry:
	pusha 										; save all registers
	stc 										; set carry flag
	int 13h 									; carry flag = 0 if success
	jnc .done 									; jmp if carry not set

	; error/fail handling
	popa 										; restore all registers
	call disk_reset

	dec di
	test di, di
	jnz .retry

.fail:
	; if we are here, we failed to read sector
	jmp floppy_error

.done:
	popa

	; restore registers
	pop di
	pop dx
	pop cx
	pop bx
	pop ax
	ret 

; reset disk controller
; params:
; 	dl: drive number
disk_reset:
	pusha
	mov ah, 0
	stc
	int 13h
	jc floppy_error
	popa
	ret

; boot sector
msg_loading: db 'Loading...', ENDL, 0
msg_read_error: db 'Error reading sector', ENDL, 0
msg_kernel_not_found: db 'Stage 2 not found', ENDL, 0
file_kernel_bin: db 'STAGE2  BIN', 0
kernel_cluster: dw 0

KERNEL_LOAD_SEGMENT equ 0x2000
KERNEL_LOAD_OFFSET equ 0

times 510-($-$$) db 0
dw 0AA55h

buffer: times 512 db 0