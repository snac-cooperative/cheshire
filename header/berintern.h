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
#include "alpha.h"
#ifndef BERINTERN_H
#define BERINTERN_H

int put_len(DBUF *dbuf, unsigned int length);
int  GetLen(DBUF *dbuf);
int put_tag(DBUF *dbuf, TAG_CLASS class_var, int type, int tagno);
int GetTag(DBUF *dbuf, int *class_var, int *type, int *tagno);
int encode_octstr(DBUF *dbuf, TAG_CLASS class_var, int tagno, OctetString obj);
ASN_NODE * EncodeOctetString(TAG_CLASS class_var, int tagno, OctetString obj);
ASN_NODE * EncodeGeneralString(TAG_CLASS class_var, int tagno, GeneralString obj);
ASN_NODE * EncodeGeneralizedTime(TAG_CLASS class_var, int tagno, GeneralizedTime obj);
ASN_NODE * EncodeVisibleString(TAG_CLASS class_var, int tagno, VisibleString obj);
int encode_bitstr(DBUF *dbuf, TAG_CLASS class_var, int tagno, OctetString obj);
ASN_NODE * EncodeBitString(TAG_CLASS class_var, int tagno, BitString obj);
int encode_integer(DBUF *dbuf, TAG_CLASS class_var, int tagno, int obj);
ASN_NODE * EncodeInteger(TAG_CLASS class_var, int tagno, int obj);
int encode_oid2(DBUF *dbuf, TAG_CLASS class_var, int tagno, ObjectIdentifier obj);
ASN_NODE * EncodeOid(TAG_CLASS class_var, int tagno, ObjectIdentifier obj);
ASN_NODE * EncodeBoolean(TAG_CLASS class_var, int tagno, UCHAR obj);
int encode_null2(DBUF *dbuf, TAG_CLASS class_var, int tagno);
ASN_NODE * EncodeNull(TAG_CLASS class_var, int tagno);
ASN_NODE * EncodeAny(TAG_CLASS class_var, int tagno, any obj);
ASN_NODE * EncodeExternal(TAG_CLASS class_var, int tagno, External *obj);


int GetTag(DBUF *dbuf, int *class_var, int *type, int *tagno);
int GetLen(DBUF *dbuf);
OctetString DecodeOctetString(ASN_NODE *node);
BitString DecodeBitString(ASN_NODE *node);
int DecodeInteger(ASN_NODE *node);
char DecodeBoolean(ASN_NODE *node);
ObjectIdentifier DecodeOid(ASN_NODE *node);
any DecodeAny(ASN_NODE *node);
External *DecodeExternal(ASN_NODE *node);
char DecodeNull(ASN_NODE *node);

#endif	/*	BERINTERN_H	*/

