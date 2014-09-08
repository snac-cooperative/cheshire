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
TKDIR = /usr/local/src/tk8.0
TCLDIR = /usr/local/src/tcl8.0
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
		-I. -I$(ID) -I$(TREE)/wordnet/lib \
                -I$(TCLDIR) -I$(TKDIR) $(XINC) -I$(LOCINC) \
		$(DBMSINC) $(DBMSFLAG)	$(DMFLAGS) $(BITTYPES) $(ZVERSION)


# Uncomment the following for DMALLOC memory checking version
#
DMALLOCLIB=       -lsgml -ltclhash -ldiag -lmarc -ljserver \
  -lcmdparse -lsearch -lsgml -lsgml2marc -lindex -lfileio -lconfig -ldb \
   -ldiag -lnetconnect -llist -lutils -lsnowball


LIBS=		-L. -lindex -L$(LIBDIR) $(DMALLOCLIB) -lwn \
		$(REENT) $(SYSLIBS) $(DBMSLIB) -lm -lc

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: libjserver.a jserver
#
DEFINES=

# the following define should contain a list of all modules

MODS= jserver.o displayrec.o searchapi.o sgml_to_marc.o \
      jserver_utils.o connection.o server_init.o\
	elem_set_init.o dbms_intf.o explain.o stubs.o pdu_free.o \
	asn_free.o utils.o cheshire_exit.o check_result_set.o get_component.o

# the following is just the modules used in the library 
OBJS = jserver_utils.o 


# ----------------
#  The detailed instructions for building the application
#  including Z39.59 and the DBopen library
# ----------------
libjserver.a: ${OBJS}
	rm -f libjserver.a
	$(AR) $(ARFLAGS) libjserver.a ${OBJS}
	$(RANLIB) libjserver.a
	mv libjserver.a ${LIBDIR}

jserver: $(MODS) $(SYSOBJ)
	@ echo "Making jserver"
	@ $(CC) $(CFLAGS) $(DEFINES) -o jserver \
	 $(MODS)  $(SYSOBJ) $(LIBS) 

displayrec.o: ../zserver/displayrec.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o displayrec.o ../zserver/displayrec.c
searchapi.o: ../zserver/searchapi.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o searchapi.o ../zserver/searchapi.c

get_component.o: ../zserver/get_component.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o get_component.o ../zserver/get_component.c

server_init.o: ../zserver/server_init.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o server_init.o ../zserver/server_init.c

sgml_to_marc.o: ../zserver/sgml_to_marc.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o sgml_to_marc.o ../zserver/sgml_to_marc.c

elem_set_init.o: ../zserver/elem_set_init.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o elem_set_init.o ../zserver/elem_set_init.c

dbms_intf.o: ../zserver/dbms_intf.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o dbms_intf.o ../zserver/dbms_intf.c

explain.o: ../zserver/explain.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o explain.o ../zserver/explain.c

pdu_free.o: ../zpdu/pdu_free.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o pdu_free.o ../zpdu/pdu_free.c

asn_free.o: ../zpdu/asn_free.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o asn_free.o ../zpdu/asn_free.c

utils.o: ../zpdu/utils.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o utils.o ../zpdu/utils.c


strtok_r.o: ../search/strtok_r.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c ../search/strtok_r.c

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	@ $(CC) $(DEMOCCOPTS) -c ../cheshire2/cheshire_exit.c

check_result_set.o: ../cmdparse/check_result_set.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o check_result_set.o ../cmdparse/check_result_set.c

clean: 
	rm -f *.o *.a core jserver

reallyclean: clean 
	rm -f *~

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
