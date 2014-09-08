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

int ConnectToServer (char *hostname, int port);
int StartServer (int port, int backlog);
int ConnectToClient (int sfd);
void CloseConnection (int fd);


void CloseClientConnection (int cfd);
void PrintClientUsage ();
void UpdateClientRecord (int cfd);
void MultipleSearchRequest (char *data, int size);
void MultipleSearchConfirm (char *data, int size);
void HandleOneSearchConfirm (int cfd, char *data, int size);
void HandleOneSearchRequest (int cfd, char *data, int size);
int ConnectToGroupServers (struct destrec *drec, int dcnt);
void InitConnectionTable ();
void CloseAllConnections ();


void PrintServerUsage ();
void InitConnectionTable ();
void CloseAllConnections ();
void Abort ();
void CloseClientConnecion (int cfd);
void CloseServerConnection (int sfd);
void UpdateConnectionState (int cfd, int state);
void HandleSearchIndication (int cfd, char *buffer, int size);
void HandleSearchResponse (int cfd, char *buffer, int *size);
/*
void HandleClientConnection (ZSESSION *session, int cfd);
*/

/*
void PrintError(char *format, ...);
void DebugMessage(char *format, ...);
*/
int WriteToSocket(int sfd, char *buf, int nbytes);
int ReadFromSocket(int sfd, char *buf, int nbytes);


