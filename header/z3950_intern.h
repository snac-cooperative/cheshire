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
#ifndef	Z3950_INTERN_H
#define	Z3950_INTERN_H

#include "ccl.h"

extern BINARY *prefix_to_binary (char **);
extern Query *PrefixToInfix (BINARY *);
extern void *FreeBinaryTree (BINARY *);
extern void PrintQuery (Query *);
extern char **StringToArgv(char *);
extern void FreeArgv(char **);
extern void PrintArgv (char **);
extern int SearchArgv  (char **, char *);
extern Query *MakeQueryType1(RPNQuery *, char *);
extern Query *MakeQueryType2(char *);
extern Query *MakeQueryType100(char *);
extern Query *MakeQueryType101(RPNQuery *, char *);
extern Query *MakeQueryType102(char *);
extern Query *MakeQueryType0(char *);
extern ElementSetNames *MakeElementSetNames (char **, char **);

extern PDU * MakeResourceControlRequest(char *, Boolean, ResourceReport *, int, Boolean, Boolean);
extern PDU *MakeResourceControlResponse(OctetString, Boolean, Boolean);
extern PDU *MakeAccessControlResponse(OctetString, char *);
extern PDU *MakeTriggerResourceControlRequest(char *, int, char *, Boolean);
extern PDU *MakeResourceReportRequest(char *, char *);
extern PDU *MakeCloseResponse(OctetString referenceId, int closeReason, 
			      char *diagnosticInfo, 
			      char *resourceReportFormat, 
			      ResourceReport *resourceReport);

extern int tk_disconnect(ZSESSION *);
extern ZSESSION *tk_connect (char *, int, GSTR*);
extern void tk_delete(char *);
extern int tk_ResourceReport(char *, char *);
extern int tk_TriggerResourceControlRequest (char *, int, char *, int);
extern int tk_present(ZSESSION *, char *, int, int, void *, ObjectIdentifier);
extern int tk_search(ZSESSION *, int, char *, char *, char *);

extern int mr_field (char *, char *, char *);
extern AC_LIST *parseMarc (char *);

extern void SetError2 (ZSESSION *, int, char *);
extern void ClearError (ZSESSION *);
extern RECORD_LISTS *RetrieveRecords2 (ZSESSION *, char *, int, int, int);
extern Records *MakeDatabaseRecords2 (RECORD_LISTS *, char *);
extern int SearchElementSetNames (char *, char *, int);
extern void FreeRecordLists (RECORD_LISTS *);
extern NamePlusRecord *MakeNamePlusRecord(char *, int, void *);

InternationalString NewInternationalString(char *in);


ResourceReport *MakeResourceReport (char *);
ResourceReport *ExpandResourceReport(ResourceReport  *, int, int, int);
PDU *MakeResourceReportResponse(OctetString, int, ResourceReport *);
PDU *MakeAccessControlRequest(char *, char *);

extern char *GetOidStr (ObjectIdentifier);
extern char *GetInternationalString (InternationalString);
extern int make_dirname (ZSESSION *, char *);
extern char *GetOctetString(OctetString o);
extern ObjectIdentifier NewOID(char *string);

#endif








