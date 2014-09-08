#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE=SOLARIS
TREE=	..
SD=	/usr/local/include
ID=	../header
LD=	$(TREE)
LIBDIR = $(TREE)/lib
TKDIR=	/usr/local/include/
TCLDIR= /usr/local/include/
TKLIBS= /usr/local/lib
TKLIBINC = -ltk8.0
TCLLIBINC = -ltcl8.0
RANLIB = :
# Solaris socket libraries and functions -- comment out if not SOLARIS
SYSLIBS= -lnsl -lsocket -lgen -ldl
SYSOBJ= strtok_r.o

#REENT = -lc_r

#DBMSFLAG= -DILLUSTRA
#DBMSINC= -I/usr/local/illustra/h
#DBMSLIB= -L/usr/local/illustra/lib -lmi


GCFLAGS=-g -DINDEXONLY
LDFLAGS=	$(GCFLAGS)

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -DDEBUG
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE)

CFLAGS=		$(GCFLAGS) \
		-I. -I$(TREE)/wordnet/lib \
                -I$(ID) -I$(TCLDIR) -I$(TKDIR) $(XINC) -I$(SD) \
		$(DBMSINC) $(DBMSFLAG) $(DMFLAGS) $(RUSAGE) $(ZVERSION)


# Uncomment the following for DMALLOC memory checking version
#
DMALLOCLIB=      -lindex -lsearch -lfileio -lconfig -lsgml -lconfig -ltclhash -ldiag -ldb -lindex -lsearch -lfileio -lconfig -lwn -lsnowball

DMALLOCLIBGRID=      -lgridindex -lsearch -lfileio -lconfig -lsgml -lconfig -ltclhash -ldiag -ldb -lindex -lsearch -lfileio -lconfig -lwn -lsnowball


LIBS=		-L$(LIBDIR) $(DMALLOCLIB) \
		-lm -lc $(REENT) $(SYSLIBS) $(DBMSLIB)

GRIDLIBS=		-L$(LIBDIR) $(DMALLOCLIBGRID) \
		-lm -lc $(REENT) $(SYSLIBS) $(DBMSLIB)

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: in_test dumpdb dumpds dumpcomp countdb dumppost highpost index_cheshire index_clusters index_vectors batch_load dumpprox dumpvector delete_recs convert2bitmap loadsorted_vectors index_cheshire_grid dumprecs2 fixcompmax

#
DEFINES=

# the following define should contain a list of all modules

MODS= in_kwadd.o getdata.o idxdata.o idxdate.o idxxkey.o idxattr.o idxfreq.o\
   idx008.o idxnumber.o stagcmp.o stem.o sstemmer.o wn_morph.o normalize_key.o regexp.o\
   in_kwdel.o in_cluster.o in_components.o in_proc_instr.o in_tag_data.o \
   in_clusmerg.o in_sync.o in_extract.o keytagcmp.o idxlatlong.o \
   lex.in_ft_.o idxextern.o idxpages.o in_batch_load.o in_pagedel.o \
   in_bitmaps.o in_dbms.o dbms_intf.o idxcoded.o idxngram.o

GRIDMODS= in_kwadd.o getdata.o idxdata_grid.o idxdate.o idxxkey.o idxattr.o idxfreq.o\
   idx008.o idxnumber.o stagcmp.o stem.o sstemmer.o wn_morph.o normalize_key_grid.o regexp.o\
   in_kwdel.o in_cluster.o in_components.o in_proc_instr.o in_tag_data.o \
   in_clusmerg.o in_sync.o in_extract_grid.o keytagcmp.o idxlatlong.o \
   lex.in_ft_.o idxextern.o idxpages.o in_batch_load.o in_pagedel.o \
   in_bitmaps.o in_dbms.o dbms_intf.o in_grid_data.o idxcoded.o idxngram.o

 

# ----------------
#  The detailed instructions for building the application
#  including tcl/tk and Z39.59 and the DBopen library
# ----------------
libindex.a: $(MODS)
	@ echo "Making index library"
	@ rm -f libindex.a 
	@ ar cr libindex.a $(MODS)
	@ $(RANLIB) libindex.a

libgridindex.a: $(GRIDMODS)
	@ echo "Making index library"
	@ rm -f libgridindex.a 
	@ ar cr libgridindex.a $(GRIDMODS)
	@ $(RANLIB) libgridindex.a



in_test: libindex.a cf_xtra.o in_test.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking in_test main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o in_test \
	 in_test.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

index_cheshire: libindex.a in_load.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking index_cheshire main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o index_cheshire \
	 in_load.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

index_cheshire_grid: libgridindex.a in_load_grid.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking index_cheshire_grid main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o index_cheshire_grid \
	 in_load_grid.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(GRIDLIBS)


batch_load: libindex.a batch_load.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking batch_load main"
	$(CC) $(CFLAGS) $(DEFINES) -o batch_load \
	 batch_load.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

index_clusters: libindex.a in_load_clus.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking index_clusters main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o index_clusters \
	 in_load_clus.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

delete_recs: libindex.a in_testdel.o cf_xtra.o in_comp_del.o get_component.o in_cheshire_exit.o $(SYSOBJ)
	@ echo "linking delete_recs main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o delete_recs \
	 in_testdel.o cf_xtra.o in_comp_del.o in_cheshire_exit.o \
	 get_component.o $(SYSOBJ) $(LIBS) 


get_component.o: ../zserver/get_component.c
	@ $(CC) $(DEMOCCOPTS) -c -o get_component.o ../zserver/get_component.c 

dbms_intf.o: ../zserver/dbms_intf.c
	@ $(CC) $(DEMOCCOPTS) -c -o dbms_intf.o ../zserver/dbms_intf.c 

strtok_r.o: ../search/strtok_r.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -c -o strtok_r.o ../search/strtok_r.c

regmatch: regexp.o regmatch.o
	@ echo "linking"
	@ $(CC) -o regmatch \
		regmatch.o regexp.o

strtok_test:
	@ echo "linking"
	@ $(CC) $(CFLAGS) -DWIN32 -c strtok_test.c
	@ $(CC) $(CFLAGS) -DWIN32 -o strtok_test \
		strtok_test.o strtok_r.o

date_test: date_test.o idxdate.o normalize_key.o
	@ echo "linking date_test main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o date_test \
		 date_test.o idxdate.o normalize_key.o \
		 -L../lib -ltclhash $(REENT) -lindex

test_extern: test_extern.o lex.in_ft_.o
	@ echo "linking test_testextern main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o test_extern \
		 test_extern.o lex.in_ft_.o \
		  $(REENT) 

dumpnewdb: dumpnewdb.o cf_xtra.o  get_component.o $(SYSOBJ)
	@ echo "linking dumpned main"
	@ $(CC) $(CFLAGS) $(DEFINES) -o dumpnewdb \
		 dumpnewdb.o cf_xtra.o $(SYSOBJ) $(LIBS)

dumpdb: dumpdb.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking dump main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumpdb \
		 dumpdb.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ) $(LIBS)

dumprecs2: dumprecs2.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking dumprecs2 main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumprecs2 \
		 dumprecs2.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ) $(LIBS)

dumpds: dumpds.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ)
	@ echo "linking dumpds main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumpds \
		 dumpds.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ) $(LIBS) 

dumpcomp: dumpcomp.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ)
	@ echo "linking dump main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumpcomp \
		 dumpcomp.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ) $(LIBS) 

fixcompmax: fixcompmax.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ)
	@ echo "linking dump main"
	$(CC) $(CFLAGS) $(DEFINES) -o fixcompmax \
		 fixcompmax.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ) $(LIBS) 

countdb: countdb.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ)
	@ echo "linking count main"
	$(CC) $(CFLAGS) $(DEFINES) -o countdb \
		 countdb.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

dumppost: dumppost.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking dumppost main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumppost \
		dumppost.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ) $(LIBS) 

index_vectors: build_vector_index.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking index_vectors main"
	$(CC) $(CFLAGS) $(DEFINES) -o index_vectors \
		build_vector_index.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ) $(LIBS) 

loadsorted_vectors: finish_vector_index.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking loadsorted_vectors main"
	$(CC) $(CFLAGS) $(DEFINES) -o loadsorted_vectors \
		finish_vector_index.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ) $(LIBS) 

convert2bitmap: convert2bitmap.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking convert2bitmap main"
	$(CC) $(CFLAGS) $(DEFINES) -o convert2bitmap \
		convert2bitmap.o cf_xtra.o in_cheshire_exit.o  get_component.o $(SYSOBJ) $(LIBS) 

dumpprox: dumpprox.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking dumpprox main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumpprox \
		dumpprox.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

dumpvector: dumpvector.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking dumpvector main"
	$(CC) $(CFLAGS) $(DEFINES) -o dumpvector \
		dumpvector.o cf_xtra.o in_cheshire_exit.o get_component.o $(SYSOBJ) $(LIBS) 

highpost: highpost.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ)
	@ echo "linking highpost main"
	$(CC) $(CFLAGS) $(DEFINES) -o highpost \
		highpost.o cf_xtra.o in_cheshire_exit.o get_component.o  $(SYSOBJ) $(LIBS) 

lex.in_ft_.c: in_fulltext.flex
	@ flex -i -Pin_ft_ in_fulltext.flex

lex.in_ft_.o: lex.in_ft_.c
	@ $(CC) -g $(DEMOCCOPTS) -DDO_LL -c lex.in_ft_.c

cf_xtra.o: ../config/cf_xtra.c
	@ $(CC) $(DEMOCCOPTS) -c ../config/cf_xtra.c 

testparse: lex.in_ft_.o testparse.o $(SYSOBJ)
	@ echo "linking testparse"
	$(CC) $(CFLAGS) $(DEFINES) -o testparse \
		lex.in_ft_.o testparse.o  $(SYSOBJ) -lc -lfl

stem_test: stem_test.o 
	@ echo "linking stem_test"
	@ $(CC) $(CFLAGS) $(DEFINES) -o stem_test \
	  stem_test.o stem.o -lc -lfl

clean: 
	rm -f *.o *.a core dumppost dumpdb dumpcomp countdb regmatch in_testdel in_test index_cheshire index_clusters highpost testparse batch_load date_test dumpprox delete_recs dumpds index_cheshire_grid

reallyclean: clean cleandata cleandata2 cleandl cleantest
	rm -f *~

cleandl: 
	rm -f TESTDL/DL.* TESTDL/*.DOCSIZES

cleandata: 
	rm -f TESTDATA/__db.* TESTDATA/log.* TESTDATA/*.index TESTDATA/*.index.PROX TESTDATA/*.index.VECTOR TESTDATA/COMPONENT* TESTDATA/comp* TESTDATA/*.history INDEX_LOGFILE TESTDATADS/TEST_DATASTORE TESTDATADS/*.index TESTDATADS/COMPONENT*  TESTDATADS/comp* TESTDATADS/*.history TESTDATA/testcluster*

cleandata2: 
	rm -f TESTDATA2/dictionary* TESTDATA2/mainfile* TESTDATA2/xx*
	rm -f TESTDATA2/*.index TESTDATA2/*.post TESTDATA2/*.assoc TESTDATA2/*.data INDEX_LOGFILE

cleantest: 
	rm -rf TEST2/*

# this is very dangerous...
#reallyclean:
#	/bin/rm -f /usr3/cheshire2/data2/*.post /usr3/cheshire2/data2/*.index /usr3/cheshire2/data2/*.data* /usr3/cheshire2/data2/*.assoc /usr3/cheshire2/data2/*.history /usr3/cheshire2/data2/INDEX_LOGFILE

# this is also very dangerous...
#cleanclusters:
#	/bin/rm -f /usr3/cheshire2/data2/classcluster.data /usr3/cheshire2/data2/classcluster.assoc  /usr3/cheshire2/data2/clasclus.terms.*  /usr3/cheshire2/data2/clasclus.lcckey.*  /usr3/cheshire2/data2/subjcluster.data /usr3/cheshire2/data2/subjcluster.assoc  /usr3/cheshire2/data2/subjcluster.terms.*  /usr3/cheshire2/data2/subjcluster.key.*

.c.o:
	$(CC)  $(DEMOCCOPTS) $(DEFINES) -c $(<F)



#
# DEPENDENCIES (generated, do not modify by hand)
# 
