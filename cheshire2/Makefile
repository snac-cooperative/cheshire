# type of CPU/OS only SOLARIS, ALPHA, and HP -- usually set by ../Makefile
SYSTYPE= SOLARIS
# all these things will need to point to the
# proper places
TREE=	..
OD=	.
SD=	.

ID=	$(TREE)/header
IDLOCAL= /usr/local/include
LIBDIR= $(TREE)/lib
LOCLIBS= /usr/local/lib

# Libraries to use when linking:  must include at least Tk, Tcl, Xlib,
# and the math library (in that order).  The "" part will be
# replaced (or has already been replaced) with relevant libraries as
# determined by the configure script.
OURLIBS= -L. -L$(LIBDIR) -lmarc -lz3_asn -lclient \
  -lnetconnect -lcmdparse -lsgml -lsgml2marc -ldiag -lwn -llist \
  -lutils -lsnowball -lm -lc $(SYSLIBS)

LIBS= -L$(TKLIBS) $(TKLIBINC) $(TCLLIBINC) $(OURLIBS) $(REENT) $(XLIBS)

ZTCLLIBS= -L$(LIBDIR) -L$(TKLIBS) $(TCLLIBINC) -lz3_asn  -lclient -lmarc \
  -lcmdparse  -lsgml -lsgml2marc -ldiag -lwn -lnetconnect -llist \
  -lutils -lsnowball $(GDLIBS) -lm -lc $(REENT) $(SYSLIBS)

LTCLLIBS= -L$(LIBDIR)  -L$(TKLIBS) $(TCLLIBINC) -lsearch -lsgml -ltclhash \
  -lindex  -lfileio -lconfig -lcmdparse -lmarc -lsgml2marc -ldiag  -lwn \
  -ldb -lz3_asn  -lclient -lmarc \
  -lnetconnect -llist -lutils -lsnowball -lm -lc $(REENT) $(SYSLIBS) $(DBMSLIB)

STCLLIBS= -L$(LIBDIR)  -L$(TKLIBS) $(TKLIBINC) $(TCLLIBINC) -lsearch -lsgml -ltclhash \
  -lindex  -lfileio -lconfig -lcmdparse -lmarc -lsgml2marc -ldiag  -lwn \
  -ldb -lz3_asn  -lclient -lmarc  -lnetconnect -llist -lutils -lsnowball \
  -lm -lc $(XLIBS) $(REENT) $(SYSLIBS) $(DBMSLIB)


CC=	gcc
GCFLAGS=-g -D USE_INTERP_RESULT
LDFLAGS=	$(GCFLAGS)
CFLAGS=		$(GCFLAGS) $(DBMSFLAG) \
		-I$(ID)  -I../index \
		-I$(TKDIR) -I$(TCLDIR) $(XINC) $(DBMSINC) $(ZVERSION) 

DIRS=		./dirs.mk
DEMOCCOPTS=	$(CFLAGS) 
#
# Makefile for making the object files to be loaded
# into the demo.  
#
.SUFFIXES: .o .c .temp .pq .g .flex


# defines 
DEFINES=-DNO_EXP10 -DNO_RINT -DNO_INF -DNO_ALLOCA -D$(SYSTYPE) -DUSE_STDARGS -DEXTERNAL_TAG_MATCH -DDMALLOC_INCLUDE -DDMALLOC_DISABLE

MODS=zMain.o web_dummy.o zAppInit.o loadcmds.o zquery.o lcctree.o \
	z_init.o z_format.o z_dispmarc.o z_dispopac.o z_dispgrs1.o pTmpNam.o \
	z_dispsgml.o cf_handling.o pTranLog.o sResults.o pSFnorm.o \
	z_explain.o  cheshire_exit.o check_result_set.o \
	loadmapcmds.o mapwidget.o conversion.o z_highlight.o \
        in_tag_data.o detag_block.o stagcmp.o stem.o

ZTCLMODS=ztcl.o web_dummy.o zAppInit.o loadcmds.o zquery.o lcctree.o \
	z_init.o z_format.o z_dispmarc.o z_dispopac.o z_dispgrs1.o pTmpNam.o \
	z_dispsgml.o cf_handling.o pTranLog.o sResults.o pSFnorm.o \
	z_explain.o cheshire_exit.o check_result_set.o loadmapcmds_nw.o \
	conversion_nw.o $(GDMODS)  z_highlight.o in_tag_data.o detag_block.o stagcmp.o stem.o

LTCLMODS=ztcl.o web_driver.o zAppInit.o loadcmds.o zquery.o lcctree.o \
	z_init.o z_format.o z_dispmarc.o z_dispopac.o z_dispgrs1.o pTmpNam.o \
	z_dispsgml.o pTranLog.o sResults.o pSFnorm.o pseudo_doc.o \
	displayrec.o z_explain.o cheshire_exit.o check_result_set.o \
	get_component.o explain.o sort_results.o dbms_local_intf.o \
	loadmapcmds_nw.o conversion_nw.o sgml_to_marc.o  z_highlight.o

STCLMODS=zStaff.o web_driver.o zAppInit.o loadcmds.o zquery.o lcctree.o \
	z_init.o z_format.o z_dispmarc.o z_dispopac.o z_dispgrs1.o pTmpNam.o \
	z_dispsgml.o pTranLog.o sResults.o pSFnorm.o pseudo_doc.o \
	displayrec.o z_explain.o cheshire_exit.o  check_result_set.o \
	get_component.o explain.o loadmapcmds.c mapwidget.o conversion.o \
	sort_results.o dbms_local_intf.o sgml_to_marc.o  z_highlight.o

# ----------------
#  Cheshire
# ----------------

all: cheshire2 ztcl webcheshire staffcheshire

cheshire2: $(MODS) $(SYSOBJ)
	@ echo "linking cheshire2"
	$(CC) $(CFLAGS) -o cheshire2 \
		$(MODS) $(SYSOBJ) $(LIBS) 

ztcl: $(ZTCLMODS) $(SYSOBJ)
	@ echo "linking ztcl"
	$(CC) $(CFLAGS) -o ztcl \
		$(ZTCLMODS) $(SYSOBJ) $(ZTCLLIBS) 

webcheshire: $(LTCLMODS) $(SYSOBJ)
	@ echo "linking webcheshire"
	$(CC) $(CFLAGS) -o webcheshire \
		$(LTCLMODS) $(SYSOBJ) $(LTCLLIBS) 


staffcheshire: $(STCLMODS) $(SYSOBJ)
	@ echo "linking staffcheshire"
	$(CC) $(CFLAGS) -o staffcheshire \
		$(STCLMODS) $(SYSOBJ) $(STCLLIBS) 


clean: 
	rm -f *.o cheshire2 ztcl webcheshire staffcheshire

reallyclean: clean
	rm -f *~ core

displayrec.o: ../zserver/displayrec.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o displayrec.o ../zserver/displayrec.c

gdCmd.o: ../gdtclft-2.2.5/gdCmd.c
	cp ../gdtclft-2.2.5/gdCmd.o .

gdhandle.o: ../gdtclft-2.2.5/gdhandle.c
	cp ../gdtclft-2.2.5/gdhandle.o .

get_component.o: ../zserver/get_component.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o get_component.o ../zserver/get_component.c

explain.o: ../zserver/explain.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o explain.o ../zserver/explain.c

strtok_r.o: ../search/strtok_r.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o strtok_r.o ../search/strtok_r.c

sort_results.o: ../zserver/sort_results.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o sort_results.o ../zserver/sort_results.c

sgml_to_marc.o: ../zserver/sgml_to_marc.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o sgml_to_marc.o ../zserver/sgml_to_marc.c

check_result_set.o: ../cmdparse/check_result_set.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -DCMD_PARSE_CLIENT -c -o check_result_set.o ../cmdparse/check_result_set.c

in_tag_data.o: ../index/in_tag_data.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o in_tag_data.o  ../index/in_tag_data.c

stagcmp.o: ../index/stagcmp.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o stagcmp.o  ../index/stagcmp.c

stem.o: ../index/stem.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o stem.o  ../index/stem.c

.c.o:
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 

