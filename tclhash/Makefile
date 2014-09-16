SYSTYPE= ALPHA
# all these things will need to point to the
# proper places
ID=	.
LOCDIR= ../header
TKDIR=	/usr/local/src/tk4.0
TCLDIR=	/usr/local/src/tcl7.4
TKLIBS=	/usr/local/lib
XLIB=	/usr/lib/libX11.a
LIBDIR= ../lib
RANLIB= :
GCFLAGS= -g
LDFLAGS=	$(GCFLAGS)

# Swap the following defines for DMALLOC memory checking version
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
NODMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE

CFLAGS=		$(GCFLAGS) \
		-I$(LOCDIR) -I$(TCLDIR) -I/usr/local/include \
		-D$(SYSTYPE) $(NODMFLAGS) $(ZVERSION)

DMCFLAGS=		$(GCFLAGS) \
		-I$(TCLDIR) -I/usr/local/include \
		-D$(SYSTYPE) $(DMFLAGS)


#DEMOCCOPTS=	$(DMCFLAGS)
DEMOCCOPTS=	$(CFLAGS) -fPIC

CC=	gcc
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq

all: libtclhash.a

#
DEFINES=

PROGMODS= tclHash.o

# ----------------
#   MAIN
# ----------------

libtclhash.a: $(PROGMODS)
	@ echo "Making hash library"
	@ rm -f libtclhash.a 
	@ ar cr libtclhash.a $(PROGMODS)
	@ $(RANLIB) libtclhash.a
	@ cp libtclhash.a $(LIBDIR)

clean: 
	rm -f *.o *.a *~

reallyclean: clean

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
# DO NOT DELETE THIS LINE -- make depend depends on it.












