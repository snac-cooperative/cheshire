#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE=ALPHA
TREE=	..
ID=	../header
LD=	$(TREE)
TKDIR = /usr/local/src/tk4.0
TCLDIR = /usr/local/src/tcl7.4
TKLIBS = /usr/local/lib
LIBDIR = $(TREE)/lib
LOCINC = /usr/local/include
REENT = -lc_r
#DBMSFLAG= -DILLUSTRA
#DBMSINC= -I/usr/local/illustra/h
#DBMSLIB= -L/usr/local/illustra/lib -lmi

ARFLAGS = crv
RANLIB	= :

GCFLAGS=-g
LDFLAGS=	$(GCFLAGS)

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -DDEBUG
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE) -DCMD_PARSE_SERVER

CFLAGS=		$(GCFLAGS) \
		-I. -I$(ID) -I/usr/include -I$(TREE)/wordnet/lib \
                -I$(TCLDIR) -I$(TKDIR) $(XINC) -I$(LOCINC) \
		$(DBMSINC) $(DBMSFLAG)	$(DMFLAGS) $(BITTYPES) $(ZVERSION)


# Uncomment the following for DMALLOC memory checking version
#
DMALLOCLIB=       -lsgml -ltclhash -ldiag -lmarc -lzserver \
  -lcmdparse -lsearch -lsgml -lsgml2marc -lindex -lfileio -lconfig -ldb \
   -lz3_asn -ldiag -lnetconnect -llist -lutils -lsnowball


LIBS=		-L. -lindex -L$(LIBDIR) $(DMALLOCLIB) -lwn \
		-lm -lc $(REENT) $(SYSLIBS) $(DBMSLIB)

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: libzserver.a zserver
test: libzserver.a test_server_init
#
DEFINES=

# the following define should contain a list of all modules

MODS= zserver.o displayrec.o searchapi.o sgml_to_marc.o \
      server_utils.o server_init.o  check_result_set.o \
      elem_set_init.o dbms_intf.o explain.o cheshire_exit.o \
      get_component.o in_tag_data.o sort_results.c

# the following is just the modules used in the library 
OBJS = server_utils.o server_init.o \
	elem_set_init.o dbms_intf.o explain.o


# ----------------
#  The detailed instructions for building the application
#  including Z39.59 and the DBopen library
# ----------------
libzserver.a: ${OBJS}
	rm -f libzserver.a
	$(AR) $(ARFLAGS) libzserver.a ${OBJS}
	$(RANLIB) libzserver.a
	mv libzserver.a ${LIBDIR}

zserver: $(MODS) $(SYSOBJ)
	@ echo "Making zserver"
	@ $(CC) $(CFLAGS) $(DEFINES) -o zserver \
	 $(MODS)  $(SYSOBJ) $(LIBS) 


test_server_init: ${LIBDIR}/libzserver.a test_server_init.o $(SYSOBJ)
	@ echo "linking test_server_init"
	@ $(CC) $(CFLAGS) -o test_server_init \
		test_server_init.o $(SYSOBJ) -L$(LIBDIR) \
		-lzserver -lz3_asn -llist

strtok_r.o: ../search/strtok_r.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c ../search/strtok_r.c

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	@ $(CC) $(DEMOCCOPTS) -c ../cheshire2/cheshire_exit.c

in_tag_data.o: ../index/in_tag_data.c
	@ $(CC) $(DEMOCCOPTS) -c ../index/in_tag_data.c

check_result_set.o: ../cmdparse/check_result_set.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o check_result_set.o ../cmdparse/check_result_set.c

clean: 
	rm -f *.o *.a core zserver

reallyclean: clean 
	rm -f *~

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
