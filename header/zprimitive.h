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
#ifndef Z_PRIMITIVE_H
#define Z_PRIMITIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifndef WIN32
#include <sys/uio.h>
#include <unistd.h>
#endif
#include <sys/types.h>
#include "asn_utils.h"

#include "alpha.h"

#include "gc.h"

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#define	Z_ERROR	0
#define	Z_OK	1

typedef enum {
	Z_FALSE = 0,
	Z_TRUE = 1,
	Z_UNKNOWN = 2
} ZBOOL;

typedef	int	ASN1_TYPE;

#define	ASN_BOOLEAN		1
#define	INTEGER		2
#define	BITSTRING	3
#define	OCTETSTRING	4
#define	ANSNULL		5
#define	OBJECTIDENTIFIER	6
#define BER_OBJECTDESCRIPTOR	7
#define	EXTERNAL	8
#define	REAL		9
#define	ENUMERATED	10
#define	SEQUENCE	16
#define	SEQUENCEOF	16
#define	ASN_SET		17
#define	ASN_SETOF	17
#define	NUMERICSTRING	18
#define	PRINTABLESTRING	19
#define VISIBLESTRING	26
#define GENERALSTRING	27

#define	ANY		100

typedef unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef unsigned int	UINT;
typedef unsigned int	ULONG;


typedef struct generic_string {
	int length;	/* number of octets in data buffer. The terminating
			 * null character is not included in the length	*/
	UCHAR *data;	/* null-terminated unsigned char string	*/
} GSTR;

#define	GDATA(gs)	((gs)->data)
#define	GSIZE(gs)	((gs)->length)

typedef unsigned char	Boolean;	/* FALSE=0, TRUE=otherwise	*/
typedef	struct generic_string	*OctetString;
typedef	struct generic_string	*BitString;
typedef	struct generic_string	*ObjectIdentifier;
typedef	struct generic_string	*ObjectDescriptor;
typedef	struct generic_string	*NumericString;
typedef	struct generic_string	*PrintableString;
typedef	struct generic_string	*IA5String;
typedef	struct generic_string	*UTCTime;
typedef	struct generic_string	*GeneralizedType;
typedef	struct generic_string	*GeneralizedTime;
typedef	struct generic_string	*GraphicsString;
typedef	struct generic_string	*VisibleString;
typedef	struct generic_string	*GeneralString;
typedef	struct generic_string	*CharacterString;

typedef enum {
	UNIVERSAL = 0,
	APPLICATION = 1,
	BER_CONTEXT = 2,
	PRIVATE = 3
} TAG_CLASS;

typedef enum {
	PRIMITIVE = 0,
	CONSTRUCTED = 1
} TAG_FORM;

#define CLASS_NONE      -1
#define TYPE_NONE       -1
#define TAGNO_NONE      -1

typedef struct asnnode	ASN_NODE;
struct asnnode {
	TAG_FORM	type;	/* PRIMITIVE or CONSTRUCTED	*/
	TAG_CLASS	class_var;
	int		tagno;
	int		length;	/* The running length.		*/
	DBUF	 	*dbuf;	/* Dynamic buffer		*/
	ASN_NODE 	*parent;
	ASN_NODE 	*head;	/* First Child. NULL for PRIMITIVE node	*/
	ASN_NODE 	*next;	/* Next Sibling.NULL for PRIMITIVE node	*/
	ASN_NODE 	*tail;	/* Last Child. NULL for PRIMITIVE node	*/
};

typedef	void *any;

typedef struct external {
	ObjectIdentifier	directReference;	/* OPTIONAL	*/
	int	indirectReference;			/* OPTIONAL	*/
	ObjectDescriptor	dataValueDescriptor;	/* OPTIONAL	*/
	enum {
		t_singleASN1type, t_octetAligned, t_arbitrary
 	} which;
	union {
		any	singleASN1type;
		OctetString octetAligned;
		BitString arbitrary;
	} u;
} External;

ASN_NODE *NewAsnNode();

typedef struct internal_oid {
	int	counts;
	int	ids[50];
} InternalOID;

#ifndef __DMALLOC_H__
#ifndef DMALLOC_INCLUDE
#define	CALLOC(x,y) (x *)calloc(y, sizeof(x))
#endif
#endif

typedef	enum {
	Z_CLIENT = 0,	
	Z_SERVER = 1
} WHOAMI;

/* Client programs should set whoami to Z_CLIENT, and server programs
 * to Z_SERVER.
 */
#include "berintern.h"

#endif /* Z_PRIMITIVE_H*/

