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
#ifndef CONNECT_H
#define CONNECT_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <sys/netconfig.h>
#endif
#ifndef WIN32
#include <sys/wait.h>
#endif
#ifndef WIN32
#include <sys/signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#endif
#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <errno.h>

/* The following ifndef clause inserted by JPM on 2/4/98 in */
/* order to try to get compilation occuring without warnings on */
/* windows nt.  However, we may have to remove the inclusion of */
/* stdarg.h instead of vararg.h under NT if it turns out we're */
/* relying on System 5 instead of ANSI C variable argument system */
/* calls */
#ifndef WIN32
#ifdef DMALLOC_DISABLED
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#else
#include <stdarg.h>
#endif

#include <time.h>
#include <string.h>

#ifdef SPARC
#include <sys/uio.h>
#include <unistd.h>
#endif

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif

#define	CONNECTION_OPEN		1
#define CONNECTION_CLOSE	2

extern int errno;
#ifndef LINUX
#ifndef MACOSX
extern char *sys_errlist[];
#endif
#endif
#define MAX_CONNECTIONS  30
#define MAX_BUFSIZE	8192

typedef struct PerConnection {
    int     sfd;            	/* socket file description      */
    int     port;           	/* port number of peer          */
    int     ip;             	/* machine ip address of peer   */
    int	state;			/* connection fsm state		*/

    int     preferredMessageSize;
    int     maximumRecordSize;
    int     search;		/* support search?		*/
    int     present;
    int     deleteResultSet;
    int     resourceReport;
    int     triggerResourceControl;
    int     ResourceControl;
    int     accessControl;
			    /*	Many more to be added.	*/
} PER_CONNECTION;

typedef struct msg_header {
    int message_type;		/* REQUEST or REPLY or ACK	*/
    int service_type;		/* SEARCH, UPDATE, or ADD	*/	
    int data_length;		/* The size of data following the header */ 
} msg_header;


typedef struct destrec {
    char hostname[80];
    int port;
} DESTREC;              /* destination record   */
	 

#define header_size sizeof(struct msg_header);

#include "coninter.h"

#endif  /* CONNECT_H */

