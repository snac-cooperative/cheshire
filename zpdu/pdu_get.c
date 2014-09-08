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
#include "z3950_3.h"
#include "connect.h"

#define	DECODE_INIT_REQUEST_ERROR	1
#define	DECODE_INIT_RESPONSE_ERROR	2
#define	DECODE_SEARCH_REQUEST_ERROR	3
#define	DECODE_SEARCH_RESPONSE_ERROR	4
#define	DECODE_PRESENT_REQUEST_ERROR	5
#define	DECODE_PRESENT_RESPONSE_ERROR	6
#define	DECODE_DELETE_RESULT_SET_REQUEST_ERROR	7
#define	DECODE_DELETE_RESULT_SET_RESPONSE_ERROR	8
#define	DECODE_ACCESS_CONTROL_REQUEST_ERROR	9
#define	DECODE_ACCESS_CONTROL_RESPONSE_ERROR	10
#define	DECODE_RESOURCE_CONTROL_REQUEST_ERROR	11
#define	DECODE_RESOURCE_CONTROL_RESPONSE_ERROR	12
#define	DECODE_TRIGGER_RESOURCE_CONTROL_REQUEST_ERROR	13
#define	DECODE_RESOURCE_REPORT_REQUEST_ERROR	14
#define	DECODE_RESOURCE_REPORT_RESPONSE_ERROR	15
#define	DECODE_UNKNOWN_PDU_TYPE_ERROR	16

extern int debug;
extern FILE *LOGFILE;

int debug_asn=0;

void
clear_zerrno()
{
    z_errno = Z_OK;
}

void
set_zerrno(errnum)
int errnum;
{
    z_errno = errnum;
}

/*
 * Read a pdu from socket fd. Returns NULL to indicate an error has occurred.
 */
PDU *
ClientGetPDU(fd, timeoutsecs)
int fd;
int timeoutsecs;
{
    	ASN_NODE *node, *GetPDUFromStream();
    	int size;
    	PDU *pdu;

    	node = GetPDUFromStream(fd, &size, timeoutsecs);
    	if (node == (ASN_NODE *)NULL) 
		return (PDU *)NULL;
	/* if (debug)
	 *	fprintf(stdout, "%d bytes read from socket: %d\n", size, fd);
	 */
	switch(node->tagno) {
	case 20:
	case 22:
	case 24:
	case 26:
	case 28:
	case 30:
	case 32:
	case 33:
	case 35:
	case 43:
	case 45:
	case 46:
	  /* Huh? Got a request from the server?? Fail */
	  return (PDU *)NULL;
	  break;
	default:
	  pdu = DecodePDU (node);
	  (void)FreeAsnNode(node);
	  return pdu;
	  break;
	}
}

PDU *
GetPDU(fd, timeoutsecs)
int fd;
int timeoutsecs;
{
    	ASN_NODE *node, *GetPDUFromStream();
    	int size;
    	PDU *pdu;

    	node = GetPDUFromStream(fd, &size, timeoutsecs);
    	if (node == (ASN_NODE *)NULL) 
		return (PDU *)NULL;
	/* if (debug)
	 *	fprintf(stdout, "%d bytes read from socket: %d\n", size, fd);
	 */
	pdu = DecodePDU (node);
	(void)FreeAsnNode(node);
	return pdu;
}


/*
 * Encode, then transmit a PDU.
 */
int
PutPDU(fd, pdu)
int	fd;
PDU	*pdu;
{
	DBUF	*buf;
	int	nBytes;

	/* pdu must NOT be NULL	*/
	if (pdu == NULL)
	    	return Z_ERROR;

	if ((buf = EncodePDU2(pdu)) == NULL) {
	    	return Z_ERROR;
	}
	nBytes = WriteToSocket (fd, buf->data, buf->size);
	if (nBytes != buf->size) {
	  if (LOGFILE == NULL)
	    fprintf(stderr, 
		    "ERROR: only %d out of %d bytes were sent to socket %d.\n",
		    nBytes, buf->size, fd);
	  else
	    fprintf(LOGFILE, 
		    "ERROR: only %d out of %d bytes were sent to socket %d.\n",
		    nBytes, buf->size, fd);
	  
	  (void) FreeDbuf(buf);
	  return Z_ERROR;
	}
	if (debug)
	  fprintf(stderr, "%d bytes sent to socket: %d.\n",nBytes,fd);
	(void) FreeDbuf(buf);
	return Z_OK;
}


int
TransmitPDU(fd, buf)
int	fd;
DBUF *buf;
{
    return WriteToSocket (fd, buf->data, buf->size);
}


/*
 * Return an undecoded pdu unit.
 */
DBUF *
ReceivePDU(fd, timeoutsecs)
int fd;	
int timeoutsecs;
{
    	DBUF *inBuf;
    	ASN_NODE *node, *GetPDUFromStream();
    	int size;

    	if ((node = GetPDUFromStream(fd, &size, timeoutsecs)) == (ASN_NODE *)NULL)
		return (DBUF *)NULL;
	if (debug)
    		fprintf(stdout, "%d bytes read from socket: %d\n", size, fd);

    	if (LengthAsnTree2(node) < 0) {
		(void)FreeAsnNode(node);
		return (DBUF *)NULL;
	}
    	if ((inBuf = SerializeAsnTree (node)) == (DBUF *)NULL) {
		(void)FreeAsnNode(node);
		return (DBUF *)NULL;
	}
    	(void)FreeAsnNode(node);
	return inBuf;
}

