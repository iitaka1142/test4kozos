# /Makefile

all: tags
	cd misc; make all
	cd bootload; make all
	cd os; make all

clean:
	rm TAGS
	cd misc; make clean
	cd bootload; make clean
	cd os; make clean

image:
	cd bootload; make image

write:
	cd bootload; make write

reset_serial:
# 自宅のOpenSuse用
#	setserial /dev/ttyS2 port 0xe800 uart 16550A irq 21 baud_base 115200
#	setserial /dev/ttyS3 port 0xe400 uart 16550A irq 21 baud_base 115200
# Let's note用
	setserial /dev/ttyS3 port 0x3400 uart 16550A irq 22 baud_base 115200

connect:
	kermit ./writer/kermitrc 

tags:
	find . -name "*.[chCHsS]" -or -name "*.scr" | etags -
