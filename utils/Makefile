# Utilities makefile
SYSTYPE= SOLARIS

# Directory in which the s source programs are
SOURCE_DIR =	.
HEADER_DIR =	../header
LIB_DIR =	../lib
BIN_DIR	= 	../bin

AR	= ar
ARFLAGS = crv
RANLIB	= :

#DBMSLIB = -L/usr/local/pgsql/lib -lpq

# Compiler. Default gcc
CC	= gcc 

DEFINES = -D$(SYSTYPE) -DDMALLOC_INCLUDE -DDMALLOC_DISABLE

# Compiler switches. Default -g.
CFLAGS = -g ${DEFINES} -fPIC -I. -I${HEADER_DIR} -I${TKDIR} -I${TCLDIR} $(XINC) -L. -L${LIB_DIR} ${ZVERSION}

all: buildassoc dumprecs getnumrecs cheshire_load read_datastore cheshire_file_filter BuildContDB DumpContDB mark_recs_deleted


buildassoc:  ${LIB_DIR}/libfileio.a getsgmlrec.o buildassoc.o cont_gram.o CONTpars.o lex.cont_.o 
	@ echo "Making buildassoc program"
	@ $(CC) $(CFLAGS) -o buildassoc getsgmlrec.o\
		buildassoc.o cont_gram.o CONTpars.o lex.cont_.o -lfileio -ltclhash

cheshire_load: getsgmlrec.o NewLoad.o md5.o cf_xtra.o cheshire_exit.o DataStore.o parse_util.o
	@ echo "Making cheshire_load program"
	$(CC) $(CFLAGS) -o cheshire_load getsgmlrec.o \
	NewLoad.o md5.o cf_xtra.o cheshire_exit.o DataStore.o parse_util.o \
	-lsgml -lconfig -ltclhash -ldiag -ldb $(DBMSLIB) $(SYSLIBS)

read_datastore: NewRead.o md5.o cf_xtra.o cheshire_exit.o DataStore.o parse_util.o
	@ echo "Making read_datastore program"
	$(CC) $(CFLAGS) -o read_datastore \
	NewRead.o md5.o cf_xtra.o cheshire_exit.o DataStore.o parse_util.o \
	-lconfig -lsgml -lconfig -ltclhash -ldiag -ldb $(DBMSLIB) $(SYSLIBS)

dumprecs: ${LIB_DIR}/libfileio.a dumprecs.o 
	@ echo "Making dumprecs program"
	@ $(CC) $(CFLAGS) -o dumprecs \
		dumprecs.o -lfileio

cheshire_file_filter: ${LIB_DIR}/libfileio.a cheshire_file_filter.o cont_gram.o CONTpars.o lex.cont_.o 
	@ echo "Making cheshire_file_filter program"
	@ $(CC) $(CFLAGS) -o cheshire_file_filter \
		cheshire_file_filter.o cont_gram.o CONTpars.o lex.cont_.o -lfileio

mark_recs_deleted: ${LIB_DIR}/libfileio.a mark_recs_deleted.o
	@ echo "Making mark_recs_deleted program"
	@ $(CC) $(CFLAGS) -o mark_recs_deleted \
		mark_recs_deleted.o -lfileio

BuildContDB: BuildContDB.o cont_gram.o CONTpars.o lex.cont_.o 
	@ echo "Making BuildContDB program"
	@ $(CC) $(CFLAGS) -o BuildContDB \
		BuildContDB.o cont_gram.o CONTpars.o lex.cont_.o -ldb $(DBMSLIB) $(SYSLIBS)

DumpContDB: DumpContDB.o 
	@ echo "Making BuildContDB program"
	@ $(CC) $(CFLAGS) -o DumpContDB \
		DumpContDB.o -ldb $(DBMSLIB) $(SYSLIBS)

getnumrecs: ${LIB_DIR}/libfileio.a getnumrecs.o 
	@ echo "Making getnumrecs program"
	@ $(CC) $(CFLAGS) -o getnumrecs \
		getnumrecs.o -lfileio 

lex.cont_.c: cont_parse.flex
	@ flex -L -i -Pcont_ cont_parse.flex

lex.cont_.o: lex.cont_.c cont_gram.c
	@ $(CC) $(DEMOCCOPTS) -DDO_LL -c lex.cont_.c

CONTpars.o: cont_gram.c
	@ $(CC) $(DEMOCCOPTS) -c CONTpars.c

cont_gram.c: cont_gram.g
	@ LLgen cont_gram.g

cf_xtra.o: ../config/cf_xtra.c
	@ $(CC) $(CFLAGS) -c ../config/cf_xtra.c

parse_util.o: ../sgmlparse/parse_util.c
	@ $(CC) $(CFLAGS) -c ../sgmlparse/parse_util.c

cheshire_exit.o: ../cheshire2/cheshire_exit.c
	@ $(CC) $(CFLAGS) -c ../cheshire2/cheshire_exit.c

clean:
	rm -f *.o core buildassoc dumprecs getnumrecs cheshire_load read_datastore BuildContDB cheshire_file_filter DumpContDB mark_recs_deleted CONTpars.* lex.cont_.* cont_gram.c

reallyclean: clean
	rm -f *~

.c.o:
	$(CC) -c $(CFLAGS) $<




