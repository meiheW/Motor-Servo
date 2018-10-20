
## Created by Anjuta

CC = gcc
CFLAGS = -g -Wall
OBJECTS = motor_term.o
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -lpthread

all: motor_term

motor_term: $(OBJECTS)
	$(CC) -o motor_term $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o motor_term

.PHONY: all
.PHONY: count
.PHONY: clean
