RELEASEDIR= ../release
SRC=	wnglobals.c wnhelp.c search.c morph.c util.c binsearch.c
OBJ=	wnglobals.o wnhelp.o search.o morph.o util.o binsearch.o
LIB=	libwn.a 

CC= gcc
CFLAGS=	-g -fPIC
LDFLAGS=	-L. -lc
release:=	CFLAGS=-O -fPIC
all:=		CFLAGS += -DLABONLY -fPIC
install:=	CFLAGS=-O -DLABONLY -fPIC
RANLIB = :
all: lib


testmorph: $(LIB) testmorph.o
	gcc -o testmorph $(LDFLAGS) testmorph.o $(LIB)

cv: convertutil.o $(LIB)
	gcc -o convertutil $(LDFLAGS) convertutil.o $(LIB)

release: veryclean lib cv
	install -d $(RELEASEDIR)
	install convertutil wngrep $(RELEASEDIR)
 
install: veryclean lib cv

lib: $(OBJ)
	ar rcv $(LIB) $(OBJ)
	$(RANLIB) $(LIB)

clean:
	rm -f $(OBJ) convertutil.o core *~ #*

lint:
	lint $(SRC)

veryclean:
	$(MAKE) clean
	rm -f $(LIB) convertutil

