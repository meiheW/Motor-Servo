TARGET=motor-servo
CC=arm-linux-gnueabihf-gcc
#CC=gcc
CFLAGS=-g -Wall -std=gnu99 \
       -I./
LIBS=-lpthread


DEPS=$(wildcard ./*.h)
SRC=$(wildcard ./*.c)
OBJS=$(patsubst %.c, %.o, $(notdir $(SRC)))


$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o:%.c $(DEPS)
	$(CC) -c $< $(CFLAGS) $(LIBS)


.PHONY:clean

clean:
	rm -rf *.o $(TARGET)
