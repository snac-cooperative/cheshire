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
#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "connect.h"

#ifdef WIN32
#include <io.h>
#include <winsock.h>
#include <windows.h>
#define write _write
#endif

#ifndef WIN32
extern	int	errno;
#endif

/* read() man page says:
 * read() returns zero if it detects an end-of-file condition on the socket,
 * the number of bytes read if it obtains input, and -1 to indicate that
 * an error has occurred. When an error occurs, the global variable errno
 * is set.
 */
/* write() man page says:
 * write() returns the number of bytes transferred if successful and -1 to
 * indicate that an error has occurred.
 */


/* If read succeeds, returns the number of bytes read.
 * returns zero if the socket connection is closed,
 * -1 if an error has occured.
 */
int
ReadFromSocket(fd, buf, nbytes)
int	fd;
char	*buf;		/* buf to store incoming data */
int	nbytes;		/* number of bytes wanted. */
{
  int	nbytesRead; 
  int	total = nbytes;
  
  while (nbytes > 0) {
#ifndef WIN32
    nbytesRead = read(fd, buf, nbytes);
#else
    nbytesRead = recv(fd, buf, nbytes, 0);
#endif
    if (nbytesRead < 0)
      perror("read");
    if (nbytesRead <= 0) {    
#ifndef WIN32
      if (errno == EINTR)
	continue;
      else
#endif
	return nbytesRead;
    }
    nbytes -= nbytesRead;
    buf += nbytesRead;
  } 
  return total - nbytes;
}

int
WriteToSocket(fd, buf, nbytes)
int	fd;
char	*buf;
int	nbytes;
{
  int	nbytesWritten; 
  int	left;		/* number of bytes to be written to socket. */
  
  left = nbytes;
  while (left > 0) {
#ifndef WIN32
    nbytesWritten = write(fd, buf,left);
#else
    nbytesWritten = send(fd, buf, left, 0);
#endif
    if (nbytesWritten < 0) {
      perror("write");
    }
    if (nbytesWritten <= 0) {
#ifndef WIN32
      if (errno == EINTR)
	continue;
      else
#endif
	return nbytesWritten;
    }
    left -= nbytesWritten;
    buf += nbytesWritten;
  } 
  return nbytes - left;
}

#ifndef WIN32
#ifdef DMALLOC_DISABLED
void
PrintError(format, va_alist)
char	*format;
va_dcl
{
    	va_list	args;
    
    	va_start(args);
    	vfprintf(stderr, format, args);
    	va_end(args);
    	exit(1);
}

void
DebugMessage(format, va_alist)
char	*format;
va_dcl
{
    	va_list args;

    	va_start(args);
    	vfprintf(stdout, format, args);
    	va_end(args);
    	fflush(stdout);
}
#else
void
PrintError(char *format, ...)
{
    	va_list	args;
    
    	va_start(args,format);
    	vfprintf(stderr, format, args);
    	va_end(args);
    	exit(1);
}
void
DebugMessage(char *format, ...)
{
    	va_list args;

    	va_start(args, format);
    	vfprintf(stdout, format, args);
    	va_end(args);
    	fflush(stdout);
}
#endif
#else
void
PrintError(char *format, ...)
{
    	va_list	args;
    
    	va_start(args,format);
    	vfprintf(stderr, format, args);
    	va_end(args);
    	exit(1);
}
void
DebugMessage(char *format, ...)
{
    	va_list args;

    	va_start(args, format);
    	vfprintf(stdout, format, args);
    	va_end(args);
    	fflush(stdout);
}
#endif

#ifdef MIPS
void
CleanUp()
{
    	union wait  status;
    	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		;
}
#endif




/* Wait for incoming data. TimeOut returns 0 if times out, it returns
 * a positive value if there is some data to read, otherwise it returns
 * -1 to indicate that an error has occurred.
 */
int 
TimeOut(fd, howLong)
int	fd;
int	howLong;	/* how long to wait (in seconds)	*/
{
    	struct timeval timeout;
    	fd_set rfdset;
    	int    code;

	timeout.tv_sec = howLong;
	timeout.tv_usec = 0;

    	FD_ZERO(&rfdset);
    	FD_SET(fd, &rfdset);

    	code = select(fd+1, &rfdset, NULL, NULL, &timeout);
	if (code < 0) {
#ifndef WIN32
		if (errno == EINTR) 	/* interruption	*/
			printf("interruption.");
#endif
		perror("select");
	}	
    	return code;
}

