# bootload/Makefile

OBJS = vector.o startup.o main.o xmodem.o elf.o intr.o
MISCS = ../misc/serial.o ../misc/interrupt.o ../misc/lib.o

TARGET = kzload

include ../Makefile.in

CFLAGS += -DKZLOAD

$(TARGET).mot : $(TARGET)
	$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image: $(TARGET).mot

write: $(TARGET).mot
	$(H8WRITE) -3069 -f20 $(TARGET).mot $(H8WRITE_SERDEV)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot *~

