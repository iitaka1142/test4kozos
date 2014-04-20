# os/Makefile

OBJS = startup.o main.o kozos.o syscall.o memory.o
MISCS = ../misc/serial.o ../misc/interrupt.o ../misc/lib.o

#OBJS += test08_1.o
#OBJS += test09_1.o test09_2.o test09_3.o
#OBJS += test10_1.o
#OBJS += test11_1.o test11_2.o
OBJS += consdrv.o command.o

TARGET = kozos

include ../Makefile.in

CFLAGS += -DKOZOS

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).c~

