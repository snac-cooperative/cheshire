#################################################################
## Master Makefile for Cheshire II            
##
## make all -- makes and installs libraries
## make newbin -- makes and installs binaries in ./bin
#################################################################
#################################################################
## the SYSTEMTYPE variable should be either:
## ALPHA -- for DEC Alphas running OSF/1 or Digital Unix
## SOLARIS -- for Sun Sparcs under Solaris
## HP -- for HP workstations under HPUX
## LINUX -- for Linux systems
#################################################################
SYSTEMTYPE=LINUX

#################################################################
## The following should point to the include directory for 
## tk (4.0) and tcl (7.4) and the location of the libraries for
## them.
#################################################################
TK_DIR=	/usr/include
TCL_DIR= /usr/include
TK_LIBS= /usr/lib
TK_LIBINC =  "-ltk -ltkstub"
TCL_LIBINC = "-ltcl -ltclstub"

# Solaris socket libraries and functions -- comment out if not SOLARIS
#SYS_LIBS= "-lnsl -lsocket -lgen -ldl"
#SYS_OBJ= strtok_r.o

# LINUX library definitions -- piggybacks on SYS_LIBS -- comment out if not
# Linux
SYS_LIBS= "-ldl -ldb-3.3"

# If NOT solaris and you have reentrant libraries for strtok, etc. in libc_r.a
# define the following and comment out the SYS_OBJ above
#RE_ENTRANT= -lc_r

#some solaris system don't have the rusage struct and getrusage
#defining the following removes it from the code
#RUSAGE_DEF = "-DNO_RUSAGE"

X11_INCLUDES = -I/usr/X11R6/include
X11_LIBS = "-L/usr/X11R6/lib -lX11"


#################################################################
## Relational dbms interface code -- These should be blank if
## there is not an ILLUSTRA, POSTGRES95, INFORMIX, ORACLE, ETC.
## database file being accessed via a z39.50 gateway.
#################################################################
# DBMSFLAG should be POSTGRESQL or ILLUSTRA, (INFORMIX, or ORACLE not available yet) 
# DBMS_FLAG= "-DILLUSTRA"
DBMS_FLAG="-DPOSTGRESQL"
# DBMS_FLAG=
# These should show the location of the include files and libraries
# for the DBMS.
#DBMS_INC= "-I/usr/local/illustra/h"
#DBMS_LIB= "-L/usr/local/illustra/lib -lmi"
DBMS_INC="-I/usr/include/pgsql"
DBMS_LIB="-L/usr/lib/pgsql -lpq"
#DBMS_INC=
#DBMS_LIB=



#################################################################
## What make utility are you using?
#################################################################
MAKE = make

all: 
	cd ac_list; $(MAKE) SYSTYPE=$(SYSTEMTYPE)
	cd ac_utils; $(MAKE) SYSTYPE=$(SYSTEMTYPE)
	cd zpdu; $(MAKE) SYSTYPE=$(SYSTEMTYPE)
	cd socket; $(MAKE) SYSTYPE=$(SYSTEMTYPE)
	cd client; $(MAKE) SYSTYPE=$(SYSTEMTYPE)
	cd sgmlparse; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) SYSLIBS=$(SYS_LIBS) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) libsgml.a ; cp lib*.a ../lib
	cd config; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) XINC=$(X11_INCLUDES) DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) libconfig.a ; cp lib*.a ../lib
	cd fileio; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) XINC=$(X11_INCLUDES) libfileio.a ; cp lib*.a ../lib
	cd tclhash; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) libtclhash.a ; cp lib*.a ../lib
	cd marclib; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) libmarc.a ; cp lib*.a ../lib
	cd diagnostic; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) libdiag.a ; cp lib*.a ../lib
	cd cmdparse; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) libcmdparse.a ; cp lib*.a ../lib
	cd cmdparse; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) testparse ; cp testparse ../bin
	cd sgml2marc; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) libsgml2marc.a ; cp lib*.a ../lib
	cd wordnet/lib; $(MAKE) SYSTYPE=$(SYSTEMTYPE) lib ; cp lib*.a ../../lib
	cd search; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) XINC=$(X11_INCLUDES)  DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) libsearch.a ; cp lib*.a ../lib
	cd index; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) XINC=$(X11_INCLUDES) libindex.a ; cp lib*.a ../lib
	cd cheshire2; $(MAKE) SYSTYPE=$(SYSTEMTYPE)  TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  XINC=$(X11_INCLUDES) XLIBS=$(X11_LIBS) SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) REENT=$(RE_ENTRANT) DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB)
	cd zserver; $(MAKE) SYSTYPE=$(SYSTEMTYPE)  TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  XINC=$(X11_INCLUDES) XLIBS=$(X11_LIBS) SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) REENT=$(RE_ENTRANT) DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB)

bin: all
	cd utils; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) DBMSLIB=$(DBMS_LIB) all ; cp buildassoc dumprecs getnumrecs cheshire_load read_datastore ../bin
	cd config; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) test_config ; cp test_config ../bin
	cd sgmlparse; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) all ; cp dtd_parser schema_parser parser ../bin
	cd search; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) REENT=$(RE_ENTRANT)  DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) testsrch2 ; cp testsrch2 ../bin
	cd index; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) RUSAGE=$(RUSAGE_DEF) SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES)   DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) REENT=$(RE_ENTRANT) all ; cp index_cheshire index_clusters in_test dumpdb dumpds countdb dumppost highpost dumpcomp dumpprox delete_recs batch_load ../bin
	cd marc2sgml; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) REENT=$(RE_ENTRANT) all; cp marc2sgml ../bin
	cd marclib; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC) cpmarc ; cp cpmarc ../bin
	cd sgml2marc; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) REENT=$(RE_ENTRANT) sgml2marc ; cp sgml2marc ../bin
	cd cheshire2; $(MAKE) SYSTYPE=$(SYSTEMTYPE) TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) XLIBS=$(X11_LIBS) XINC=$(X11_INCLUDES) REENT=$(RE_ENTRANT)  DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) all; cp cheshire2 ztcl webcheshire staffcheshire ../bin
	cd zserver; $(MAKE) SYSTYPE=$(SYSTEMTYPE)  TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  XINC=$(X11_INCLUDES) XLIBS=$(X11_LIBS) SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) REENT=$(RE_ENTRANT)  DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) all; cp zserver ../bin
	cd jserver; $(MAKE) SYSTYPE=$(SYSTEMTYPE)  TKDIR=$(TK_DIR) TCLDIR=$(TCL_DIR) TKLIBS=$(TK_LIBS) TKLIBINC=$(TK_LIBINC) TCLLIBINC=$(TCL_LIBINC)  XINC=$(X11_INCLUDES) XLIBS=$(X11_LIBS) SYSLIBS=$(SYS_LIBS) SYSOBJ=$(SYS_OBJ) REENT=$(RE_ENTRANT)  DBMSFLAG=$(DBMS_FLAG) DBMSINC=$(DBMS_INC) DBMSLIB=$(DBMS_LIB) all; cp jserver ../bin


clean:
	cd ac_list; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd ac_utils; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd zpdu; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd socket; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd client; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd config; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd fileio; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd utils; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd tclhash; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd marclib; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd marc2sgml; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd diagnostic; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd cmdparse; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd sgmlparse; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd sgml2marc; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd wordnet/lib; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd search; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd index; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd cheshire2; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd zserver; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean
	cd jserver; $(MAKE) SYSTYPE=$(SYSTEMTYPE) clean

reallyclean: clean
	rm -rf bin/*
	rm -rf lib/*
	cd index; $(MAKE) SYSTYPE=$(SYSTEMTYPE) reallyclean
	cd wordnet/lib; rm -f libwn.a

newbin:
	touch cheshire2/z_init.c
	touch zserver/zserver.c
	touch index/idxxkey.c
	- mkdir lib
	- mkdir bin
	$(MAKE) bin
