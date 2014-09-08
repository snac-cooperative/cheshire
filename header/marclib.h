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

/************************************************************************
*
*	Header Name:	marclib.h
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	definitions for libmarc.a library routines
*                       and marc.h and memcntl.h include files
*	Usage:		#include "marclib.h"
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/29/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "marc.h"


/*********************************************************************/
/* GetNum converts a specifed number of characters to a number       */
/*********************************************************************/
extern int GetNum(char *s, int n);


/*********************************************************************/
/* SetSubF  - attaches  a list of subfield records to a field record */
/*            returns NULL if there is no more memory                */
/*********************************************************************/
extern int SetSubF(MARC_FIELD *f, char *dat);

/*********************************************************************/
/* SetField - installs a marc field into a field processing structure*/
/*            returns null if there are problems with allocation     */
/*********************************************************************/
extern MARC_FIELD *SetField(MARC_REC *rec, MARC_DIRENTRY_OVER *dir);

/*********************************************************************/
/* ReadMARC - reads a marc record into a buffer and returns          */
/*            the length of the record - also appends a NULL to the  */
/*            end the buffer                                         */
/*           Returns 0 at end-of-file                                */
/*********************************************************************/
extern int ReadMARC (int file, char *buffer, int buffsize);

/*********************************************************************/
/* SeekMARC - uses offsets in an associator file to position the     */
/*            MARC file to a given logical record number -           */
/*            the associator file and MARC files must be open        */
/*            returns -1 on seek errors and the marc file position   */
/*            when successful.                                       */
/*********************************************************************/
extern int SeekMARC(int marcfile, int assocfile, int recnumber);

/*********************************************************************/
/* GetMARC - Moves a marc record into the marc processing structure  */
/*           and returns a pointer to the structure                  */
/*           if the copy flag is not zero a new_var record buffer is     */
/*           allocated and the input buffer copied to it             */
/*********************************************************************/
extern MARC_REC *GetMARC(char *buffer, int bufferlength, int copy);

/*********************************************************************/
/* fieldcopy  -  copy string From to string To stopping when either  */
/*            terminal null character, or a field terminator         */
/*            is encountered in the From string.                     */
/*            returns number of chars copied                         */
/*********************************************************************/
extern void fieldcopy(char *To, char *From);

/*********************************************************************/
/* codeconvert  - convert subfield codes and other non-print chars   */
/*                in a null terminated string                        */
/*********************************************************************/
extern void codeconvert(char *string);


/*********************************************************************/
/* charconvert  - convert subfield codes and other non-print chars   */
/*                normal characters are returned unchanged           */
/*********************************************************************/
extern char charconvert(char c);

/*********************************************************************/
/* subfcopy  -  copy string From to string To stopping when either   */
/*            terminal null character, subfield delim or a field     */
/*            terminator is encountered in the From string.          */
/*            returns number of chars copied                         */
/*            if flag is 1, non-ascii characters are removed         */
/*********************************************************************/
extern int subfcopy(char *To, char *From, int flag);

/********************************************************************/
/* tagcmp - compare two marc tags with X or x as wildcards.         */
/*          returns -1 for no match and 0 for match                 */
/********************************************************************/
extern int tagcmp(char *pattag, char *comptag);

/*********************************************************************/
/* GetField - extract a field with a given tag from a marc           */
/*            processing structure. Returns a pointer to the         */
/*            field and copies the field to the buffer(if provided)  */
/*            successful. Returns NULL if it fails to find the       */
/*            field. Startf lets it start from later in a field list.*/
/*            Permits "wildcard" comparisons using tagcmp            */
/*********************************************************************/
extern MARC_FIELD *GetField (MARC_REC *rec, 
		      MARC_FIELD *startf, 
		      char *buffer,
		      char *tag);
     

/*********************************************************************/
/* GetSubf - get a named subfield from a marc processing structure   */
/*           for a given field. returns a pointer to the subfield    */
/*           and copies it to a buffer (if provided) when successful */
/*           Returns NULL if it fails to find the subfield           */
/*********************************************************************/
extern MARC_SUBFIELD *GetSubf (MARC_FIELD *f,
			char *buffer,
			char code);

/************************************************************************/
/* NORMALIZE -- convert an LC call number to a standardized format      */
/*              returns NULL if call number is abnormal, out otherwise  */
/************************************************************************/
extern char *normalize(char *in, char *out);

/************************************************************************/
/* FreeMARC_REC -- free the space allocated for a marc record structure */
/*              and all sub-structures                                  */
/************************************************************************/
int FreeMARC_REC(MARC_REC *in);



