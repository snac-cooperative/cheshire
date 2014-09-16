SYSTYPE= ALPHA
BIN = /home/cheshire/cheshire2/bin
TRANS = trans.o marc2sgml.o m2sldr.o m2sflds.o m2sdflds.o sfnorm.o m2sindi.o entsub.o tagcmp.o
CC=		gcc

#CFLAGS = 	-g -O3 -I../header -I/usr/local/include -L../lib -DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE)
CFLAGS = 	-g -I../header -I/usr/local/include -L../lib -DDMALLOC_INCLUDE -DDMALLOC_DISABLE -D$(SYSTYPE) $(ZVERSION) -fPIC
LDFLAGS = 	-g

all: marc2sgml

marc2sgml: $(TRANS)
	$(CC) -g -L../lib -L/usr/local/lib $(TRANS) -o marc2sgml -lmarc
#	$(CC) -g -O3 -L../lib -L/usr/local/lib $(TRANS) -o marc2sgml -lmarc

clean: 
	/bin/rm -f *.o       

reallyclean: clean
	/bin/rm -f *~ marc2sgml

print: 
	enscript *.c

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above

