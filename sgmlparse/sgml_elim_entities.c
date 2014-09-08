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
*	Header Name:	sgml_elim_entities.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to extract and parse in-document
*                       DTD declarations, and to substitute entities
*                       in the supplied document.
*	Usage:		
*          to parse an SGML in-doc DTD subset declaration ( optional)
*
*            int sgml_elim_entities(SGML_Document *);
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		5/3/2000
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2000.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifndef WIN32
#include <strings.h>
#else
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
#include "sgml.h"
#include <sys/stat.h>

#include "dmalloc.h"

/* #define DEBUGDOC */

#define ALLOC_ERROR 100
#define UNDEFINED_TAG_ERROR 200
#define PARSE_ERROR 1
#define INFORM_ERROR 0

/* this looks for files in multiple places */
extern char *build_full_name( char *in_name);
extern int string_buffer_switch (char *new_contents, int nested);
extern int sgml_parse_error; /* in sgml_gram.g */


int
sgml_elim_entities(SGML_Document *doc)
{
  char *docstart;
  int startfound = 0;
  char *buffer, *orig_buffer, *prolog_buf, *old_buf;
  char *entity_start, *new_entity_start, *entity_end, *realloc_buf;
  int orig_buffsize, prolog_size, current_size, entity_size;
  int prolog_entities_flag = 0;
  char refname[200];
  char testname[200];
  int i, j, innumchar;
  int offset, reflen;
  char *c, *test, *testend;
  extern SGML_Data *error_sgml_data;
  Tcl_HashEntry *hash_entry; 
  SGML_Entity *ent;
  struct stat filestatus;
  char *entfilename;
  FILE *entfile;
  extern FILE *LOGFILE;
  int in_comment;
  int import_flag;
  char *imported[200];
  int import_count = 0;
  if (doc->buffer == NULL)
    return 0;

  if (doc->DTD && (doc->DTD->display_options & 16) == 16) {
    /* this is the KEEP_ENTITIES flag, so no substitutions will be done */
    return 1;
  }

  /* the first task is to scan the buffer and find where the actual   */
  /* start tag really is located -- then split the buffer and do      */
  /* DTD subset parsing on the DTD part to get any local entity defs  */
  /* or other DTD subset stuff and add it to the DTD definition       */
  /* The document structure should be pointing to the the current DTD */
  orig_buffsize =  strlen(doc->buffer);
  orig_buffer = doc->buffer;

  in_comment = 0;
  
  for (i=0; i<200; i++) 
    imported[i] = NULL;

  for (docstart = doc->buffer; !startfound && *docstart ; docstart++) {

    if (*docstart == '<' && *(docstart+1) == '!' && *(docstart+2) == '-'
	&& *(docstart+3) == '-') {
      docstart += 3;
      in_comment = 1;
      while (in_comment) {
	if (*docstart == '-' && *(docstart+1) == '-' 
	    && *(docstart+2) == '>')
	  in_comment = 0;
	docstart++;
      } 
      docstart += 2;
    } else if (*docstart == '<') {
      if ((strncasecmp((docstart+1), "!ENTITY", 7) == 0)) 
	prolog_entities_flag = 1;
      else if ((strncasecmp((docstart+1), "?CHESHIRE", 9) == 0)) 
	prolog_entities_flag = 1;
      else if (isalpha(*(docstart+1))) {
	startfound = 1;
      }
    }
  }
  
  if (startfound) 
    docstart--;
  else 
    return 0;

  if (prolog_entities_flag) {
    /* there are entity declarations in the document prolog that */
    /* need to be expanded and added to the DTD                  */
    prolog_size = docstart - doc->buffer;
    prolog_buf = CALLOC(char, prolog_size+3);
    memcpy(prolog_buf, doc->buffer, prolog_size);
    doc->buffer = prolog_buf;

    /* now parse it... */

    string_buffer_switch (doc->buffer,0 /* not nested */);

    /* call the entry for documents in sgml_gram.g */
    sgml_doc_parse();

    if (sgml_parse_error == 1) {
      sgml_cleanup_parser();
      /* keep track of the last document */
      if(error_sgml_data)
	FREE(error_sgml_data);
      error_sgml_data = NULL;
    }

    /* reset the scanner conditions for a document */
    sgml_init_parser();
    
    start_document_parsing();
    FREE(prolog_buf);

  }
  
  /* We should have handled the local prolog declarations now, so go */
  /* ahead and replace entity references                             */
  /* we only match GENERAL and CHARACTER entity refs in the doc      */
  /* the parameter entities in the DTD have already been done        */
  /* first create a buffer without the prolog */
  current_size = strlen(docstart);
  buffer = CALLOC(char, strlen(docstart)+1);
  strcpy(buffer, docstart);
  /* we can't toss the original buffer now -- it may be freed by callers */
  /* FREE(orig_buffer); */
  doc->buffer = buffer;

  entity_start = strchr(buffer, '&');

  while (entity_start) {

    for (i = 0, entity_end = entity_start+1; 
	 *entity_end != ' ' && *entity_end != ';' && *entity_end != '<' && i < 199;
	 entity_end++, i++)
      refname[i] = *entity_end;
    
    refname[i] = '\0';
    reflen = i+2;
    
    sprintf(testname, "&%s;", refname);

    /* have the name - now match it */

    /* no longer case insensitive */
    /* for (c = refname; *c; c++) *c = toupper(*c); */

    if (refname[0] == '\0') {
      hash_entry = NULL;
    }
    else
      hash_entry = Tcl_FindHashEntry(
				     doc->DTD->SGML_Entity_hash_tab,
				     refname);

    if (hash_entry == NULL) {
      if (*refname == '#') {
	/* Numeric character reference */
	innumchar = 0;
	*refname = '0';
	if (*(refname+1) == 'x') {
	  /* hex reference */
	  innumchar = (int)strtol(refname, &c, 16);
	}
	else if (isdigit(*(refname+1))) {
	  innumchar = (int)atoi(refname);	  
	}
	else {
	  if (strcmp(refname, "amp") == 0
	      || strcmp(refname, "lt") == 0
	      || strcmp(refname, "gt") == 0
	      || strcmp(refname, "quot") == 0
	      || strcmp(refname, "apos") == 0) {
	    /* These are undeclared XML entities */
	    /* that can be ignored               */
	  }
	  else {
#ifdef DEBUG_ENTITIES
	    fprintf(LOGFILE,"Entity '%s' not defined in record %d -- continuing\n",
		    refname, doc->record_id);
#endif
	  }
	  entity_start++; /* skip it and try again in the parser proper */
	}
	/* insert the character move up and change the buffer size */
	*entity_start = (char)innumchar;
	memccpy((entity_start+1), entity_end+1,'\0', 
		current_size - (entity_start - buffer));
      }
      else {
	if (strcmp(refname, "amp") == 0
	    || strcmp(refname, "lt") == 0
	    || strcmp(refname, "gt") == 0
	    || strcmp(refname, "quot") == 0
	    || strcmp(refname, "apos") == 0) {
	  /* These are undeclared XML entities */
	  /* that can be ignored               */
	}
	else {
#ifdef DEBUG_ENTITIES
	  if (refname[0] != 0)
	    fprintf(LOGFILE,"Entity '%s' not defined in record %d -- continuing\n",
		    refname, doc->record_id);
#endif
	}
	entity_start++; /* skip it and try again in the parser proper */
      }
    }
    else { /* found the entity in the hash table */
      ent = (	SGML_Entity *) Tcl_GetHashValue(hash_entry);
      /* have an entity struct */
#ifdef DEBUGDOC
      if ( ent != NULL) {
	if (ent->name != NULL)
	  printf("Doc #%d: Entity name %s type %d ", 
		 doc->record_id, ent->name, ent->type);
	
	if (ent->entity_text_string != NULL)
	  printf("string '%s' ", ent->entity_text_string);
	
	if (ent->public_name != NULL)
	  printf("pub %s ", ent->public_name);
	
	if (ent->system_name != NULL)
	  printf("sys %s", ent->system_name); 
	
	printf("\n");
      }
      
#endif
    
      switch (ent->entity_text_type) {
      case PARAMETER_LIT_TEXT:
      case DATA_TEXT:
      case BRACKETED_TEXT:
	/* all of these we treat the same way... */
	if (ent->entity_text_string == NULL) {
	  /* public entity with no system reference */
	  SGML_ERROR(PARSE_ERROR,"Entity '",
		     refname,"' No text string defined ","");
	  return 0;
	}
	
	/* Check for (simple) recursive definition */
	if (strcmp(testname,ent->entity_text_string) != 0) {
	  current_size = strlen(buffer);
	  entity_size = strlen(ent->entity_text_string);

	  /* shift the buffer down the size of the entity */
	  if (entity_size < reflen) {
	    memmove(entity_start,ent->entity_text_string, entity_size);
	    memmove(entity_start+entity_size,entity_start+reflen, current_size - (((entity_start+reflen) - buffer) - 1));
	  }
	  else if (entity_size == reflen) {
	    memmove(entity_start,ent->entity_text_string, entity_size);
	  }
	  else {
	    /* the data is longer than the original buffer so reallocate */
	    realloc_buf = CALLOC(char, current_size + entity_size);
	    offset = entity_start - buffer;
	    new_entity_start = realloc_buf + offset;
	    memmove(realloc_buf, buffer, offset);
	    memmove(new_entity_start,ent->entity_text_string, entity_size);
	    memmove(new_entity_start+entity_size,entity_start+reflen, 
		    (current_size - (((entity_start+reflen) - buffer) - 1)));
	    old_buf = buffer;
	  buffer = realloc_buf;
	  entity_start = new_entity_start;
	  FREE(old_buf);
	  }
	  c = entity_start + entity_size; /* for testing only */
	  test = c - 70;
	  testend = buffer + (current_size - 40);
	} 
	else {
	  fprintf(LOGFILE,"WARNING: Entity '%s' has recursive definition %s in record %d -- No substitution done\n",
		    refname, testname, doc->record_id);
	  fprintf(LOGFILE,"WARNING: Further references to Entity '%s' will be ignored \n",
		    refname);
	  entity_start++;
	  /* remove the entity from the hash table */
	  Tcl_DeleteHashEntry(hash_entry);
	  
	}
	break;

      case EXTERNAL_ENTITY_SPEC:
	
	if (ent->entity_text_subtype == PUBLIC_IDENT) {
	  if (ent->system_name == NULL) {
	    /* public entity with no system reference */
	    SGML_ERROR(PARSE_ERROR,"Entity '",
		       refname,"' No system location for PUBLIC ID: ", 
		       ent->public_name);
	    return (0);
	    
	  }
	}
	/* just take the system file name */
	entfilename = build_full_name(ent->system_name);
	
	if (stat(entfilename, &filestatus) != 0) {
	  fprintf(LOGFILE,
		  "SGML_Elim: Stat for ENTITY Include data file '%s' failed to find file in record %d\n",
		  ent->system_name, doc->record_id);
	  exit(1);
	  
	}
	entity_size = (int)filestatus.st_size;
	current_size += entity_size; /* this leaves some, since ref is counted */
	realloc_buf = REALLOC(buffer, char, current_size);

	if (realloc_buf != buffer) {
	  /* changed memory locations */
	  offset = entity_start - buffer;
	  entity_start = realloc_buf + offset;
	  entity_end = realloc_buf + (offset + reflen) - 1;
	  buffer = realloc_buf;
	}

	c = entity_start + entity_size; /* for testing only */
	test = c - 70;
	testend = buffer + (current_size - 40);

	/* shift the buffer down the size of the entity */
	memmove(entity_start+entity_size, entity_end+1, strlen(entity_end)+1);
	entfile = fopen(entfilename, "r");
	/* read the entire file into the buffer area */
	fread(entity_start, entity_size, 1, entfile);

	fclose(entfile);
	FREE(entfilename);
	
	break;
      }
    }
    entity_start = strchr(entity_start, '&');
  }

  /* if this is an XML Schema document we do a second pass to resolve */
  /* imports of other schema elements                                 */
  if (doc->DTD->type == 3) {

    import_flag = 1;
    entity_start = strstr(buffer, "import ");
    if (entity_start == NULL) {
      import_flag = 0;
      entity_start = strstr(buffer, "include ");
    }
    
    while (entity_start) {
      char *last_start;
      char *import_file_name;
      char *full_import_name;
      char *loc_start, *loc_buffer, *c, *sch_endtag;
      
      last_start = entity_start;

      /* do a bit a verification */
      if (*(entity_start - 1) != ':' && 
	  *(entity_start - 1) != '<') {
	import_flag = 1;
	entity_start = strstr(entity_start+1, "import ");
	if (entity_start == NULL) {
	  import_flag = 0;
	  entity_start = strstr(last_start+1, "include ");
	} 

	continue;
      }
      
      
      /* get the schemaLocation from the tag */      
      /* first locate the end of the import or include */
      if (entity_end = strstr(entity_start, "import>")) {
	/* found it */
	entity_end += 7;
      }
      else if (entity_end = strstr(entity_start, "include>")) {
	entity_end += 8;
      }
      else if (entity_end = strstr(entity_start, "/>")) {
	entity_end += 2;
      }
      
      loc_buffer = CALLOC(char, entity_end - entity_start + 1);
      memcpy(loc_buffer, entity_start, entity_end - entity_start);
      
      loc_start = strstr(loc_buffer, "schemaLocation"); 
      
      for (import_file_name = loc_start + 15; 
	   *import_file_name == ' ' || *import_file_name == '=' 
	     || *import_file_name == '"' ; import_file_name++);
      for (c = import_file_name; *c != '"'; c++);
      *c = '\0';
      
      /* just take the system file name */
      full_import_name = build_full_name(import_file_name);


      for (j=0;j < import_count; j++) {
	if (imported[j] != NULL && strcmp(full_import_name,imported[j]) == 0) {
	  /* already imported this one */
	  FREE(full_import_name);
	  full_import_name = NULL;
	  break;
	}
      }

      if (full_import_name != NULL) { 
	/* import the added schema file */
	if (stat(full_import_name, &filestatus) != 0) {
	  fprintf(LOGFILE,
		  "SGML_Elim: Failed to find XML Schema import or include data file '%s'\n",
		  full_import_name);
	  exit(1);
	
	}
      

	entity_size = (int)filestatus.st_size;
	current_size += entity_size + 20; /* this leaves some, since ref is counted */
	realloc_buf = REALLOC(buffer, char, current_size);
      
	if (realloc_buf != buffer) {
	  /* changed memory locations */
	  offset = entity_start - buffer;
	  reflen = entity_end - entity_start;
	  last_start = entity_start = realloc_buf + offset;
	  entity_end = realloc_buf + (offset + reflen);
	  buffer = realloc_buf;
	}
      
	c = entity_end + entity_size; /* for testing only */
	test = c - 70;
	testend = buffer + (current_size - 40);

	/* shift the buffer down the size of the entity */
	memmove(entity_end+entity_size, entity_end, strlen(entity_end)+1);
	entfile = fopen(full_import_name, "r");
	/* read the entire file into the buffer area */
	fread(entity_end, entity_size, 1, entfile);
      
	fclose(entfile);
	/* FREE(full_import_name); */      
	imported[import_count++] = full_import_name;

	/* now we go in and clobber the schema tags in the included file... */
	/* this is dubious code -- it may go wrong very easily */
	c = strstr(last_start, "schema ");
	if (c > entity_end && c < (entity_end + entity_size)) {
	  while (*c != '<') c--;
	  while (*c != '>') *c++ = ' ';
	  *c = ' ';
	}

	sch_endtag = strstr(c,"schema>");
	if (sch_endtag > entity_end 
	    && sch_endtag < (entity_end + entity_size)) {
	  while (*sch_endtag != '<') sch_endtag--;
	  for (c = sch_endtag; c < (sch_endtag + 9); c++) *c = ' ';
	}
      }
	/* locate the next import or include section ... */
      import_flag = 1;
      entity_start = strstr(last_start+1, "import ");
      if (entity_start == NULL) {
	import_flag = 0;
	entity_start = strstr(last_start+1, "include ");
      }
    }
  }
  
  doc->buffer = buffer;
  
  /* eliminate the import file names */

  for (i=0; i<200; i++) {
    if (imported[i] != NULL) 
      FREE(imported[i]);
    if (imported[i] == NULL)
      break;
  }

  return (1); /* success */
  
}


