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
*	Header Name:	sgml_interface.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to provide an interface for parsing
*                       SGML declarations, DTDs and documents.
*	Usage:		
*          to parse an SGML declaration and DTD (declaration is optional)
*
*            SGML_DTD *sgml_parse_dtd(filename, catalog_name, schema, type);
*
*          to parse an SGML document:
*
*            SGML_Document *sgml_parse_document(DTD_pointer, file_name, 
*                                               buffer, record_num)
*          
*
*            calling sgml_parse_document for a particular filename will 
*            with a NULL DTD_pointer will load and parse
*            the corresponding DTD.
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		8/18/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* cheshire.h includes sgml.h -- the important one for this module */
#include <string.h>
#include <cheshire.h>

/* #define DEBUGPARSER */

extern char *temp_parse_buffer; /* this is declared in sgml_scanner.flex */
SGML_Doclist  *PRS_first_doc=NULL, *PRS_current_doc=NULL, *PRS_last_doc=NULL;
extern char *cf_getdatafilename(char *filename, int recordnum);

extern SGML_DTD *xml_schema_2_dtd(SGML_Document *schemadoc, int first_pass);

/* forward declaration */
SGML_Document *sgml_parse_document(SGML_DTD *dtd, char *filename, 
				   char *buffer, 
				   int recordnum, int keep_buffer);

/* the following is used to pass the file name to the doc & DTD parsers */
char *sgml_in_name;
/* the following is used to pass the file name to the doc & DTD parsers */
int sgml_in_dtd_type = 0;

/* this allocates some space in the scanner to keep the parsed data */
sgml_init_parser ()
{
  extern char *temp_parse_buffer; /* in sgml_scanner.flex */
  extern int temp_parse_buffer_size;
  extern int sgml_parse_error;

  sgml_parse_error = 0;

  if (temp_parse_buffer != NULL) 
    free(temp_parse_buffer);

  temp_parse_buffer = CALLOC(char, TEMP_PARSE_BUFFER_SIZE);
  temp_parse_buffer_size = TEMP_PARSE_BUFFER_SIZE;

}

/* the following is called for each "chunk" of data processed by */
/* the parser -- it copies the element to the temp_parse_buffer  */
/* and it re-allocates space in the scanner for large data       */
sgml_realloc_buffer (int parsed_file_chars, char *yytext, int yylen)
{
  extern char *temp_parse_buffer; /* in sgml_scanner.flex */
  extern int temp_parse_buffer_size;
  char *tempbuf;
  int  REALLOC_FACTOR;
  int newbufsize;
  
  REALLOC_FACTOR = 1;
  
  newbufsize = temp_parse_buffer_size;

  if ((parsed_file_chars + yylen) >= temp_parse_buffer_size) { 
    while (newbufsize < (parsed_file_chars + yylen 
			 + (TEMP_PARSE_BUFFER_SIZE/2))) {
      newbufsize = TEMP_PARSE_BUFFER_SIZE * REALLOC_FACTOR++;
    }
#ifdef WIN32
    /*  -- old version problem with REALLOC???  but WORKS??? for WIN */
    temp_parse_buffer = REALLOC(temp_parse_buffer, char, 
				newbufsize);
    if (temp_parse_buffer == NULL && LOGFILE != NULL) {
      fprintf(LOGFILE,"FAILED REALLOCATION in sgml_realloc_buffer\n");
      fflush(LOGFILE);
    }

    temp_parse_buffer_size = newbufsize;

#else    
    tempbuf = CALLOC(char, newbufsize);
    if (tempbuf == NULL && LOGFILE != NULL) {
      fprintf(LOGFILE,"FAILED REALLOCATION in sgml_realloc_buffer\n");
      fflush(LOGFILE);
    }
    else {
      strcpy(tempbuf, temp_parse_buffer);
      free(temp_parse_buffer);
      temp_parse_buffer = tempbuf;
      temp_parse_buffer_size = newbufsize;
    }
#endif      

    
  }

  strcpy(&temp_parse_buffer[parsed_file_chars], yytext);

}

sgml_cleanup_parser() 
{
  extern char *temp_parse_buffer; /* in sgml_scanner.flex */
  extern int temp_parse_buffer_size;

  clean_up_scan();

  if (temp_parse_buffer == NULL) return;

  FREE(temp_parse_buffer);
  temp_parse_buffer = NULL;
  temp_parse_buffer_size = 0;

}

SGML_DTD *sgml_parse_dtd(char *filename, char *sgml_catalog_name, 
			 char *in_schema_name, filelist *fl, int dtd_type)
{
  extern FILE *sgml_in;
  extern SGML_DTD *DTD_list, *current_sgml_dtd;
  extern SGML_Declaration *current_sgml_declaration;
  extern int sgml_parse_error;
  extern Tcl_HashTable *Current_SGML_Catalog;	
  SGML_DTD *dtd, *xml_schema_dtd, *primary_schema_dtd;
  char *catalog_name, *getenv();
  char *name_end;
  int catalog_found = 0;
  char *schema_file_name;


  /* first we check to see if this is already parsed */
  
  for (dtd = DTD_list; dtd != NULL; dtd = dtd->next_dtd) {
    if (strcmp (dtd->DTD_file_name, filename) == 0)
      return(dtd);
  }

  if (LOGFILE == NULL)
    LOGFILE = stderr;

  /* must be a new DTD --         */
  /* first parse the catalog info */
  Current_SGML_Catalog = NULL;
  sgml_in = NULL;
  
  if (sgml_catalog_name != NULL ) { /* if the name is provided --use it */
    sgml_in = fopen(sgml_catalog_name,"r");
    if (sgml_in == NULL) {
      if (LOGFILE) 
	fprintf(LOGFILE, 
		"Unable to open SGML catalog file '%s' in sgml_parse_dtd\n",
		sgml_catalog_name);
    }
    else catalog_found = 1;
  }
  else if ((catalog_name = getenv("SGML_CATALOG_FILES")) != NULL) {
    /* if the name is not provided but is set in an environment variable */
    /* use it, but (currently) only the first item in a colon list       */
    name_end = strchr(catalog_name,':');
    if (name_end != NULL)
      *name_end = '\0';
    sgml_in = fopen(catalog_name,"r");
    if (sgml_in == NULL) {
      if (LOGFILE)
	fprintf(LOGFILE, 
	      "Unable to open SGML catalog file from '%s' \n from environment variable SGML_CATALOG_FILES\n",
	      sgml_catalog_name);
    } 
    else catalog_found = 1;    
  }
  else {
    char *path, catbuf[500];
    char *filecopy;
    char *slash;

    path = filecopy = strdup(filename);
#ifndef WIN32
    slash = strrchr(filecopy, '/');
#else
    slash = strrchr(filecopy, '\\');
#endif

    if (slash) {
      *slash = '\0';
      sprintf(catbuf, "%s/%s", path, "catalog");
    }
    else
      sprintf(catbuf, "%s", "catalog");

    sgml_in = fopen(catbuf,"r");
    if (sgml_in == NULL) {
      /* NOTE this is a common case where there is no catalog */
      /* no need to clutter output with pointless messages    */
      /* fprintf(LOGFILE, 
	      "Unable to open local SGML catalog file '%s' from DTD directory\n",
	      catbuf);
       */
    }
    else catalog_found = 1;
    FREE(filecopy);
  }
  if (sgml_in != NULL) { /* have a catalog file */
    
    sgml_init_parser();
    
    start_catalog_parsing(); /* set for catalog input */
    
    sgml_cat_parse();
    
    if (sgml_parse_error) {
      if (LOGFILE)
	fprintf(LOGFILE,"SGML parse error in SGML catalog: %s\n",
		sgml_catalog_name);
      else
	fprintf(stderr,"SGML parse error in SGML catalog: %s\n",
		sgml_catalog_name);

      return(NULL);
    }
    /* NOTE: the EOF processing in the parser closes sgml_in */
    sgml_cleanup_parser();

  }



  sgml_in_name = strdup(filename);
  sgml_in_dtd_type = dtd_type;

  sgml_in = fopen(filename,"r");

  if (sgml_in == NULL) {
    if (LOGFILE)
      fprintf(LOGFILE, "Unable to open DTD '%s' in sgml_parse_dtd\n",
	      filename);
    else
      fprintf(stderr,  "Unable to open DTD '%s' in sgml_parse_dtd\n",
	      filename);
    cheshire_exit (0); /* might as well exit, because the dtd is mandatory */
  }


  if (current_sgml_dtd != NULL)
    sgml_restart(sgml_in);       

  sgml_init_parser();

  /* HANDLE XMLSchemas here too */

  if (dtd_type == 2) {
    char *docbuffer;
    struct stat filestatus, filestatus2;
    int statresult;
    int datasize;
    FILE *indocfile;
    SGML_Document *doc;
    int first_schema;
    filelist *currfl = NULL;

    start_dtd_parsing(); /* set for DTD input */

    sgml_dtd_parse();

    if (sgml_parse_error) {
      fprintf(LOGFILE,"SGML parse error in XMLSchema DTD: %s\n",filename);
      return (NULL);
    }
    else {
      current_sgml_dtd->sgml_declaration = current_sgml_declaration;
    }

    schema_file_name = in_schema_name;
    first_schema = 1;
    dtd = primary_schema_dtd = NULL;
    xml_schema_dtd = current_sgml_dtd;
    xml_schema_dtd->type = 3; /* special for the schema DTD */
    xml_schema_add_any_el(xml_schema_dtd, "##ANY##", 0);

    do {

      sgml_cleanup_parser();


      if (schema_file_name == NULL) {
	if (LOGFILE)
	  fprintf(LOGFILE, "No XMLSchema tag defined in configfile\n");
	else
	  fprintf(stderr,  "No XMLSchema tag defined in configfile\n");
	cheshire_exit (0); /* might as well exit, because the dtd is mandatory */
      }

      start_schema_parsing(); /* set for Schema input */
      /* call document parsing for the schema... */

      if ((statresult = stat(schema_file_name, &filestatus)) == 0) {

	docbuffer = CALLOC(char, filestatus.st_size +1);
	if ((indocfile = fopen(schema_file_name, "r")) == NULL) {
	  if (LOGFILE)
	    fprintf(LOGFILE, "Unable to open XMLSchema '%s' in sgml_parse_dtd\n",
		    schema_file_name);
	  else
	    fprintf(stderr,  "Unable to open XMLSchema '%s' in sgml_parse_dtd\n",
		    schema_file_name);
	  cheshire_exit (0); /* might as well exit, because the dtd is mandatory */
	}
	datasize = fread(docbuffer, sizeof(char), filestatus.st_size, indocfile);

	doc = sgml_parse_document(xml_schema_dtd, "***dummy***", docbuffer, 1, 1);
	/* if the schema doc had imports, then the docbuffer in doc will be */
	/* different from the allocated docbuffer                           */
	if (docbuffer != doc->buffer) {
	  FREE(docbuffer);
	}
	dtd = xml_schema_2_dtd(doc, first_schema);

      }
      else {
	if (LOGFILE)
	  fprintf(LOGFILE, "Unable to open XMLSchema '%s' in sgml_parse_dtd\n",
		  schema_file_name);
	else
	  fprintf(stderr,  "Unable to open XMLSchema '%s' in sgml_parse_dtd\n",
		  schema_file_name);
	cheshire_exit (0); /* might as well exit, because the dtd is mandatory */
      }
      if (first_schema) {
	char *lastslash;
	/* the first dtd is the primary one... */
	primary_schema_dtd = dtd;
	currfl = fl;
	first_schema = 0;
	
#ifndef WIN32
	lastslash = strrchr(schema_file_name, '/');
#else
	lastslash = strrchr(schema_file_name, '\\');
#endif 
	if (lastslash != NULL)
	  lastslash++;
	else
	  lastslash = schema_file_name;
	
	primary_schema_dtd->DTD_name = strdup(lastslash);
	primary_schema_dtd->DTD_system_name = strdup(filename);
	primary_schema_dtd->DTD_file_name = strdup(schema_file_name);
      }
      else {
	currfl = currfl->next_filename;	  
	/* merge the new dtd into the primary dtd... (yeah I know...) */
	xml_schema_merge_schemas(primary_schema_dtd, dtd);
      }
      if (currfl != NULL) {
	schema_file_name = currfl->filename;
      }
      else { 
	schema_file_name = NULL;
      }

    } while (schema_file_name != NULL);

  }
  else {
    start_dtd_parsing(); /* set for DTD input */

    sgml_dtd_parse();
  }
  sgml_cleanup_parser();

  fclose(sgml_in);

  if (sgml_parse_error) {
    fprintf(LOGFILE,"SGML parse error in DTD: %s\n",filename);
    if (catalog_found == 0)
      fprintf(LOGFILE,"Possibly missing SGML Catalog?\n");

    return (NULL);
  }
  else {
    if (dtd_type == 2) {
      current_sgml_dtd = primary_schema_dtd;
    }
    current_sgml_dtd->sgml_declaration = current_sgml_declaration;
    return(current_sgml_dtd);
  }

}


SGML_Document *
sgml_parse_document(SGML_DTD *dtd, char *filename, char *buffer, 
		    int recordnum, int keep_buffer)
{
  extern SGML_DTD *current_sgml_dtd, *cf_getDTD();
  extern SGML_Declaration *current_sgml_declaration;
  extern SGML_Document *current_sgml_document;
  extern SGML_Data *error_sgml_data;
  extern char *temp_parse_buffer;
  extern int temp_parse_buffer_differs;
  extern int parsed_file_chars;
  extern FILE *sgml_in;
  extern int sgml_parse_error;
  extern int current_record_id_number;
  extern char *current_full_file_name;

  current_record_id_number = recordnum;

  if (filename == NULL) {
    fprintf(LOGFILE, "Null filename in sgml_parse_document\n");
    return(NULL);
  }

  if (dtd == NULL) {
    /* find the matching dtd and parse it if needed */
    current_sgml_dtd = dtd = cf_getDTD(filename);
    current_sgml_declaration = dtd->sgml_declaration;
  }
  else if (dtd != NULL) {
    current_sgml_dtd = dtd;
    current_sgml_declaration = dtd->sgml_declaration;
  }
  
  if (buffer == NULL) {	
    /* all parsing must be done from a buffer now */
    fprintf(LOGFILE, "NULL buffer in sgml_parse_doc (not allowed)\n");
    return (NULL);
  }
    
  /* set display options codes in dtd */
  current_sgml_dtd->display_options = cf_getdispopt(filename);

  /* create a new document and list element */
  if ((current_sgml_document = CALLOC(SGML_Document,1)) == NULL)
    {
      fprintf(LOGFILE,"Unable to allocate SGML_Document\n");
      cheshire_exit (1);
    }
	
  /* initialize some elements of the document structure   */
  /* for now assume that the current SGML declaration and */
  /* DTD are the correct ones for this document           */
  /* if not, the parser will fail anyway.                 */
  current_sgml_document->DCL = current_sgml_declaration;
  current_sgml_document->DTD = current_sgml_dtd;
  /* the incoming buffer will be changed by parser if anything */
  /* like character references or entity references occur in it*/
  current_sgml_document->buffer = buffer; 
	
  /* the following SHOULD be the cf_name */
  if (keep_buffer == -1 || recordnum == 0)
    current_sgml_document->file_name = filename;
  else
    current_sgml_document->file_name = cf_getdatafilename(filename, recordnum);
  current_sgml_document->record_id = recordnum;
  current_sgml_document->file_offset = 0;
  current_sgml_document->data = NULL;
  
  /* set the scanner conditions for a document */
  sgml_init_parser();
    
  start_document_parsing();

  /* separate in-document DTD elements and parse them    */
  /* and eliminate (substitute) entity references in the */
  /* document buffer                                     */
  sgml_elim_entities(current_sgml_document);
      
  if (current_sgml_document->buffer != buffer && keep_buffer == 0) /* changed buffer */
    FREE(buffer);

  /*  just for checking the entity substitution ...
   *  printf("******************** DUMP OF BUFFER ***************\n%s\n\n",
   *  current_sgml_document->buffer);
   */

  /* parse the doc from the buffer (may have changed in elim entities */
  string_buffer_switch (current_sgml_document->buffer,0 /* not nested */);

  /* call the entry for documents in sgml_gram.g */
  sgml_doc_parse();

  if (sgml_parse_error == 1) {
    free_error_doc(current_sgml_document);
    sgml_cleanup_parser();
    /* keep track of the last document */
    if(error_sgml_data)
      FREE(error_sgml_data);
    error_sgml_data = NULL;
    return NULL;
  }

  /* keep track of the last document */
  if (PRS_first_doc == NULL && PRS_current_doc != NULL)
    PRS_last_doc = NULL;
  else
    PRS_last_doc = PRS_current_doc;
    
  if ((PRS_current_doc = CALLOC(SGML_Doclist,1)) == NULL)
    {
      fprintf(LOGFILE,"Unable to allocate SGML_Doclist element\n");
      cheshire_exit (1);
    }
  
  /* keep track of parsed documents so they can be reused */
  /* or freed or whatever                                 */
  if (PRS_first_doc == NULL) 
    {
      PRS_first_doc = PRS_current_doc;
    }
  if (PRS_last_doc) 
    {
      PRS_last_doc->next_doc = PRS_current_doc;
    }
  PRS_current_doc->doc = current_sgml_document;

  if (temp_parse_buffer_differs) {
    /* this copy ensures that any changes in parsing (additional entity */
    /* expansions, etc.) are included in the buffer          */
    if (strlen(current_sgml_document->buffer) >= strlen(temp_parse_buffer))
      strcpy(current_sgml_document->buffer, temp_parse_buffer);
    else { /* new buffer is larger than the old one... */
      FREE(current_sgml_document->buffer);
      current_sgml_document->buffer = strdup(temp_parse_buffer);
    }
  }



  /* cleanup and free parser stuff */
  sgml_cleanup_parser();
  

  /* build the pointers into the buffer from the offsets */
  /* and build the document's tag hash table             */
  /* and build processing instruction links to tags      */
  link_sgml_data(current_sgml_document, current_sgml_document->data, 
		 current_sgml_document->buffer,
		 current_sgml_document->Processing_Inst);

#ifdef DEBUGPARSER
  Dump_tag_hash(current_sgml_document->Tag_hash_tab);
#endif

  return (current_sgml_document);
    
}


#ifdef DEBUGPARSER

int
Dump_tag_hash(Tcl_HashTable *hash_tab) 
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  char *tag_ptr;
  SGML_Data *dat;
  SGML_Tag_Data *t;
    
  if (hash_tab == NULL)
    return 0;

  for (entry = Tcl_FirstHashEntry(hash_tab,
				  &hash_search); 
       entry != NULL; 
       entry = Tcl_NextHashEntry(&hash_search)) {
    /* get the tag */
    tag_ptr = Tcl_GetHashKey(hash_tab,entry); 
    /* and the list of tags info */
    printf("\nTag: %s is located at offsets:", tag_ptr);
    for (t =  (struct SGML_Tag_Data  *) Tcl_GetHashValue(entry);
	 t != NULL; t = t->next_tag_data) {
      printf(" %d", t->item->start_tag_offset);
    }
    printf("\n");
  }
}	

#endif

