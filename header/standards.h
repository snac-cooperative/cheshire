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
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1995    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/*	
 *	@(#)$RCSfile: standards.h,v $ $Revision: 4.2.9.3 $ (DEC) $Date: 1994/03/29 15:21:43 $
 */ 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * standards.h
 *
 *	Modification History:
 *
 * 11-Apr-91	Peter H. Smith
 *	If _POSIX_4SOURCE is defined, define _POSIX_SOURCE if it isn't
 *      already defined.
 *
 * 01-Apr-91	Fred Canter
 *	MIPS C 2.20+, changes for -std
 *
 */

/*
 * COMPONENT_NAME: standards.h
 *                                                                    
 * ORIGIN: IBM
 *
  * Copyright International Business Machines Corp. 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   
#ifndef _STANDARDS_H_
#define _STANDARDS_H_

/*
 * If _POSIX_4SOURCE was turned on explicitly, make sure that _POSIX_SOURCE
 * is also turned on.
 */

#ifdef  _POSIX_4SOURCE
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif
#endif

#ifdef _POSIX_SOURCE
#define _ANSI_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#define _POSIX_SOURCE
#define _ANSI_C_SOURCE
#endif

#ifdef _AES_SOURCE
#define _XOPEN_SOURCE
#define _POSIX_SOURCE
#define _ANSI_C_SOURCE
#endif

#if (!defined (_XOPEN_SOURCE)) && (!defined (_POSIX_SOURCE)) && (!defined (_ANSI_C_SOURCE)) && (!defined (_AES_SOURCE)) && (!defined (__LANGUAGE_ASSEMBLY__)) && (!defined(_POSIX_4SOURCE))
#define _AES_SOURCE
#define _XOPEN_SOURCE
#define _POSIX_SOURCE
#define _POSIX_4SOURCE
#define _ANSI_C_SOURCE
#define _OSF_SOURCE
#endif

#ifdef __LANGUAGE_ASSEMBLY__
#define _OSF_SOURCE
#endif

/* macros to simplify C++ ifdef blocks */
#ifdef __cplusplus
#ifndef _BEGIN_CPLUSPLUS
#define _BEGIN_CPLUSPLUS extern "C" {
#endif
#ifndef _END_CPLUSPLUS
#define _END_CPLUSPLUS }
#endif
#else
#ifndef _BEGIN_CPLUSPLUS
#define _BEGIN_CPLUSPLUS
#endif
#ifndef _END_CPLUSPLUS
#define _END_CPLUSPLUS
#endif
#endif

/* automation for non ANSI compilers */
/*
 * GNU and MIPS C compilers define __STDC__ differently.
 *	MIPS: -std0 = undefined, -std = 0, -std1 = 1
 *	GNU:  -traditional = undefined, otherwise = 1
 */
#ifdef __GNUC__

#if !__STDC__
#ifndef _NO_PROTO
#define _NO_PROTO
#endif
#ifndef _NONSTD_TYPES
#define _NONSTD_TYPES
#endif
#endif

#else	/* !__GNUC__ */

#if !defined(__STDC__) && !defined(__cplusplus)
#ifndef _NO_PROTO
#define _NO_PROTO
#endif
#ifndef _NONSTD_TYPES
#define _NONSTD_TYPES
#endif
#endif

#if defined(__cplusplus) && !defined(__LANGUAGE_C__)
#define __LANGUAGE_C__
#endif

#endif	/* __GNUC__ */

#ifdef _NO_PROTO
#define __(args)	()
#else /* _NO_PROTO */
#define __(args)	args
#endif /* _NO_PROTO */

#if defined(__LANGUAGE_PASCAL__)
#undef _AES_SOURCE
#undef _XOPEN_SOURCE
#undef _POSIX_SOURCE
#undef _ANSI_C_SOURCE
#undef _OSF_SOURCE
#endif /* __LANGUAGE_PASCAL__ */

#endif /* _STANDARDS_H_ */

