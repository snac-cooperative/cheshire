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
*	Header Name:	diagnostic.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	handling of Z39.50 bib-1 diagnostics messages
*
*	Usage:		diagnostic_init(); initialize the array of messages
*                       diagnostic_set(number,addnum,addstr); sets global 
*                                     diagnostic code and addinfo values.
*			diagnostic_get(); returns global diagnostic code.
*			diagnostic_clear(); clears the global diagnostic code.
*			diagnostic_string(number); returns the string.
*						   corresponding to the number.
*			diagnostic_addinfo(); returns the string corresponding
*			              to the Addinfo stuff for this diagnostic
*                                     numerics are converted to string form.
*
*
*	Variables:	
*
*	Return Conditions and Return Codes:	vary.
*
*	Date:		1/12/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#ifdef WIN32
#include <stdlib.h>
void *memset(void *dest, int c, size_t count );
#endif
#include <stdio.h>


struct diagnostic {
  int code;
  int add_type;
  char *string;
} diagnostics[1500];

/* the following is the global diagnostic code        */
/* It is originally set to -1 as a flag to initialize */
static int current_diagnostic_code = -1;
static int current_diagnostic_addinfo_num = 0;
char *current_diagnostic_addinfo_str = NULL;

/* codes to indicate type of addinfo included with diagnostic */
#define TYPE      1
#define VALUE     2
#define NAME      3
#define TERM      4
#define NUMBER    5
#define ATTLIST   6
#define DISTANCE  7
#define KEY       8
#define MAXIMUM   9
#define OID       10
#define OPERATOR  11
#define RELATION  12
#define SEQUENCE  13
#define STEP_SIZE 14
#define TERM_LIST 15
#define SYNTAX    16
#define SYNTAX_LIST 17
#define TIME 18
#define STRING 19



int
diagnostic_init() 
{
  
  memset((void *)diagnostics, 0, sizeof(diagnostics));
  diagnostics[1].code = 1 ;
  diagnostics[1].add_type = 0;
  diagnostics[1].string = "permanent system error";
  diagnostics[2].code = 2 ;
  diagnostics[2].add_type = 0;
  diagnostics[2].string = "temporary system error";
  diagnostics[3].code = 3 ;
  diagnostics[3].add_type = 0;
  diagnostics[3].string = "unsupported search";
  diagnostics[4].code = 4 ;
  diagnostics[4].add_type = 0;
  diagnostics[4].string = "Terms only exclusion (stop) words";
  diagnostics[5].code = 5 ;
  diagnostics[5].add_type = 0;
  diagnostics[5].string = "Too many argument words";
  diagnostics[6].code = 6 ;
  diagnostics[6].add_type = 0;
  diagnostics[6].string = "Too many boolean operators";
  diagnostics[7].code = 7 ;
  diagnostics[7].add_type = 0;
  diagnostics[7].string = "Too many truncated words";
  diagnostics[8].code = 8 ;
  diagnostics[8].add_type = 0;
  diagnostics[8].string = "Too many incomplete subfields";
  diagnostics[9].code = 9 ;
  diagnostics[9].add_type = 0;
  diagnostics[9].string = "Truncated words too short";
  diagnostics[10].code = 10;
  diagnostics[10].add_type = 0;
  diagnostics[10].string = "Invalid format for rec. number (search term)";
  diagnostics[11].code = 11;
  diagnostics[11].add_type = 0;
  diagnostics[11].string = "Too many characters in search statement";
  diagnostics[12].code = 12;
  diagnostics[12].add_type = 0;
  diagnostics[12].string = "Too many records retrieved";
  diagnostics[13].code = 13;
  diagnostics[13].add_type = 0;
  diagnostics[13].string = "Present request out-of-range";
  diagnostics[14].code = 14;
  diagnostics[14].add_type = 0;
  diagnostics[14].string = "System error in presenting records";
  diagnostics[15].code = 15;
  diagnostics[15].add_type = 0;
  diagnostics[15].string = "Record not authorized to be sent intersystem";
  diagnostics[16].code = 16;
  diagnostics[16].add_type = 0;
  diagnostics[16].string = "Record exceeds Preferred-message-size";
  diagnostics[17].code = 17;
  diagnostics[17].add_type = 0;
  diagnostics[17].string = "Record exceeds Maximum-record-size";
  diagnostics[18].code = 18;
  diagnostics[18].add_type = 0;
  diagnostics[18].string = "Result set not supported as a search term";
  diagnostics[19].code = 19;
  diagnostics[19].add_type = 0;
  diagnostics[19].string = "Only single result as search term supported";
  diagnostics[20].code = 20;
  diagnostics[20].add_type = 0;
  diagnostics[20].string = "Only ANDing of a single result set as search term";
  diagnostics[21].code = 21;
  diagnostics[21].add_type = 0;
  diagnostics[21].string = "Result set exists and replace indicator off";
  diagnostics[22].code = 22;
  diagnostics[22].add_type = 0;
  diagnostics[22].string = "Result set naming not supported";
  diagnostics[23].code = 23;
  diagnostics[23].add_type = 0;
  diagnostics[23].string = "Specified combination of databases not supported";
  diagnostics[24].code = 24;
  diagnostics[24].add_type = 0;
  diagnostics[24].string = "Element set names not supported";
  diagnostics[25].code = 25;
  diagnostics[25].add_type = 0;
  diagnostics[25].string = "Specified element set name not valid for specified database";
  diagnostics[26].code = 26;
  diagnostics[26].add_type = 0;
  diagnostics[26].string = "Only generic form of element set name supported";
  diagnostics[27].code = 27;
  diagnostics[27].add_type = 0;
  diagnostics[27].string = "Result set no longer exists - deleted by host";
  diagnostics[28].code = 28;
  diagnostics[28].add_type = 0;
  diagnostics[28].string = "Result set is in use";
  diagnostics[29].code = 29;
  diagnostics[29].add_type = 0;
  diagnostics[29].string = "One of the specified databases is locked";
  diagnostics[30].code = 30;
  diagnostics[30].add_type = 0;
  diagnostics[30].string = "Specified result set does not exist";
  diagnostics[31].code = 31;
  diagnostics[31].add_type = 0;
  diagnostics[31].string = "Resources exhausted - no results available";
  diagnostics[32].code = 32;
  diagnostics[32].add_type = 0;
  diagnostics[32].string = "Resources exhausted - unpred. partial results available";
  diagnostics[33].code = 33;
  diagnostics[33].add_type = 0;
  diagnostics[33].string = "Resources exhausted - valid subset of results available";
  diagnostics[100].code = 100;
  diagnostics[100].add_type = 0;
  diagnostics[100].string = "(unspecified) error";
  diagnostics[101].code = 101;
  diagnostics[101].add_type = 0;
  diagnostics[101].string = "Access-control failure";
  diagnostics[102].code = 102;
  diagnostics[102].add_type = 0;
  diagnostics[102].string = "Challenge required, could not be issued - oper. terminated";
  diagnostics[103].code = 103;
  diagnostics[103].add_type = 0;
  diagnostics[103].string = "Challenge required, could not be issued - record not included";
  diagnostics[104].code = 104;
  diagnostics[104].add_type = 0;
  diagnostics[104].string = "Challenge failed - record not included";
  diagnostics[105].code = 105;
  diagnostics[105].add_type = 0;
  diagnostics[105].string = "Terminated at origin request";
  diagnostics[106].code = 106;
  diagnostics[106].add_type = 0;
  diagnostics[106].string = "No abstract syntaxes agreed to for this record";
  diagnostics[107].code = 107;
  diagnostics[107].add_type = 0;
  diagnostics[107].string = "Query type not supported";
  diagnostics[108].code = 108;
  diagnostics[108].add_type = 0;
  diagnostics[108].string = "Malformed query";
  diagnostics[109].code = 109;
  diagnostics[109].add_type = NAME;
  diagnostics[109].string = "Database unavailable";
  diagnostics[110].code = 110;
  diagnostics[110].add_type = OPERATOR;
  diagnostics[110].string = "Operator unsupported";
  diagnostics[111].code = 111;
  diagnostics[111].add_type = MAXIMUM;
  diagnostics[111].string = "Too many databases specified";
  diagnostics[112].code = 112;
  diagnostics[112].add_type = MAXIMUM;
  diagnostics[112].string = "Too many result sets created";
  diagnostics[113].code = 113;
  diagnostics[113].add_type = TYPE;
  diagnostics[113].string = "Unsupported attribute type";
  diagnostics[114].code = 114;
  diagnostics[114].add_type = VALUE;
  diagnostics[114].string = "Unsupported Use attribute";
  diagnostics[115].code = 115;
  diagnostics[115].add_type = TERM;
  diagnostics[115].string = "Unsupported value for Use attribute";
  diagnostics[116].code = 116;
  diagnostics[116].add_type = 0;
  diagnostics[116].string = "Use attribute required but not supplied";
  diagnostics[117].code = 117;
  diagnostics[117].add_type = VALUE;
  diagnostics[117].string = "Unsupported Relation attribute";
  diagnostics[118].code = 118;
  diagnostics[118].add_type = VALUE;
  diagnostics[118].string = "Unsupported Structure attribute";
  diagnostics[119].code = 119;
  diagnostics[119].add_type = VALUE;
  diagnostics[119].string = "Unsupported Position attribute";
  diagnostics[120].code = 120;
  diagnostics[120].add_type = VALUE;
  diagnostics[120].string = "Unsupported Truncation attribute";
  diagnostics[121].code = 121;
  diagnostics[121].add_type = OID;
  diagnostics[121].string = "Unsupported Attribute Set";
  diagnostics[122].code = 122;
  diagnostics[122].add_type = VALUE;
  diagnostics[122].string = "Unsupported Completeness attribute";
  diagnostics[123].code = 123;
  diagnostics[123].add_type = 0;
  diagnostics[123].string = "Unsupported attribute combination";
  diagnostics[124].code = 124;
  diagnostics[124].add_type = VALUE;
  diagnostics[124].string = "Unsupported coded value for term";
  diagnostics[125].code = 125;
  diagnostics[125].add_type = 0;
  diagnostics[125].string = "Malformed search term";
  diagnostics[126].code = 126;
  diagnostics[126].add_type = TERM;
  diagnostics[126].string = "Illegal term value for attribute";
  diagnostics[127].code = 127;
  diagnostics[127].add_type = VALUE;
  diagnostics[127].string = "Unparsable format for un-normalized value";
  diagnostics[128].code = 128;
  diagnostics[128].add_type = NAME;
  diagnostics[128].string = "Illegal result set name";
  diagnostics[129].code = 129;
  diagnostics[129].add_type = 0;
  diagnostics[129].string = "Proximity search of sets not supported";
  diagnostics[130].code = 130;
  diagnostics[130].add_type = NAME;
  diagnostics[130].string = "Illegal result set in proximity search";
  diagnostics[131].code = 131;
  diagnostics[131].add_type = VALUE;
  diagnostics[131].string = "Unsupported proximity relation";
  diagnostics[132].code = 132;
  diagnostics[132].add_type = VALUE;
  diagnostics[132].string = "Unsupported proximity unit code";
  diagnostics[201].code = 201;
  diagnostics[201].add_type = ATTLIST;
  diagnostics[201].string = "Proximity not supported with this attribute combination";
  diagnostics[202].code = 202;
  diagnostics[202].add_type = DISTANCE;
  diagnostics[202].string = "Unsupported distance for proximity";
  diagnostics[203].code = 203;
  diagnostics[203].add_type = 0;
  diagnostics[203].string = "Ordered flag not supported for proximity";
  diagnostics[204].code = 204;
  diagnostics[204].add_type = 0;
  diagnostics[204].string = "Exclusion flag not supported for proximity";
  diagnostics[205].code = 205;
  diagnostics[205].add_type = 0;
  diagnostics[205].string = "Only zero step size supported for Scan";
  diagnostics[206].code = 206;
  diagnostics[206].add_type = STEP_SIZE;
  diagnostics[206].string = "Specified step size not supported for Scan";
  diagnostics[207].code = 207;
  diagnostics[207].add_type = SEQUENCE;
  diagnostics[207].string = "Cannot sort according to sequence";
  diagnostics[208].code = 208;
  diagnostics[208].add_type = 0;
  diagnostics[208].string = "No result set name supplied on Sort";
  diagnostics[209].code = 209;
  diagnostics[209].add_type = 0;
  diagnostics[209].string = "Generic sort not supported (db specific only)";
  diagnostics[210].code = 210;
  diagnostics[210].add_type = 0;
  diagnostics[210].string = "Db specific sort not supported";
  diagnostics[211].code = 211;
  diagnostics[211].add_type = NUMBER;
  diagnostics[211].string = "Too many sort keys";
  diagnostics[212].code = 212;
  diagnostics[212].add_type = KEY;
  diagnostics[212].string = "Duplicate sort keys";
  diagnostics[213].code = 213;
  diagnostics[213].add_type = VALUE;
  diagnostics[213].string = "Unuspported missing data action";
  diagnostics[214].code = 214;
  diagnostics[214].add_type = RELATION;
  diagnostics[214].string = "Illegal sort relation";
  diagnostics[215].code = 215;
  diagnostics[215].add_type = VALUE;
  diagnostics[215].string = "Illegal case value";
  diagnostics[216].code = 216;
  diagnostics[216].add_type = VALUE;
  diagnostics[216].string = "Illegal missing data action";
  diagnostics[217].code = 217;
  diagnostics[217].add_type = 0;
  diagnostics[217].string = "Cannot guarantee records will fit in specified segments";
  diagnostics[218].code = 218;
  diagnostics[218].add_type = NAME;
  diagnostics[218].string = "ES: Package name already in use";
  diagnostics[219].code = 219;
  diagnostics[219].add_type = NAME;
  diagnostics[219].string = "ES: no such package, on modify/delete";
  diagnostics[220].code = 220;
  diagnostics[220].add_type = 0;
  diagnostics[220].string = "ES: quota exceeded";
  diagnostics[221].code = 221;
  diagnostics[221].add_type = TYPE;
  diagnostics[221].string = "ES: extended service type not supported";
  diagnostics[222].code = 222;
  diagnostics[222].add_type = 0;
  diagnostics[222].string = "ES: permission denied on ES - id not authorized";
  diagnostics[223].code = 223;
  diagnostics[223].add_type = 0;
  diagnostics[223].string = "ES: permission denied on ES - cannot modify or delete";
  diagnostics[224].code = 224;
  diagnostics[224].add_type = 0;
  diagnostics[224].string = "ES: immediate execution failed";
  diagnostics[225].code = 225;
  diagnostics[225].add_type = 0;
  diagnostics[225].string = "ES: immediate execution not supported for this service";
  diagnostics[226].code = 226;
  diagnostics[226].add_type = 0;
  diagnostics[226].string = "ES: immediate execution not supported for these parameters";
  diagnostics[227].code = 227;
  diagnostics[227].add_type = 0;
  diagnostics[227].string = "No data available in requested record syntax";
  diagnostics[228].code = 228;
  diagnostics[228].add_type = 0;
  diagnostics[228].string = "Scan: Malformed scan";
  diagnostics[229].code = 229;
  diagnostics[229].add_type = TYPE;
  diagnostics[229].string = "Term type not supported";
  diagnostics[230].code = 230;
  diagnostics[230].add_type = MAXIMUM;
  diagnostics[230].string = "Sort: too many input records";
  diagnostics[231].code = 231;
  diagnostics[231].add_type = 0;
  diagnostics[231].string = "Sort: incompatible record formats";
  diagnostics[232].code = 232;
  diagnostics[232].add_type = TERM_LIST;
  diagnostics[232].string = "Scan: term list not supported";
  diagnostics[233].code = 233;
  diagnostics[233].add_type = VALUE;
  diagnostics[233].string = "Scan: unsupported value of position-in-response";
  diagnostics[234].code = 234;
  diagnostics[234].add_type = NUMBER;
  diagnostics[234].string = "Too many index terms processed";
  diagnostics[235].code = 235;
  diagnostics[235].add_type = NAME;
  diagnostics[235].string = "Database does not exist";
  diagnostics[236].code = 236;
  diagnostics[236].add_type = NAME;
  diagnostics[236].string = "Access to specified database denied";
  diagnostics[237].code = 237;
  diagnostics[237].add_type = 0;
  diagnostics[237].string = "Sort: illegal sort";
  diagnostics[238].code = 238;
  diagnostics[238].add_type = SYNTAX_LIST;
  diagnostics[238].string = "Record not available in requested syntax";
  diagnostics[239].code = 239;
  diagnostics[239].add_type = SYNTAX;
  diagnostics[239].string = "Record syntax not supported";
  diagnostics[240].code = 240;
  diagnostics[240].add_type = 0;
  diagnostics[240].string = "Scan: Resources exhausted looking for satisfying terms";
  diagnostics[241].code = 241;
  diagnostics[241].add_type = 0;
  diagnostics[241].string = "Scan: Beginning or end of term list";
  diagnostics[242].code = 242;
  diagnostics[242].add_type = NUMBER;
  diagnostics[242].string = "Segmentation: max-segment-size too small to segment record";
  diagnostics[243].code = 243;
  diagnostics[243].add_type = 0;
  diagnostics[243].string = "Present:  additional-ranges parameter not supported";
  diagnostics[244].code = 244;
  diagnostics[244].add_type = 0;
  diagnostics[244].string = "Present:  comp-spec parameter not supported";
  diagnostics[245].code = 245;
  diagnostics[245].add_type = 0;
  diagnostics[245].string = "Type-1 query: restriction ('resultAttr') operand not supported";
  diagnostics[246].code = 246;
  diagnostics[246].add_type = 0;
  diagnostics[246].string = "Type-1 query: 'complex' attributeValue not supported";
  diagnostics[247].code = 247;
  diagnostics[247].add_type = 0;
  diagnostics[247].string = "Type-1 query: 'attributeSet' as part of AttributeElement not supported";

  diagnostics[1001].code = 1001;
  diagnostics[1001].add_type = 0;
  diagnostics[1001].string = "Malformed APDU.";
  diagnostics[1002].code = 1002;
  diagnostics[1002].add_type = 0;
  diagnostics[1002].string = "ES: EXTERNAL form of Item Order request not supported.";
  diagnostics[1003].code = 1003;
  diagnostics[1003].add_type = 0;
  diagnostics[1003].string = "ES: Result set item form of Item Order request not supported.";
  diagnostics[1004].code = 1004;
  diagnostics[1004].add_type = 0;
  diagnostics[1004].string = "ES: Extended services not supported unless access control is in effect.";
  diagnostics[1005].code = 1005;
  diagnostics[1005].add_type = 0;
  diagnostics[1005].string = "Response records in Search response not supported.";
  diagnostics[1006].code = 1006;
  diagnostics[1006].add_type = 0;
  diagnostics[1006].string = "Response records in Search response not possible for specified database (or database combination).";
  diagnostics[1007].code = 1007;
  diagnostics[1007].add_type = TERM_LIST;
  diagnostics[1007].string = "No Explain server.";
  diagnostics[1008].code = 1008;
  diagnostics[1008].add_type = TERM_LIST;
  diagnostics[1008].string = "ES: missing mandatory parameter for specified function parameter.";
  diagnostics[1009].code = 1009;
  diagnostics[1009].add_type = OID;
  diagnostics[1009].string = "ES: Item Order, unsupported OID in itemRequest.";
  diagnostics[1010].code = 1010;
  diagnostics[1010].add_type = 0;
  diagnostics[1010].string = "Init/AC: Bad Userid.";
  diagnostics[1011].code = 1011;
  diagnostics[1011].add_type = 0;
  diagnostics[1011].string = "Init/AC: Bad Userid and/or Password.";
  diagnostics[1012].code = 1012;
  diagnostics[1012].add_type = 0;
  diagnostics[1012].string = "Init/AC: No searches remaining (pre-purchased searches exhausted).";
  diagnostics[1013].code = 1013;
  diagnostics[1013].add_type = 0;
  diagnostics[1013].string = "Init/AC: Incorrect interface type (specified id valid only when used with a particular access method or client).";
  diagnostics[1014].code = 1014;
  diagnostics[1014].add_type = 0;
  diagnostics[1014].string = "Init/AC: Authentication System error.";
  diagnostics[1015].code = 1015;
  diagnostics[1015].add_type = 0;
  diagnostics[1015].string = "Init/AC: Maximum number of simultaneous sessions for Userid.";
  diagnostics[1016].code = 1016;
  diagnostics[1016].add_type = 0;
  diagnostics[1016].string = "Init/AC: Blocked network address.";
  diagnostics[1017].code = 1017;
  diagnostics[1017].add_type = 0;
  diagnostics[1017].string = "Init/AC: No databases available for specified userId.";
  diagnostics[1018].code = 1018;
  diagnostics[1018].add_type = 0;
  diagnostics[1018].string = "Init/AC: System temporarily out of resources.";
  diagnostics[1019].code = 1019;
  diagnostics[1019].add_type = TIME;
  diagnostics[1019].string = "Init/AC: System not available due to maintenance.";
  diagnostics[1020].code = 1020;
  diagnostics[1020].add_type = TIME;
  diagnostics[1020].string = "Init/AC: System temporarily unavailable.";
  diagnostics[1021].code = 1021;
  diagnostics[1021].add_type = 0;
  diagnostics[1021].string = "Init/AC: Account has expired.";
  diagnostics[1022].code = 1022;
  diagnostics[1022].add_type = 0;
  diagnostics[1022].string = "Init/AC: Password has expired so a new one must be supplied.";
  diagnostics[1023].code = 1023;
  diagnostics[1023].add_type = 0;
  diagnostics[1023].string = "Init/AC: Password has been changed by an administrator so a new one must be supplied.";
  diagnostics[1024].code = 1024;
  diagnostics[1024].add_type = STRING;
  diagnostics[1024].string = "Unsupported Attribute.";
  diagnostics[1025].code = 1025;
  diagnostics[1025].add_type = 0;
  diagnostics[1025].string = "Service not supported for this database.";
  diagnostics[1026].code = 1026;
  diagnostics[1026].add_type = 0;
  diagnostics[1026].string = "Record cannot be opened because it is locked.";
  diagnostics[1027].code = 1027;
  diagnostics[1027].add_type = 0;
  diagnostics[1027].string = "SQL error.";
  diagnostics[1028].code = 1028;
  diagnostics[1028].add_type = 0;
  diagnostics[1028].string = "Record deleted.";
  diagnostics[1029].code = 1029;
  diagnostics[1029].add_type = NUMBER;
  diagnostics[1029].string = "Scan: too many terms requested.";
  diagnostics[1040].code = 1040;
  diagnostics[1040].add_type = STRING;
  diagnostics[1040].string = "ES: Invalid function function.";
  diagnostics[1041].code = 1041;
  diagnostics[1041].add_type = 0;
  diagnostics[1041].string = "ES: Error in retention time (unspecified).";
  diagnostics[1042].code = 1042;
  diagnostics[1042].add_type = STRING;
  diagnostics[1042].string = "ES: Permissions data not understood permissions.";
  diagnostics[1043].code = 1043;
  diagnostics[1043].add_type = OID;
  diagnostics[1043].string = "ES: Invalid OID for task specific parameters oid.";
  diagnostics[1044].code = 1044;
  diagnostics[1044].add_type = STRING;
  diagnostics[1044].string = "ES: Invalid action.";
  diagnostics[1045].code = 1045;
  diagnostics[1045].add_type = STRING;
  diagnostics[1045].string = "ES: Unknown schema.";
  diagnostics[1046].code = 1046;
  diagnostics[1046].add_type = NUMBER;
  diagnostics[1046].string = "ES: Too many records in package maximum number allowed.";
  diagnostics[1047].code = 1047;
  diagnostics[1047].add_type = STRING;
  diagnostics[1047].string = "ES: Invalid wait action wait action.";
  diagnostics[1048].code = 1048;
  diagnostics[1048].add_type = NUMBER;
  diagnostics[1048].string = "ES: Cannot create task package -- exceeds maximum permissable size.";
  diagnostics[1049].code = 1049;
  diagnostics[1049].add_type = NUMBER;
  diagnostics[1049].string = "ES: Cannot return task package -- exceeds maximum permissable size for ES response.";
  diagnostics[1050].code = 1050;
  diagnostics[1050].add_type = NUMBER;
  diagnostics[1050].string = "ES: Extended services request too large.";
  diagnostics[1051].code = 1051;
  diagnostics[1051].add_type = 0;
  diagnostics[1051].string = "Scan: Attribute set id required -- not supplied.";
  diagnostics[1052].code = 1052;
  diagnostics[1052].add_type = NUMBER;
  diagnostics[1052].string = "ES: Cannot process task package record -- exceeds maximum permissible record size for ES.";
  diagnostics[1053].code = 1053;
  diagnostics[1053].add_type = NUMBER;
  diagnostics[1053].string = "ES: Cannot return task package record -- exceeds maximum permissible record size for ES response.";
  diagnostics[1054].code = 1054;
  diagnostics[1054].add_type = NUMBER;
  diagnostics[1054].string = "Init: Required negotiation record not included.";
  diagnostics[1055].code = 1055;
  diagnostics[1055].add_type = 0;
  diagnostics[1055].string = "Init: negotiation option required.";
  diagnostics[1056].code = 1056;
  diagnostics[1056].add_type = STRING;
  diagnostics[1056].string = "Attribute not supported for database attribute.";
  diagnostics[1057].code = 1057;
  diagnostics[1057].add_type = STRING;
  diagnostics[1057].string = "ES: Unsupported value of task package parameter.";
  diagnostics[1058].code = 1058;
  diagnostics[1058].add_type = 0;
  diagnostics[1058].string = "Duplicate Detection: Cannot dedup on requested record portion.";
  diagnostics[1059].code = 1059;
  diagnostics[1059].add_type = STRING;
  diagnostics[1059].string = "Duplicate Detection: Requested detection criterion not supported.";
  diagnostics[1060].code = 1060;
  diagnostics[1060].add_type = 0;
  diagnostics[1060].string = "Duplicate Detection: Requested level of match not supported.";
  diagnostics[1061].code = 1061;
  diagnostics[1061].add_type = 0;
  diagnostics[1061].string = "Duplicate Detection: Requested regular expression not supported.";
  diagnostics[1062].code = 1062;
  diagnostics[1062].add_type = 0;
  diagnostics[1062].string = "Duplicate Detection: Cannot do clustering.";
  diagnostics[1063].code = 1063;
  diagnostics[1063].add_type = STRING;
  diagnostics[1063].string = "Duplicate Detection: Retention criterion not supported.";
  diagnostics[1064].code = 1064;
  diagnostics[1064].add_type = 0;
  diagnostics[1064].string = "Duplicate Detection: Requested number (or percentage) of entries for retention too large.";
  diagnostics[1065].code = 1065;
  diagnostics[1065].add_type = 0;
  diagnostics[1065].string = "Duplicate Detection: Requested sort criterion not supported sort criterion.";
  diagnostics[1066].code = 1066;
  diagnostics[1066].add_type = 0;
  diagnostics[1066].string = "CompSpec: Unknown schema, or schema not supported.";
  diagnostics[1067].code = 1067;
  diagnostics[1067].add_type = STRING;
  diagnostics[1067].string = "Encapsulation: Encapsulated sequence of APDUs not supported.";
  diagnostics[1068].code = 1068;
  diagnostics[1068].add_type = 0;
  diagnostics[1068].string = "Encapsulation: Base operation (and encapsulated APDUs) not executed based on pre-screening analysis.";
  diagnostics[1069].code = 1069;
  diagnostics[1069].add_type = 0;
  diagnostics[1069].string = "No syntaxes available for this request.";
  diagnostics[1070].code = 1070;
  diagnostics[1070].add_type = 0;
  diagnostics[1070].string = "user not authorized to receive record(s) in requested syntax.";
  diagnostics[1071].code = 1071;
  diagnostics[1071].add_type = 0;
  diagnostics[1071].string = "preferredRecordSyntax not supplied.";
  diagnostics[1072].code = 1072;
  diagnostics[1072].add_type = STRING;
  diagnostics[1072].string = "Query term includes characters that do not translate into the target character set.";

  current_diagnostic_code = 0; 

  return(0);
}

/*  sets global diagnostic code. */
void
diagnostic_set(int number, int add_num, char *add_str) 
{ 
  if (current_diagnostic_code == -1)  diagnostic_init();

  if (number > 0 && (number < 250 || (number >=1001 && number < 1072))) {
    if (diagnostics[number].code > 0) {
      current_diagnostic_code = number;
      current_diagnostic_addinfo_num = add_num;
      current_diagnostic_addinfo_str = add_str;
    }
    else
      fprintf(stderr, "Invalid diagnostic number %d\n", number);
  }
  else
    fprintf(stderr, "unable to set diagnostic number %d\n", number);

}

/*  returns the global diagnostic code. */
int 
diagnostic_get() {
  if (current_diagnostic_code == -1)  diagnostic_init();

  return(current_diagnostic_code);
}

/* clears the global diadnostic code */
void
diagnostic_clear() {
  if (current_diagnostic_code == -1)  diagnostic_init();

  current_diagnostic_code = 0;
  current_diagnostic_addinfo_num = 0;
  current_diagnostic_addinfo_str = NULL;
}

/* Return the diagnostic message string corresponding to the number. */
/* returns NULL if no corresponding message.                         */
char *
diagnostic_string(number) {
  
  if (current_diagnostic_code == -1)  diagnostic_init();

  if (number > 0 && number < 250) 
    return (diagnostics[number].string);
  else
    return (NULL);
}

/* return the current diagnostic addinfo value as a string */
char *
diagnostic_addinfo() {
  int code;
  static char diagnostic_number[20];

  if (current_diagnostic_code == -1)  diagnostic_init();

  if ((code = diagnostic_get()) == 0)
    return ("");

  if (current_diagnostic_addinfo_num > 0 
      || current_diagnostic_addinfo_str == NULL) {
    sprintf(diagnostic_number,"%d",current_diagnostic_addinfo_num);
    return (diagnostic_number);
  }
  else
    return(current_diagnostic_addinfo_str);
  
}
















