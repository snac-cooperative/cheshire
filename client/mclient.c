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
 * Ignore all routines except for those search programs in this file.
 */
#include <sys/types.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>

#endif
#include "connect.h"
#include "z3950_3.h"
#include "ccl.h"
#include "z_parameters.h"
#include "list.h"

PER_CONNECTION	ClientConnections[MAX_CONNECTIONS];
int	CurConnection; 
#ifndef WIN32
int select(
          int nfds,
          fd_set *readfds,
          fd_set *writefds,
          fd_set *exceptfds,
          struct timeval *timeout) ;
#endif
fd_set	/* rdfds, wrfds,  */ acfds;
int	nfds;
int	mfd;			/* maximum fd	*/

int OneInitRequest(int fd);
int OneInitConfirm(int fd);
int MultipleInitRequests();
void MultipleInitConfirms();

int OnePresentRequest(int fd, char *, int start, int number);
void MultiplePresentRequests(char *, int, int);
void MultiplePresentConfirms();

int MultipleSearchRequests(ZASSOCIATION *, char *, char *);
int MultipleSearchConfirms(ZASSOCIATION *);
int OneSearchConfirm(ZSESSION *);
int OneSearchRequest(ZSESSION *, char *, char *);
int ExecuteParallelSearch (ZASSOCIATION *, char *, char *);

void ExecuteDisconnect();

int MultipleConnectRequests(DESTREC *, int);
void OnePresentConfirm(int fd);
ZSESSION *LookupAssociation(ZASSOCIATION *, int);
PDU *MakeResourceControlResponse(char *, Boolean, Boolean);
int ProcessSearchResponse (ZSESSION *, SearchResponse *);
int ClearPreviousSearch (ZSESSION *);
char **StringToArgv (char *); 
Query *MakeQueryType101  (RPNQuery *, char *);
void FreeArgv(char **);

int
OneInitRequest(fd)
int 	fd;
{
    PDU *client_pdu;
    DBUF *client_buf;

    client_pdu =  MakeInitializeRequest(NULL,
			"11",
			"1110000",
			8192,
			16*1024,
			NULL,
			"1995",		/* ID	*/
			"Z3950 CLIENT",	/* NAME	*/
			"0.1",		/* VERSION */
			NULL);
    client_buf = EncodePDU2(client_pdu);
    TransmitPDU(fd, client_buf);
    fprintf(stderr, "sending Init Request to server: %d\n", fd);
    return 1;
}

int
OneInitConfirm(fd)
int fd;
{
    PDU *server_pdu;

    server_pdu = GetPDU(fd, 300); /* timeout should be a parameter */
    if (server_pdu->which != e1_initResponse) {
	fprintf(stderr, "Didn't receive the initialization response from %d.\n", fd);
	return -1;
    }
    PrintPDU(stdout,server_pdu);
    return (int)server_pdu->u.initResponse->result;
}

/*
 * Send the same initialization request to multiple servers.
 */
int
MultipleInitRequests()
{
    int	i;
    int sfd;		/* socket file descriptor */
    fd_set	wfds;	/* Write FD_SET	*/

    FD_ZERO(&wfds);
    memmove((char *)&wfds, (char *)&acfds, sizeof(wfds));
    for (i=0; i<nfds; i++) {
	sfd = ClientConnections[i].sfd;
	if (FD_ISSET(sfd, &wfds))
	    OneInitRequest(sfd);
    }	
    return 1;
}

/* Note on select() return code:
 * select returns the number of ready file descriptors if successful,
 * 0 if the time limit was reached, and
 * -1 to indicate that an error has occurred. When an error occurs,
 * the global variable errno contains one of the following values:
 *
 * EBADF	One of the descriptor sets specifies an invalid descriptor.
 * EINTR	A signal arrived before the time limit or any of the
 *		selected descriptors became ready.
 * EINVAL	The time limit value is incorrect.
 */

void
MultipleInitConfirms()
{
#ifndef WIN32
    int select();
#endif
    int	fd;
    int	numfds = nfds;
    int	maxfd = mfd+1;
    fd_set	rfds;
    fd_set	irfds;		/* The outstanding connections.	*/	
    int sc;
    struct timeval timeout;

    FD_ZERO(&irfds);
    memmove((char *)&irfds, (char *)&acfds, sizeof(rfds));
    while (numfds) {
	timeout.tv_sec = 100;
	timeout.tv_usec = 0;
    	FD_ZERO(&rfds);
    	memmove((char *)&rfds, (char *)&irfds, sizeof(rfds));
	sc = select(maxfd, &rfds, (fd_set *)0, (fd_set *)0, 
		   (struct timeval *)&timeout);
	if (sc == 0)	/* Time out	*/
	    continue;
	if (errno == EINTR)	/* Be interruped.	*/
	    continue;

	if (sc < 0) {
	    printf("can't select: %s", strerror(errno));
	}

	fprintf(stderr, "%d is ready for reading.\n", sc);

	/* some connections have incoming data to read. */
	for (fd=0; fd < maxfd; fd++) {
	    if (FD_ISSET(fd, &rfds)) {
		OneInitConfirm(fd);

		/* Just read in the data, clear fd bit. */
		FD_CLR(fd, &irfds);
		numfds--;
	    }
	}
    }
    return;
}

int 
OnePresentRequest(fd, resultSetId, start, number)
int	fd;
char	*resultSetId;
int	start;
int	number;
{
    PDU *pdu;
    DBUF *buf;

    pdu = MakePresentRequest(NULL, resultSetId, start, number, "F", 
			     OID_USMARC, NULL);
    buf = EncodePDU2(pdu);
    return TransmitPDU(fd, buf);
}

void
MultiplePresentRequests(resultSetId, startPoint, numberOfRecordsRequested)
char 	*resultSetId;
int	startPoint; 
int	numberOfRecordsRequested;
{
    int	i;
    int sfd;		/* socket file descriptor */
    fd_set	wfds;	/* Write FD_SET	*/

    FD_ZERO(&wfds);
    memmove((char *)&wfds, (char *)&acfds, sizeof(wfds));
    for (i=0; i<nfds; i++) {
	sfd = ClientConnections[i].sfd;
	if (FD_ISSET(sfd, &wfds))
	    OnePresentRequest(sfd, resultSetId, startPoint, numberOfRecordsRequested);
    }	
    return;
}

void
MultiplePresentConfirms()
{
    int	fd;
    int	numfds = nfds;
    int	maxfd = mfd+1;
    fd_set	rfds;

    FD_ZERO(&rfds);
    memmove((char *)&rfds, (char *)&acfds, sizeof(rfds));
    while (numfds) {
	if (select(maxfd, &rfds, (fd_set *)0, (fd_set *)0, 
		   (struct timeval *)0 ) < 0) {
	    printf("can't select: %s", strerror(errno));
	}

	for (fd=0; fd < maxfd; fd++) {
	    if (FD_ISSET(fd, &rfds)) {
		OnePresentConfirm(fd);
		FD_CLR(fd, &rfds);
		numfds--;
	    }
	}
    }
    return;
}

int
MultipleSearchRequests (assocs, queryString, resultSetName)
ZASSOCIATION *assocs;
char	*queryString;
char	*resultSetName;
{
    ZSESSION *session;

    if (assocs == NULL)
	return Z_ERROR;
    if (!queryString) {
	printf("empty query!\n");
	return Z_ERROR;
    }
    for (session=assocs->head; session!=NULL; session=session->next) {
	if (session->parallelSearch == Z_TRUE) {
            if (OneSearchRequest(session, queryString, resultSetName) == Z_OK)
	        session->state = S_SEARCH_SENT;
	    else
	        session->state = S_OPEN;
	}
    }
    return Z_OK;
}

int
MultipleSearchConfirms(assocs)
ZASSOCIATION *assocs;
{
    int	fd;
    int	numfds;
    int	maxfd;
    fd_set	rfds;
    fd_set	irfds;		/* The outstanding read fd_set.	*/	
    int code;
    struct timeval timeout;
    ZSESSION *session;

    if (assocs == NULL)
	return Z_ERROR;

    maxfd = assocs->maximum_fd + 1;
    FD_ZERO(&irfds);
    numfds = 0;
    for (session=assocs->head; session!=NULL; session=session->next) {
	if ((session->parallelSearch == Z_TRUE) && 
	    (session->state == S_SEARCH_SENT)) {
            FD_SET(session->fd, &irfds);
	    numfds++;
	}
    }
    while (numfds) {
	timeout.tv_sec = 100;
	timeout.tv_usec = 0;
    	FD_ZERO(&rfds);
    	memmove((char *)&rfds, (char *)&irfds, sizeof(rfds));
	code = select(maxfd, &rfds, (fd_set *)0, (fd_set *)0, 
		   (struct timeval *)&timeout);
	if (code == 0)
	    continue;
	if (errno == EINTR)	/* interrupted.	*/
	    continue;

	if (code < 0) {
	    printf("can't select: %s", strerror(errno));
	}

	/* some connections have incoming data to be read. */
	for (fd=0; fd < maxfd; fd++) {
	    if (FD_ISSET(fd, &rfds)) {
		session = LookupAssociation (assocs, fd);
		if (OneSearchConfirm(session) == Z_OK) {
		    session->state = S_SEARCH_RECEIVED;
		}
		/* Just read in the data, clear fd bit. */
		FD_CLR(fd, &irfds);
		numfds--;
	    }
	}
    }
    return Z_OK;
}


/* Read in datalen bytes of data from cfd connection. 
 */
int
OneSearchConfirm(session)
ZSESSION *session;
{
    int fd;
    PDU *in_pdu, *out_pdu;

    if (session == NULL)
	return Z_OK;
    fd = session->fd;
    do {
    	if ((in_pdu = GetPDU(fd, session->c_timeOut)) == NULL) {
	    fprintf(stderr, "Didn't get a search response pdu.\n");
	    return Z_ERROR;
    	}
 	if (in_pdu->which  == e1_resourceControlRequest) {	
	    PrintPDU(stdout,in_pdu); 
	    if (in_pdu->u.resourceControlRequest->responseRequired != 0) {
	        out_pdu = MakeResourceControlResponse(NULL, 1, 1);
	        PrintPDU(stdout,out_pdu); 
  	        PutPDU (fd, out_pdu);
    	        (void) FreePDU (out_pdu);
	    }
	} 
    } while (in_pdu->which != e1_searchResponse);

    if (ProcessSearchResponse (session, in_pdu->u.searchResponse) == Z_OK)
	fprintf(stderr, "search response processed normally.\n");
    else 
	fprintf(stderr, "Error in processing search response.\n");
    (void) FreePDU (in_pdu);
    return Z_OK;
}

int
OneSearchRequest(session, queryString, resultSetName)
ZSESSION *session;
char	*queryString;
char	*resultSetName;
{
    int fd;
    PDU *out_pdu;
    char **databaseNames;
    Query *query;
    RPNQuery *RPN_Query_ptr, *queryparse();


    if (!session)
	return Z_ERROR;
    if (!queryString) {
	printf("empty query!\n");
	return Z_ERROR;
    }
    if (!resultSetName) {
	printf("empty resultSetName!\n");
	return Z_ERROR;
    }
    ClearPreviousSearch (session);
    if (strlen(resultSetName) >= MAX_RSN_LENGTH)
	strncpy(session->resultSetName, resultSetName, MAX_RSN_LENGTH-1);
    else
	strcpy(session->resultSetName, resultSetName);
    session->queryString = (char *)strdup(queryString);
    fd = session->fd;		/* file descriptor	*/
    databaseNames = StringToArgv (session->databaseName);

    RPN_Query_ptr = queryparse(queryString, session->c_attributeSetId);
    query = MakeQueryType101(RPN_Query_ptr, session->c_attributeSetId);

    out_pdu = MakeSearchRequest(
		NULL, 
		SMALL_SET_UPPER_BOUND,
		LARGE_SET_LOWER_BOUND,
		MEDIUM_SET_PRESENT_NUMBER,
		Z_TRUE,
		resultSetName,
		databaseNames,
		NULL,
		NULL,
		OID_USMARC,
		query, NULL);
    if (PutPDU (fd, out_pdu) != Z_OK) 
	return Z_ERROR;
    (void) FreePDU (out_pdu);
    (void) FreeArgv(databaseNames);
    fprintf(stdout, "sending search request to host: %s\n", 
	session->s_serverMachineName);
    return Z_OK;
}

int
ExecuteParallelSearch(assocs, queryString, resultSetName)
ZASSOCIATION	*assocs;
char	*queryString;	/* Query in common command language.	*/
char	*resultSetName;
{
    int code;

    if (!assocs)
	return Z_ERROR;
    if (!queryString) {
	printf("empty search query!\n");
	return Z_ERROR;
    }
    /* Send off the search request.
     */
    if (MultipleSearchRequests(assocs, queryString, resultSetName) != Z_OK)
    	return Z_ERROR;
    /* Wait for search responses.
     */
    code = MultipleSearchConfirms(assocs);
    return code;
}

void
ExecuteDisconnect()
{
    CloseAllConnections();
}

int
MultipleConnectRequests(drec, numberServers)
DESTREC *drec;
int	numberServers;
{
    CurConnection = 0; 
    InitConnectionTable();
    ConnectToGroupServers(drec, numberServers);
    MultipleInitRequests();
    MultipleInitConfirms();
    return 1;
}

void
OnePresentConfirm(fd)
int fd;
{
    PDU *pdu;

    if ((pdu = GetPDU(fd, 300)) == NULL) { /* timeout should be parameter */
	fprintf(stderr, "Failed to get a search response pdu.\n");
	return;
    }
    PrintPDU(stdout,pdu);
    return;
}


void
InitConnectionTable()
{
    int i;
 
    for (i=0; i<MAX_CONNECTIONS; i++) {
        ClientConnections[i].state = CONNECTION_CLOSE;
        ClientConnections[i].port = 0;
        ClientConnections[i].sfd = 0;
    }
    CurConnection = 0;
}

void
CloseAllConnections()
{
    int i;
 
    for (i=0; i<MAX_CONNECTIONS; i++) {
        if (ClientConnections[i].sfd != 0)
            close(ClientConnections[i].sfd);
    }
    CurConnection = 0;
}
 
/*
 * Open multiple connections to a group of servers. One connection
 * per server.
 */
int
ConnectToGroupServers(drec, dcnt)
struct destrec  *drec;
int     dcnt;
{
    int i;
    int         cfd;
 
    nfds = 0;
    FD_ZERO(&acfds);
    CurConnection = 0;
 
    /* open a connection to each server */
    for (i=0; i<dcnt; i++) {
        fprintf(stderr, "trying... %s ... %d\n",drec[i].hostname, drec[i].port);
        cfd = ConnectToServer(drec[i].hostname, drec[i].port);
        if (cfd < 0)
            continue;
        else {
            ClientConnections[nfds].sfd = cfd;
            ClientConnections[nfds].port = drec[i].port;
            ClientConnections[nfds].state = CONNECTION_OPEN;
            FD_SET(cfd, &acfds);
            nfds++;
            if (cfd > mfd)
                mfd = cfd;
            CurConnection++;
        }
    }
    return CurConnection;
}

