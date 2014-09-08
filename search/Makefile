#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE= ALPHA
TREE= ..
ID=	$(TREE)/header
RANLIB= :

TKDIR = /usr/local/src/tk8.0
TCLDIR = /usr/local/src/tcl8.0
TKLIBS = /usr/local/lib
#TKLIBINC= -ltk8.0
#TCLLIBINC= -ltcl8.0
LIBDIR = $(TREE)/lib
LIBZ=	$(TREE)/lib
XINC=  -I/usr/include 
REENT= -lc_r

GCFLAGS=-g
LDFLAGS=	$(GCFLAGS)

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK -D$(SYSTYPE)
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE) -DCMD_PARSE_SERVER 

CFLAGS=		$(GCFLAGS) \
                -I$(ID) -I$(TCLDIR) -I$(TKDIR) $(XINC) \
		$(DBMSINC) $(DBMSFLAG) $(DMFLAGS) $(ZVERSION)


# Uncomment the following for DMALLOC memory checking version
#
DMALLOCLIB=         -lsgml -ltclhash -lindex -lfileio -lconfig -lcmdparse -lmarc -lsgml2marc -ldiag -lsnowball -L$(TKLIBS) $(TCLLIBINC) -lz3_asn


LIBS=	 -L$(LIBDIR) $(DMALLOCLIB) -lwn -ldb \
		-lm -lc ${REENT} ${SYSLIBS} ${DBMSLIB}

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#
all: libsearch.a testsrch2

#
DEFINES=

# the following define should contain a list of all modules

MOREPROGS= se_process_query.o se_process_terms.o se_bool_element.o\
	se_dbms_query.o se_getterm.o se_gettrunc.o se_prob.o se_rank_docs.o \
	se_phrase_match.o se_get_result.o se_process_meta.o se_bool_range.o \
        se_andmerge.o se_notmerge.o se_ormerge.o se_pagemerge.o se_proxmerge.o\
        se_sortset.o se_process_feedback.o se_getdoclen.o se_get_page_docs.o \
	se_blobs.o se_prox_phrase.o se_comp2doc.o se_comp_minmax.o \
	se_virtualmerge.o se_okapi.o se_cori.o se_bitmap.o se_bitmap_merge.o \
	se_georank.o se_regex_scan.o se_tfidf.o se_prob_trec2_3.o \
	se_tfidf_lucene.o se_hybridmerge.o se_boolngrams.o

# ----------------
#  The detailed instructions for building the application
#  including tcl/tk and Z39.59 and the DBopen library
# ----------------
#
testsrch2: se_driver.o sgml_to_marc.o z_dispmarc.o z_dispsgml.o \
	displayrec.o pSFnorm.o libsearch.a cheshire_exit.o  \
	check_result_set.o get_component.o  $(SYSOBJ)
	@ echo "making testsrch2"
	@ $(CC) $(CFLAGS) $(DEFINES) -DSGML_INITIALIZE -o testsrch2 \
	   se_driver.o sgml_to_marc.o z_dispmarc.o z_dispsgml.o displayrec.o \
	   cheshire_exit.o  check_result_set.o get_component.o pSFnorm.o \
	    $(SYSOBJ) -L. -lsearch $(LIBS)

libsearch.a: $(MOREPROGS)
	@ echo "Making Search Library"
	@ rm -f libsearch.a
	@ ar cr libsearch.a  $(MOREPROGS)
	@ $(RANLIB) libsearch.a

reallyclean: clean cleandata
	rm -f *~

clean: 
	rm -f *.o *.a testsrch2 core

cleandata: 
	rm -f TESTDATA/dictionary* TESTDATA/mainfile*

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



#
# DEPENDENCIES (generated, do not modify by hand)
# 
