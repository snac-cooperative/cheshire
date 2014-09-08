# all these things will need to point to the
# proper places
SYSTYPE= ALPHA
TREE=	..
OD=	.
SD=	.

ID=	$(TREE)/header
IDLOCAL= /usr/local/include
LIBDIR= $(TREE)/lib
TKDIR=	/usr/local/include
TCLDIR=	/usr/local/include
TKLIBS= /usr/local/lib
#LOCLIBS= /usr/local/lib
#XLIBS=	-L/usr/lib/libX11.a -lX11
TKLIBINC= -ltk8.3 -ltkstub8.3
TCLLIBINC= -ltcl8.3 -ltclstub8.3

# if you don't have the re-entrant version of the c library
# clear the following definition
#REENT= -lc_r
SYSOBJ= 

# Linker switch(es) to use to link with the X11 library archive (the
# configure script will try to set this value automatically, but you
# can override it).


# Libraries to use when linking:  must include at least Tk, Tcl, Xlib,
# and the math library (in that order).  The "" part will be
# replaced (or has already been replaced) with relevant libraries as
# determined by the configure script.
OURLIBS= -L. -L$(LIBDIR) $(SYSLIBS) -lm -lc $(REENT)

LIBS= -L$(TKLIBS) $(TKLIBINC) $(TCLLIBINC) $(XLIBS) $(OURLIBS)

CC=	gcc
GCFLAGS=-g
LDFLAGS=	$(GCFLAGS)
CFLAGS=		$(GCFLAGS) -I. \
		-I$(TKDIR) -I$(TCLDIR) -I$(ID) \
		 $(XINC) $(ZVERSION)
DIRS=		./dirs.mk
DEMOCCOPTS=	$(CFLAGS) 
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq .g .flex


#
DEFINES=-DNO_EXP10 -DNO_RINT -DNO_INF -DNO_ALLOCA -D$(SYSTYPE) -DUSE_STDARGS -DEXTERNAL_TAG_MATCH -DDMALLOC_INCLUDE -DDMALLOC_DISABLE 


MODS=zMain.o loadcmds.o pLogTool.o $(SYSOBJ)


# ----------------
#  Cheshire
# ----------------

all: logtool

logtool: $(MODS) 
	@ echo "linking logtool"
	$(CC) $(CFLAGS) -o logtool \
		$(MODS) $(LIBS) 

strtok_r.o: ../search/strtok_r.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o strtok_r.o ../search/strtok_r.c

clean: 
	rm -f *.o logtool

.c.o:
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
