# os/Makefile

OBJS = startup.o main.o kozos.o syscall.o
MISCS = ../misc/serial.o ../misc/interrupt.o ../misc/lib.o

OBJS += test08_1.o

TARGET = kozos

include ../Makefile.in

CFLAGS += -DKOZOS

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).c~

