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
*       Header Name:    se_get_page_docs.c
*
*       Programmer:     Ray R. Larson
*              
*
*       Purpose:        
*                       
*
*       Usage:          weighted_result *se_get_page_docs (weighted_result *);
*
*       Variables:	
*
*       Return Conditions and Return Codes:
*			returns pointer to new DOCS weighted_result set
*			or NULL on error
*
*       Date:           11/20/96
*       Version:        1.0
*       Copyright (c) 1996.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <cheshire.h>

/* external functions */
weighted_result *se_rank_docs(weighted_result *newset);


/* build in-memory tables for the page associator files    */
/* this is to avoid a lot of seeking when building results */
/* we will allow only MAXPAGEBUFFS buffers                 */
#define MAXPAGEBUFFS 10
struct page_info_buffers {
  char *tag;
  int maxpagenum;
  page_assoc_rec *pagedata;
} page_info_buffers[MAXPAGEBUFFS];

page_assoc_rec *se_get_page_assoc(char *filename, char *indextag, 
				  int pagenum)
{

  FILE *pagemap;
  int i;
  static firstcall = 1;
  struct stat filestat;
  page_assoc_rec *newdata;

  if (filename == NULL || indextag == NULL)
    return (NULL);

  if (firstcall) {
    for (i = 0; i < MAXPAGEBUFFS; i++) {
      page_info_buffers[i].tag = NULL;
      page_info_buffers[i].maxpagenum = 0;
      page_info_buffers[i].pagedata = NULL;
    }
    firstcall = 0;
  } 

  for (i = 0; i < MAXPAGEBUFFS && page_info_buffers[i].tag ; i++) {
    if (strcmp(page_info_buffers[i].tag, indextag) == 0) {
      /* there is an existing page buffer */
      if (pagenum > page_info_buffers[i].maxpagenum)
	return (NULL);
      return (page_info_buffers[i].pagedata + pagenum);
    }
    
  }
  
  if (i == MAXPAGEBUFFS) {
    fprintf(LOGFILE, "Exceeded page buffer maximum\n");
    return (NULL);
  }
  
  /* if we get to here page_info_buffers[i].filename is probably NULL */
  page_info_buffers[i].tag = strdup(indextag);
  pagemap = (FILE *) cf_index_open(filename, indextag, PAGEASSOCFILE);
  /* use fstat to get the file info */
  fstat(fileno(pagemap), &filestat);
  
  page_info_buffers[i].maxpagenum = 
    filestat.st_size / sizeof(page_assoc_rec);
  
  newdata = CALLOC(page_assoc_rec,
		   page_info_buffers[i].maxpagenum +1);
  
  page_info_buffers[i].pagedata = newdata;
  rewind(pagemap);
  fread (newdata, sizeof(page_assoc_rec), 
	 page_info_buffers[i].maxpagenum, pagemap);
  
  return (page_info_buffers[i].pagedata + pagenum);

}


/* the get_page_docs function */
weighted_result *se_get_page_docs(weighted_result *inset, 
				  Tcl_HashTable **return_hash,
				  int idnumber)
{

  int i = 0; /* loop and array indices */
  int prob_flag = 0;
  int doc_count = 0;
  int exists;

  page_assoc_rec *page_data;

  long pagenum, recnum, int_pagenum;

  char *c_ptr;
  char *index_tag;
  char filename[100];

  Tcl_HashTable *hash_tab;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
 
  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *lastpage;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts;


  int total_postings, newset_size;
  weighted_result *newset; /* result set*/
  
  if (inset == NULL)
    return(NULL);
  
  /* check to see if the set is from a paged search    */
  if ((inset->result_type & PAGED_RESULT) != PAGED_RESULT) {
    fprintf(LOGFILE, "Not PAGED_RESULT in se_get_page_docs\n");
    return (NULL);
  }

  /* check to see if the set is a probabilistic search */
  if (inset->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT)) {
    prob_flag = (inset->result_type 
		 & (PROBABILISTIC_RESULT | OKAPI_RESULT 
		    | CORI_RESULT |FUZZY_RESULT | BLOB_RESULT));
  }

  /* find size of largest possible merged set, allocate memory */
  /* for mapping structure                                     */
  total_postings = inset->num_hits;
 

  /* we need to open up the appropriate index file stuff */
  /* the index name should be stored as part of the file */
  /* name in the input set...                            */
  strcpy(filename, inset->filename);

  if ((c_ptr = strchr(filename, ':')) != NULL) {
    if (*(c_ptr+1) == ':') {
      *c_ptr = '\0';
      index_tag = (char *)(c_ptr + 2);
    }
    else {
      fprintf(LOGFILE, "Can't find index name in se_get_page_docs\n");
      return (NULL);
    }
  }

  /* Init the hash table for collecting weights for each document */
  hash_tab = CALLOC(Tcl_HashTable,1);
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);
 

  for (i = 0; i < total_postings; i++) {
    /* get the (internal) page information from the page associator file */
    int_pagenum = inset->entries[i].record_num;
    page_data = se_get_page_assoc(filename, index_tag, int_pagenum);
    
    /* add it to the DOC hash table -- incrementing weights       */
    recnum = page_data->parentid;
    pagenum = page_data->pagenum;

    entry = Tcl_FindHashEntry(hash_tab, (void *)recnum);
    
    if (entry == NULL) {
      doc_wts = CALLOC(struct docsum,1);

      doc_wts->page_ptr = doc_wts->lastpage = lastpage = 
	CALLOC(struct pageitem,1);
      doc_wts->lastpage->pagenum = pagenum;
      doc_wts->lastpage->setindex = i;
      doc_wts->lastpage->pageid = idnumber; /* a kind of set id */
      doc_wts->num_pages = 1;
      doc_wts->sum_wts = inset->entries[i].weight;

      doc_count++;

      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   hash_tab,
					   (void *)recnum,
					   &exists),
		       (ClientData)doc_wts);
    }
    else {
      /* add to an existing doc entry */
      doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
      doc_wts->num_pages++;
      doc_wts->sum_wts += inset->entries[i].weight;
      doc_wts->lastpage->next = lastpage = CALLOC(struct pageitem,1);
      doc_wts->lastpage->next->pagenum = pagenum;
      doc_wts->lastpage->next->setindex = i;
      doc_wts->lastpage->next->pageid = idnumber;
      doc_wts->lastpage = lastpage;
    }
  }

  /* now we know how many docs there are... */
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * doc_count);
  newset = (weighted_result *) CALLOC(char, newset_size);

  

  newset->num_hits = doc_count;
  newset->setid[0] = '\0';
  newset->result_type = 0; /* default Boolean */
  strcpy(newset->filename, inset->filename);
  
  newset->result_type |= PAGED_DOC_RESULT;

  i = 0;

  for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {
    newset->entries[i].record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
    doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
    newset->entries[i].weight = 
      doc_wts->sum_wts/ ((float)doc_wts->num_pages + 1.0);
    i++;
  }
  /* The hash table is not freed here -- it is passed back for further */
  /* use of the associated page lists                                  */
  *return_hash = hash_tab;

  if (prob_flag) {
    newset->result_type |= prob_flag;
    return(se_rank_docs(newset));
  }
  else
    return(newset);

}

/* this function is called to free up the hash tables allocated by */
/* se_get_page_docs and its associated page lists                  */
int 
free_page_hash_tab( Tcl_HashTable *hash_tab)
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *lastpage, *page;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts;


  if (hash_tab == NULL)
    return(0);

  for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {
    doc_wts = (struct docsum *) Tcl_GetHashValue(entry);

    lastpage = doc_wts->page_ptr;

    if (lastpage != NULL) {
      for (page = doc_wts->page_ptr->next; page != NULL; page = page->next) {
	free(lastpage);
	lastpage = page;
      } 
      free(lastpage);
    }
    free(doc_wts); /* not needed any more */
  }
  Tcl_DeleteHashTable(hash_tab); /* clean up */

  free (hash_tab);

  return(0);

}




