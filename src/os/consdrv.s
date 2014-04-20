	.file	"consdrv.c"
	.comm	_consreg,6,16
	.section .text
	.align 1
_send_char:
	mov.w	r6,@-r7
	mov.w	r7,r6
	subs	#2,r7
	mov.w	r0,@(-2,r6)
	sub.w	r2,r2
	mov.w	r2,r0
	adds	#2,r7
	mov.w	@r7+,r6
	rts
	.size	_send_char, .-_send_char
	.align 1
_send_string:
	mov.w	r6,@-r7
	mov.w	r7,r6
	mov.w	#-8,r3
	add.w	r3,r7
	mov.w	r0,@(-4,r6)
	mov.w	r1,@(-6,r6)
	mov.w	r2,@(-8,r6)
	sub.w	r2,r2
	mov.w	r2,@(-2,r6)
	bra	.L4
.L6:
	mov.w	@(-2,r6),r2
	mov.w	@(-6,r6),r3
	add.w	r3,r2
	mov.b	@r2,r2l
	cmp.b	#10,r2l
	bne	.L5
	mov.w	@(-4,r6),r2
	mov.w	@(2,r2),r3
	mov.w	@(-4,r6),r2
	mov.w	@(4,r2),r2
	add.w	r2,r3
	mov.b	#13,r0l
	mov.b	r0l,@r3
	mov.w	r2,r3
	adds	#1,r3
	mov.w	@(-4,r6),r2
	mov.w	r3,@(4,r2)
.L5:
	mov.w	@(-4,r6),r2
	mov.w	@(2,r2),r3
	mov.w	@(-4,r6),r2
	mov.w	@(4,r2),r2
	add.w	r2,r3
	mov.w	@(-2,r6),r0
	mov.w	@(-6,r6),r1
	add.w	r1,r0
	mov.b	@r0,r0l
	mov.b	r0l,@r3
	mov.w	r2,r3
	adds	#1,r3
	mov.w	@(-4,r6),r2
	mov.w	r3,@(4,r2)
	mov.w	@(-2,r6),r2
	adds	#1,r2
	mov.w	r2,@(-2,r6)
.L4:
	mov.w	@(-2,r6),r3
	mov.w	@(-8,r6),r2
	cmp.w	r2,r3
	blo	.L6
	mov.w	@(-4,r6),r2
	mov.w	@(4,r2),r2
	mov.w	r2,r2
	beq	.L7
	mov.w	@(-4,r6),r2
	mov.w	@r2,r2
	mov.w	r2,r0
	jsr	@_serial_intr_is_send_enable
	mov.w	r0,r2
	mov.w	r2,r2
	beq	.L7
	mov.w	@(-4,r6),r2
	mov.w	@r2,r2
	mov.w	r2,r0
	jsr	@_serial_intr_send_enable
	mov.w	@(-4,r6),r2
	mov.w	r2,r0
	jsr	@_send_char
.L7:
	sub.w	r2,r2
	mov.w	r2,r0
	mov.w	#8,r3
	add.w	r3,r7
	mov.w	@r7+,r6
	rts
	.size	_send_string, .-_send_string
	.align 1
_consdrv_command:
	mov.w	r6,@-r7
	mov.w	r7,r6
	mov.w	#-10,r3
	add.w	r3,r7
	mov.w	r0,@(-6,r6)
	mov.w	r1,@(-8,r6)
	mov.w	r2,@(-10,r6)
	mov.w	@(-10,r6),r2
	mov.b	@r2,r2l
	mov.b	r2l,@(-1,r6)
	mov.w	@(-10,r6),r2
	adds	#1,r2
	mov.w	r2,@(-4,r6)
	mov.w	@(-8,r6),r2
	subs	#1,r2
	mov.w	r2,@(-8,r6)
; #APP
;# 60 "consdrv.c" 1
	orc.b #0xc0,ccr
;# 0 "" 2
; #NO_APP
	mov.w	@(-8,r6),r2
	mov.w	@(-4,r6),r0
	mov.w	@(-6,r6),r3
	mov.w	r0,r1
	mov.w	r3,r0
	jsr	@_send_string
; #APP
;# 64 "consdrv.c" 1
	andc.b #0x3f,ccr
;# 0 "" 2
; #NO_APP
	nop
	sub.w	r2,r2
	mov.w	r2,r0
	mov.w	#10,r3
	add.w	r3,r7
	mov.w	@r7+,r6
	rts
	.size	_consdrv_command, .-_consdrv_command
	.align 1
	.global _consdrv_main
_consdrv_main:
	mov.w	r6,@-r7
	mov.w	r7,r6
	mov.w	#-14,r3
	add.w	r3,r7
	mov.w	r0,@(-12,r6)
	mov.w	r1,@(-14,r6)
.L12:
	mov.w	r6,r2
	add.b	#246,r2l
	addx	#255,r2h
	mov.w	r6,r3
	add.b	#248,r3l
	addx	#255,r3h
	mov.w	r3,r1
	mov.w	#1,r0
	jsr	@_kz_recv
	mov.w	r1,r3
	mov.w	r0,r2
	mov.w	r2,@(-4,r6)
	mov.w	r3,@(-2,r6)
	mov.w	@(-10,r6),r2
	mov.b	@r2,r2l
	mov.b	#0,r2h
	add.b	#208,r2l
	addx	#255,r2h
	mov.w	r2,@(-6,r6)
	mov.w	@(-10,r6),r1
	mov.w	@(-8,r6),r0
	mov.w	@(-6,r6),r3
	mov.w	r3,r2
	add.w	r2,r2
	add.w	r3,r2
	add.w	r2,r2
	mov.w	#_consreg,r3
	add.w	r2,r3
	mov.w	r1,r2
	mov.w	r0,r1
	mov.w	r3,r0
	jsr	@_consdrv_command
	mov.w	@(-10,r6),r2
	mov.w	r2,r0
	jsr	@_kz_kmfree
	bra	.L12
	.size	_consdrv_main, .-_consdrv_main
	.ident	"GCC: (GNU) 4.7.3"
	.end
