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
*       Header Name:    tagsets.h
*
*       Programmer:     Ray Larson
*
*       Purpose:        Data structures tagsets G and M for use in GRS-1
*                       formatting of records.
*
*       Return Conditions and Return Codes:
*
*       Date:           7/8/98
*       Version:        1.0
*       Copyright (c) 1998.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#ifndef TAGSETSDEF
#define TAGSETSDEF


typedef struct tagset_elements {
  char *element_name;
  int tag;
} TAGSETS;

TAGSETS tagset_g[] = {
  {"title", 1},
  {"author", 2},
  {"publicationPlace", 3},
  {"publicationDate", 4},
  {"documentId", 5},
  {"abstract", 6},
  {"name", 7},
  {"date", 8},
  {"dateTime", 8},
  {"DisplayObject", 9},
  {"bodyOfDisplay", 9},
  {"organization", 10},
  {"postalAddress", 11},
  {"networkaddress", 12},
  {"emailAddress", 13},
  {"phoneNumber", 14},
  {"faxNumber", 15},
  {"country", 16},
  {"description", 17},
  {"time", 18},
  {"documentContent", 19},
  {"language", 20},
  {"subject", 21},
  {"resourcetype", 22},
  {"city", 23},
  {"stateOrProvince", 24},
  {"zipOrPostalCode", 25},
  {"cost", 26},
  {"format", 27},
  {"identifier", 28},
  {"rights", 29},
  {"relation", 30},
  {"publisher", 31},
  {"contributor", 32},
  {"source", 33},
  {"coverage", 34},
  {"private", 35},
  {"databasename", 36},
  {"recordid", 37},
  {NULL, 0}
};

TAGSETS tagset_m[] = {
  {"schemaIdentifier", 1},
  {"elementsOrdered", 2},
  {"elementOrdering", 3},
  {"defaultTagType", 4},
  {"defaultVarianSetId", 5},
  {"defaultVariantSpec", 6},
  {"ProcessingInstructions", 7},
  {"recordUsage", 8},
  {"restriction", 9},
  {"rank", 10},
  {"userMessage", 11},
  {"url", 12},
  {"record", 13},
  {"localcontrolnumber", 14},
  {"creationdate", 15},
  {"dateOfLastModification", 16},
  {"dateOfLastReview", 17},
  {"score", 18},
  {"wellKnown", 19},
  {"recordWrapper", 20},
  {"defaultTagSetId", 21},
  {"languageOfRecord", 22},
  {"type", 23},
  {"scheme", 24},
  {"costinfo", 25},
  {"costflag", 26},
  {"recordcreatedby", 27},
  {"recordmodifiedby", 28},
  {NULL, 0},
};


#endif

