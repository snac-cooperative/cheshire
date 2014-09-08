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
 * This file contains a set of primitive data type encoders.
 * 11/30/94	-Aitao Chen
 * Fixes and ASN-1 record support by Ray Larson
 */
#include "zprimitive.h"
#include "z_parameters.h"
#include "z3950_3.h"

extern int db_put(DBUF *,UCHAR *,int);
extern int db_get(DBUF *, UCHAR *,int);
extern void FreeDbuf(DBUF *);
extern ASN_NODE *Attach(ASN_NODE *,ASN_NODE *);
extern ASN_NODE *EncodeGenericRecord(int class_var, int type, int tagno,
				     GenericRecord *genericRecord);
extern ASN_NODE *EncodeOPACRecord(int class_var, int type, int tagno,
				  OPACRecord *oPACRecord);
extern ASN_NODE *EncodeExplain_Record(int class_var, int type, int tagno,
				      Explain_Record *explain_Record);

extern ASN_NODE *EncodeInternationalString(int class_var, int tagno, 
					   InternationalString obj);



/*
 * The length can be as large as (2^31 - 1).
 */
int
PutLEN(dbuf, length)
DBUF 	*dbuf;
int   length;			/* must not be positive.	*/
{
    	UCHAR   data[6];
    	int     i=0;
	int	n=0;
    	UCHAR   ch;

	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;
	if (length < 0)
		return Z_ERROR;

    	/* Short, definite length	*/
	/* +-+------------+
	 * |0| length     |
	 * +-+------------+
	 */
    	if (length < 128) {	
		ch = (UCHAR) (length & 0x7F);
		if (db_put(dbuf, &ch, 1) != 1)
			return Z_ERROR;
		return Z_OK;
    	}

	/* The long, definite length form	*/
	/* +-+----------+  +----------+     +----------+ 
	 * |1| #-octets |  | octet    | ... | octet    |
	 * +-+----------+  +----------+     +----------+
	 */
	i = 0;
    	while (length) {
		data[i++] = length & 0xFF;
		length >>= 8;
    	}

    	/* Number of octets	*/
    	ch = (UCHAR)(i | 0x80);
    	if (db_put(dbuf, &ch, 1) != 1)
		return Z_ERROR;

    	for (n = i-1; n >= 0; n--) {
		ch = data[n];
		if (db_put(dbuf, &ch, 1) != 1)
			return Z_ERROR;
    	}
	return Z_OK;
}

/*
 * Put an octet that marks the beginning of an indefinite length form.
 */

int
PutIndLenStart(dbuf)
DBUF 	*dbuf;
{
    	UCHAR   ch;

	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;

	/* Indefinite length form	*/
	/* +-+-------+
	 * |1|0000000|
	 * +-+-------+
	 */
	ch = 0x80;
	if (db_put(dbuf, &ch, 1) != 1)
		return Z_ERROR;
	return Z_OK;
}


/*
 * Put two end-of-contents octets.
 */
int
PutIndLenEnd(dbuf)
DBUF 	*dbuf;
{
    	UCHAR   ch;

	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;

	/* end-of-contents octets	*/
	/* +--------+ +--------+
	 * |00000000| |00000000|
	 * +--------+ +--------+
	 */
	ch = 0x00;
	if (db_put(dbuf, &ch, 1) != 1)
		return Z_ERROR;
	ch = 0x00;
	if (db_put(dbuf, &ch, 1) != 1)
		return Z_ERROR;
	return Z_OK;
}


int
PutTAG(dbuf, class_var, form, tagno)
DBUF 	*dbuf;
TAG_CLASS class_var;
TAG_FORM  form;
int	tagno;
{
    	UCHAR   data[20];
    	int     n = 0;
    	UCHAR   ch;

 	if (dbuf == (DBUF *)NULL)
	    return Z_ERROR;
	if ((class_var < 0) || (class_var > 3) ||
	    (form < 0) || (form > 1) ||
	    (tagno < 0)) {
	    return Z_ERROR;
	}

    	/* short form 	*/
	/* +-----+-+---------+
	 * |class|f| tag no  |
	 * +-----+-+---------+
	 */
    	if (tagno < 31) {	
		ch = ((class_var << 6) | (form << 5) | (tagno & 0x1F));
		if (db_put(dbuf, &ch, 1) < 0)
			return Z_ERROR;
		return Z_OK;
    	}

    	/* long form */
	/* +-----+-+-------+ +-+--------+   +-+----------+ +-+-------+
	 * |class|f| 11111 | |1|        |...|1|          | |0|       |
	 * +-----+-+-------+ +-+--------+   +-+----------+ +-+-------+
	 */
    	ch = ((class_var << 6) | (form << 5) | 0x1F);
    	if (db_put(dbuf, &ch, 1) < 0)
	   	return Z_ERROR;

	n = 0;
    	while (tagno) {
		data[n++] = tagno & 0x7F;
		tagno >>= 7;
    	}
	for (n=n-1; n>0; n--) {
		ch = (data[n] | 0x80);
		if (db_put(dbuf, &ch, 1) < 0)
			return Z_ERROR;
    	}
    	ch = data[n];
    	if (db_put(dbuf, &ch, 1) < 0)
		return Z_ERROR;
	return Z_OK;
}

int
encode_generic_string(dbuf, class_var, tagno, gstr)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno; 
GSTR *gstr;
{
	if ((dbuf == (DBUF *)NULL) || (gstr == NULL))
		return Z_ERROR;
	if ((GDATA(gstr) == (UCHAR *)NULL) || (GSIZE(gstr) <= 0))
		return Z_ERROR;
    	if (PutTAG(dbuf, class_var, PRIMITIVE, tagno) != Z_OK)
		return Z_ERROR;
    	if (PutLEN(dbuf, gstr->length) != Z_OK)
		return Z_ERROR;
    	if (db_put(dbuf, gstr->data, gstr->length) != gstr->length)
		return Z_ERROR;
	return Z_OK;
}


int
encode_octstr(dbuf, class_var, tagno, ostr)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno; 
OctetString ostr;
{
	return encode_generic_string (dbuf, class_var, tagno, (GSTR *)ostr);
}


int
encode_bitstr(dbuf, class_var, tagno, bstr)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno; 
OctetString bstr;
{
    	int numberBits;		
    	int numberFullOctets;
    	int numberUnusedBits;
    	int remainder; 
    	int length;
    	int i, j;
    	UCHAR ch = 0x00;

	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;
	if ((GDATA(bstr) == (UCHAR *)NULL) || (GSIZE(bstr) <= 0))
		return Z_ERROR;

    	numberBits = bstr->length;
    	numberFullOctets = numberBits / 8;
    	remainder = numberBits % 8;
    	numberUnusedBits = remainder ? (8 - remainder) : 0;
    	length = 1 + (remainder ? (numberFullOctets + 1) : numberFullOctets);

    	if (PutTAG(dbuf, class_var, PRIMITIVE, tagno) != Z_OK)
		return Z_ERROR;
    	if (PutLEN(dbuf, length) != Z_OK)
		return Z_ERROR;
    	if (PutLEN(dbuf, numberUnusedBits) != Z_OK)
		return Z_ERROR;
    	for (i=0; i<length-1; i++) {
		ch = 0x00;
		for (j=0; j<8; j++) {
	    		ch = ch << 1;
	    		if ((i*8+j) < numberBits)
	       			ch |= (bstr->data[i*8+j] == '1') ? 1 : 0;
	    		else
	  			ch |= 0;
		}
    		if (db_put(dbuf, &ch, 1) != 1)
			return Z_ERROR;
    	}
	return Z_OK;
}

/*
 * Note: encode_interger was derived from Simon Spero's program.
 * It will be replaced in future release.
 */
int
encode_integer(dbuf, class_var, tagno, obj)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno; 
int obj;
{
    	int             val;
    	int             mask = 0xFF80;
    	int             key;
    	unsigned int    foo;
    	int             t;

    	val = obj;
    	/*
     	 * skip over redundant padding we check the high nine bits to see if 
     	 * they all have the same value. if so, we can skip them.
     	 */
    	if (val >= 0) {
		key = 0;
    	} else {
		key = mask;
    	}

    	for (t = sizeof(int) - 2; t >= 0; t--) {
		foo = ((ULONG) val & (mask << t * 8)) >> t * 8;
		if (foo != key)
	    		break;
    	}

    	PutTAG(dbuf, class_var, 0, tagno);
    	PutLEN(dbuf, t + 2);
    	for (t = t + 1; t >= 0; t--) {
		UCHAR   c;
		foo = ((ULONG) val & ((ULONG) 0xff << t * 8)) >> t * 8;
		c = foo;
		db_put(dbuf, &c, 1);
    	}
	return Z_OK;
}


int
encode_oid_component(buf, component)
DBUF *buf;
int component;
{
    	UCHAR data[1024];
    	UCHAR ch;
    	int t=0;
    	int i=0;

	if (buf == (DBUF *)NULL)
		return Z_ERROR;
    	while(component) {
		data[t] = component & 0x7F;
		component >>= 7;
		t++;
    	}
    	for(i=t-1;i>0;i--) {
		ch = data[i] | 0x80;
		if (db_put(buf, &ch, 1) != 1)
			return Z_ERROR;
    	}
    	if (db_put(buf, &data[0], 1) != 1)
		return Z_ERROR;
	return Z_OK;
}

int
encode_oid(dbuf, class_var, tagno, obj)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno; 
ObjectIdentifier obj;
{
    	struct internal_oid *the_oid, *oid_convert();
    	int tmp, i;
    	DBUF *buf = NewDbuf();
    
	if ((dbuf == (DBUF *)NULL) || 
	    (buf == (DBUF *)NULL) ||
	    (obj == NULL))
		return Z_ERROR;
    	if ((the_oid = oid_convert(obj->data)) == NULL) {
		fprintf(stderr, "invalid object identifier.\n");
		return Z_ERROR;
	}
    	tmp = the_oid->ids[0] * 40 + the_oid->ids[1];
    	if (encode_oid_component(buf, tmp) != Z_OK)
		return Z_ERROR;
    	for (i=2; i<the_oid->counts; i++)
		if (encode_oid_component(buf, the_oid->ids[i]) != Z_OK)
			return Z_ERROR;

    	if (PutTAG(dbuf, class_var, 0, tagno) != Z_OK)
		return Z_ERROR;
    	if (PutLEN(dbuf, buf->size) != Z_OK)
		return Z_ERROR;
    	if (db_put(dbuf, buf->data, buf->size) != buf->size)
		return Z_ERROR;

    	(void)free((char *)the_oid);
	(void)FreeDbuf(buf);
	return Z_OK;
}

/*
 *      +--+-+-----+ +--------+ +--------+
 *      |00|0|  1  | |    1   | |00000000|  <== FALSE
 *      +--+-+-----+ +--------+ +--------+
 *
 *      +--+-+-----+ +--------+ +--------+
 *      |00|0|  1  | |    1   | |11111111|  <== TRUE
 *      +--+-+-----+ +--------+ +--------+
 */
int
encode_boolean(dbuf, class_var, tagno, boolval)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno; 
UCHAR boolval;
{
    	UCHAR	ch;
    	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;
    	if (PutTAG(dbuf, class_var, PRIMITIVE, tagno) != Z_OK)
		return Z_ERROR;
    	if (PutLEN(dbuf, 1) != Z_OK)
		return Z_ERROR;
    	/*
     	 * ch = boolval?0xff:0x00;
     	 */
    	ch = boolval?0x01:0x00;
    	if (db_put(dbuf, &ch, 1) != 1)
		return Z_ERROR;
    	return Z_OK;
}

/*
 *	+--+-+-----+ +--------+
 *	|00|0|  5  | |    0   |
 *      +--+-+-----+ +--------+
 */
int
encode_null(dbuf, class_var, tagno)
DBUF *dbuf; 
TAG_CLASS class_var; 
int tagno;
{
    	if (dbuf == (DBUF *)NULL)
		return Z_ERROR;
    	if (PutTAG(dbuf, class_var, PRIMITIVE, tagno) != Z_OK)
		return Z_ERROR;
    	if (PutLEN(dbuf, 0) != Z_OK)
		return Z_ERROR;
    	return Z_OK;
}


ASN_NODE *
EncodeBoolean(class_var, tagno, obj)
TAG_CLASS class_var; 
int tagno; 
UCHAR obj;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;
    	if (encode_boolean(node->dbuf, class_var, tagno, obj) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}


ASN_NODE *
EncodeNull(class_var, tagno)
TAG_CLASS class_var;
int tagno;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;
    	if (encode_null(node->dbuf, class_var, tagno) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

/*
 *	TO BE IMPLEMENTED.
 */
ASN_NODE *
EncodeAny(class_var, tagno, obj)
TAG_CLASS class_var; 
int tagno; 
any obj;
{
  ASN_NODE *node;
  if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
    return (ASN_NODE *)NULL;
  if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)obj) != Z_OK)
    return (ASN_NODE *)NULL;
  node->length = node->dbuf->size;
  return node; 
}


ASN_NODE *
EncodeOid(class_var, tagno, obj)
TAG_CLASS class_var; 
int tagno; 
ObjectIdentifier obj;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;
    	if (encode_oid(node->dbuf, class_var, tagno, obj) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}


ASN_NODE *
EncodeInteger(class_var, tagno, obj)
TAG_CLASS class_var; 
int tagno; 
int obj;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;
    	encode_integer(node->dbuf, class_var, tagno, obj);
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeOctetString(class_var, tagno, ostr)
TAG_CLASS class_var; 
int tagno; 
OctetString ostr;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;
    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)ostr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeNumericString(class_var, tagno, nstr)
TAG_CLASS class_var; 
int tagno; 
NumericString nstr;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

	/* Check nstr for invalid characters. A numeric string consists of
	 * only digits. i.e '0'-'9'.
	 */
    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)nstr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodePrintableString(class_var, tagno, pstr)
TAG_CLASS class_var; 
int tagno; 
PrintableString pstr;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)pstr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeGeneralString(class_var, tagno, gstr)
TAG_CLASS class_var; 
int tagno; 
GeneralString gstr;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)gstr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeVisibleString(class_var, tagno, vstr)
TAG_CLASS class_var; 
int tagno; 
VisibleString vstr;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)vstr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeIA5String(class_var, tagno, istr)
TAG_CLASS class_var; 
int tagno; 
IA5String istr;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)istr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeUTCTime(class_var, tagno, utcTime)
TAG_CLASS class_var; 
int tagno; 
UTCTime utcTime;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)utcTime) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeGeneralizedType(class_var, tagno, gtime)
TAG_CLASS class_var; 
int tagno; 
GeneralizedType gtime;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)gtime) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeGeneralizedTime(class_var, tagno, gtime)
TAG_CLASS class_var; 
int tagno; 
GeneralizedTime gtime;
{
    	ASN_NODE *node;
    	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;

    	if (encode_generic_string(node->dbuf, class_var, tagno, (GSTR *)gtime) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeObjectDescriptor(class_var, tagno, odstr)
TAG_CLASS class_var; 
int tagno; 
ObjectDescriptor odstr;
{
	return EncodeOctetString(class_var, tagno, (OctetString)odstr);
}


ASN_NODE *
EncodeBitString(class_var, tagno, bstr)
TAG_CLASS class_var; 
int tagno; 
BitString bstr;
{
    	ASN_NODE *node;

	if (bstr == NULL)
		return (ASN_NODE *)NULL;
	/* The character set for the bit string consists of '0' and '1'.
	 * check for possible invalid characters in the bit string.
	 */
	if ((node = NewAsnNode(class_var, PRIMITIVE, tagno)) == (ASN_NODE *)NULL)
		return (ASN_NODE *)NULL;
    	if (encode_bitstr(node->dbuf, class_var, tagno, bstr) != Z_OK)
		return (ASN_NODE *)NULL;
    	node->length = node->dbuf->size;
    	return node; 
}

ASN_NODE *
EncodeExternal(class_var, tagno, external)
TAG_CLASS	class_var;
int 	tagno;
External	*external;
{
  char *oid;

  ASN_NODE *node, *node0, *node1;
  
  if ((node = NewAsnNode(class_var, CONSTRUCTED, tagno)) == (ASN_NODE *)NULL)
    return (ASN_NODE *)NULL;
  if (external->directReference)
    Attach(node, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,external->directReference));
  if (external->indirectReference)
    Attach(node, EncodeInteger(UNIVERSAL,INTEGER,external->indirectReference));
  if (external->dataValueDescriptor)
    Attach(node, EncodeObjectDescriptor(UNIVERSAL,BER_OBJECTDESCRIPTOR,
					external->dataValueDescriptor));
  switch (external->which) {
  case t_singleASN1type:
	      
    oid = (char *)external->directReference->data;
    /* Assuming that this record is defined as an ASN1 structure 
     * and that we have encoding for it, find out from the OID 
     * which type it is and do it.
     */
    
    /* Generic records most common */
    if (strcmp(oid, GRS1RECSYNTAX) == 0) {
      node0 = EncodeGenericRecord(CLASS_NONE, 0 , 0,
				  external->u.singleASN1type);

      node1 = NewAsnNode(BER_CONTEXT, CONSTRUCTED, 0);
      Attach(node1, node0);      
      Attach(node, node1);
    }
    
    /* OPAC Records (unlikely) */
    else if (strcmp(oid, OPACRECSYNTAX) == 0)
      Attach(node, EncodeOPACRecord(CLASS_NONE, 0, 0,
				    external->u.singleASN1type));
    /* Explain Records (also common RSN?) */
    else if (strcmp(oid, EXPLAINRECSYNTAX) == 0) {
      node0 = EncodeExplain_Record(CLASS_NONE, 0, 0, 
				   external->u.singleASN1type);
      node1 = NewAsnNode(BER_CONTEXT, CONSTRUCTED, 0);
      Attach(node1, node0);      
      Attach(node, node1);
    }
    
    /* SUTRS Records (also common RSN?) */
    else if (strcmp(oid, SUTRECSYNTAX) == 0) {
      node0 = EncodeInternationalString(UNIVERSAL, GENERALSTRING, 
					external->u.singleASN1type);
      node1 = NewAsnNode(BER_CONTEXT, CONSTRUCTED, 0);
      Attach(node1, node0);      
      Attach(node, node1);
    }
    /* Summary records */
    else if (strcmp(oid, SUMMARYRECSYNTAX) == 0)
      Attach(node, EncodeAny(BER_CONTEXT, 0, external->u.singleASN1type));
    /* Extended Services */
    else if (strcmp(oid, ESRECSYNTAX) == 0)
      Attach(node, EncodeAny(BER_CONTEXT, 0, external->u.singleASN1type));
    else /* we don't know this one... */
      Attach(node, EncodeAny(BER_CONTEXT, 0, external->u.singleASN1type));
  
    break;
  case t_octetAligned:
    Attach(node, EncodeOctetString(BER_CONTEXT, 1, external->u.octetAligned));
    break;
  case t_arbitrary:
    Attach(node, EncodeBitString(BER_CONTEXT, 2, external->u.arbitrary));
    break;
  }
  return node;
}

