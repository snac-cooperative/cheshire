SYSTYPE	= ALPHA

# Directory in which the cat source programs are
HDR_DIR =	../header
LIB_DIR =	../lib
BIN_DIR	= 	../bin

AR	= ar
ARFLAGS = crv
RANLIB	= :

# Compiler. Default gcc
CC	= gcc 

# Compiler switches. Default -g.
CFLAGS = -g -Wall -fPIC -D$(SYSTYPE) $(ZVERSION) -I. -I${HDR_DIR} \
	 -I/usr/local/include -L. -L${LIB_DIR}

OBJS = client.o \
	mclient.o \
	search_result.o

SRCS =  connect.h

all: libclient.a 

libclient.a: $(OBJS)
	rm -f libclient.a
	ar cr libclient.a $(OBJS)
	$(RANLIB) libclient.a
	mv libclient.a ${LIB_DIR}

.c.o:
	$(CC) -c $(CFLAGS) $<
clean:
	rm -f $(OBJS) *.o libclient*.a a.out core zcclient zcserver 

$(OBJS): $(HDR_DIR)/connect.h
