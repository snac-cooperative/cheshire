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

#define PROXIMITY_WORKS 1

/* forward declarations */
weighted_result *se_andmerge(weighted_result *left, weighted_result *right);
weighted_result *se_ormerge(weighted_result *left, weighted_result *right,
			    int addweights);
weighted_result *se_notmerge(weighted_result *left, weighted_result *right);
weighted_result *se_proxmerge(ProximityOperator *op, 
			      weighted_result *left, weighted_result *right);
weighted_result *se_get_result_set(char *setname);
weighted_result *se_process_feedback(char *dbname, weighted_result *temp_res);
weighted_result *se_process_terms(char *dbname, 
				  AttributesPlusTerm *attrPlusTerm);

/* process_rpn_structure: recursively resolves the query posed in the */
/* RPN structure.  NULL is returned on errors -- a weighted result   */
/* list is returned otherwise.                                       */


weighted_result *se_process_rpn_structure(char *dbname, RPNStructure *s) {

  weighted_result *left_res, *right_res, *bool_res;

  Operand *oper; 

  if (s == NULL) return (NULL); /* This SHOULD never happen */

  if (s->which == e3_rpnRpnOp) {
    /* recursively process the left hand side ... */
    left_res = se_process_rpn_structure(dbname, s->u.rpnRpnOp.rpn1); 
    if (left_res == NULL && (s->u.rpnRpnOp.op->which == e6_and
			     || s->u.rpnRpnOp.op->which == e6_and_not))
      return (NULL);
    /* recursively process the right hand side ... */
    right_res = se_process_rpn_structure(dbname, s->u.rpnRpnOp.rpn2);
    if (right_res == NULL && s->u.rpnRpnOp.op->which == e6_and)
      return (NULL);

    /* Have a left and right side, so process the operator */
    if (s->u.rpnRpnOp.op->which == e6_and) {
      bool_res = se_andmerge (left_res, right_res);
      if (left_res->setid[0] == '\0') /* not stored set */
	FREE(left_res);
      if (right_res->setid[0] == '\0') /* not stored set */
	FREE(right_res);
      return(bool_res);
    }

    if (s->u.rpnRpnOp.op->which == e6_or) {
      bool_res = se_ormerge (left_res, right_res, 0);
      if (left_res && left_res->setid[0] == '\0') /* not stored set */
	FREE(left_res);
      if (right_res && right_res->setid[0] == '\0') /* not stored set */
	FREE(right_res);
      return(bool_res);
    }

    if (s->u.rpnRpnOp.op->which == e6_and_not) {
      bool_res = se_notmerge (left_res, right_res);
      if (left_res->setid[0] == '\0') /* not stored set */
	FREE(left_res);
      if (right_res->setid[0] == '\0') /* not stored set */
	FREE(right_res);
      return(bool_res);
    }

    if (s->u.rpnRpnOp.op->which == e6_prox) {
#ifndef PROXIMITY_WORKS
      diagnostic_set(129,0,""); /* no proximity support right now */
      return (NULL);
#else
      /* Proximity merging handles all other "boolean-like" operators */
      /* including Fuzzy Boolean operations and component restriction */
      /* however the output set may be the same as one of the inputs */
      bool_res = se_proxmerge (s->u.rpnRpnOp.op->u.prox, left_res, right_res);
      if (left_res != NULL && left_res != bool_res && left_res->setid[0] == '\0') /* not stored set */
	FREE(left_res);
      if (right_res != NULL && right_res != bool_res && right_res->setid[0] == '\0') /* not stored */
	FREE(right_res);
      return(bool_res);
#endif
    }

  }
  else if (s->which == e3_op) {
    oper = s->u.op;
    if (oper->which == e4_resultSet) {
      weighted_result *temp_res, *feedback_res;

      /* retrieve a stored result set id */
      temp_res = se_get_result_set(oper->u.resultSet->data);

      if (temp_res != NULL && temp_res->setid[0] == '\0') {
	/* this is a new result set -- therefore it must be from a */
	/* relevance feedback or nearest neighbor search */
	feedback_res = se_process_feedback(dbname, temp_res);
	FREE(temp_res);
	return (feedback_res);
      }
      else
	return (temp_res);
    }
    else { 
      /* Have to process a term or set of terms */
      return(se_process_terms(dbname, oper->u.attrTerm));
    }	   
  }
  else
    return(NULL); /* error: neither operand or binop?? */
}


weighted_result *se_process_query(char *database_name, RPNQuery *RPN_Query)
{
  ObjectIdentifier attributeSetId;

  if (RPN_Query == NULL) 
    return (NULL);

  /* should check RPN_Query->attributeSetId to be sure it is one */
  /* we can process                                              */
  return (se_process_rpn_structure(database_name, RPN_Query->rpn));

}








