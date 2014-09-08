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
#include <signal.h>
#include "connect.h"
#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#endif
#ifndef WIN32
#define INVALID_SOCKET -1
#endif

int close();


void
CloseConnection(fd)
int fd;
{
#ifndef WIN32
    	close(fd);
#else
	closesocket(fd);
#endif

}


int 
ConnectToServer(char *hostname, int port)
{
    	char hostnamebuf[80];
    	long rc, i;
    	struct hostent *he;
    	struct sockaddr_in sin;
	int	sfd;

    	memset((char *)&sin, '\0', sizeof(sin));

    	/* Get the internet address of the server machine. */
    	sin.sin_addr.s_addr = inet_addr(hostname);

/* #ifdef MIPS -- the same as for the ALPHA */
#ifdef ALPHA
    	if (sin.sin_addr.s_addr != INADDR_NONE) {
		sin.sin_family = AF_INET;
		strcpy(hostnamebuf, hostname);
    	} else {
		if ((he = gethostbyname(hostname)) == NULL) {
			return -1;
		}
		sin.sin_family = he->h_addrtype;
		bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
		strcpy(hostnamebuf, he->h_name);
    	}
#else
    	if ((he = gethostbyname(hostname)) == NULL) {
#ifdef WIN32
	  i = WSAGetLastError();
	  switch (i) {
	  case WSANOTINITIALISED:
	    fprintf(stderr, "CONNECT.C: A successful WSAStartup must occur before using this function.\n");
	    break;
	  case WSAENETDOWN:
	    fprintf(stderr, "CONNECT.C: The network subsystem has failed.\n");
	    break;
	  case WSAHOST_NOT_FOUND:
	    fprintf(stderr, "CONNECT.C: Authoritative Answer Host not found.\n");
	    break;
	  case WSATRY_AGAIN:
	    fprintf(stderr, "CONNECT.C: Non-Authoritative Host not found, or server failure.\n");
	    break;
	  case WSANO_RECOVERY:
	    fprintf(stderr, "CONNECT.C: Nonrecoverable error occurred.\n");
	    break;
	  case WSANO_DATA:
	    fprintf(stderr, "CONNECT.C: Valid name, no data record of requested type.\n");
	    break;
	  case WSAEINPROGRESS:
	    fprintf(stderr, "CONNECT.C: A blocking Windows Sockets 1.1 call is in progress\n");
	    break;
	  case WSAEFAULT:
	    fprintf(stderr, "CONNECT.C: The name parameter is not a valid part of the user address space.\n");
	    break;
	  case WSAEINTR:
	    fprintf(stderr, "CONNECT.C: A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n");
	    break;
	  }
#endif
		return -1;
    	}
    	sin.sin_family = AF_INET;
		memmove((char *)&sin.sin_addr, he->h_addr, he->h_length);
    	strcpy(hostnamebuf, he->h_name);
#endif
    	sin.sin_port = htons((unsigned short) port);
/*    	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket");
		return -1;
    	}
*/
		if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		perror("create socket");
		return -1;
    	}
		strcpy(hostnamebuf, hostname);

    	/* zero (0) is returned if the connection succeeds. Otherwise
     	* a -1 is returned, and the error code is stored in errno.
     	*/
    	for(i = 0; i < 5; i++) {
#ifndef WIN32
		errno = 0;
#endif
		rc = connect(sfd, (struct sockaddr *)&sin, sizeof(sin));
		if (rc == 0) {
	    		/* Successful connection, return the fd */
	    		return sfd;
       		}
		/* Retry only if it is interrupted.	*/
#ifndef WIN32
		if (errno == EINTR) {
	    		continue;
        	}
#endif
		perror("connect");
		/* Otherwise, print out an error message, then exit. */
#ifndef WIN32
		close(sfd);
#else
		closesocket(sfd);
#endif

		return -1;
    	}
    	return -1;
}

/* Start up a server on the local machine. Create a socket, then bind
 * the new socket to an address.
 * StartServer returns the socket descriptor if successful, -1 to
 * indicate an error has occured. 
 */
int
StartServer(int port, int backlog)
{ 
    	struct sockaddr_in sin;
    	int	sfd;

    	/* Create a new socket for the server.	*/
    	memset((char *)&sin, '\0', sizeof(sin));

    	if ((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror ("create socket");
		return -1;
    	}

    	/* Bind the new socket to the server.	*/
    	sin.sin_family = AF_INET;
    	sin.sin_addr.s_addr = htonl(INADDR_ANY);
    	sin.sin_port = htons((unsigned short) port);

    	if (bind(sfd, (struct sockaddr*)&sin, sizeof(struct sockaddr)) < 0) {
		perror ("bind address");
#ifndef WIN32
		close(sfd);
#else
		closesocket(sfd);
#endif
		return -1;
    	}


#ifdef MIPS
    	signal(SIGCHLD, CleanUp);
#endif

#ifdef SPARC
    	signal(SIGCHLD, SIG_IGN);
#endif

#ifdef ALPHA
    	signal(SIGCHLD, SIG_IGN);
#endif

    	/* Listen to new connection request.	*/
    	if (listen(sfd, backlog) < 0) {
		perror ("listen to");
#ifndef WIN32
		close(sfd);
#else
		closesocket(sfd);
#endif
		return -1;
    	}
    	return sfd;
}

#ifdef WIN32
/* Start up a server on the local machine. Create a socket, then bind
 * the new socket to an address ALREADY IN USE.
 * StartServer returns the socket descriptor if successful, -1 to
 * indicate an error has occured. 
 */
int
ReStartServer(int port, int backlog)
{ 
  extern FILE *LOGFILE;
  struct sockaddr_in sin;
  int	sfd;
  int     optval = 1;
  struct linger lingerparm;

  /* Create a new socket for the server.	*/
  memset((char *)&sin, '\0', sizeof(sin));
  
  if ((sfd = socket(AF_INET,SOCK_STREAM,0)) == SOCKET_ERROR){
    fprintf (LOGFILE, "Create socket error: %d\n", WSAGetLastError());
    return SOCKET_ERROR;
  }
  
  /* Bind the new socket to the server.	*/
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons((unsigned short) port);
  
  /* set up the port for reuse */
  if ((setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)))
       == SOCKET_ERROR) {
    fprintf (LOGFILE, "Reuse not set: %d\n", WSAGetLastError());
    closesocket(sfd);
    return SOCKET_ERROR;
  } 
  /* set up for hard close */
  lingerparm.l_onoff = 1;
  lingerparm.l_linger = 0;
  if ((setsockopt(sfd, SOL_SOCKET, SO_LINGER, &lingerparm, sizeof(struct linger)))
       == SOCKET_ERROR) {
    fprintf (LOGFILE, "linger not set: %d\n", WSAGetLastError());
    closesocket(sfd);
    return SOCKET_ERROR;
  } 
  
  if (bind(sfd, (struct sockaddr*)&sin, sizeof(struct sockaddr)) 
      == SOCKET_ERROR) {
    fprintf (LOGFILE, "Bind socket error: %d\n", WSAGetLastError());
    closesocket(sfd);
    return SOCKET_ERROR;
  }
  
  /* Listen to new connection request.	*/
  if (listen(sfd, backlog) < 0) {
    fprintf (LOGFILE, "Bind socket error: %d\n", WSAGetLastError());
    closesocket(sfd);
    return SOCKET_ERROR;
  }
  return sfd;
}
#endif

int
ConnectToClient(sfd)
int	sfd;
{
    	struct sockaddr_in sin;
    	int	cfd;
    	int sinlen;

    	sinlen = sizeof(sin);
    	do {
#ifndef WIN32
		errno = 0;
#endif
		cfd = accept(sfd, (struct sockaddr *)&sin, &sinlen);
		/* Try again if the call was interrupted. */ 
#ifndef WIN32
    	} while ((cfd < 0) && (errno == EINTR));
#else
    	} while ((cfd < 0));
#endif
    	if (cfd < 0) {
		perror ("accept connection");
#ifndef WIN32
		close(cfd);
#else
		closesocket(cfd);
#endif
		return -1;
    	}
    	return cfd;
}

int
ClientStatus (socket, buffer)
int socket;
char *buffer;
{
  struct sockaddr_in Name;
  int NameLen ;
  struct hostent *Peer;

  NameLen = sizeof(struct sockaddr);
  getpeername(socket, (struct sockaddr *)&Name, &NameLen);

  Peer = gethostbyaddr((char *)&Name.sin_addr, 4, AF_INET);
  if (Peer != NULL) {
    sprintf(buffer,"%s [%s]", Peer->h_name, inet_ntoa(Name.sin_addr));
  }
  else {
    sprintf(buffer,"[%s]", inet_ntoa(Name.sin_addr));

  }
  return(0);
  
}


