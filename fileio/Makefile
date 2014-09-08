# all these things will need to point to the
# proper placesz
SYSTYPE= ALPHA
TREE=	..
OD=	.
SD=	.
ID=	$(TREE)/header
XMLDIR= /usr/include/libxml2
LOCDIR= /usr/local/include
TKDIR=	/usr/local/src/tk4.0
TCLDIR=	/usr/local/src/tcl7.4
TKLIBS=	/usr/local/lib

CC=		gcc
GCFLAGS=	-g
LDFLAGS=	$(GCFLAGS)
RANLIB = :
CFLAGS=		$(GCFLAGS) \
		-I$(ID) -I$(XMLDIR) -I$(LOCDIR) -I$(TKDIR) -I$(TCLDIR)  $(XINC) $(ZVERSION)


# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE

DEFINES=	-D$(SYSTYPE) $(DMFLAGS)

# Uncomment the following lines if dmalloc debugging is to be used
#OURLIBS= -L$(TREE)/lib -lconfigdm -lsgmldm -ltclhashdm -ldb -ldiagdm \
#	-ldmalloc -ll -lm -lc 
OURLIBS= -L$(TREE)/lib -lconfig -lsgml -ltclhash -ldb -ldiag -ll -lm -lc $(SYSLIBS)


LIBS= $(OURLIBS) 

DEMOCCOPTS=	$(CFLAGS)
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq

all: libfileio.a

#

PROGMODS=  fil_add.o fil_replace.o fil_del.o fil_hist.o fil_seek.o \
	fil_read.o fil_getsgml.o fil_load_xml2.o DataStore.o

# ----------------
#  MAIN STUFF
# ----------------

libfileio.a: $(PROGMODS)
	@ echo "Making fileio library"
	@ rm -f libfileio.a 
	@ ar cr libfileio.a $(PROGMODS)
	@ $(RANLIB) libfileio.a

fil_test: libfileio.a fil_drive.o getsgmlrec.o
	@ echo "Making Test program"
	@ $(CC) $(CFLAGS) -o fil_test getsgmlrec.o\
		fil_drive.o -L. -lfileio $(OURLIBS)

xtest:  libfileio.a getsgmlrec.o xtest.o 
	@ echo "Making xTest program"
	@ $(CC) $(CFLAGS) -o xtest getsgmlrec.o\
		xtest.o -L. -lfileio $(OURLIBS)

DataStore.o: ../utils/DataStore.c
	@ $(CC) $(CFLAGS) $(DEMOCCOPTS) $(DEFINES) -c -o DataStore.o ../utils/DataStore.c

clean: 
	rm -f *.o *.a zz.* xtest core

reallyclean: clean
	rm -f *~

cleandata: 
	rm -f zz.*

.c.o:
	$(CC) $(CFLAGS) $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
# DO NOT DELETE THIS LINE -- make depend depends on it.

