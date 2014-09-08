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
 * This file handles the reading and writing for JServer
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#endif

#include "zprimitive.h"
#include "connect.h"

int debug = 0;
WHOAMI	context = Z_SERVER;	/* client should set it to Z_CLIENT	*/

char *GetErrorMsg(int errorid);


extern FILE *LOGFILE; /* this should be set in the main program */
#define DEFAULTBUFFERSIZE 1000000

/*
 * A client program waits for reading for at most two minutes,
 * and a server program waits for reading for at most ten minutes before
 * close the connection. The easy approach to timeout is to shut down the
 * connection instead of retrying.  The timeout values are arbitrarily
 * chosen. They should be an entry for timeout in the client and server
 * initialization files.
 */

int 
WaitForData (fd, timeoutsecs)
int fd;
int timeoutsecs;
{

  int code;
  if (context == Z_CLIENT)  {	
    if ((code = TimeOut(fd, timeoutsecs)) <= 0) {
#ifdef WIN32
      closesocket(fd);
#else
      close(fd);
#endif
      if (LOGFILE == NULL)
	fprintf(stderr, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);
      else
	fprintf(LOGFILE, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);

      return -1;
    } else {
      return code;
    }
  } else if (context == Z_SERVER) {	
    if ((code = TimeOut(fd, timeoutsecs)) <= 0) {
#ifdef WIN32
      closesocket(fd);
#else
      close(fd);
#endif
      if (LOGFILE == NULL)
	fprintf(stderr, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);
      else
	fprintf(LOGFILE, "Client has shut down the connection to %d due totimeout or connection error.\n", fd);
      return -1;
    } else {
      return code;
    }
  } else {
    if (LOGFILE == NULL)
      fprintf(stderr, "Unknown context in waiting for data\n");
    else
      fprintf(LOGFILE, "Unknown context in waiting for data\n");    
    return -1;
  } 
  return code;
}

/* Read a command into a buffer */
char *
GetCommandFromSocket(int fd, int *size, int timeoutsecs)
{
  char *buffer, *bstart, *returnbuf=NULL;
  int nbytesRead=0;
  char inchar = 0;
  
  if (WaitForData(fd, timeoutsecs) <= 0)
    return NULL;
  
  bstart = buffer = CALLOC(char, DEFAULTBUFFERSIZE);

  while (inchar != '\n') {
#ifdef WIN32
    nbytesRead += recv(fd, &inchar, 1, 0);
#else
    nbytesRead += read(fd, &inchar, 1);
#endif
    if (nbytesRead <= 0) {
      if (errno == EINTR)
	continue;
      else
	return NULL;
    }
    if (inchar != '\n' && inchar != '\r' && nbytesRead < DEFAULTBUFFERSIZE)
      *buffer++ = inchar;
  }
  *size = nbytesRead;
  returnbuf = strdup(bstart);
  FREE(bstart);
  return returnbuf;

}

int
PutResultsToSocket(int fd, char *results, int resultcode)
{
  char errorout[500];
  char *errmsg;

  if (resultcode == 0) {
    if (results == NULL)
      return 0;
    /* we are sending back a normal response, ready to go */
    WriteToSocket(fd, results, strlen(results));
  }
  else {/* get the error message and send it back */
    errmsg = GetErrorMsg(resultcode);
    sprintf(errorout, "%08d%c %s", strlen(errmsg)+2, 'E', errmsg);
    WriteToSocket(fd, errorout, strlen(errorout));
  }

}



