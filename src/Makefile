# /Makefile

all: tags
	cd misc; make all
	cd bootload; make all
	cd os; make all

clean:
	cd misc; make clean
	cd bootload; make clean
	cd os; make clean

image:
	cd bootload; make image

write:
	cd bootload; make write

reset_serial:
	setserial /dev/ttyS2 port 0xe800 uart 16550A irq 21 baud_base 115200
	setserial /dev/ttyS3 port 0xe400 uart 16550A irq 21 baud_base 115200

connect:
	kermit ./writer/kermitrc 

tags: TAGS
	find . -name "*.[chCHsS]" -or -name "*.scr" | etags -
