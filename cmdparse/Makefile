#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE= SOLARIS
SD=	/usr/local/include
CHES= 	/u
ID=	../header
LIBDIR = ../lib
LIBZ=	../lib
FLEX= /usr/local/bin/flex
CC=	gcc
GCFLAGS=-g -fPIC 
LDFLAGS=	$(GCFLAGS)
RANLIB= :
# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK -D$(SYSTYPE)
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE) -DCMD_PARSE_SERVER -DNEEDUSERIDS
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE) -DCMD_PARSE_CLIENT -DUSE_STDARGS

CFLAGS=		$(GCFLAGS) \
		-I. -I$(ID) -I$(LIBZ) -I$(TKDIR) -I$(TCLDIR) $(XINC) \
		$(DMFLAGS) $(ZVERSION)


# Uncomment the following for DMALLOC memory checking version
#
#DMALLOCLIB=      -ldmalloc
DMALLOCLIB=
#SERVERSIDE= -lsearch -ltclhash -ldiag

LIBS=	 -L$(LIBDIR) $(DMALLOCLIB) -lz3_asn -lclient -lutils  -lz3_asn \
		$(SERVERSIDE) -lm -lc

DEMOCCOPTS=	$(CFLAGS)

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: libcmdparse.a 
# testparse

#
DEFINES=

# the following define should contain a list of all modules

GRAMMARFILES= q_grammar.g q_scan.flex

GRAMOBJECTS= z__pars.o q_grammar.o lex.z__.o queryparse.o 

# ----------------
#  The detailed instructions for building the application
# ----------------
libcmdparse.a: $(GRAMOBJECTS)
	@ echo "Making Command Parse Library"
	@ rm -f libcmdparse.a
	@ ar cr libcmdparse.a $(GRAMOBJECTS)
	@ $(RANLIB) libcmdparse.a
	@ cp libcmdparse.a ../lib

testparse: libcmdparse.a testparse.o check_result_set.o
	@ echo "making testparse"
	$(CC) $(CFLAGS) $(DEFINES) -o testparse \
		testparse.o check_result_set.o -L. -lcmdparse $(LIBS) -llist

z__pars.o: z__pars.c
	@ $(CC) $(DEMOCCOPTS) -c z__pars.c

z__pars.c: $(GRAMMARFILES)
	@ LLgen q_grammar.g

lex.z__.c: $(GRAMMARFILES)
	@ $(FLEX) -L -i -Pz__ q_scan.flex
	@ $(CC) $(DEMOCCOPTS) -c lex.z__.c

q_grammar.o: q_grammar.c
	@ $(CC) $(DEMOCCOPTS) -c q_grammar.c

q_grammar.c: $(GRAMMARFILES)
	@ LLgen q_grammar.g

clean: 
	rm -f *.o libcmdparse.a libcmdparsedm.a q_grammar.c lex.z__.c z__pars* testparse 

cleandata: 
	rm -f TESTDATA/dictionary* TESTDATA/mainfile*

reallyclean: clean cleandata
	rm -f *~

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
