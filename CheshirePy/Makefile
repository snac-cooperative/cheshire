#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE= LINUX
TREE= ..
ID=	$(TREE)/header
RANLIB= :

TKDIR=/usr/include/tk
TCLDIR= /usr/include/tcl
TKLIBS= /usr/include/tk
TKLIBINC =  -ltk -ltk8.6
TCLLIBINC = -ltcl -ltcl8.6

LIBDIR = $(TREE)/lib
LIBZ=	$(TREE)/lib
XINC=  -I/usr/include 
REENT= 
SYSLIBS= -L/usr/include -lxml2 -ldl -lcrypt -lpthread -lpq

GCFLAGS=-g -O2 -fPIC
LDFLAGS=	$(GCFLAGS)

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK -D$(SYSTYPE)
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE) -DCMD_PARSE_SERVER 

CFLAGS=		$(GCFLAGS) \
                -I$(ID) -I$(TCLDIR) -I$(TKDIR) $(XINC) \
		$(DBMSINC) $(DBMSFLAG) $(DMFLAGS) $(ZVERSION)


DMALLOCLIB=         -lsgml -ltclhash -lindex -lsearch -lfileio -lconfig -lcmdparse -lmarc -lsgml2marc -ldiag -lsnowball -lz3_asn

PYTHONLIB= -L/usr/include/python2.7 -lpython2.7

LIBS=	 -L$(LIBDIR) $(DMALLOCLIB) ${SYSLIBS} ${TCLLIBINC} ${PYTHONLIB} -lwn -ldb -lm -lc ${REENT} ${DBMSLIB}  -L$(LIBDIR) $(DMALLOCLIB) ${SYSLIBS} ${TCLLIBINC} -lwn -ldb -lm -lc ${REENT} ${DBMSLIB} ${PYTHONLIB}

DEMOCCOPTS=	$(CFLAGS)
PYTHONINC= -I/usr/include/python2.7
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: testCheshirePy _CheshirePy.so

#
DEFINES=

# the following define should contain a list of all modules


# ----------------
#  The detailed instructions for building the application
#  including tcl/tk and Z39.59 and the DBopen library
# ----------------
#

testCheshirePy: cp_driver.o CheshirePy.o sgml_to_marc.o z_dispmarc.o z_dispsgml.o \
	displayrec.o pSFnorm.o cheshire_exit.o  \
	check_result_set.o get_component.o  $(SYSOBJ)
	@ echo "making testCheshirePy"
	$(CC) $(CFLAGS) $(DEFINES) -DSGML_INITIALIZE -o testCheshirePy \
	   cp_driver.o CheshirePy.o sgml_to_marc.o z_dispmarc.o z_dispsgml.o \
	   displayrec.o \
	   cheshire_exit.o  check_result_set.o get_component.o pSFnorm.o \
	    $(SYSOBJ) $(LIBS)


_CheshirePy.so: CheshirePy.o CheshirePy_wrap.o sgml_to_marc.o z_dispmarc.o z_dispsgml.o \
	displayrec.o pSFnorm.o cheshire_exit.o  \
	check_result_set.o get_component.o  $(SYSOBJ)
	@ echo "making _CheshirePy.so"
	$(CC) $(CFLAGS) $(DEFINES) -DSGML_INITIALIZE -shared -o _CheshirePy.so \
	   CheshirePy_wrap.o CheshirePy.o \
	   sgml_to_marc.o z_dispmarc.o z_dispsgml.o displayrec.o \
	   cheshire_exit.o  check_result_set.o get_component.o pSFnorm.o \
	   $(SYSOBJ) $(LIBS)


CheshirePy_wrap.o: CheshirePy.o CheshirePy.i
	@ echo "Making wrapper code via SWIG"
	@ swig -python CheshirePy.i
	$(CC) $(DEMOCCOPTS) $(PYTHONINC) $(DEFINES) -c -o CheshirePy_wrap.o CheshirePy_wrap.c

reallyclean: clean cleanex
	rm -f *~ 

clean: 
	rm -f *.o *.so *.a  _CheshirePy.so CheshirePy_wrap.c CheshirePy.py* core

cleanex: 
	rm -f  _CheshirePy.so CheshirePy_wrap.c CheshirePy.py* testCheshirePy

sgml_to_marc.o: ../zserver/sgml_to_marc.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o sgml_to_marc.o ../zserver/sgml_to_marc.c

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	@ $(CC) $(DEMOCCOPTS) -c ../cheshire2/cheshire_exit.c

displayrec.o: ../zserver/displayrec.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o displayrec.o ../zserver/displayrec.c
z_dispmarc.o: ../cheshire2/z_dispmarc.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o z_dispmarc.o ../cheshire2/z_dispmarc.c

get_component.o: ../zserver/get_component.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o get_component.o ../zserver/get_component.c

z_dispsgml.o: ../cheshire2/z_dispsgml.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o z_dispsgml.o ../cheshire2/z_dispsgml.c

check_result_set.o: ../cmdparse/check_result_set.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o check_result_set.o ../cmdparse/check_result_set.c

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)

.c.so:
	$(CC)  $(DEMOCCOPTS) -shared $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
