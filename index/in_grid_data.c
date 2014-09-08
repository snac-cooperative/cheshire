/*
 *  Copyright (c) 1990-2009 [see Other Notes, below]. The Regents of the
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
 *  Created by Ray R. Larson
 *             School of Information 
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
*	Header Name:	in_grid_data.c
*
*	Programmer:	Ray Larson
*
*	Purpose:	Dumping intermediate processing elements for the
*                       CLEF Grid task
*                       
*
*	Usage:		
*
*	Variables:	
*
*
*	Date:		7/21/2009
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2009.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

/* #define DEBUGIND */

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include "cheshire.h"
#include "configfiles.h"
#include "grid_defs.h"


extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

extern SGML_Tag_Data *exclude_tag_data( SGML_Tag_Data *exclude_tags, 
				 SGML_Tag_Data *data_tags);

extern int idxdateattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);


extern int idxdate(SGML_Data *data,
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);

extern int idxlatlongattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);


extern int idxlatlong(SGML_Data *data,
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);

extern int idxattr(SGML_Data *data, char *attr_name, Tcl_HashTable *hash_tab,
		   idx_list_entry *idx);

extern int idx008(SGML_Data *data, char *attr_name, Tcl_HashTable *hash_tab,
		  idx_list_entry *idx);

extern int idxextern(SGML_Data *data, Tcl_HashTable *hash_tab,
		     idx_list_entry *idx,
		     int batch_flag, idx_key *replacekey);

extern int idxpages(SGML_Data *data, Tcl_HashTable *hash_tab, 
		    int recnum, idx_list_entry *idx, int batch_flag);

extern int idxdata_grid(SGML_Data *data, Tcl_HashTable *hash_tab, 
			idx_list_entry *idx, SGML_Document *sgml_rec);


extern int idxxkey( SGML_Data *data, Tcl_HashTable *hash_tab,
		    idx_list_entry *idx,
		    char **current_buffer, int addkey);

extern void free_tag_data_list(SGML_Tag_Data *in_tags);


/* files for output  defined in grid_defs.h */
FILE *GRID_TOKENS_FILE;
FILE *GRID_LOWERNORM_FILE;
FILE *GRID_STEM_FILE;
FILE *GRID_STOP_FILE;
FILE *GRID_HYPHEN_FILE;

#define TOKENS_FILE_NAME "/projects/cheshire/DATA/GRID/OUTPUT/GRID_TOKENS_OUT"
static int TOKENS_FILE_OPEN=0;
static int TOKEN_ID=0;
static int LAST_TOKEN_RECID=0;


#define LOWER_FILE_NAME "/projects/cheshire/DATA/GRID/OUTPUT/GRID_LOWERNORM_OUT"
static int LOWER_FILE_OPEN=0;
static int LOWER_ID=0;
static int LAST_LOWER_RECID=0;

#define STEM_FILE_NAME "/projects/cheshire/DATA/GRID/OUTPUT/GRID_STEMS_OUT"
static int STEM_FILE_OPEN=0;
static int STEM_ID=0;
static int LAST_STEM_RECID=0;


#define STOP_FILE_NAME "/projects/cheshire/DATA/GRID/OUTPUT/GRID_STOP_OUT"
static int STOP_FILE_OPEN=0;
static int STOP_ID=0;
static int LAST_STOP_RECID=0;


#define HYPHEN_FILE_NAME "/projects/cheshire/DATA/GRID/OUTPUT/GRID_HYPHEN_OUT"
static int HYPHEN_FILE_OPEN=0;
static int HYPHEN_ID=0;
static int LAST_HYPHEN_RECID=0;

static int last_doc_number=0;


/* finally -- the actual program... */

int close_grid_data()
{
  if (TOKENS_FILE_OPEN) {
    fprintf(GRID_TOKENS_FILE, "				</tokens>\n");
    fprintf(GRID_TOKENS_FILE, "			</resource>");
    fprintf(GRID_TOKENS_FILE, "		</resources>");
    fprintf(GRID_TOKENS_FILE, "	</stream>");
    fprintf(GRID_TOKENS_FILE, "</circo>");
    fclose(GRID_TOKENS_FILE);
  }
  if (LOWER_FILE_OPEN) {
    fprintf(GRID_LOWERNORM_FILE, "				</tokens>\n");
    fprintf(GRID_LOWERNORM_FILE, "			</resource>");
    fprintf(GRID_LOWERNORM_FILE, "		</resources>");
    fprintf(GRID_LOWERNORM_FILE, "	</stream>");
    fprintf(GRID_LOWERNORM_FILE, "</circo>");
    fclose(GRID_LOWERNORM_FILE);
  }

  if (STEM_FILE_OPEN) {
    fprintf(GRID_STEM_FILE, "				</tokens>\n");
    fprintf(GRID_STEM_FILE, "			</resource>");
    fprintf(GRID_STEM_FILE, "		</resources>");
    fprintf(GRID_STEM_FILE, "	</stream>");
    fprintf(GRID_STEM_FILE, "</circo>");
    fclose(GRID_STEM_FILE);
  }
  if (STOP_FILE_OPEN) {
    fprintf(GRID_STOP_FILE, "				</tokens>\n");
    fprintf(GRID_STOP_FILE, "			</resource>");
    fprintf(GRID_STOP_FILE, "		</resources>");
    fprintf(GRID_STOP_FILE, "	</stream>");
    fprintf(GRID_STOP_FILE, "</circo>");
    fclose(GRID_STOP_FILE);
  }

  if (HYPHEN_FILE_OPEN) {
    fprintf(GRID_HYPHEN_FILE, "				</tokens>\n");
    fprintf(GRID_HYPHEN_FILE, "			</resource>");
    fprintf(GRID_HYPHEN_FILE, "		</resources>");
    fprintf(GRID_HYPHEN_FILE, "	</stream>");
    fprintf(GRID_HYPHEN_FILE, "</circo>");
    fclose(GRID_HYPHEN_FILE);
  }


}

int in_grid_data(SGML_Document *sgml_rec, SGML_Data *sgml_dat, int which_file, char *data, int token_offset, idx_list_entry *idx)
{
  int i;
  int attribute_flag = 0;
  int casesensitive;
  time_t time_t_data;
  int newresource=0;



  if (sgml_rec == NULL)
    return (0);
  
  switch (which_file) {
    
  case GRID_TOKENS :
    if (TOKENS_FILE_OPEN==0) {
      /* open the file */
      GRID_TOKENS_FILE = fopen(TOKENS_FILE_NAME, "w");
      TOKENS_FILE_OPEN = 1;
      
      fprintf(GRID_TOKENS_FILE, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fprintf(GRID_TOKENS_FILE, "<circo xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://circo.dei.unipd.it/\" xsi:schemalocation=\"http://circo.dei.unipd.it/ http://ims.dei.unipd.it/xml/circo-schema-instance\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n");
      fprintf(GRID_TOKENS_FILE, "  <metadata>\n    <dc:creator>\n      Cheshire II Grid Version\n    </dc:creator>\n    <dc:rights>\n       Copyright (c) 1990-2009 Regents of the University of California, All Rights Reserved. \n    </dc:rights>\n");
      time(&time_t_data);
      fprintf(GRID_TOKENS_FILE, "    <dc:date>\n      %s \n    </dc:date>\n", ctime(&time_t_data));
      fprintf(GRID_TOKENS_FILE, "  </metadata>\n");
      
      fprintf(GRID_TOKENS_FILE, "  <stream identifier=\"Cheshire_Raw_Tokens_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\">\n                <component identifier=\"cheshire_idxdata1\" type=\"tokenizer\" description=\"A tokenizer separates an input document into a stream of tokens.\">\n                        <actor identifier=\"Larson\" />\n                </component>", sgml_rec->file_name);
      fprintf(GRID_TOKENS_FILE,"         <resources>\n                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Raw_Tokens_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      fprintf(GRID_TOKENS_FILE,"      <tokens>\n");
      LAST_TOKEN_RECID = sgml_rec->record_id;
    }

    if (LAST_TOKEN_RECID != sgml_rec->record_id) {
      /* treat each record as a new resource */
      LAST_TOKEN_RECID = sgml_rec->record_id;

      fprintf(GRID_TOKENS_FILE, "				</tokens>\n");
      fprintf(GRID_TOKENS_FILE, "			</resource>\n");
      fprintf(GRID_TOKENS_FILE,"                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Raw_Tokens_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      fprintf(GRID_TOKENS_FILE,"      <tokens>\n");
      
    }

    fprintf(GRID_TOKENS_FILE,"       <token identifier=\"Cheshire_Raw_Tokens_%s-%d-%d\" value=\"%s\">\n", sgml_rec->file_name, sgml_rec->record_id, TOKEN_ID++, data);
    
    fprintf(GRID_TOKENS_FILE,"            <resource identifier=\"%s-%d\" mime-type=\"text/plain\"/>\n  ", sgml_rec->file_name, sgml_rec->record_id);
    
    fprintf(GRID_TOKENS_FILE,"      </token>\n");

    break;
  case GRID_LOWERNORM :
	    
    if (LOWER_FILE_OPEN==0) {
      /* open the file */
      GRID_LOWERNORM_FILE = fopen(LOWER_FILE_NAME,"w");
      LOWER_FILE_OPEN = 1;
      
      fprintf(GRID_LOWERNORM_FILE, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fprintf(GRID_LOWERNORM_FILE, "<circo xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://circo.dei.unipd.it/\" xsi:schemalocation=\"http://circo.dei.unipd.it/ http://ims.dei.unipd.it/xml/circo-schema-instance\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n");
      
      fprintf(GRID_LOWERNORM_FILE, "  <metadata>\n    <dc:creator>\n      Cheshire II Grid Version\n    </dc:creator>\n    <dc:rights>\n       Copyright (c) 1990-2009 Regents of the University of California, All Rights Reserved. \n    </dc:rights>\n");
      time(&time_t_data);
      fprintf(GRID_LOWERNORM_FILE, "    <dc:date>\n      %s \n    </dc:date>\n", ctime(&time_t_data));
      
      fprintf(GRID_LOWERNORM_FILE, "  </metadata>\n");
      
      fprintf(GRID_LOWERNORM_FILE, "  <stream identifier=\"Cheshire_Lowercase_Tokens_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\">\n                <component identifier=\"cheshire_ToLowerCase\" type=\"tokenizer\" description=\"Converts individual input tokens into lowercase versions of those tokens.\">\n                        <actor identifier=\"Larson\" />\n                </component>", sgml_rec->file_name);
      
      fprintf(GRID_LOWERNORM_FILE,"         <resources>\n                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Lowercase_Tokens_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      
      fprintf(GRID_LOWERNORM_FILE,"      <tokens>\n");
      LAST_LOWER_RECID = sgml_rec->record_id;      
    }
    
    if (LAST_LOWER_RECID != sgml_rec->record_id) {
      /* treat each record as a new resource */
      LAST_LOWER_RECID = sgml_rec->record_id;

      fprintf(GRID_LOWERNORM_FILE, "				</tokens>\n");
      fprintf(GRID_LOWERNORM_FILE, "			</resource>\n");
      fprintf(GRID_LOWERNORM_FILE,"                     <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Lowercase_Tokens_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);      
      fprintf(GRID_LOWERNORM_FILE,"      <tokens>\n");
    }

    fprintf(GRID_LOWERNORM_FILE,"       <token identifier=\"Cheshire_Lowercase_Tokens_%s-%d-%d\" value=\"%s\">\n", sgml_rec->file_name, sgml_rec->record_id, LOWER_ID++, data);
    
    fprintf(GRID_LOWERNORM_FILE,"            <resource identifier=\"%s-%d\" mime-type=\"text/plain\"/>\n  ", sgml_rec->file_name, sgml_rec->record_id);
    
    fprintf(GRID_LOWERNORM_FILE,"      </token>\n");

  break;
  case GRID_STEM :

    if (STEM_FILE_OPEN==0) {
      /* open the file */
      GRID_STEM_FILE = fopen(STEM_FILE_NAME,"w");
      STEM_FILE_OPEN = 1;
      
      fprintf(GRID_STEM_FILE, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fprintf(GRID_STEM_FILE, "<circo xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://circo.dei.unipd.it/\" xsi:schemalocation=\"http://circo.dei.unipd.it/ http://ims.dei.unipd.it/xml/circo-schema-instance\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n");
      
      fprintf(GRID_STEM_FILE, "  <metadata>\n    <dc:creator>\n      Cheshire II Grid Version\n    </dc:creator>\n    <dc:rights>\n       Copyright (c) 1990-2009 Regents of the University of California, All Rights Reserved. \n    </dc:rights>\n");
      time(&time_t_data);
      fprintf(GRID_STEM_FILE, "    <dc:date>\n      %s \n    </dc:date>\n", ctime(&time_t_data));
      
      fprintf(GRID_STEM_FILE, "  </metadata>\n");
      
      fprintf(GRID_STEM_FILE, "  <stream identifier=\"Cheshire_Stems_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\">\n", sgml_rec->file_name);
      
      if (idx->type & STEMS) {
	
	fprintf(GRID_STEM_FILE, "              <component identifier=\"cheshireDefault_Stemmer\" type=\"stemmer\" description=\"Stemmer based on the Porter stemmer with custom additions\">\n                        <actor identifier=\"Larson\" />\n                </component>");
	
      }

      if (idx->type & SSTEMS) {
	
	fprintf(GRID_STEM_FILE, "              <component identifier=\"cheshireS_Stemmer\" type=\"stemmer\" description=\"Simple English Plural Stemmer\">\n                        <actor identifier=\"Larson\" />\n                </component>");
	
      }


      if (idx->type & SNOWBALL_STEMS) {
	char *language=NULL;
	
	switch (idx->snowball_stem_type) {
	case SNOWBALL_PORTER :
	  language = "Porter";
	  break;
	case SNOWBALL_ENGLISH :
	  language = "English";	  
	  break;
	case SNOWBALL_FRENCH :
	  language = "French";
	  
	  break;
	case SNOWBALL_GERMAN :
	  language = "German";
	  
	  break;
	case SNOWBALL_DUTCH :
	  language = "Dutch";
	  
	  break;
	case SNOWBALL_SPANISH :
	  language = "Spanish";
	  
	  break;
	case SNOWBALL_ITALIAN :
	  language = "Italian";
	  
	  break;
	case SNOWBALL_SWEDISH :
	  language = "Swedish";
	  
	  break;
	case SNOWBALL_PORTUGUESE :	
	  language = "Portuguese";
	  
	  break;
	case SNOWBALL_RUSSIAN :
	  language = "Russian";
	
	  break;
	case SNOWBALL_DANISH :
	  language = "Danish";
	  
	  break;
	case SNOWBALL_NORWEGIAN :
	  language = "Norwegian";
	  
	  break;
	  
	}
	fprintf(GRID_STEM_FILE, "              <component identifier=\"cheshire_Snowball_Stemmer\" type=\"stemmer\" description=\"Cheshire Snowball Stemmer inplementation for %s stemming\">\n                        <actor identifier=\"Larson\" />\n                </component>", language);
      }

      fprintf(GRID_STEM_FILE,"         <resources>\n                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Stems_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      
      fprintf(GRID_STEM_FILE,"      <tokens>\n");
      /* end of header info for file */
      LAST_STEM_RECID = sgml_rec->record_id;      
    }
    
    if (LAST_STEM_RECID != sgml_rec->record_id) {
      /* treat each record as a new resource */
      LAST_STEM_RECID = sgml_rec->record_id;

      fprintf(GRID_STEM_FILE, "				</tokens>\n");
      fprintf(GRID_STEM_FILE, "			</resource>\n");
      fprintf(GRID_STEM_FILE,"                  <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Stems_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);      
      fprintf(GRID_STEM_FILE,"      <tokens>\n");
    }
    
    fprintf(GRID_STEM_FILE,"       <token identifier=\"Cheshire_Stems_%s-%d-%d\" value=\"%s\">\n", sgml_rec->file_name, sgml_rec->record_id, STEM_ID++, data);
    
    fprintf(GRID_STEM_FILE,"            <resource identifier=\"%s-%d\" mime-type=\"text/plain\"/>\n  ", sgml_rec->file_name, sgml_rec->record_id);
    
    fprintf(GRID_STEM_FILE,"      </token>\n");
    break;

  case GRID_STOP :

    if (STOP_FILE_OPEN==0) {
      /* open the file */
      GRID_STOP_FILE = fopen(STOP_FILE_NAME, "w");
      STOP_FILE_OPEN = 1;
      
      fprintf(GRID_STOP_FILE, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fprintf(GRID_STOP_FILE, "<circo xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://circo.dei.unipd.it/\" xsi:schemalocation=\"http://circo.dei.unipd.it/ http://ims.dei.unipd.it/xml/circo-schema-instance\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n");
      fprintf(GRID_STOP_FILE, "  <metadata>\n    <dc:creator>\n      Cheshire II Grid Version\n    </dc:creator>\n    <dc:rights>\n       Copyright (c) 1990-2009 Regents of the University of California, All Rights Reserved. \n    </dc:rights>\n");
      time(&time_t_data);
      fprintf(GRID_STOP_FILE, "    <dc:date>\n      %s \n    </dc:date>\n", ctime(&time_t_data));
      fprintf(GRID_STOP_FILE, "  </metadata>\n");
      
      if (idx->stopwords_name != NULL) {
	fprintf(GRID_STOP_FILE, "  <stream identifier=\"Cheshire_Stoplist_Output_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\">\n                <component identifier=\"cheshire_Normalize\" type=\"stop-list\" description=\"Stoplist processing eliminates list of words (from %s) from a stream of tokens.\">\n                        <actor identifier=\"Larson\" />\n                </component>", idx->stopwords_name);
      }
      else {
	fprintf(GRID_STOP_FILE, "  <stream identifier=\"Cheshire_Stoplist_Output_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\">\n                <component identifier=\"cheshire_Normalize\" type=\"stop-list\" description=\"Stoplist processing eliminates list of words (from NULL stoplist) from a stream of tokens.\">\n                        <actor identifier=\"Larson\" />\n                </component>");
      }
      fprintf(GRID_STOP_FILE,"         <resources>\n                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Stoplist_Output_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      
      
      fprintf(GRID_STOP_FILE,"      <tokens>\n");
      /* end of header info for file */
      LAST_STOP_RECID = sgml_rec->record_id;      
    }
    
    if (LAST_STOP_RECID != sgml_rec->record_id) {
      /* treat each record as a new resource */
      LAST_STOP_RECID = sgml_rec->record_id;

      fprintf(GRID_STOP_FILE, "				</tokens>\n");
      fprintf(GRID_STOP_FILE, "			</resource>\n");
      fprintf(GRID_STOP_FILE,"                  <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Stoplist_Output_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      fprintf(GRID_STOP_FILE,"      <tokens>\n");
    }
    
    fprintf(GRID_STOP_FILE,"       <token identifier=\"Cheshire_Stoplist_Output_%s-%d-%d\" value=\"%s\">\n", sgml_rec->file_name, sgml_rec->record_id, STOP_ID++, data);
    
    fprintf(GRID_STOP_FILE,"            <resource identifier=\"%s-%d\" mime-type=\"text/plain\"/>\n  ", sgml_rec->file_name, sgml_rec->record_id);
    
    fprintf(GRID_STOP_FILE,"      </token>\n");
    
    break;

  case GRID_HYPHEN :
    if (HYPHEN_FILE_OPEN==0) {
      /* open the file */
      GRID_HYPHEN_FILE = fopen(HYPHEN_FILE_NAME, "w");
      HYPHEN_FILE_OPEN = 1;
      
      fprintf(GRID_HYPHEN_FILE, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fprintf(GRID_HYPHEN_FILE, "<circo xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://circo.dei.unipd.it/\" xsi:schemalocation=\"http://circo.dei.unipd.it/ http://ims.dei.unipd.it/xml/circo-schema-instance\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n");
      fprintf(GRID_HYPHEN_FILE, "  <metadata>\n    <dc:creator>\n      Cheshire II Grid Version\n    </dc:creator>\n    <dc:rights>\n       Copyright (c) 1990-2009 Regents of the University of California, All Rights Reserved. \n    </dc:rights>\n");
      time(&time_t_data);
      fprintf(GRID_HYPHEN_FILE, "    <dc:date>\n      %s \n    </dc:date>\n", ctime(&time_t_data));
      fprintf(GRID_HYPHEN_FILE, "  </metadata>\n");
      
      fprintf(GRID_HYPHEN_FILE, "  <stream identifier=\"Cheshire_Hyphen_Processing_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\">\n                <component identifier=\"cheshire_Hyphen_Normalize\" type=\"normalizer\" description=\"A Hyphen normalizer takes hyphenated words and slits them at the hyphen, while also retaining the hyphenated version.\">\n                        <actor identifier=\"Larson\" />\n                </component>", sgml_rec->file_name);
      fprintf(GRID_HYPHEN_FILE,"         <resources>\n                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Hyphen_Processing_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      fprintf(GRID_HYPHEN_FILE,"      <tokens>\n");
      /* end of header info for file */
      LAST_HYPHEN_RECID = sgml_rec->record_id;      
    }
    
    if (LAST_HYPHEN_RECID != sgml_rec->record_id) {
      /* treat each record as a new resource */
      LAST_HYPHEN_RECID = sgml_rec->record_id;

      fprintf(GRID_HYPHEN_FILE, "				</tokens>\n");
      fprintf(GRID_HYPHEN_FILE, "			</resource>\n");
      fprintf(GRID_HYPHEN_FILE,"                        <resource identifier=\"%s-%d\" mime-type=\"text/plain\">\n                                <stream identifier=\"Cheshire_Hyphen_Processing_%s\" chunked=\"false\" chunk-number=\"0\" last-chunk=\"false\" digest-type=\"NONE\" />", sgml_rec->file_name, sgml_rec->record_id, sgml_rec->file_name);
      fprintf(GRID_HYPHEN_FILE,"      <tokens>\n");
    }

    fprintf(GRID_HYPHEN_FILE,"       <token identifier=\"Cheshire_Hyphen_Processing_%s-%d-%d\" value=\"%s\">\n", sgml_rec->file_name, sgml_rec->record_id, HYPHEN_ID++, data);
    
      fprintf(GRID_HYPHEN_FILE,"            <resource identifier=\"%s-%d\" mime-type=\"text/plain\"/>\n  ", sgml_rec->file_name, sgml_rec->record_id);
    
    fprintf(GRID_HYPHEN_FILE,"      </token>\n");
  break;



  }
  

  return (0);

}







  








