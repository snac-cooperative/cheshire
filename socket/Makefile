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
  -L. -L${LIB_DIR}

OBJS = connect.o conutils.o

SRCS =  connect.h

all: libnetconnect.a 

libnetconnect.a: $(OBJS)
	@ rm -f libnetconnect.a
	@ ar cr libnetconnect.a $(OBJS)
	@ $(RANLIB) libnetconnect.a
	@ mv libnetconnect.a ${LIB_DIR}/.

clean:
	rm -f $(OBJS) *.o lib*.a a.out core

reallyclean: clean
	rm -f *~

$(OBJS): $(HDR_DIR)/connect.h





