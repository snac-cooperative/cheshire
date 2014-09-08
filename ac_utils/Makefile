# SEARCH MODULE MAKE FILE
# Working Directory.
SYSTYPE= ALPHA
PREFIX_DIR = $(HOME)/z3950_3

# Directory in which the s source programs are
SOURCE_DIR =	../zpdu
HEADER_DIR =	../header
LIB_DIR =	../lib
BIN_DIR	= 	../bin

AR	= ar
ARFLAGS = crv
RANLIB	= :

# Compiler. Default gcc
CC	= gcc 

# Compiler switches. Default -g.
CFLAGS = -g -Wall -fPIC -D$(SYSTYPE) -I. -I${HEADER_DIR} -L. -L${LIB_DIR}

UTILS_OBJS =  mr_print.o\
	mr_field.o \
	parse_marc.o

all: libutils.a  

libutils.a: ${UTILS_OBJS}
	rm -f libutils.a
	$(AR) $(ARFLAGS) libutils.a ${UTILS_OBJS}
	$(RANLIB) libutils.a
	mv libutils.a ${LIB_DIR}

clean:
	rm -f *.o core a.out lex.yy.c y.tab.[ch] asn_scan.c

.c.o:
	$(CC) -c $(CFLAGS) $<

# $(UTILS_OBJS): $(HEADER_DIR)/asn_parser.h 


