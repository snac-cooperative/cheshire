# SEARCH MODULE MAKE FILE
# Working Directory.
SYSTYPE= ALPHA
PREFIX_DIR = $(HOME)/z3950_3

# Directory in which the s source programs are
HEADER_DIR =	../header
LIB_DIR =	../lib
BIN_DIR	= 	../bin

AR	= ar
ARFLAGS = crv
RANLIB	= :

# Compiler. Default gcc
CC	= gcc 

# Swap the following defines for DMALLOC memory checking version
#DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_FUNC_CHECK -D$(SYSTYPE)
DMFLAGS=	-DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE)

# Compiler switches. Default -g.
CFLAGS = -g -Wall -fPIC -I. -I${HEADER_DIR} -D$(SYSTYPE) $(DMFLAGS) -L. -L$(LIB_DIR) $(ZVERSION)

CCL_OBJS = z3_decode.o\
	z3_encode.o \
	asn_utils.o ber_encode.o \
        utils.o ber_decode.o pdu_make.o\
        pdu_get.o pdu_encode.o\
	pdu_free.o\
	pdu_print.o \
	asn_node.o \
	asn_free.o


all: libz3_asn.a  

libz3_asn.a: ${CCL_OBJS}
	rm -f libz3_asn.a
	$(AR) $(ARFLAGS) libz3_asn.a ${CCL_OBJS}
	$(RANLIB) libz3_asn.a
	mv libz3_asn.a ${LIB_DIR}

clean:
	rm -f *.o core a.out lex.yy.c y.tab.[ch] asn_scan.c

reallyclean: clean
	rm -f *~
.c.o:
	$(CC) -c $(CFLAGS) $<

# $(CCL_OBJS): $(HEADER_DIR)/asn_parser.h 
