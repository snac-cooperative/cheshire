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
*	Header Name:	se_process_query.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Traverse a Z39.50 RPN query structure and
*                       build a weighted result set. The weights will
*                       depend on the operators used in the query
*
*	Usage:		se_process_query(databasename, RPNquery)
*
*	Variables:	RPNQuery -the query in Z39.50 RPN form.
*                       databasename - the filename or file tag of the
*                           desired database.
*
*	Return Conditions and Return Codes:	
*
*	Date:		12/5/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* the following includes the Z39.50 defs */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "cheshire.h"
#include "configfiles.h"
#include "z_parse.h"

#ifdef POSTGRESQL
#include "libpq-fe.h"
#endif

#ifdef MYSQLDBMS
#include <mysql.h>
#endif

void *cf_open_DBMS(config_file_info *cf, char *login, char *pwd);
extern void diagnostic_set();
extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
extern config_file_info *find_file_config(char *filename);
extern idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[], 
				  char *in_strattr[],
				  ObjectIdentifier in_oid[]);

char *se_dbms_terms(char *file, AttributesPlusTerm *attrPlusTerm, 
		       char **SQL_Query, Tcl_HashTable *attrib_hash_tab)
{
  AttributeList *l;
  AttributeElement *elem; 
  Term *term;

  int use = 0, relation = 0, position = 0, structure = 0;
  int truncation = 0, completeness = 0;
  idx_list_entry *idx;
  char *result_term;
  int size;
  Tcl_HashEntry *entry;
  int exists;
  char *relat_val;
  char *local_index_name;
  struct list_List2 *l1;
  int i;
  int attr_nums[MAXATTRTYPE];
  char *attr_str[MAXATTRTYPE];
  ObjectIdentifier attr_oid[MAXATTRTYPE];


  for (i = 0; i < MAXATTRTYPE; i++) {
    attr_nums[i] = 0;
    attr_str[i] = NULL;
    attr_oid[i] = NULL;
  }



  for (l = attrPlusTerm->attributes; l ; l = l->next) {
    elem = l->item;
    
    if (elem->attributeValue.which == e7_numeric) {
      if (elem->attributeType < MAXATTRTYPE) {
	attr_nums[elem->attributeType] = elem->attributeValue.u.numeric;
	attr_oid[elem->attributeType] = elem->attributeSet;
      } else {
	/* diagnostic unsupported attribute type... */
	diagnostic_set(113,elem->attributeType,NULL);
	return NULL;
      }
    }
    else {  /* complex attribute -- Must be local index name or string valued attribute type */
      if (elem->attributeType < MAXATTRTYPE) {
	attr_nums[elem->attributeType] = -1;
	for (l1 = elem->attributeValue.u.complex.list;
	     l1; l1 = l1->next) {
	  if (l1->item->which == e25_numeric) {
	    /* we don't handle any numeric types */
	    diagnostic_set(123,0,NULL);
	    return NULL;
	  } 
	  else { /* string type, so set the local name... */
	    /* in principle there can be multiple items, take only last */
	    if (l1->item == NULL || l1->item->u.string == NULL) {
	      diagnostic_set(126,0,"NULL String as attribute name");
	      return NULL;
	    }
	    attr_str[elem->attributeType] =
	      local_index_name = l1->item->u.string->data;
	    attr_oid[elem->attributeType] = elem->attributeSet;

	  }	  
	}
      } else {
	/* diagnostic unsupported attribute type... */
	diagnostic_set(113,elem->attributeType,NULL);
	return NULL;
      }
    }
  }  
  
  if (attr_nums[USE_ATTR] == -1) {
    idx = cf_getidx_entry(file, local_index_name);
  }
  else 
    idx = cf_getidx_mapattr(file,
			    attr_nums, 
			    attr_str,
			    attr_oid);
  
  relation = attr_nums[RELATION_ATTR];

  if (idx == NULL) { /* try without relation operator */
    attr_nums[RELATION_ATTR] = 0;
    idx = cf_getidx_mapattr(file,
			    attr_nums, 
			    attr_str,
			    attr_oid);

    if (idx == NULL) { /* try without relation or truncation operator */
      attr_nums[RELATION_ATTR] = 0;
      attr_nums[TRUNC_ATTR] = 0;
      idx = cf_getidx_mapattr(file,
			      attr_nums, 
			      attr_str,
			      attr_oid);
      
      if (idx == NULL) { /* try with only USE */
	for (i = 2; i < MAXATTRTYPE; i++)
	  attr_nums[i] = 0;
	idx = cf_getidx_mapattr(file,
				attr_nums, 
				attr_str,
				attr_oid);
      }
      
    }
    
  }
  if (idx == NULL) { /* check the names for this USE attribute */
      int i;
      char *temp;

      for (i = 0; all_indexes[i].name[0] != 0; i++) {
	if (all_indexes[i].attributes[1] == use) {
	  temp = strdup(all_indexes[i].name);
	  ToLowerCase(temp);
	  idx = cf_getidx_entry(file, temp);
	  free(temp);
	  if (idx != NULL) 
	    break;
	}
      }
  }


  if (idx) {
    char lastchar;
    
    entry = Tcl_FindHashEntry(attrib_hash_tab,idx->name);
    
    if (entry == NULL){
      if ((lastchar = (*SQL_Query)[strlen(*SQL_Query)-1]) == ' ') {
	strcat(*SQL_Query, idx->name);
	strcat(*SQL_Query, " A");
	lastchar = 'A';
      }
      else {
	char cbuf[3];       
	lastchar++; /* potentially we could have more than 28 items... */
	
	strcat(*SQL_Query, ", ");
	strcat(*SQL_Query, idx->name);
	sprintf(cbuf, " %c", lastchar);
	strcat(*SQL_Query, cbuf);
      }
      Tcl_SetHashValue( Tcl_CreateHashEntry(attrib_hash_tab,
					    idx->name,
					    &exists),
			(ClientData)lastchar);
    }
    else {
      lastchar = (char) Tcl_GetHashValue(entry);
    }
    
    switch (relation) {
    case LT:
      relat_val = "<";
      break;
    case LTEQ:
      relat_val = "<=";
      break;
    case EQUAL:
      relat_val = "=";
      break;
    case GTEQ:
      relat_val = ">=";
      break;
    case GT:
      relat_val = ">";
      break;
    case NEQ:
      relat_val = "<>";
      break;
    default:
      relat_val = "=";
      break;
    }
    
    term = attrPlusTerm->term;
    
    switch (term->which) {
    case e5_general:
    case e5_oid:
    case e5_characterString:
      size = (term->u.general->length) + 20 + strlen(idx->tag);
      result_term = CALLOC(char, size);            
      if (idx->type & KEYWORD) {
	if (relation == 0 || relation == EQUAL || relation > NEQ) {
	  sprintf(result_term,"%c.%s like '%%%s%%'",lastchar, idx->tag, 
		  term->u.general->data);
	}
	else {
	  sprintf(result_term,"%c.%s %s '%s'",lastchar, idx->tag, 
		  relat_val, term->u.general->data);
	}
      }
      else if (idx->type & INTEGER_KEY || idx->type & DECIMAL_KEY
	       || idx->type & FLOAT_KEY) {
	sprintf(result_term,"%c.%s %s %s",lastchar, idx->tag, 
		relat_val, term->u.general->data);
      }
      else {
	sprintf(result_term,"%c.%s %s '%s'",lastchar, idx->tag, 
		relat_val, term->u.general->data);
      }
      
      
      return(result_term);
      break;
    case e5_numeric: 
      size = term->u.general->length + 20 + strlen(idx->tag);
      result_term = CALLOC(char, size);      
      sprintf(result_term,"%c.%s %s %d",lastchar, idx->tag, 
	      relat_val, term->u.numeric);
      return(result_term);
      break;
    case e5_dateTime: 
      diagnostic_set(115,0,"DateTime");
      return(NULL);      
      break;
    case e5_external: 
      diagnostic_set(115,0,"External");
      return(NULL);      
      break;
    case e5_integerAndUnit:
      diagnostic_set(115,0,"IntegerAndUnit");
      return(NULL);      
      break;
    }
    
  }
  else { /* no matching index with all the criteria */

    diagnostic_set(114,use,NULL);
  }
  
  
  return(NULL);
}


/* se_dbms_rpn_structure: recursively resolves the query posed in the */
/* RPN structure. building and submitting an SQL query to create the  */
/* resultset (stored on the database) NULL is returned on errors -- a */
/* name of a resultset table is returned otherwise.                   */

char *se_dbms_rpn_structure(char *dbname, RPNStructure *s, char **SQL_Query,
			    ObjectIdentifier attributeSetId,
			    Tcl_HashTable *attrib_hash_tab) {

  char *left_res, *right_res, *bool_res;
  char *final_query;
  char *term_result;

  char *new_sql_query;
  int topflag = 0;

  int size;

  Operand *oper; 

  if (s == NULL) return (NULL); /* This SHOULD never happen */

  if (*SQL_Query == NULL) {
    new_sql_query = CALLOC(char, 1000);
    sprintf (new_sql_query,"SELECT * FROM ");
    *SQL_Query = new_sql_query;
    topflag = 1;
  }

  if (s->which == e3_rpnRpnOp) {
    /* recursively process the left hand side ... */
    left_res = se_dbms_rpn_structure(dbname, s->u.rpnRpnOp.rpn1, SQL_Query,
				     attributeSetId, attrib_hash_tab); 
    if (left_res == NULL)
      return (NULL);
    /* recursively process the right hand side ... */
    right_res = se_dbms_rpn_structure(dbname, s->u.rpnRpnOp.rpn2, SQL_Query,
				      attributeSetId, attrib_hash_tab);
    if (right_res == NULL)
      return (NULL);

    size = strlen(left_res) + strlen(right_res) + 6;
    bool_res = CALLOC(char, size);
    /* Have a left and right side, so process the operator */
    if (s->u.rpnRpnOp.op->which == e6_and) {
      sprintf(bool_res,"%s AND %s", left_res, right_res);
    }

    if (s->u.rpnRpnOp.op->which == e6_or) {
      sprintf(bool_res,"%s OR %s", left_res, right_res);
    }

    if (s->u.rpnRpnOp.op->which == e6_and_not) {
      sprintf(bool_res,"%s NOT %s", left_res, right_res);
    }

    if (s->u.rpnRpnOp.op->which == e6_prox) {
      FREE(left_res);
      FREE(right_res);
      diagnostic_set(129,0,""); /* no proximity support for SQL */
      return (NULL);
    }

    FREE(left_res);
    FREE(right_res);
    /* if we get to here */
    if (!topflag) {
      return(bool_res);
    }
    else {
      /* return the whole query */
      size = strlen(*SQL_Query) + strlen(bool_res) + 20;
      final_query = CALLOC(char, size);
      sprintf(final_query, "%s WHERE %s ", *SQL_Query, bool_res);
      FREE(new_sql_query);
      FREE(bool_res);
      *SQL_Query = final_query;
      return(final_query);
    }
  }
  else if (s->which == e3_op) {
    oper = s->u.op;
    if (oper->which == e4_resultSet) {
      diagnostic_set(129,0,""); /* no proximity support for SQL */
      return (NULL);
    }
    else { 
      /* Have to process a term or set of terms */      
      term_result = se_dbms_terms(dbname, oper->u.attrTerm, SQL_Query,
				  attrib_hash_tab);

      if (term_result == NULL) 
	return (term_result);

      if (!topflag) {
	return(term_result);
      }
      else {
	/* return the whole query */
	size = (strlen(*SQL_Query) + strlen(term_result) + 20);
	final_query = CALLOC(char, size);
	sprintf(final_query, "%s WHERE %s ", *SQL_Query, term_result);
	FREE(term_result);
	FREE(new_sql_query);
	*SQL_Query = final_query;
	return(final_query);
      }
    }
  }
  else
    return(NULL); /* error: neither operand or binop?? */
}


char *se_dbms_query(char *database_name, RPNQuery *RPN_Query)
{
  char *sql_query;
  char *result_query;

  Tcl_HashTable hash_tab_data;

  if (RPN_Query == NULL) 
    return (NULL);

  sql_query = NULL;

  Tcl_InitHashTable(&hash_tab_data,TCL_STRING_KEYS);

  /* should check RPN_Query->attributeSetId to be sure it is one */
  /* we can process                                              */
  result_query = se_dbms_rpn_structure(database_name, RPN_Query->rpn, 
				       &sql_query,
				       RPN_Query->attributeSet,
				       &hash_tab_data);

  Tcl_DeleteHashTable(&hash_tab_data);

  return(result_query);

}


/* this emulates a scan against the appropriate index */
int
se_dbms_scan( ZSESSION *session, char *filename, idx_list_entry *idx, Term *term, 
	      ListEntries *le, int stepSize, int numberOfTermsRequested,
	      int preferredPositionInResponse) 
{

  struct entries_List13	*entry, *entry_head, *entry_last;  
  char *dbname;
  char *column;
  char *startterm;
  char SQLString[500];
  
  int returncode;
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j;
  char **colnames;
  char  *fullquery;
  config_file_info *cf; 
  TermInfo *ti; 
#ifdef MYSQLDBMS
  MYSQL *conn;

  conn = mysql_init(NULL);
  

#endif

#ifdef POSTGRESQL
  PGconn *conn;
  PGresult *res;

  cf = find_file_config(filename);

  if (preferredPositionInResponse > 1 
      || preferredPositionInResponse < 0) {
    char pos[50];
    /* do a diagnostic */
    sprintf(pos, "%d", preferredPositionInResponse);
    SetError(session, 233,diagnostic_string(233), pos);
    return(233);

  }

  if (cf->file_ptr != NULL) 
    conn = (PGconn *)cf->file_ptr;
  else
    conn = (PGconn *)cf_open_DBMS(cf, session->c_login_id, 
				  session->c_login_pwd);

  if (conn == NULL) {
    SetError(session, 2,diagnostic_string(2),
	     "Unable to open POSTGRESQL database");
    return(2);
  }

  entry = entry_head = entry_last = NULL;

  /* start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(LOGFILE, "Postgres BEGIN command failed\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL Error");
    return(2);
  }

  /*
   * should PQclear PGresult whenever it is no longer needed to avoid
   * memory leaks
   */
  PQclear(res);

  /*
   * fetch instances from the pg_database, the system catalog of
   * databases
   */

  sprintf (SQLString, "DECLARE myportal CURSOR FOR SELECT %s, count(%s) FROM %s GROUP BY %s ", 
	   idx->tag, idx->tag, idx->name, idx->tag);
  
  res = PQexec(conn, SQLString);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(LOGFILE, "DECLARE CURSOR command failed\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;

    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL Error");
    return(2);
  }
  PQclear(res);

  /* save the query in the session info */
  if (session->c_DBMS_query != NULL) {
    /* replace the old query */
    FREE(session->c_DBMS_query);
  }

  session->c_DBMS_query = strdup(SQLString);

  res = PQexec(conn, "FETCH ALL in myportal");
  if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
    fprintf(LOGFILE, "FETCH ALL command didn't return tuples properly\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;

    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL Error");
    return(2);
  }


  nFields = PQnfields(res);
  nTuples = PQntuples(res);
  count = 0;
  /* for each row ...*/
  for (i = 0; i < PQntuples(res) && count < numberOfTermsRequested ; i++) {
    count++;
    entry_last = entry;      
    entry = CALLOC(struct entries_List13, 1);
	
    if (entry_head == NULL)
      entry_head = entry;
    else
      entry_last->next = entry;
    
    entry->item = CALLOC(Entry, 1);
	
    entry->item->which = e19_termInfo;
    ti = entry->item->u.termInfo =  CALLOC(TermInfo, 1);
    
    ti->term = CALLOC(Term, 1);
    ti->term->which = e5_general;
    ti->term->u.general = NewOctetString(PQgetvalue(res,i,0));
    ti->globalOccurrences = atoi(PQgetvalue(res,i,1));


    i+= stepSize;

  }    

  le->entries = entry_head;

#endif

  return(0);

}






