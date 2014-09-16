SYSTYPE= SOLARIS
# all these things will need to point to the
# proper places
ID=	.
TKDIR=	/usr/local/src/tk4.0
TCLDIR=	/usr/local/src/tcl7.4
TKLIBS=	/usr/local/lib
XLIB=	/usr/lib/libX11.a
ZLIB=  ../lib
LIBDIR= ../lib
DMALLOC_LIB_PATH= /opt/lib

GCFLAGS= -g -fPIC
LDFLAGS=	$(GCFLAGS)
RANLIB=:
#DBMSFLAG= -DILLUSTRA
#DBMSINC= -I/usr/local/illustra/h
#DBMSLIB= -L/usr/local/illustra/lib -lmi

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE

CFLAGS=		$(GCFLAGS) \
		-I$(ID) -I../header \
		-I$(TCLDIR) -I$(TKDIR) -I/usr/local/include $(XINC) \
		$(DBMSINC) -D$(SYSTYPE) $(DMFLAGS) $(DBMSFLAG) $(ZVERSION)

# Uncomment the following for DMALLOC memory checking version
#DMALLOCLIB=      -ldmalloc

LIBS=		$(DMALLOCLIB) $(DBMSLIB) -lm -lc

# Uncomment the following for DMALLOC memory checking version
#XTRALIBS=	-L../lib -lsgmldm -ltclhashdm -ldb -ldiagdm
XTRALIBS=	-L../lib -lsgml -ltclhash -ldb -ldiag  $(SYSLIBS)

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq

all: libconfig.a test_config

#
DEFINES=

GRAMMARFILES= cf_grammar.g cf_parse.flex

PROGMODS= cf_grammar.o CFpars.o lex.cf_.o cf_DBMS.o \
  cf_initialize.o cf_print_info.o cf_open.o cf_open_cont.o cf_createall.o \
  cf_closeall.o cf_close.o cf_free_info.o cf_getidx.o cf_getstop.o \
  cf_getftype.o cf_getfname.o cf_getnumdocs.o cf_getclus.o cf_getdisplay.o \
  cf_getdispopt.o cf_build_xpath.o

# ----------------
#   MAIN
# ----------------

libconfig.a: $(PROGMODS)
	@ echo "Making config library"
	@ rm -f libconfig.a 
	@ ar cr libconfig.a $(PROGMODS)
	@ $(RANLIB) libconfig.a
	@ cp libconfig.a $(LIBDIR)

test_config: cf_check.o cf_xtra.o cheshire_exit.o libconfig.a parse_util.o
	@ echo "linking test_config main"
	$(CC) $(CFLAGS) -o test_config \
		cf_check.o cf_xtra.o cheshire_exit.o parse_util.o \
		-L. -lconfig $(XTRALIBS) $(LIBS) -lconfig

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	@ $(CC) $(DEMOCCOPTS) -c ../cheshire2/cheshire_exit.c

parse_util.o: ../sgmlparse/parse_util.c
	@ $(CC) $(DEMOCCOPTS) -c ../sgmlparse/parse_util.c

lex.cf_.c: cf_parse.flex
	@ flex -L -i -Pcf_ cf_parse.flex

lex.cf_.o: lex.cf_.c cf_grammar.c
	@ $(CC) $(DEMOCCOPTS) -DDO_LL -c lex.cf_.c

CFpars.o: cf_grammar.c
	@ $(CC) $(DEMOCCOPTS) -c CFpars.c

cf_grammar.c: cf_grammar.g
	@ LLgen cf_grammar.g

clean: 
	rm -f *.o xx.* *.a CFpars.* lex.cf_.* cf_grammar.c core cf_test cf_testmap test_config
 
reallyclean: clean cleandata
	rm -f *~
	rm -f TESTDL/*~
	rm -f TESTDATA/*~

cleandata:
	rm -f xx.*
	rm -f TESTDL/DL.* 
	rm -f TESTDATA/COMPONENT* TESTDATA/dictionary* TESTDATA/xx* TESTDATA/*index* TESTDATA/*history

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
# DO NOT DELETE THIS LINE -- make depend depends on it.
