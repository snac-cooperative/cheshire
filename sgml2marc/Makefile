#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE= SOLARIS
TREE=	..
SD=	/usr/local/include
ID=	$(TREE)/header
LIBDIR = $(TREE)/lib
RANLIB = :

#GCFLAGS=-g -O3 -ansi -pedantic
GCFLAGS=-g -ansi -pedantic -fPIC
LDFLAGS=	$(GCFLAGS)
CFLAGS=		$(GCFLAGS) \
		-I$(ID) -I$(SD) -I$(TKDIR) -I$(TCLDIR) $(XINC) $(ZVERSION)


LIBS=	-L$(LIBDIR) -lsgml -lmarc -ltclhash ${REENT} ${SYSLIBS} -lm -lc

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: sgml2marc


#DEFINES=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
DEFINES=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE)

# the following define should contain a list of all modules
MODS= tests2m.o getsgml.o sgml2marc.o getldr.o getvcfld.o getvdfld.o \
	s2mindi.o SFnorm.o cheshire_exit.o parse_util.o

MODS2= sgml2marc.o getldr.o getvcfld.o getvdfld.o s2mindi.o SFnorm.o

libsgml2marc.a: $(MODS2)
	@ echo "Making sgml2marc library"
	@ rm -f libsgml2marc.a 
	@ ar cr libsgml2marc.a $(MODS2)
	@ $(RANLIB) libsgml2marc.a

sgml2marc: $(MODS)
	@ echo "linking sgml2marc"
	@ $(CC) $(CFLAGS) -o sgml2marc \
		$(MODS) -L$(LIBDIR)  $(LIBS)

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c ../cheshire2/cheshire_exit.c

parse_util.o: ../sgmlparse/parse_util.c
	cp ../sgmlparse/parse_util.o .

clean: 
	rm -f *.o *.a sgml2marc

reallyclean: clean
	rm -f *~

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
