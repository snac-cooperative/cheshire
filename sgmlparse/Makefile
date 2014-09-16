SYSTYPE= ALPHA
# all these things will need to point to the
# proper places
TREE=	.
OD=	.
SD=	.
ID=	../header
TKDIR=	/usr/local/src/tk4.0
TCLDIR=	/usr/local/src/tcl7.4
TKLIBS=	/usr/local/lib
XLIBS=	/usr/lib/libX11.a
LIBDIR= ../lib
RANLIB= :

GCFLAGS= -g
LDFLAGS=	$(GCFLAGS)

#DBMSFLAG= -DPOSTGRES
#DBMSINC= -I/export/home/postgres/pgsql/include
#DBMSLIB= -L/export/home/postgres/pgsql/lib -lpq



# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE

DEFINES=	-D$(SYSTYPE)

CFLAGS=		$(GCFLAGS) \
		-I. -I$(ID) -I$(TCLDIR) -I$(TKDIR) \
		-I/usr/local/include $(XINC) \
		$(DEFINES) $(DMFLAGS) $(ZVERSION)

DIRS=		./dirs.mk
# Uncomment the following for DMALLOC memory checking
#DMALLOCLIB=	-ldmalloc

LIBS=	        $(DMALLOCLIB)  $(SYSLIBS) -ll -lm -lc 
DMLIBS2=	-L$(LIBDIR) -lfileio -lconfig -ltclhash -ldiag $(DBMSLIB) $(SYSLIBS) -ll -lm -lc 

DEMOCCOPTS=	$(CFLAGS) -fPIC

CC=	gcc
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq

all: libsgml.a
	@ make parser
	@ make dtd_parser
	@ make schema_parser
	@ make element_count

#

GRAMMARFILES= sgml_gram.g sgml_scan.flex

PROGMODS2= sgml_pars.o sgml_gram.o lex.sgml_.o sgml_complete.o \
   sgml_dump_dtd.o sgml_dump_doc.o sgml_intrface.o xml_schema_2_dtd.o \
   sgml_free_doc.o sgml_free_dtd.o sgml_free_dcl.o sgml_elim_entities.o

# ----------------
#  test
# ----------------

libsgml.a: $(PROGMODS2)
	@ echo "Making sgml library"
	@ rm -f libsgml.a 
	@ ar cr libsgml.a $(PROGMODS2)
	@ $(RANLIB) libsgml.a
	@ cp libsgml.a $(LIBDIR)

parser: parse.o cf_xtra.o in_cheshire_exit.o libsgml.a parse_util.o
	$(CC) $(CFLAGS) -DDO_LL -o parser parse.o cf_xtra.o in_cheshire_exit.o \
	parse_util.o -L. -lsgml $(DMLIBS2) -ldb $(DMALLOCLIB)

element_count: element_count.o cf_xtra.o in_cheshire_exit.o libsgml.a parse_util.o
	$(CC) $(CFLAGS) -DDO_LL -o element_count element_count.o cf_xtra.o in_cheshire_exit.o \
	parse_util.o -L. -lsgml $(DMLIBS2) -ldb $(DMALLOCLIB)

dtd_parser: dtd_parse.o cf_xtra.o cheshire_exit.o libsgml.a parse_util.o
	$(CC) $(CFLAGS) -DDO_LL -o dtd_parser dtd_parse.o cf_xtra.o \
	parse_util.o cheshire_exit.o -L. -lsgml $(DMLIBS2) -ldb $(DMALLOCLIB)

schema_parser: schema_parse.o cf_xtra.o cheshire_exit.o libsgml.a parse_util.o
	$(CC) $(CFLAGS) -DDO_LL -o schema_parser schema_parse.o cf_xtra.o \
	parse_util.o cheshire_exit.o -L. -lsgml $(DMLIBS2) -ldb $(DMALLOCLIB)

lex.sgml_.o: lex.sgml_.c
	@ $(CC) $(DEMOCCOPTS) -DDO_LL -c lex.sgml_.c 

lex.sgml_.c: sgml_scan.flex
	@ flex -L -Psgml_ sgml_scan.flex

sgml_pars.o: sgml_gram.c
	@ $(CC) $(DEMOCCOPTS) -c sgml_pars.c 

sgml_gram.c: sgml_gram.g
	@ /usr/local/bin/LLgen sgml_gram.g

cf_xtra.o: ../config/cf_xtra.c
	@ $(CC) $(DEMOCCOPTS) -c ../config/cf_xtra.c 

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	@ $(CC) $(DEMOCCOPTS) -c ../cheshire2/cheshire_exit.c

in_cheshire_exit.o: ../index/in_cheshire_exit.c
	@ $(CC) $(DEMOCCOPTS) -c ../index/in_cheshire_exit.c

clean: 
	rm -f *.o libsgml.a libsgmldm.a parser dtd_parser core schema_parser element_count 

cleandata: 
	rm -f xx.* yy.* zz.*

reallyclean: clean cleandata
	rm -f *~ sgml_gram.c sgml_pars.c sgml_pars.h lex.sgml_.c

.c.o:
	$(CC) $(DEMOCCOPTS) -c $(<F)

#
# DEPENDENCIES (generated, do not modify by hand)
# 
# DO NOT DELETE THIS LINE -- make depend depends on it.
