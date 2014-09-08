SYSTYPE= ALPHA
# all these things will need to point to the
# proper places
ID=	.
LIBDIR= ../lib
HDRDIR= ../header

GCFLAGS= -g
LDFLAGS=	$(GCFLAGS)
RANLIB  = :

# Swap the following defines for DMALLOC memory checking version
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
NODMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE

CFLAGS=		$(GCFLAGS) \
		-I$(HDRDIR) -I/usr/local/include \
		-D$(SYSTYPE) $(NODMFLAGS)

DMCFLAGS=		$(GCFLAGS) \
		-I$(TCLDIR) -I/usr/local/include \
		-D$(SYSTYPE) $(DMFLAGS)


#DEMOCCOPTS=	$(DMCFLAGS)
DEMOCCOPTS=	$(CFLAGS)

CC=	gcc
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq

all: libdiag.a

#
DEFINES=

PROGMODS= diagnostic.o seterror.o

# ----------------
#   MAIN
# ----------------

libdiag.a: $(PROGMODS)
	@ echo "Making diagnostic library"
	@ rm -f libdiag.a 
	@ ar cr libdiag.a $(PROGMODS)
	@ $(RANLIB) libdiag.a
	@ cp libdiag.a $(LIBDIR)

clean: 
	rm -f *.o libdiag.a libdiagdm.a *~

reallyclean: clean

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
# DO NOT DELETE THIS LINE -- make depend depends on it.












