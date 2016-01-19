TARGET=analyzer

CC=clang
CFLAGS=-std=c11

OBJS=il.o mips.o main.o

all: $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
