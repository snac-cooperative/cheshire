# Directory in which the mr source programs are
HEADER_DIR =	../header
LIBRARY_DIR =	../lib
BIN_DIR	= 	../bin

AR	= ar
ARFLAGS = crv
RANLIB	= :

# Compiler. Default gcc
CC	= gcc 

# Compiler switches. Default -g.
CFLAGS = -g -Wall -fPIC -DMAJOR_VERSION=1 -DMINOR_VERSION=0 -I/usr/include/python2.7 -I${HEADER_DIR}

LIBRARIES = -llist

LIST_OBJC = list_init.o list_append.o \
	    list_utils.o

all: ZCheshire.so

ZCheshire.so: ZCheshire.o
	$(CC) -shared -o ZCheshire.so -L../lib -lcmdparse -lclient -lnetconnect -lutils -llist -ldiag -lz3_asn -lc -lm  ZCheshire.o

clean:
	rm -f *.o *.so core a.out

.c.o:
	$(CC) -c $(CFLAGS) $<

