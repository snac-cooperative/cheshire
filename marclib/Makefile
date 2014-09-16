BIN = /usr/local/bin
LIBDEST = ../lib
LIB = marclib.o
DMARC = dispmarc.o dispmarctest.o 
CC=		gcc
RANLIB = :

CFLAGS = 	-g -I../header -I/usr/local/include  -DDMALLOC_INCLUDE -DDMALLOC_DISABLE -DEXTERNAL_TAG_MATCH -D$(SYSTYPE) $(ZVERSION) -fPIC
LDFLAGS = 	-g
XEC = libmarc.a dispmarc

all: $(XEC)

libmarc.a: 	$(LIB)
	@ echo "Making marc library"
	@ rm -f libmarc.a 
	@ ar cr libmarc.a $(LIB)
	@ $(RANLIB) libmarc.a
	@ echo "Copying marc library"
	@ cp libmarc.a $(LIBDEST)

cpmarc:
	$(CC) -g -o cpmarc cpmarc.c

dispmarc: libmarc.a $(DMARC)
	$(CC) -g $(DMARC) -o dispmarc -L$(LIBDEST) -lmarc

buildmarcas: 
	$(CC) -g -o buildmarcas marc_asso.c 

#########################################################################
# Installation of programs                                              #
#########################################################################

#########################################################################
# Odds and ends                                                         #
#########################################################################
clean: 
	/bin/rm -f *.o *~ $(XEC) cpmarc         

libclean:
	rm -f libmarc.a

reallyclean: clean libclean

print: 
	enscript *.c

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above

