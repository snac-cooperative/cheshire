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
 *  Install error info into a session structure
 */
#include "list.h"
#include "hash.h"
#include "z3950_3.h"
#include "session.h"

#define	MAX_OID_LENGTH	100	/* Maximum number of chars of oid string.*/	
extern CopyOidStr (char *, ObjectIdentifier);
extern char *diagnostic_string(int number);
extern char *diagnostic_addinfo();
extern void diagnostic_set(int number, int add_num, char *add_str);


extern struct diagMessage {
	int errorCode;
	char *errorMessage;
} DIAG_MESSAGE[];

int ac_list_search(AC_LIST *list, char *item);

void
ClearError (session)
ZSESSION *session;
{
	if (session == (ZSESSION *)NULL)
	    return;
	session->status = Z_OK;
}

void
SetError (session, condition, meaning, addinfo)
ZSESSION	*session;
int	condition;
char	*meaning;
char	*addinfo;
{
	if (session == (ZSESSION *)NULL)
		return;
	session->status = Z_ERROR;
	diagnostic_set(condition,0,addinfo);
	session->errorCode = condition;
	session->errorMessage[0] = '\0';
	session->errorAddInfo[0] = '\0';
	strncpy(session->errorMessage, diagnostic_string(condition),
		MAX_EM_LENGTH-1);
	if (addinfo != (char *)NULL) {
		if (strlen(addinfo) >= MAX_EM_LENGTH)
			strncpy(session->errorAddInfo, addinfo,MAX_EM_LENGTH-1);
		else
			strcpy(session->errorAddInfo, addinfo);
		session->errorAddInfo[MAX_EM_LENGTH-1] = '\0';
	}
}

char *
errMessage(condition)
int condition;
{
  return(diagnostic_string(condition));
}	

void
SetError2(session, condition, addinfo)
ZSESSION	*session;
int	condition;
char	*addinfo;
{
	char	*meaning;

	if (session == (ZSESSION *)NULL)
		return;
	session->status = Z_ERROR;
	session->errorCode = condition;
	diagnostic_set(condition,0,addinfo);
	meaning = errMessage(condition);
	session->errorMessage[0] = '\0';
	session->errorAddInfo[0] = '\0';
	if (meaning != (char *)NULL) {
		if (strlen(meaning) >= MAX_EM_LENGTH)
			strncpy(session->errorMessage, meaning,MAX_EM_LENGTH-1);
		else
			strcpy(session->errorMessage, meaning);
		session->errorMessage[MAX_EM_LENGTH-1] = '\0';
	}
	if (addinfo != (char *)NULL) {
		if (strlen(addinfo) >= MAX_EM_LENGTH)
			strncpy(session->errorAddInfo, addinfo,MAX_EM_LENGTH-1);
		else
			strcpy(session->errorAddInfo, addinfo);
		session->errorAddInfo[MAX_EM_LENGTH-1] = '\0';
	}
}

