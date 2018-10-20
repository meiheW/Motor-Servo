CC=arm-linux-gnueabihf-gcc
#CC=gcc

AR=ar
RANLIB=ranlib


CFLAGS=-g -Wall
LIBS=-L./ -lcgic

all: libcgic.a  platforminfo  updatex info platop shmserver

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

platforminfo: platforminfo.o libcgic.a
	$(CC) platforminfo.o -o platforminfo ${LIBS}

updatex: updatex.o libcgic.a
	$(CC) updatex.o -o updatex ${LIBS}

info: info.o libcgic.a
	$(CC) info.o -o info ${LIBS}

platop: platop.o libcgic.a
	$(CC) platop.o -o platop ${LIBS}
shmserver: shmserver.o 
	$(CC) shmserver.o -o shmserver



clean:
	rm -f *.o *.a platforminfo updatex info platop shmserver

