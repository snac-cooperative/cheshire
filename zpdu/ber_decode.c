/*
 *  Copyright (c) 1990-1999 [see Other Notes, below]. The Regents of the
 *  University of California (Regents). All Rights Reserved.
 *  
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for educational, research, and not-for-profit purposes,
 *  without fee and without a signed licensing agreement, is hereby
 *  granted, provided that the above copyright notice, this paragraph and
 *  the following two paragraphs appear in all copies, modifications, and
 *  distributions. Contact The Office of Technology Licensing, UC
 *  Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
 *  (510) 643-7201, for commercial licensing opportunities. 
 *  
 *  Created by Ray R. Larson, Aitao Chen, and Jerome McDonough, 
 *             School of Information Management and Systems, 
 *             University of California, Berkeley.
 *  
 *    
 *       IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT,
 *       INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 *       INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
 *       AND ITS DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE
 *       POSSIBILITY OF SUCH DAMAGE. 
 *    
 *       REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
 *       NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *       FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND
 *       ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
 *       PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 *       MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 */
/*
 * This file contains a set of primitive data type decoders.
 * 11/30/94	-Aitao Chen
 */
#include <stdio.h>
#include <stdlib.h>
#include "zprimitive.h"
#include "connect.h"

#ifdef WIN32
#include <io.h>
#endif

extern int db_put(DBUF *,UCHAR *,int);
extern int db_get(DBUF *, UCHAR *,int);
extern void FreeDbuf(DBUF *);
extern ASN_NODE *Attach(ASN_NODE *,ASN_NODE *);
extern int TimeOut(int, int);
extern void DecodeASN1Records(External *, ASN_NODE *);
extern void FreeGSTR (GSTR *);

int debug = 0;
WHOAMI	context = Z_SERVER;	/* client should set it to Z_CLIENT	*/

extern FILE *LOGFILE; /* this should be set in the main program */


int
GetTAG(dbuf, class_var, type, tagno)
DBUF *dbuf;
int *class_var;
int *type;
int *tagno;
{
    	UCHAR   ch;

	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;
    	*tagno = 0;
    	if (db_get(dbuf, (UCHAR *)&ch, 1) != 1) {
		perror("getting tag:");
		return Z_ERROR;
    	}
    	if (ch == 0) {		/* end-of-contents octets.	*/
		if (db_get(dbuf, (UCHAR *)&ch, 1) != 1)
			return Z_ERROR;
		if (ch != 0) {
		  if (LOGFILE == NULL)
		    fprintf(stderr,"Error: BER decode: not end-of-contents!\n"); 
		  else
		    fprintf(LOGFILE,"Error: BER decode: not end-of-contents!\n"); 
		  return Z_ERROR;
		}
		return Z_OK;
    	}
	/*
	 * +-+-+---------+
	 * |c|f| number  |
	 * +-+-+---------+
	 */
	*class_var = (ch >> 6) & 0x03;
    	*type = (ch >> 5) & 0x01;
    	*tagno = ch & 0x1F;

	/*
	 * +-+-+-+-+-+-+-+  +-+--------+     +-+------------+   +-+---------+
	 * |c|f|1|1|1|1|1|  |1|        | ... |1|            |   |0|         |
	 * +-+-+-+-+-+-+-+  +-+--------+     +-+------------+   +-+---------+
	 */
    	if (*tagno == 31) {		/* long form representation	*/
		*tagno = 0;
		while (1) {
	    		if (db_get(dbuf, (UCHAR *) &ch, 1) != 1)
				return Z_ERROR;
	    		if (ch < 128) {	/* the last octet	*/
				*tagno <<= 7;
				*tagno += ch;
				break;
	    		}
	    		ch &= 0x7F;	/* turn off the msb in an octet	*/
	    		*tagno <<= 7;
	    		*tagno += ch;
		}
    	}
    	return Z_OK;
}

int
GetLEN(dbuf)
DBUF *dbuf;
{
    	UCHAR   ch;
    	int  nocs;
    	int  i=0;
    	int  len = 0;

    	if (db_get(dbuf, (UCHAR *) &ch, 1) != 1)
		return -1;
    	/*
     	 * +-----------+
     	 * |0| length  |
     	 * +-----------+
     	 */
    	if (!(ch & 0x80)) 
		return (int)ch;
	
    	/* 
     	 * +---------------+       +---------------+ +---------------+
     	 * |1|0|0|0|0|0|0|0|  ...  |0|0|0|0|0|0|0|0| |0|0|0|0|0|0|0|0|
     	 * +---------------+       +---------------+ +---------------+
     	 */
	/* Indefinite length form is used.
	 */
    	if (ch == 0x80) 
		return -1;

    	/*
     	 * +------------+  +----------+     +----------+ 
     	 * |1| #-octets |  | octet    | ... | octet    |
     	 * +------------+  +----------+     +----------+
     	 */
	/* Long, definite length form.
	 */
    	nocs = ch & 0x7F;
    	if (nocs > sizeof(int)) 
		return 0;
    	i = nocs;
    	while (i--) {
		len <<= 8;
		if (db_get(dbuf, (UCHAR *)&ch, 1) != 1)
			return Z_ERROR;
		len += ch;
    	}
    	return len;
}

/*
 * GetTAGFromStream returns 1 if succeeds, 0 if the connection
 * is closed, and -1 to indicate that an error has occurred.
 */
int
GetTAGFromStream(fd, class_var, type, tagno, size)
int	fd;
int	*class_var;		/* Universal, Context, Application, Private? 	*/
int	*type;		/* Primitive or Constructed?	*/
int	*tagno;		/* Tag number.		*/
int	*size;		/* Number of octets.	*/
{
    UCHAR   ch;
    int    code = 0;
    int	len = 0;	

    *tagno = 0;
    *class_var = 0;
    *type = 0;
    *size = 0;
    if (1 != (code = ReadFromSocket(fd, (UCHAR *) &ch, 1))) {
      if (code == 0) {
	if (LOGFILE == NULL)
	  fprintf(stderr, "BER decode: Connection closed by foreign host.\n");
	else
	  fprintf(LOGFILE, "BER decode: Connection closed by foreign host.\n");
      }
      return code;
    }
    len = 1;
    if (ch == 0) {
	if ((code = ReadFromSocket(fd, (UCHAR *)&ch, 1)) != 1)
	    return code;
        len++;
	if (ch != 0) {	/* end-of-contents octets	*/
	  if (LOGFILE == NULL)
	    fprintf(stderr, "BER decode error: not end-of-contents octet.\n");
	  else
	    fprintf(LOGFILE, "BER decode error: not end-of-contents octet.\n");
	  return -1;
	}
	*size = len;
	return 1;
    }

    /*
     * +-+-+---------+
     * |c|f| number  |
     * +-+-+---------+
     */
    *class_var = (ch >> 6) & 0x03;
    *type = (ch >> 5) & 0x01;
    *tagno = ch & 0x1f;

    /*
     * +-+-+-+-+-+-+-+  +-+--------+     +-+------------+   +-+---------+
     * |c|f|1|1|1|1|1|  |1|        | ... |1|            |   |0|         |
     * +-+-+-+-+-+-+-+  +-+--------+     +-+------------+   +-+---------+
     */
    if (*tagno == 31) {
	*tagno = 0;
	while (1) {		/* long form representation	*/
	    if ((code = ReadFromSocket(fd, (UCHAR *) &ch, 1)) != 1)
		return code;
	    len++;
	    if (ch < 128) {	/* the last octet of the tag	*/
		*tagno <<= 7;
		*tagno += ch;
		break;
	    }
	    ch &= 0x7F;
	    *tagno <<= 7;
	    *tagno += ch;
	}
    }
    *size = len;
    /*
    if (debug) 
    	fprintf(stdout, "GT: class_var = %d\ttype = %d\ttagno = %d\n", 
    		*class_var, *type, *tagno);
    */
    return 1;
}

/* This routine reads the length field from the stream. It returns the
 * length value if the length value is encoded in definite length form,
 * otherwise it returns -1 to indicate that the actual length value
 * is unknown. And the indefinite length form is used to mark the beginning
 * and ending of the data value.
 */ 
int
GetLENFromStream(fd, size)
int	fd;
int	*size;
{
    	UCHAR   ch;
    	int nocs;
    	int t;
    	int len = 0;
	int code;

	if ((code = ReadFromSocket(fd, (UCHAR *)&ch, 1)) != 1)
		return code;
    	/*
     	 * +-----------+
     	 * |0| length  |
     	 * +-----------+
     	 */
    	if (!(ch & 0x80)) {
        	*size = 1;
		return ch;
    	}
    	/* 
     	 * +---------------+       +---------------+ +---------------+
     	 * |1|0|0|0|0|0|0|0|  ...  |0|0|0|0|0|0|0|0| |0|0|0|0|0|0|0|0|
     	 * +---------------+       +---------------+ +---------------+
     	 */
	/* Indefinite length form.
	 */
    	if (ch == 0x80) {
		*size = 1;
		return -1;
    	}

    	/*
     	 * +------------+  +----------+     +----------+ 
     	 * |1| #-octets |  | octet    | ... | octet    |
     	 * +------------+  +----------+     +----------+
     	 */
    	nocs = ch & 0x7f;
    	if (nocs > sizeof(int)) 
		return 0L;
    	t = nocs;
    	while (t--) {
		len <<= 8;
		if ((code = ReadFromSocket(fd, (UCHAR *)&ch, 1)) != 1)
			return code;
		len += ch;
    	}
    	*size = nocs + 1;
    	return len;
}

/*
 * A client program waits for reading for at most two minutes,
 * and a server program waits for reading for at most ten minutes before
 * close the connection. The easy approach to timeout is to shut down the
 * connection instead of retrying.  The timeout values are arbitrarily
 * chosen. They should be an entry for timeout in the client and server
 * initialization files.
 */

int 
WaitForData (fd, timeoutsecs)
int fd;
int timeoutsecs;
{

  int code;
  if (context == Z_CLIENT)  {	
    if ((code = TimeOut(fd, timeoutsecs)) <= 0) {
      close(fd);
      if (LOGFILE == NULL)
	fprintf(stderr, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);
      else
	fprintf(LOGFILE, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);

      return -1;
    } else {
      return code;
    }
  } else if (context == Z_SERVER) {	
    if ((code = TimeOut(fd, timeoutsecs)) <= 0) {
      close(fd);
      if (LOGFILE == NULL)
	fprintf(stderr, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);
      else
	fprintf(LOGFILE, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);
      return -1;
    } else {
      return code;
    }
  } else {
    if (LOGFILE == NULL)
      fprintf(stderr, "Unknown context in waiting for data\n");
    else
      fprintf(LOGFILE, "Unknown context in waiting for data\n");    
    return -1;
  } 
  return code;
}

ASN_NODE *
GetPDUFromFile(fd, size)
int fd;
int *size;
{
    	int	len;
    	unsigned int   tag;
    	TAG_CLASS	class_var;
    	TAG_FORM	form;
    	int     stmp = 0;
    	int	ltmp;
    	ASN_NODE *parent, *child;
    	char	*data;
    	int 	running_size = 0;
    	int  	done = 0;
    	int	status;

    	if ((parent = (ASN_NODE *)NewAsnNode(0, 0, 0)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	status = GetTAGFromStream(fd, &class_var, &form, &tag, &stmp);
    	if (status <= 0) {	/* either the connection is broken or 	*/
		close(fd);	/* an error has occurred.		*/
		return (ASN_NODE *)NULL;
    	}
    	parent->class_var = class_var;
    	parent->type = form;
    	parent->tagno = tag;
    	running_size += stmp;
    	if ((tag == 0) && (class_var == 0) && (form == 0)) {
		*size = running_size;
		if (debug)
	    		printf("end-of-contents\n");
		return (ASN_NODE *)NULL;
	}
    	len = GetLENFromStream(fd, &stmp);
    	/*
     	 * Check the value of len for error.
     	 */
    	parent->length = len;
    	running_size += stmp;
    	if (debug) {
		printf("tag = [%d  %d  %-4d]\tlength = %d\tsize = %d\n",
			parent->class_var, parent->type, 
			parent->tagno, parent->length,
			running_size+len);	/* TLV length	*/
    	}

    	/* Primitive data type.	*/
    	if (form == PRIMITIVE) {
		/*
	 	 * Watch out for the NULL primitive type.
	 	 * The data lenght is zero for NULL type.
	 	 */
		if (len > 0) {
	    		data = calloc(1,len+1);
	    		ReadFromSocket(fd, (char *)data, len);
  	    		db_put(parent->dbuf, data, len);
	    		(void)free(data);
		} 
		running_size += len;
		*size = running_size;
		return parent;
    	} 

    	/* Constructed data type.	*/
    	if (len == 0) {
	  if (LOGFILE == NULL)
	    fprintf(stderr, "Invalid length value (0).\n");
	  else
	    fprintf(LOGFILE, "Invalid length value (0).\n");
	  
	  return (ASN_NODE *)NULL;
    	} else if (len == -1) {
	  if (debug)
	    printf("beginning of indefinite-length form.\n");
	}
    	ltmp = len;
    	while (!done) {
	  child = GetPDUFromFile(fd, &stmp);
	  running_size += stmp;
	  if (child == (ASN_NODE *)NULL) { /* end-of-contents or error */
	    if (stmp == 0) {
	      if (LOGFILE == NULL)
    		fprintf(stderr,"Invalid size zero\n");
	      else
    		fprintf(LOGFILE,"Invalid size zero\n");

	      return (ASN_NODE *)NULL;
	    } else {
	      if (len < 0) {
		done = 1;
	      } else {
		if (LOGFILE == NULL)
		  fprintf(stderr,"unexpected empty child node\n");
		else
		  fprintf(LOGFILE,"unexpected empty child node\n");

		return (ASN_NODE *)NULL;
	      }
	    }
	  } else {
	    if (len >= 0) {
	      ltmp -= stmp;
	      if (ltmp <= 0) {
		done = 1;
	      }
	    }
	    if (!Attach(parent, child))
	      return (ASN_NODE *)NULL;
	  }
    	}
    	*size = running_size;
    	return parent;
}

/*
 * GetPDUFromStream returns NULL if an error has occurred.
 * Note: This routine was derived from Simon Spero's program.
 */
ASN_NODE *
GetPDUFromStream(fd, size, timeoutsecs)
int fd;
int *size;
int timeoutsecs;
{
    	int	len;
    	unsigned int   tag;
    	TAG_CLASS	class_var;
    	TAG_FORM	form;
    	int     stmp = 0;
    	int	ltmp;
    	ASN_NODE *parent, *child;
    	char	*data;
    	int 	running_size = 0;
    	int  	done = 0;
    	int	status;

    	if ((parent = (ASN_NODE *)NewAsnNode(0, 0, 0)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	/* Indefinitely waiting for response.
     	 * Time-out will be implemented.
     	 */
	/*
    	while (TimeOut(fd, 60) <= 0) {
		fprintf(stdout, "Time out: getting tag from socket %d\n", fd);
		;
    	}
	*/
	if (WaitForData(fd, timeoutsecs) <= 0)
		return (ASN_NODE *)NULL;
    	status = GetTAGFromStream(fd, &class_var, &form, &tag, &stmp);
    	if (status <= 0) {	/* either the connection is broken or 	*/
		close(fd);	/* an error has occurred.		*/
		return (ASN_NODE *)NULL;
    	}
    	parent->class_var = class_var;
    	parent->type = form;
    	parent->tagno = tag;
    	running_size += stmp;
    	if ((tag == 0) && (class_var == 0) && (form == 0)) {
		*size = running_size;
		if (debug)
	    		printf("end-of-contents\n");
		return (ASN_NODE *)NULL;
	}
    	/* Indefinitely waiting for response.
     	 * Time-out will be implemented.
     	 */
	/*
    	while (TimeOut(fd, 60) <= 0) {
		fprintf(stdout, "Time out: getting len from socket %d\n", fd);
    	}
	*/
	if (WaitForData(fd, timeoutsecs) <= 0)
		return (ASN_NODE *)NULL;
    	len = GetLENFromStream(fd, &stmp);
    	/*
     	 * Check the value of len for error.
     	 */
    	parent->length = len;
    	running_size += stmp;
    	if (debug) {
		printf("tag = [%d  %d  %-4d]\tlength = %d\tsize = %d\n",
			parent->class_var, parent->type, 
			parent->tagno, parent->length,
			running_size+len);	/* TLV length	*/
    	}

    	/* Primitive data type.	*/
    	if (form == PRIMITIVE) {
		/*
	 	 * Watch out for the NULL primitive type.
	 	 * The data lenght is zero for NULL type.
	 	 */
		if (len > 0) {
	    		data = calloc(1,len+1);
			/*
            		while (TimeOut(fd, 60) <= 0) {
	        		fprintf(stdout,"Time out: getting primitive data.\n");
	    		}
			*/
			if (WaitForData(fd, timeoutsecs) <= 0) 
				return (ASN_NODE *)NULL;
	    		ReadFromSocket(fd, (char *)data, len);
  	    		db_put(parent->dbuf, data, len);
	    		(void)free(data);
		} 
		running_size += len;
		*size = running_size;
		return parent;
    	} 

    	/* Constructed data type.	*/
    	if (len == 0) { /* this might be a null structure */
	  /* if (LOGFILE == NULL) we will assume it is OK instead 
	   * fprintf(stderr, "BER Decoding: Possible Invalid length value (0). Null result from Server?\n");
	   * else
	   *fprintf(LOGFILE,"BER Decoding: Possible Invalid length value (0). Null result from Server?\n");
	   */
	  *size = running_size;
	  return (ASN_NODE *)parent;
    	} else if (len == -1) {
	  if (debug)
	    printf("beginning of indefinite-length form.\n");
	}
    	ltmp = len;
    	while (!done) {
	  child = GetPDUFromStream(fd, &stmp, timeoutsecs);
	  running_size += stmp;
	  if (child == (ASN_NODE *)NULL) { /* end-of-contents or error */
	    if (stmp == 0) {
	      if (LOGFILE == NULL)
		fprintf(stderr,"Invalid size zero\n");
	      else
		fprintf(LOGFILE,"Invalid size zero\n");

	      return (ASN_NODE *)child;
	    } else {
	      if (len < 0) {
		done = 1;
		if (debug)
		  printf("end of indefinite\n");
	      } else {
		if (LOGFILE == NULL)
		  fprintf(stderr, "unexpected empty child node\n");
		else
		  fprintf(LOGFILE, "unexpected empty child node\n");

		return (ASN_NODE *)NULL;
	      }
	    }
	  } else {
	    if (len >= 0) {
	      ltmp -= stmp;
	      if (ltmp <= 0) {
		done = 1;
	      }
	    }
	    if (!Attach(parent, child))
	      return (ASN_NODE *)NULL;
	  }
    	}
    	*size = running_size;
    	return parent;
}

GSTR *
DecodeGSTR(dbuf, length)
DBUF *dbuf;
int length;
{
    	GSTR *gstr;

    	if ((gstr = CALLOC(GSTR,1)) == NULL)
		return (GSTR *)NULL;
    	gstr->length = length;
    	gstr->data = (UCHAR *)calloc(1, length+1);
	if (gstr->data == NULL) {
		FreeGSTR(gstr);
		return (GSTR *)NULL;
	}
    	if (db_get(dbuf, gstr->data, length) != length) {
		FreeGSTR(gstr);
		return (GSTR *)NULL;
	}
    	return gstr;
}


OctetString
DecodeOctetString(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
	return (OctetString)DecodeGSTR(dbuf,length);
}

VisibleString
DecodeVisibleString(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
	return (VisibleString)DecodeGSTR(dbuf, length);
}

GeneralString
DecodeGeneralString(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
	return (GeneralString)DecodeGSTR(dbuf, length);
}

GeneralizedTime
DecodeGeneralizedTime(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
	return (GeneralizedTime)DecodeGSTR(dbuf, length);
}

ObjectDescriptor
DecodeObjectDescriptor(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
	return (ObjectDescriptor)DecodeGSTR(dbuf, length);
}

NumericString
DecodeNumericString(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
	return (NumericString)DecodeGSTR(dbuf, length);
}

/* Add more string decoders here when needed.
 */

int
DecodeInteger(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
    	int    i = 0;
    	int    intVal = 0;
    	UCHAR  ch;	
 
    	for (i = 0; i < length; i++) {
		db_get(dbuf, &ch, 1);
		intVal <<= 8;
		intVal += ch;
    	}
    	return intVal;
}

char
DecodeBoolean(node)
ASN_NODE *node;
{
    	return ((char)DecodeInteger(node));
}


BitString
DecodeBitString(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
    	BitString tmp;
    	int numberUnusedBits;
    	int numberBits;
    	int numberOctets;
    	DBUF *bits = NewDbuf();
    	UCHAR ch;
    	int i, j;

    	numberUnusedBits = GetLEN (dbuf);
    	numberOctets = length - 1;

    	for (i=0; i<numberOctets; i++) {
		db_get (dbuf, &ch, 1);
        	if (i == numberOctets-1)
	    		numberBits = numberUnusedBits;
		else
	    		numberBits = 0;
		for (j=7; j>=numberBits; j--) {
	    		if (0x01 & (ch >> j))
			db_put (bits, "1", 1);
	    	else
			db_put (bits, "0", 1);
		}
    	}
    	tmp = CALLOC(GSTR,1);
    	tmp->length = bits->size;
    	tmp->data = (UCHAR *)calloc(1, tmp->length+1);
    	strncpy (tmp->data, bits->data, bits->size);
    	tmp->data [tmp->length] = '\0';
	(void) FreeDbuf(bits);
    	return tmp;
}



/* return a null terminated character string */
any
DecodeAny(node)
ASN_NODE *node;
{
  DBUF *dbuf = node->dbuf;
  int length = node->length;
  char *result;

  result = calloc(1, length+1);
  strncpy (result, (char *)dbuf->data, length);
  return result;

}

char
DecodeNull(node)
ASN_NODE *node;
{
/*	DBUF *dbuf = node->dbuf;
	int length = node->length;
*/
     	return 1;
}

int
high_bit_set (ch)
UCHAR ch;
{
    	if ((ch & 0x80) == 0x80)
        	return 1;
    	return 0;
}
 
struct internal_oid *
decode_oid (buf, length)
DBUF *buf;
int length;
{
    	UCHAR ch;
    	int t =0;
    	int i =0;
    	int j = 0;
    	struct internal_oid *inter_oid;
 
    	inter_oid = (InternalOID *)calloc(1,sizeof(InternalOID));
    	while ( i < length) {
        	t = 0;
        	do {
            		db_get(buf, &ch, 1);
            		i++;
            		t = (t << 7) + (ch & 0x7f);
        	} while (high_bit_set(ch));
		if (j == 0) {
	    		inter_oid->ids[0] = t / 40;
	    		inter_oid->ids[1] = t % 40;
	    		j = 2;
		} else {
	    		inter_oid->ids[j++] = t;
		}
    	}
    	inter_oid->counts = j;
    	return inter_oid;
}


ObjectIdentifier
DecodeOid(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
    	ObjectIdentifier new_oid;
    	DBUF *tmp;
    	int i = 0;
    	char numStr[50];
    	struct internal_oid *inter_oid;

    	new_oid = CALLOC(GSTR,1);
    	tmp = NewDbuf();
    	inter_oid = decode_oid (dbuf, length);
    	for (i=0; i<inter_oid->counts; i++) {
		memset (numStr, '\0', sizeof(numStr));
		sprintf(numStr, "%d", inter_oid->ids[i]);
		db_put (tmp, numStr, strlen(numStr));
		if (i != (inter_oid->counts-1))
	    		db_put (tmp, ".", 1);
    	}
    	new_oid->length = tmp->size;
    	new_oid->data = CALLOC(char,new_oid->length + 1);
    	strncpy (new_oid->data, tmp->data, tmp->size);
    	new_oid->data [new_oid->length] = '\0';
    	FreeDbuf(tmp);
	(void) free ((char *)inter_oid);
    	return new_oid;
}

External *
DecodeExternal(node)
ASN_NODE *node;
{
    int class_var, tagno;
    ASN_NODE *child0;
    External *external = CALLOC (External,1);
     
    if ((!external) || (!node))
	return (External *)NULL;

    for (child0=node->head; child0!=NULL; child0=child0->next) {
        tagno = child0->tagno;
        class_var = child0->class_var;
        switch (tagno) {
        case OBJECTIDENTIFIER:
	    external->directReference = DecodeOid(child0);
	    break;
/* ----
	case INTEGER: 
	    external->indirectReference = DecodeInteger(dbuf, len);
	    break;
----- */
	case BER_OBJECTDESCRIPTOR:
	    external->dataValueDescriptor = DecodeObjectDescriptor(child0);
	    break;

        case 0:
	    external->which = t_singleASN1type;
	    DecodeASN1Records(external,child0);
	    break;

        case 1:
	    external->which = t_octetAligned;
	    external->u.octetAligned = DecodeOctetString(child0);
	    break;

        case 2:
	    if (class_var == UNIVERSAL) {
	      external->indirectReference = DecodeInteger(child0);
	    } else {
	        external->which = t_arbitrary;
	        external->u.arbitrary = DecodeBitString(child0);
	    }
	    break;
        }
    }
    return external;
}


