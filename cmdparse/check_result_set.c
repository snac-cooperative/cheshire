/*
 *  Copyright (c) 1990-2000 [see Other Notes, below]. The Regents of the
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
/************************************************************************
*
*       Header Name:    check_result_sets
*
*       Programmer:     Ray Larson
*
*       Purpose:        Check if a string is a resultset name -- used
*                       primarily by the command parser for local index
*                       names.

*       Usage:          int = is_string_result_set (char *string)
*
*       Variables:
*
*       Return Conditions and Return Codes:
*			Returns pointer to RPN structure or NULL on error
*
*       Date:           4/7/2000
*       Version:        1.0
*       Copyright (c) 2000.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

/* session information for searching resultsets */
#include "z3950_3.h"
#include "session.h"

#ifdef CMD_PARSE_CLIENT

extern ZSESSION         *TKZ_User_Session;

/* If this is the client side, then we just look up in the session data */
int 
is_string_result_set (char *searchstring)
{

  /* any resultset name with a colon in it MUST be a resultsetid */
  if (strchr(searchstring,':') != NULL) 
    return 1;

  if (TKZ_User_Session == NULL)
    return 0;

  if (TKZ_User_Session->c_resultSetNames == NULL) 
    return 0;

  return (ac_list_search(TKZ_User_Session->c_resultSetNames, searchstring));
}

#else
#ifdef CMD_PARSE_SERVER

#include <tk.h>

/* If this is the server side, we check the stored result sets */
int 
is_string_result_set (char *searchstring)
{
  char *recnos = NULL;

  Tcl_HashEntry *entry, *se_check_result_set(); 

  entry = se_check_result_set(searchstring, &recnos);

  if (entry == NULL) 
    return 0;

  /* otherwise, something was found */
  return(1);

}

#ifdef NEEDUSERIDS
int
current_user_id (ZSESSION *session) {
  return(0);
}
#endif

#else
/* neither client or server defined, so just say no */
int 
is_string_result_set (char *searchstring)
{
  /* any resultset name with a colon in it MUST be a resultsetid */
  if (strchr(searchstring,':') != NULL) 
    return 1;

  return 0;
}
#endif
#endif



