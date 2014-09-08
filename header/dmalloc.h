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

#ifdef WIN32
#include "dmalloc_nt.h"
#else
#ifdef DMALLOC_INCLUDE
#ifndef DMALLOC_DISABLE
#include "dmalloc_dist.h"
#else
#include <sys/types.h>
#define DMALLOC_SIZE size_t
#endif /* ! DMALLOC_DISABLE */
#endif /* DMALLOC_INCLUDE */

/*
 * alloc macros to improve memory usage readibility...
 * from older versions of DMALLOC...
 */
#undef ALLOC
#define ALLOC(type, count) \
  (type *)malloc((DMALLOC_SIZE)(sizeof(type) * (count)))

#undef MALLOC
#define MALLOC(size) \
  (char *)malloc((DMALLOC_SIZE)(size))

/* NOTICE: the arguments are REVERSED from normal calloc() */
#undef CALLOC
#define CALLOC(type, count) \
  (type *)calloc((DMALLOC_SIZE)(count), (DMALLOC_SIZE)sizeof(type))

#undef REALLOC
#define REALLOC(ptr, type, count) \
  (type *)realloc((char *)(ptr), (DMALLOC_SIZE)(sizeof(type) * (count)))

#undef REMALLOC
#define REMALLOC(ptr, size) \
  (char *)realloc((char *)(ptr), (DMALLOC_SIZE)(size))

#undef FREE
#define FREE(ptr) \
  free((char *)(ptr))

#endif /* WIN32 */
