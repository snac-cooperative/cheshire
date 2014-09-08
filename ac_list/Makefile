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
CFLAGS = -g -Wall -fPIC -I. -I${HEADER_DIR} -DDMALLOC_INCLUDE -DDMALLOC_DISABLE

LIBRARIES = -llist

LIST_OBJC = list_init.o list_append.o \
	    list_utils.o

all: liblist.a

liblist.a: ${LIST_OBJC}
	rm -f liblist.a
	$(AR) $(ARFLAGS) liblist.a ${LIST_OBJC}
	$(RANLIB) liblist.a
	mv liblist.a $(LIBRARY_DIR)/.

clean:
	rm -f *.o core a.out

.c.o:
	$(CC) -c $(CFLAGS) $<

