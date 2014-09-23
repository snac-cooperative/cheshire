/*
 *  Copyright (c) 1990-2012 [see Other Notes, below]. The Regents of the
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
/************************************************************************
*
*	Header Name:	compress_dat.h
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Header for compressed index (postings) handling
*                       symbolic definitions based on the BSD bitstring.h
*	Usage:		#include compress_dat.h
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		11/21/2012
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2012.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#ifndef _COMPRESS_H
#define _COMPRESS_H

#define cdata_blocksize_bytes 8192
#define cdata_blocksize_bits 65536

typedef	unsigned char cdatmap_t;

typedef struct cdatablock_key {
    short blockid;
    int termid;
  } cdatablock_key;


/* internal macros */
				/* byte of the bitmaping bit is in */
#define	_bitmap_byte(bit) \
	((bit) >> 3)

				/* mask for the bit within its byte */
#define	_bitmap_mask(bit) \
	(1 << ((bit)&0x7))



/* external macros */

#define bitblock_id(recno) \
  (recno >> 16)

#define bitblock_bit(recno) \
  ((recno-1) & 0xffff)
				/* bytes in a bitmaping of nbits bits */
#define	bitmap_size(nbits) \
	((((nbits) - 1) >> 3) + 1)

				/* allocate a bitmaping */
#define	bitmap_block_alloc() \
	(bitmap_t *)calloc(1, bitmap_blocksize_bytes)

				/* is bit N of bitmaping name set? */
#define	bitmap_test(name, bit) \
	((name)[_bitmap_byte(bit)] & _bitmap_mask(bit))

				/* set bit N of bitmaping name */
#define	bit_set(name, bit) \
	(name)[_bitmap_byte(bit)] |= _bitmap_mask(bit)

				/* clear bit N of bitmaping name */
#define	bit_clear(name, bit) \
	(name)[_bitmap_byte(bit)] &= ~_bitmap_mask(bit)

				/* clear bits start ... stop in bitmaping */
#define	bitblock_set(name, recno) \
  (name)[_bitmap_byte(bitblock_bit(recno))] |= _bitmap_mask(bitblock_bit(recno))

#define	bitblock_test(name, recno) \
  (name)[_bitmap_byte(bitblock_bit(recno))] & _bitmap_mask(bitblock_bit(recno))

				/* clear bit N of bitmaping name */
#define	bitblock_clear(name, recno) \
  (name)[_bitmap_byte(bitblock_bit(recno))] &= ~_bitmap_mask(bitblock_bit(recno))

				/* clear bits start ... stop in bitmaping */
#define	bit_nclear(name, start, stop) { \
	register bitmap_t *_name = name; \
	register int _start = start, _stop = stop; \
	register int _startbyte = _bitmap_byte(_start); \
	register int _stopbyte = _bitmap_byte(_stop); \
	if (_startbyte == _stopbyte) { \
		_name[_startbyte] &= ((0xff >> (8 - (_start&0x7))) | \
				      (0xff << ((_stop&0x7) + 1))); \
	} else { \
		_name[_startbyte] &= 0xff >> (8 - (_start&0x7)); \
		while (++_startbyte < _stopbyte) \
			_name[_startbyte] = 0; \
		_name[_stopbyte] &= 0xff << ((_stop&0x7) + 1); \
	} \
}

				/* set bits start ... stop in bitmaping */
#define	bit_nset(name, start, stop) { \
	register bitmap_t *_name = name; \
	register int _start = start, _stop = stop; \
	register int _startbyte = _bitmap_byte(_start); \
	register int _stopbyte = _bitmap_byte(_stop); \
	if (_startbyte == _stopbyte) { \
		_name[_startbyte] |= ((0xff << (_start&0x7)) & \
				    (0xff >> (7 - (_stop&0x7)))); \
	} else { \
		_name[_startbyte] |= 0xff << ((_start)&0x7); \
		while (++_startbyte < _stopbyte) \
	    		_name[_startbyte] = 0xff; \
		_name[_stopbyte] |= 0xff >> (7 - (_stop&0x7)); \
	} \
}

				/* find first bit clear in name */
#define	bit_ffc(name, nbits, value) { \
	register bitmap_t *_name = name; \
	register int _byte, _nbits = nbits; \
	register int _stopbyte = _bitmap_byte(_nbits), _value = -1; \
	for (_byte = 0; _byte < _stopbyte; ++_byte) \
		if (_name[_byte] != 0xff) { \
			_value = _byte << 3; \
			for (_stopbyte = _name[_byte]; (_stopbyte&0x1); \
			    ++_value, _stopbyte >>= 1); \
			break; \
		} \
	*(value) = _value; \
}

				/* find first bit set in name */
#define	bit_ffs(name, nbits, value) { \
	register bitmap_t *_name = name; \
	register int _byte, _nbits = nbits; \
	register int _stopbyte = _bitmap_byte(_nbits), _value = -1; \
	for (_byte = 0; _byte < _stopbyte; ++_byte) \
		if (_name[_byte]) { \
			_value = _byte << 3; \
			for (_stopbyte = _name[_byte]; !(_stopbyte&0x1); \
			    ++_value, _stopbyte >>= 1); \
			break; \
		} \
	*(value) = _value; \
}



#endif

