/*
 *  Copyright (c) 1990-2014 [see Other Notes, below]. The Regents of the
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
*       Header Name:    CheshirePy.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        drive cheshire search engine from Python 
*                      
*
*       Usage:          
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           05/05/2014
*       Revised:        
*       Version:        0.1
*       Copyright (c) 2014.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAIN
#define STATMAIN
#include "CheshirePy.h"


int init(char  *config_path) {

char *crflags, *opflags;
#ifdef WIN32
  crflags = "rb";
  opflags = "rb";
#else
  crflags = "r";
  opflags = "r";
#endif

LOGFILE = stderr;


  cf_info_base = (config_file_info *) cf_initialize(config_path, crflags, opflags);
  if (cf_info_base == NULL) {
    return (-1);

  setenv("CHESHIRE_CONFIGFILE", config_path, 1);

  return(0);

  }
}

int setdb(char *dbname) {

  if (dbname != NULL)
    setenv("CHESHIRE_DATABASE", dbname, 1);
  else
    return (-1);
  return(0);

}

int setnumwanted(int numwanted) {
  char temp[20];
  
  sprintf(temp,"%d",numwanted);
   
  setenv("CHESHIRE_NUMREQUESTED", temp, 1);
  return(numwanted);

}

int setsyntax(char *syntax) {

  if (syntax != NULL) 
    setenv("CHESHIRE_RECSYNTAX", syntax, 1);
  else
    return (-1);

  return(0);
}

int setresultformat(char *format) {
  if (format != NULL)
    setenv("CHESHIRE_ELEMENTSET", format, 1);
  else
    return (-1);

  return(0);
}

int setresultname(char *name) {

  if (name != NULL)
    setenv("CHESHIRE_RESULTSETNAME", name, 1);
  else
    return (-1);

  return(0);
}

char *showconfig() {
  if (cf_info_base == NULL) 
    return((char *)NULL);

  return(strdup(getenv("CHESHIRE_CONFIGFILE")));
}

char *getconfigpath() {
  if (cf_info_base == NULL) 
    return((char *)NULL);

  return(strdup(getenv("CHESHIRE_CONFIGFILE")));
}

char *getconfigindexnames() {

  idx_list_entry *idx;
  config_file_info *cf = NULL;
  int total_size = 0;
  char *resultstring = NULL;

  if (cf_info_base == NULL) 
    return((char *)NULL);
  else
    cf = cf_info_base;

  /* make two passes to get lengths and allocate output string */
  for (idx = cf->indexes; idx != NULL; idx = idx->next_entry) {
    total_size += strlen(idx->tag) + 2;
  }

  if (total_size > 0)
    resultstring = CALLOC(char, total_size + 1);
  else
    return(NULL);
    
  for (idx = cf->indexes; idx != NULL; idx = idx->next_entry) {
    strcat(resultstring,idx->tag);
    strcat(resultstring," ");

  }

  return(resultstring);

}

char *showsyntax() {

  return(strdup(getenv("CHESHIRE_RECSYNTAX")));

}

char *showformat() {

  return(strdup(getenv("CHESHIRE_ELEMENTSET")));

}

char *showresultname() {

  return(strdup(getenv("CHESHIRE_RESULTSETNAME")));

}

int shownumwanted() {
  char *tmp;
  int result;

  tmp = getenv("CHESHIRE_NUMREQUESTED");
  if (tmp == NULL)
    return(-1);
  else {
    result = atoi(tmp);
    return(result);
  }

}


char *showdbs() {
  /* in principle this should handle multiple linked configfiles - someday */
  if (cf_info_base == NULL) 
    return((char *)NULL);

  return(strdup(cf_info_base->nickname));
}

char *showdb() {
  /* in principle this should handle multiple linked configfiles - someday */
  if (cf_info_base == NULL) 
    return((char *)NULL);

  return(strdup(getenv("CHESHIRE_DATABASE")));
}

char *getdbname() {
  return(showdb());
}


weighted_result *Search(char *querystring) {

  RPNQuery *RPN_Query_ptr;
  extern SGML_Doclist  *PRS_first_doc;

  char *filename = NULL;
  int num_request = 0;
  int filetype = 0;
  weighted_result *final_set;	/* result from query processing */
  
  LOGFILE = stderr;

  if (cf_info_base == NULL) { 
    fprintf(LOGFILE, 
	    "bad Cheshire config file ? Use init to set configfile path\n");
    return((weighted_result *)NULL);
  }
  
  filename = showdb();
  
  
  filetype = cf_getfiletype(filename);
 
  RPN_Query_ptr = queryparse(querystring, "default"); /* defaults to bib-1 */

  clean_up_query_parse();  

/*  printf("Query in RPN form:\n");
    dump_rpn_query(RPN_Query_ptr);
    printf("\n");
*/
  
  /* Normal query processing */
  final_set = se_process_query(filename, RPN_Query_ptr);

  FreeRPNQuery(RPN_Query_ptr);

  free(filename);

  return (final_set);

}

int getnumfound(weighted_result *final_set) {

  if (final_set == NULL) 
    return(-1);

  return (final_set->num_hits);
}

char *getrecord(weighted_result *final_set, int recnum) {

  SGML_Document *doc, *GetSGML();
  char *resultdoc;

  if (final_set == NULL) 
    return(NULL);

  if (final_set->num_hits < recnum)
    return(NULL);
	
  doc = GetSGML(final_set->filename, final_set->entries[recnum].record_num);

  if (doc == NULL)
    return (NULL);

  resultdoc = strdup(doc->buffer);
  
  /* free up the doc, should be finished with it */
  free_doc(doc);
  
  return(resultdoc);
	    
}

float getrelevance(weighted_result *final_set, int recnum) {

  float result;

  if (final_set == NULL) 
    return(-1.0);

  if (final_set->num_hits < recnum || recnum < 0)
    return(-1.0);
	
  return(final_set->entries[recnum].weight);

}

void closeresult(weighted_result *final_set) {

  
  FREE(final_set);

  return;

}

void CheshireClose() {

  /* clean up everything */
  
  /* free all the stored set info and remove the files */
  se_delete_all_result_sets();
  
  /* free all the DTDs */
  free_all_dtds();
  
  cf_closeall();

}



/* This function can be used to give the current "id" for the current user */
/* this is actually the connection file descriptor number in this version  */
/* when called with a non-NULL session pointer, it stores the file desc.   */
/* when called with a NULL session pointer it returns the current file desc*/
/* or zero if no file descriptor has been set                              */
int
current_user_id(ZSESSION *session)
{
  static int current_user_id_number=0;

  if (session == NULL)
    return (current_user_id_number);
  else
    current_user_id_number = session->fd;
  return(current_user_id_number);
}

/* the following are used in building some stuff during config file parsing */


StringOrNumeric *
BuildStringOrNumeric(char *string, int numeric, int which)
{
  StringOrNumeric *tmp;

  tmp = CALLOC(StringOrNumeric, 1);

  if (which == 0) {
    tmp->which = e25_string;
    tmp->u.string = NewInternationalString(string);
  }
  else {
    tmp->which = e25_numeric;
    tmp->u.numeric = numeric;
  }
  
  return (tmp);
}

HumanString *
BuildHumanString(char *text, char *language)
{
  HumanString *tmp;
  
  if (text == NULL)
    return NULL;

  tmp = CALLOC(HumanString, 1);
  tmp->item.language = (LanguageCode *)NewInternationalString(language);
  tmp->item.text = NewInternationalString(text);

  return(tmp);

}




