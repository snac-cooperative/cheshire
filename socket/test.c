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
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "connect.h"

int
ConnectToServer(hostname, port)
char	*hostname;	/* Host name or IP address	*/
int 	port;
{
    	char hostnamebuf[80];
    	long rc, i;
    	struct hostent *he;
    	struct sockaddr_in sin;
    	int	sfd;

    	memset((char *)&sin, '\0', sizeof(sin));

    	/* Get the internet address of the server machine. */
    	sin.sin_addr.s_addr = inet_addr(hostname);
#ifdef MIPS
    	if (sin.sin_addr.s_addr != INADDR_NONE) {
		sin.sin_family = AF_INET;
		strcpy(hostnamebuf, hostname);
    	} else {
		if ((he = gethostbyname(hostname)) == NULL) {
        		fprintf(stderr, "Unknown host: %s\n", hostname);
			return -1;
		}
		sin.sin_family = he->h_addrtype;
		bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
		strcpy(hostnamebuf, he->h_name);
    	}
#else
    	if ((he = gethostbyname(hostname)) == NULL) {
        	fprintf(stderr, "Unknown host: %s\n", hostname);
		return -1;
    	}
    	sin.sin_family = AF_INET;
    	memmove((char *)&sin.sin_addr, he->h_addr, he->h_length);
    	strcpy(hostnamebuf, he->h_name);
#endif
    	sin.sin_port = htons(port);
    	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket");
		return -1;
    	}
    	strcpy(hostnamebuf, hostname);

    	/* zero (0) is returned if the connection succeeds. Otherwise
     	* a -1 is returned, and the error code is stored in errno.
     	*/
    	for(i = 0; i < 5; i++) {
		errno = 0;
		rc = connect(sfd, (struct sockaddr *)&sin, sizeof(sin));
		if (rc == 0) {
	    		/* Successful connection, return the fd */
	    		return sfd;
       		}
		/* Retry only if it is interrupted.	*/
		if (errno == EINTR) {
	    		continue;
        	}
		perror("connect");
		/* Otherwise, print out an error message, then exit. */
		fprintf(stdout, "can't connect to host %s at port %d: %s\n",
		   	hostnamebuf, port, strerror(errno));
		close(sfd);
		return -1;
    	}
    	return -1;
}


main(int argc, char **argv)
{
  int portnum;
  int result;

   if (argc < 3) {
     printf ("format: test machineid portnumber\n");
     exit(1);
   }
   
  portnum = atoi(argv[2]);
  
  printf ("attempting machine %s at port %d\n", argv[1], portnum); 
  result = ConnectToServer(argv[1], portnum);
 
  printf ("result is %d\n"); 

}




