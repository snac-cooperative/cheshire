#
# Generic Makefile for making the object files and linking
# the application
#
# all these things will need to point to the
# proper places
SYSTYPE=LINUX
TREE=..
SD=/usr/include
ID=../header
LD=$(TREE)
LIBDIR=$(TREE)/lib
TKDIR=/usr/include/
TCLDIR=/usr/include/
TKLIBS=/usr/lib64
TKLIBINC=-ltk8.5
TCLLIBINC=-ltcl8.5
RANLIB = :
# Solaris socket libraries and functions -- comment out if not SOLARIS
SYSLIBS=-lnsl -lsocket -lgen -ldl
SYSOBJ=strtok_r.o

#REENT = -lc_r

#DBMSFLAG= -DILLUSTRA
#DBMSINC= -I/usr/local/illustra/h
#DBMSLIB= -L/usr/local/illustra/lib -lmi


GCFLAGS=-g -O2 -fPIC -DINDEXONLY
LDFLAGS=	$(GCFLAGS)

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -DDEBUG
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE)

CFLAGS=		$(GCFLAGS) \
		-I. \
                -I$(ID) -I$(TCLDIR) -I$(TKDIR) $(XINC) -I$(SD) \
		$(DBMSINC) $(DBMSFLAG) $(DMFLAGS) $(RUSAGE) $(ZVERSION)


# Uncomment the following for DMALLOC memory checking version
#
DMALLOCLIB=       -lfileio -lconfig -lsgml -lconfig -ltclhash -ldiag -ldb


LIBS=		-L. -lindex -L$(LIBDIR) $(DMALLOCLIB) -lindex -lwn \
		-lm -lc $(REENT) $(SYSLIBS) $(DBMSLIB)

DEMOCCOPTS=	$(CFLAGS)
CC=	gcc

.SUFFIXES: .o .c .temp .pq
#
# The following target should list the names of your programs
#

snowball_obj= \
  libstemmer_c/src_c/stem_ISO_8859_1_danish.o \
  libstemmer_c/src_c/stem_UTF_8_danish.o \
  libstemmer_c/src_c/stem_ISO_8859_1_dutch.o \
  libstemmer_c/src_c/stem_UTF_8_dutch.o \
  libstemmer_c/src_c/stem_ISO_8859_1_english.o \
  libstemmer_c/src_c/stem_UTF_8_english.o \
  libstemmer_c/src_c/stem_ISO_8859_1_finnish.o \
  libstemmer_c/src_c/stem_UTF_8_finnish.o \
  libstemmer_c/src_c/stem_ISO_8859_1_french.o \
  libstemmer_c/src_c/stem_UTF_8_french.o \
  libstemmer_c/src_c/stem_ISO_8859_1_german.o \
  libstemmer_c/src_c/stem_UTF_8_german.o \
  libstemmer_c/src_c/stem_ISO_8859_1_hungarian.o \
  libstemmer_c/src_c/stem_UTF_8_hungarian.o \
  libstemmer_c/src_c/stem_ISO_8859_1_italian.o \
  libstemmer_c/src_c/stem_UTF_8_italian.o \
  libstemmer_c/src_c/stem_ISO_8859_1_norwegian.o \
  libstemmer_c/src_c/stem_UTF_8_norwegian.o \
  libstemmer_c/src_c/stem_ISO_8859_1_porter.o \
  libstemmer_c/src_c/stem_UTF_8_porter.o \
  libstemmer_c/src_c/stem_ISO_8859_1_portuguese.o \
  libstemmer_c/src_c/stem_UTF_8_portuguese.o \
  libstemmer_c/src_c/stem_ISO_8859_2_romanian.o \
  libstemmer_c/src_c/stem_UTF_8_romanian.o \
  libstemmer_c/src_c/stem_KOI8_R_russian.o \
  libstemmer_c/src_c/stem_UTF_8_russian.o \
  libstemmer_c/src_c/stem_ISO_8859_1_spanish.o \
  libstemmer_c/src_c/stem_UTF_8_spanish.o \
  libstemmer_c/src_c/stem_ISO_8859_1_swedish.o \
  libstemmer_c/src_c/stem_UTF_8_swedish.o \
  libstemmer_c/src_c/stem_UTF_8_turkish.o \
  libstemmer_c/runtime/api.o \
  libstemmer_c/runtime/utilities.o \
  libstemmer_c/libstemmer/libstemmer.o


all: libstemmer.o snowball_stem_test libsnowball.a 

#
DEFINES=

# the following define should contain a list of all modules

MODS= snowball_stem.o conversions.o libstemmer.o


# ----------------
#  The detailed instructions for building the application
#  including tcl/tk and Z39.59 and the DBopen library
# ----------------
libsnowball.a: snowball_stem.o conversions.o
	@ echo "Making Snowball Stemmer library"
	@ rm -f libsnowball.a 
	@ ar cr libsnowball.a snowball_stem.o conversions.o $(snowball_obj)
	@ ar s libsnowball.a
	@ $(RANLIB) libsnowball.a

snowball_stem_test: snowball_stem_test.o snowball_stem.o libstemmer.o
	@ echo "linking snowball_stem_test"
	@ $(CC) -g -fPIC -o $@ $^

libstemmer.o:
	@ cd libstemmer_c; make ; cp libstemmer.o ..

snowball_stem.o: snowball_stem.c
	$(CC) $(DEMOCCOPTS) $(DEFINES) -Ilibstemmer_c/include -c snowball_stem.c

snowball_stem_test.o: snowball_stem_test.c 
	$(CC) $(DEMOCCOPTS) $(DEFINES) -Ilibstemmer_c/include -c snowball_stem_test.c

clean: 
	rm -f *.o *.a core snowball_stem_test
	cd libstemmer_c; make clean


reallyclean: clean
	rm -f *~



#
# DEPENDENCIES (generated, do not modify by hand)
# 
