global outb             ; make the label outb visible outside this file

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address

outb:
	mov 	dx, [esp + 4]    ; move the address of the I/O port into the dx register
	mov 	al, [esp + 8]    ; move the data to be sent into the al register
	out 	dx, al           ; send the data to the I/O port
	ret                  ; return to the calling function

global inb

; inb - returns a byte from the given I/O port
; stack: [esp + 4] The address of the I/O port
;        [esp    ] The return address
inb:
	mov dx, [esp + 4]       ; move the address of the I/O port to the dx register
	in  al, dx              ; read a byte from the I/O port and store it in the al register
	ret                     ; return the read byte

global loadgdt

loadgdt:
	mov		eax, [esp + 4]
	lgdt 	[eax]
	ret

global initgdt

initgdt:
	jmp		0x08:flush_cs

flush_cs:
	mov 	ax, 0x10
	mov 	ds, ax
	mov 	ss, ax
	mov 	es, ax
	mov 	fs, ax
	mov 	gs, ax
	ret