/*
 *  Copyright (c) 1990-2001 [see Other Notes, below]. The Regents of the
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
/* This function take an SGML document and "flattens" it for storage in
 * a BerkeleyDB file in parsed form for storage -- 
 * a hash of the record (using code from md5) is passed into the function
 */
#include <stdlib.h>
#include <string.h>
#include "cheshire.h"
#include "flatrec.h"
#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif

#include "dmalloc.h"


SGML_Document *rebuild_flat_record(Output_Record *inrec, int length,
				   SGML_DTD *DTD);

extern FILE *LOGFILE;
extern SGML_DTD *cf_getDTD(char *filename);
extern config_file_info *find_file_config(char *filename);
extern SGML_Processing_Inst *DupPI(SGML_Processing_Inst *pi);
extern SGML_Document *current_sgml_document;
extern SGML_Element *xml_schema_add_any_el(SGML_DTD *in_dtd, char *tagname,
                                           int force);


int 
Count_elements(SGML_Data *dat)
{
  int count;
  
  if (dat == NULL)
    return (0);
  count = Count_elements(dat->sub_data_element);
  
  count += Count_elements(dat->next_data_element);
  
  return (count + 1);

}

int 
Count_PI(SGML_Processing_Inst *in)
{
  int count;

  if (in == NULL)
    return 0;

  count = Count_PI(in->next_processing_inst);
  return (count + 1);
}

int
Store_elements(SGML_Data *dat, int *element_num, Output_Record *rec)
{
  int count;
  int current;
  int i;
  char *outdata;
  char *data_end;
  char *temp;
  SGML_Attribute_Data *attr;

  if (dat == NULL)
    return (0);
  
  current = *element_num;

  rec->sdata[current].start_tag_offset = dat->start_tag_offset;
  rec->sdata[current].content_start_offset = dat->content_start_offset;
  rec->sdata[current].content_end_offset = dat->content_end_offset;
  rec->sdata[current].end_tag_offset = dat->end_tag_offset;
  rec->sdata[current].data_end_offset = dat->data_end_offset;
  rec->sdata[current].processing_flags  = dat->processing_flags;
  rec->sdata[current].data_element_id  = dat->data_element_id;
  rec->sdata[current].occurrence_no  = dat->occurrence_no;
  rec->sdata[current].data = dat;
 
  if (current > 0) {
    for (i = 0; i < rec->n_sgml_data; i++) {
      
      /* quit if at end of filled in records */
      if (rec->sdata[i].content_start_offset == 0) 
	break;

      if (rec->sdata[i].data != NULL
	  && rec->sdata[i].data == dat->parent)
	rec->sdata[current].parent = i;
    }
  }

  /* point to next element number */
  (*element_num)++;

  count = Store_elements(dat->sub_data_element, element_num, rec);

  if (count == 0) {
    if (dat->next_data_element) {
      rec->sdata[current].nextdata = current+1;
      count = Store_elements(dat->next_data_element, element_num, rec);
    }
  }
  else {
    rec->sdata[current].subdata = current+1;
    if (dat->next_data_element) {
      rec->sdata[current].nextdata = current+count+1;
      count += Store_elements(dat->next_data_element, element_num, rec);
    }
  }

  /* now we store the attributes for this element in the buffer */
  /* after the full data and before the hash table              */
  outdata = (char *)rec;
  data_end = outdata + (sizeof(Output_Record) 
			+ (rec->n_sgml_data * sizeof(sdata_info)));

  data_end += rec->buffersize + 1 + rec->attr_size;
  temp = data_end - 20;
    
  for (attr = dat->attributes; attr; attr = attr->next_attribute_data) {
    int outsize;
    
    rec->sdata[current].n_attr++;
    if (rec->sdata[current].attr_start_offset == 0)
      rec->sdata[current].attr_start_offset = data_end - outdata;
    if (attr->string_val != NULL)
      outsize = sprintf(data_end,"%s=%s",attr->name, attr->string_val);
    else
      outsize = sprintf(data_end,"%s=%d",attr->name, attr->int_val);
    data_end += outsize+1;
    rec->attr_size += outsize + 1;
    temp = data_end - 4;
  }

  return (count + 1);

}


SGML_Element *match_dtd_element(char *tagstart, SGML_DTD *dtd) 
{
  char temptag[200];
  char *c;
  int i;
  int namespace=0;

  Tcl_HashEntry *entry; 
  
  for (i = 0, c = tagstart; 
       *c != ' ' && *c != '>' && *c != '\t' 
       && *c != '\n' && *c != '\r' && *c != '/' ; 
       i++, c++) {
    if (dtd->type) /* xml or xmlschema -- case sensitive */
      temptag[i] = *c;
    else
      temptag[i] = toupper(*c);
    if (*c == ':')
      namespace = i+1;
  } 
  temptag[i] = '\0';

  if (namespace) {
    entry = Tcl_FindHashEntry(dtd->SGML_Element_hash_tab, temptag+namespace);
    if (entry == NULL && (dtd->type == 2 || dtd->type == 3)) {
      /* looks like an undefined arbitrary XML addition... */
      return(xml_schema_add_any_el(dtd, temptag, 1));
    }
  }
  else 
    entry = Tcl_FindHashEntry(dtd->SGML_Element_hash_tab, temptag);
  

  if (entry == NULL) {
    fprintf(LOGFILE,"Tag element %s not defined", temptag);
    exit(0);
  }
  else {
    return ((SGML_Element *) Tcl_GetHashValue(entry));
  }
}


/* Take the record and create a representation that can be rebuilt 
 * without parsing the entire thing.
 */
Output_Record *flatten_sgml_document(SGML_Document *doc, char digest[17],
				int *length, int docid)
{
  int num_elements;
  int data_length;
  int num_pi;
  Output_Record *rec;
  int i;
  char *outdata;
  char *data_end;
  char *data_start;
  int highest_element_num = 0;
  SGML_Document *result;
  pi_info *out_pi;
  SGML_Processing_Inst *pi;
  Tcl_HashEntry *tag_entry;
  Tcl_HashSearch tag_hash_search;
  char *tag_name;
  char *temp;
  SGML_Tag_Data *tag_data, *td;

  num_elements = Count_elements(doc->data);
  num_pi = Count_PI(doc->Processing_Inst);

  data_length = strlen(doc->buffer);

  /* allocate space for the output data -- more than needed, but only the 
   * used space will be stored.
   */
  rec = (Output_Record *)CALLOC(char, (sizeof(Output_Record) 
				       + (num_elements * sizeof(sdata_info)) 
				       + (num_pi * sizeof(pi_info))
				       + (2 * data_length) + 500)); 

  /* store a copy of the original digest in the record */
  for (i = 0; i < 16; i++) {
    rec->digest[i] = digest[i];
  }

  outdata = (char *)rec;
  data_end = outdata + (sizeof(Output_Record) 
			+ (num_elements * sizeof(sdata_info)));

  rec->n_sgml_data = num_elements;
  rec->n_pi = num_pi;
  rec->attr_size = 0;
  rec->var_area = 0;
  rec->docid = docid;

  /* copy the real data buffer to the output record */
  rec->buffersize = data_length;
  memcpy(data_end, doc->buffer, data_length + 1);
  data_start = data_end;
  data_end += data_length + 1;

    
  Store_elements(doc->data, &highest_element_num, rec);

  /* attributes for each element have been appended */
  data_end += rec->attr_size;

  out_pi = (pi_info *)data_end;

  for (pi = doc->Processing_Inst; pi ; pi = pi->next_processing_inst) {
    out_pi->Instruction_Type = pi->Instruction_Type ;
    out_pi->Instruction = pi->Instruction; 
    if ( pi->tag)
      strcpy(out_pi->tag, pi->tag); 
    if (pi->attr)
      strcpy(out_pi->attr, pi->attr);
    if (pi->index_name)
      strcpy(out_pi->index_name, pi->index_name);
    out_pi++;
  }

  rec->var_area = (num_pi * sizeof(pi_info));

  data_end += (num_pi * sizeof(pi_info));

  /* now add the processing instructions to the end */
  for (tag_entry = Tcl_FirstHashEntry(doc->Tag_hash_tab,&tag_hash_search); 
       tag_entry != NULL; 
       tag_entry = Tcl_NextHashEntry(&tag_hash_search)) {
    int ptr_index;
    int io_size;
    
    /* get the tag from the hash table indexes */
    tag_name = Tcl_GetHashKey(doc->Tag_hash_tab,tag_entry); 
    /* compare the tag with the key pattern */
    tag_data = (SGML_Tag_Data *) Tcl_GetHashValue(tag_entry);

    rec->hash_tag_count++;
    
    io_size = sprintf(data_end,"%c%s", strlen(tag_name)+2, tag_name);
    data_end += io_size + 1;
    temp = data_end;
    
    for (td = tag_data; td; td = td->next_tag_data) {
      /* now scan the data for the matching tag ptr */
      for (ptr_index = 0 ; ptr_index < num_elements; ptr_index++) {
	if (rec->sdata[ptr_index].data == td->item) {
	  /* have a match -- output the index */
	  io_size = sprintf(data_end," %d", ptr_index);
	  data_end += io_size;
	} 
      }
    }
    /* set a null at the end of the current data */
    *data_end++ = '\0';

  }
  if (doc->file_name) {
    sprintf(data_end, "%s", doc->file_name);
    data_end += (strlen(doc->file_name) + 1);
  }
  else 
    *data_end++ = '\0';

  *length = ((int)data_end) - ((int)outdata);

  /* this is temp -- just for testing */
  /*  result = rebuild_flat_record(rec, *length,
   *		       doc->DTD); 
   */

  return (rec);
}

SGML_Document *rebuild_flat_record(Output_Record *inrec, int length,
				   SGML_DTD *dtd)
{
  char *data_buffer;
  char *data_in;
  char *c;
  char *temp;
  int i, j;
  int fnamesize;
  SGML_Tag_Data *tag_data, *td, *last_td;  
  SGML_Data *dat;
  SGML_Processing_Inst *out_pi, *last_pi, *pi;
  SGML_Document *out_doc;
  Tcl_HashEntry *entry; 
  int exists;
  SGML_Attribute_Data *attr, *last_attr, *first_attr;
  SGML_Attribute *at;
  pi_info *in_pi;
  int pi_count;

  /* copy the stored SGML/XML data to new data_buffer */
  data_buffer = CALLOC(char, inrec->buffersize+1);
  data_in = ((char *)inrec) + (sizeof(Output_Record) 
			+ (inrec->n_sgml_data * sizeof(sdata_info)));
  memcpy(data_buffer, data_in, inrec->buffersize);

  /* First pass, just allocate the SGML_Data structures and set the */
  /* offsets and pointers into the buffer                           */
  out_doc = CALLOC(SGML_Document,1);

  if (dtd != NULL) {
    out_doc->DTD = dtd;
    out_doc->DCL = dtd->sgml_declaration;
  }

  out_doc->buffer = data_buffer;
  out_doc->record_id = inrec->docid;

  for (i = 0; i < inrec->n_sgml_data; i++) {
    dat = CALLOC(SGML_Data,1);
    /* keep track of the new data */
    inrec->sdata[i].data = dat;
    
    dat->start_tag_offset = inrec->sdata[i].start_tag_offset;
    dat->content_start_offset = inrec->sdata[i].content_start_offset;
    dat->content_end_offset = inrec->sdata[i].content_end_offset;
    dat->end_tag_offset = inrec->sdata[i].end_tag_offset;
    dat->data_end_offset = inrec->sdata[i].data_end_offset;
    dat->processing_flags = inrec->sdata[i].processing_flags;
    dat->data_element_id = inrec->sdata[i].data_element_id;
    dat->occurrence_no = inrec->sdata[i].occurrence_no;
    dat->start_tag = data_buffer + dat->start_tag_offset;
    dat->content_start = data_buffer + dat->content_start_offset;
    dat->content_end = data_buffer + dat->content_end_offset;
    dat->end_tag = data_buffer + dat->end_tag_offset;
    dat->data_end = data_buffer + dat->data_end_offset;

    dat->element = match_dtd_element((dat->start_tag) + 1, dtd);

    attr = first_attr = NULL;
    c = ((char *)inrec) + inrec->sdata[i].attr_start_offset;

    for (j = 0; j < inrec->sdata[i].n_attr; j++) {
      char *tempdata;
      int numlength;
      last_attr = attr;
      attr = CALLOC(SGML_Attribute_Data, 1);
      tempdata = strdup(c);
      temp = strchr(tempdata, '=');
      if (temp == NULL) {
	fprintf(LOGFILE,"Error in rebuilding records: no = in stored attribute\n");
	exit(1);
      }
      *temp = '\0';
      attr->name = strdup(tempdata);
      attr->string_val = strdup(temp+1);
      /* match the attribute name */	
      for (at = dat->element->attribute_list; 
	   at && attr->attr_def == NULL ;
	   at = at->next_attribute) {

	if (strcasecmp(at->name, attr->name) == 0) {
	  attr->attr_def = at;
	}
      }

      FREE(tempdata);
      if (first_attr == NULL) {
	first_attr = attr;
      }
      else {
	last_attr->next_attribute_data = attr;
      }
      c += strlen(c) + 1;
    }
    dat->attributes = first_attr;

  }

  in_pi = (pi_info *) (data_in + (inrec->buffersize + inrec->attr_size + 1));

  last_pi = NULL;

  for (pi_count = 0; pi_count < inrec->n_pi ; pi_count++) {
    int tlen, alen, ilen;
    out_pi = CALLOC(SGML_Processing_Inst, 1);
    out_pi->Instruction_Type = in_pi->Instruction_Type ;
    out_pi->Instruction = in_pi->Instruction; 
    if ((tlen = strlen(in_pi->tag)) > 0) {
      out_pi->tag = CALLOC(char, tlen + 1);
      strcpy(out_pi->tag, in_pi->tag); 
    }
    if ((alen = strlen(in_pi->attr)) > 0) { 
      out_pi->attr = CALLOC(char, alen + 1);
      strcpy(out_pi->attr, in_pi->attr);
    }
    if ((ilen = strlen(in_pi->index_name)) > 0) {
      out_pi->index_name = CALLOC(char, ilen + 1);
      strcpy(out_pi->index_name, in_pi->index_name);
    }

    if (out_doc->Processing_Inst == NULL) {
      out_doc->Processing_Inst = out_pi;
    }
    else {
      last_pi->next_processing_inst = out_pi;
    }

    last_pi = out_pi;

    in_pi++;
  }


  /* make a second pass through the data, linking parent, children and sibs */
  for (i = 0; i < inrec->n_sgml_data; i++) {
    if (i > 0)
      inrec->sdata[i].data->parent = inrec->sdata[inrec->sdata[i].parent].data;
    if (inrec->sdata[i].subdata > 0)
      inrec->sdata[i].data->sub_data_element = inrec->sdata[inrec->sdata[i].subdata].data;
    if (inrec->sdata[i].nextdata > 0)
      inrec->sdata[i].data->next_data_element = inrec->sdata[inrec->sdata[i].nextdata].data;
    if (inrec->sdata[i].processing_flags > 0 && 
	inrec->sdata[i].processing_flags != PROC_FLAG_IN_CHILD) {
      /* dup processing instructions from doc  */
      for (pi = out_doc->Processing_Inst; pi; pi = pi->next_processing_inst){
	if (strncasecmp(pi->tag, 
			inrec->sdata[i].data->start_tag+1,
			strlen(pi->tag)) == 0) {
	  
	  inrec->sdata[i].data->Processing_instr = DupPI(pi); /* ONE processing instr for now */
	}
      }
    }
  }


  /* return the top level SGML_data element */
  out_doc->data = inrec->sdata[0].data;

  /* read and rebuild the hash table for tags... */
  out_doc->Tag_hash_tab = CALLOC(Tcl_HashTable,1);
  Tcl_InitHashTable(out_doc->Tag_hash_tab, TCL_STRING_KEYS);
  
  c = data_in + inrec->buffersize + inrec->attr_size + inrec->var_area + 1;

  for (i = 0; i < inrec->hash_tag_count; i++) {
    int tag_size;
    int tag_index;
    int chars_read;
    /* create new hash entry */
    tag_size = *c;
    data_in = c+1;
    entry = Tcl_CreateHashEntry(out_doc->Tag_hash_tab,
				data_in, &exists );
    tag_data = td = last_td = NULL;
    data_in += tag_size;

    while ((chars_read = sscanf(data_in," %d", &tag_index)) > 0) { 
      last_td = td;
      td = CALLOC(SGML_Tag_Data, 1);
      td->item = inrec->sdata[tag_index].data;

      if (tag_data == NULL)
	last_td = tag_data = td;
      else {
	last_td->next_tag_data = td;
      }
      if (*data_in) {
	for(data_in++; *data_in != '\0' && *data_in != ' '; data_in++) ;
      }
    }
    Tcl_SetHashValue(entry, (ClientData)tag_data);
    c = data_in + 1;
  }

  data_in = c;
  fnamesize = strlen(c);
  if (fnamesize) {
    out_doc->file_name = CALLOC(char, fnamesize+1);
    strcpy(out_doc->file_name, c);
  }
  
  return (out_doc);
   
}


SGML_Document *FetchSGMLDataStore(char *filename, int docid) 
{
  
  SGML_DTD *dtd;
  SGML_Document *doc;
  config_file_info *cf;
  DB *db_handle;
  DBTYPE dbtype;
  int db_errno;
  DBT keyval;
  DBT dataval;
  int returncode;
  Output_Record *outrec;
  int outrec_length;
  int i;

  
  dtd = cf_getDTD(filename);
  if (dtd == NULL) {
    fprintf(LOGFILE, "Couldn't get main dtd '%s' for filename or filetag '%s', datastore record #%d\n", 
	   cf_info_base->DTD_name, filename, docid);
    return (NULL);
  }
  
  cf = find_file_config(filename);

  if (cf != NULL) {
    if (cf->file_db != NULL) { 
      db_handle = cf->file_db;
    }
    else {
      cf_open_datastore(cf);
      if (cf->file_db == NULL) {
	fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized\n",
		filename);
	return (NULL);  
      }
      else
	db_handle = cf->file_db;
    }
  }
  else {
    fprintf(LOGFILE, "Couldn't get configfile info for filename or filetag '%s', datastore record #%d\n", 
	    filename, docid);
    return (NULL);  
  }
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  
  /* set up the key */
  keyval.data = (void *)&docid;
  keyval.size = sizeof(int);
    
    
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
  if (returncode == 0) { /* found the record */
    outrec = (Output_Record *)CALLOC(char, dataval.size);       
    memcpy(outrec,dataval.data, dataval.size);
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    fprintf(LOGFILE,"Record #%d not found in database\n", docid);
    return (NULL);
  }
    
  doc = rebuild_flat_record(outrec, dataval.size, dtd); 

  FREE(outrec);

  current_sgml_document = doc;

  return(doc);
  
}


char *FetchSGMLRawDataStore(char *filename, int docid) 
{
  
  config_file_info *cf;
  DB *db_handle;
  DBT keyval;
  DBT dataval;
  int returncode;
  Output_Record *outrec;
  char *data_buffer, *data_in;

  
  cf = find_file_config(filename);

  if (cf != NULL) {
    if (cf->file_db != NULL) { 
      db_handle = cf->file_db;
    }
    else {
      cf_open_datastore(cf);
      if (cf->file_db == NULL) {
	fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized\n",
		filename);
	return (NULL);  
      }
      else
	db_handle = cf->file_db;
    }
  }
  else {
    fprintf(LOGFILE, "Couldn't get configfile info for filename or filetag '%s', datastore record #%d\n", 
	    filename, docid);
    return (NULL);  
  }
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  
  /* set up the key */
  keyval.data = (void *)&docid;
  keyval.size = sizeof(int);
    
    
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
  if (returncode == 0) { /* found the record */
    outrec = (Output_Record *)CALLOC(char, dataval.size);       
    memcpy(outrec,dataval.data, dataval.size);
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    fprintf(LOGFILE,"Record #%d not found in database\n", docid);
    return (NULL);
  }
    

  /* copy the stored SGML/XML data to new data_buffer */
  data_buffer = CALLOC(char, outrec->buffersize+1);
  data_in = ((char *)outrec) + (sizeof(Output_Record) 
			+ (outrec->n_sgml_data * sizeof(sdata_info)));
  memcpy(data_buffer, data_in, outrec->buffersize);

  FREE(outrec);

  return(data_buffer);
  
}



char *FetchDataStoreDigest(config_file_info *cf, int docid) 
{
  
  DB *db_handle;
  DBT keyval;
  DBT dataval;
  int returncode;
  Output_Record *outrec;
  char *data_buffer, *data_in;
  int i;
  
  if (cf != NULL) {
    if (cf->file_db != NULL) { 
      db_handle = cf->file_db;
    }
    else {
      cf_open_datastore(cf);
      if (cf->file_db == NULL) {
	fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized\n",
		cf->nickname);
	return (NULL);  
      }
      else
	db_handle = cf->file_db;
    }
  }
  else {
    fprintf(LOGFILE, "No configfile info for filename or filetag '%s', datastore record #%d\n", 
	    cf->nickname, docid);
    return (NULL);  
  }
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  
  /* set up the key */
  keyval.data = (void *)&docid;
  keyval.size = sizeof(int);
    
    
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
  if (returncode == 0) { /* found the record */
    outrec = (Output_Record *)CALLOC(char, dataval.size);       
    memcpy(outrec,dataval.data, dataval.size);
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    fprintf(LOGFILE,"Record #%d not found in database\n", docid);
    return (NULL);
  }
    
  
  /* copy the stored SGML/XML data to new data_buffer */
  data_buffer = CALLOC(char, 17);
  for (i = 0; i < 16; i++) {
    data_buffer[i] = outrec->digest[i];
  }

  FREE(outrec);

  return(data_buffer);
  
}



int
DeleteDataStore(char *filename, int docid) 
{
  config_file_info *cf;  
  DB *db_handle;
  int db_errno;
  DBT keyval;
  int returncode;
  char *digest;
  
  cf = find_file_config(filename);

  if (cf != NULL) {
    if (cf->file_db != NULL) { 
      db_handle = cf->file_db;
    }
    else {
      cf_open_datastore(cf);
      if (cf->file_db == NULL) {
	fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized\n",
		filename);
	return (FAIL);  
      }
      else
	db_handle = cf->file_db;

    }
  }
  else {
    fprintf(LOGFILE, "Couldn't get configfile info for filename or filetag '%s', datastore record #%d\n", 
	    filename, docid);
    return (FAIL);  
  }
  
  digest = FetchDataStoreDigest(cf, docid);

  if (digest == NULL) {
    return (SUCCEED); /* No such record */
  }

  memset(&keyval, 0, sizeof(keyval));
  
  /* set up the key */
  keyval.data = (void *)digest;
  keyval.size = 17;
    
  returncode = db_handle->del(db_handle, NULL, &keyval, 0); 

  if (returncode != 0) { /* didn't delete the record */
    /* a new record not in the database */
    fprintf(LOGFILE,"Record #%d not found in database\n", docid);
    return (FAIL);
  }

  db_handle->sync(db_handle,0);

  memset(&keyval, 0, sizeof(keyval));
  
  /* set up the key */
  keyval.data = (void *)&docid;
  keyval.size = sizeof(int);
        
  returncode = db_handle->del(db_handle, NULL, &keyval, 0); 

  if (returncode == 0) {
    /* sync the database -- flushing all of the buffers to disk */
    db_handle->sync(db_handle,0);
    return (SUCCEED);
  }
  else if (returncode == DB_NOTFOUND) { /* didn't delete the record */
    /* a new record not in the database */
    fprintf(LOGFILE,"Record #%d not found in database\n", docid);
    return (FAIL);
  }


}



int
VerifyDataStore(char *filename, int docid, assoc_rec *assocrec)
{
  config_file_info *cf;
  DB *db_handle;
  DBT keyval;
  DBT dataval;
  int returncode;
  char dummy_buffer;
  int rec_size;

  cf = find_file_config(filename);
  
  if (cf != NULL) {
    if (cf->file_db != NULL) { 
      db_handle = cf->file_db;
    }
    else {
      cf_open_datastore(cf);
      if (cf->file_db == NULL) {
	fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized\n",
		filename);
	return (-1);  
      }
      else
	db_handle = cf->file_db;
    }
  }
  else {
    fprintf(LOGFILE, "No configfile info for filename or filetag '%s', datastore record #%d\n", 
	    cf->nickname, docid);
    return (-1);  
  }
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  
  /* set up the key */
  keyval.data = (void *)&docid;
  keyval.size = sizeof(int);
    
  /* this combination of dataval values and flags will return only the size */
  dataval.data = (void *)&dummy_buffer;
  dataval.ulen = 0;
  dataval.flags = DB_DBT_USERMEM;
    
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
  if (returncode == ENOMEM) { /* found the record */
    rec_size = dataval.size/2;       
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    fprintf(LOGFILE,"Record #%d not found in database\n", docid);
    return (-1);
  }

  assocrec->offset = 0;
  assocrec->recsize = rec_size;
    
  return(0);
  
}

int
AvgDataStoreSize(char *filename,   config_file_info *cf)
{
  DB *db_handle;
  DBT keyval;
  DBT dataval;
  int high_recno=0;
  int total_size=0;
  int returncode;


  if (cf == NULL) {
    fprintf(LOGFILE, "NULL configfile info for filename or filetag '%s' in AvgDataStoreSize\n", 
	    filename);
    return -1;
  }
  if (cf->file_db != NULL) { 
    db_handle = cf->file_db;
  }
  else {
    cf_open_datastore(cf);
    if (cf->file_db == NULL) {
      fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized in AvgDataStoreSize\n",
	      filename);
      return (-1);  
    }
    else
      db_handle = cf->file_db;
  }
 
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
	  
  keyval.data = (void *)"MAXIMUM_RECORD_NUMBER";
  keyval.size = strlen("MAXIMUM_RECORD_NUMBER");
	  
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the key in the index already */
    /* The database is set up */
    memcpy(&high_recno,dataval.data, sizeof(int));
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    fprintf(LOGFILE, "Datastore file '%s' not initialized (no data?)\n", 
	    filename);
    return (-1);  
    
  }

  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
	  
  keyval.data = (void *)"TOTAL_RECORD_SIZE";
  keyval.size = strlen("TOTAL_RECORD_SIZE");
	  
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the key in the index already */
    /* The database is set up */
    memcpy(&total_size,dataval.data, sizeof(int));
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    fprintf(LOGFILE, "Datastore file '%s' not initialized (no TOTAL_RECORD_SIZE?)\n", 
	    filename);
    return (-1);  
  }

  /* OK should have everything we need... */
  if (high_recno == 0 || total_size == 0) {
    return (0);
  }
  
  return((total_size/2)/high_recno);

} 
