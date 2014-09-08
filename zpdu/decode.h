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

GSTR *
DecodeGSTR(dbuf, length)
DBUF *dbuf;
int length;
{
    	GSTR *gstr;

    	if ((gstr = MALLOC(GSTR)) == NULL)
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
    	tmp = MALLOC(GSTR);
    	tmp->length = bits->size;
    	tmp->data = (UCHAR *)calloc(1, tmp->length+1);
    	strncpy (tmp->data, bits->data, bits->size);
    	tmp->data [tmp->length] = '\0';
	(void) FreeDbuf(bits);
    	return tmp;
}


/*	To be implemented.	*/
any
DecodeAny(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
    return NULL;
}

char
DecodeNull(node)
ASN_NODE *node;
{
	DBUF *dbuf = node->dbuf;
	int length = node->length;
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

    	new_oid = MALLOC(GSTR);
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
    	new_oid->data = malloc(new_oid->length + 1);
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
    int class_var, type, tagno;
    int len;
    ASN_NODE *child0;
    External *external = MALLOC (External);
     
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
	case OBJECTDESCRIPTOR:
	    external->dataValueDescriptor = DecodeObjectDescriptor(child0);
	    break;

        case 0:
	    external->which = t_singleASN1type;
	    external->u.singleASN1type = DecodeAny(child0);
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


