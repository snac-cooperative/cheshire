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
*	Header Name:	lcctree.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to build and search a tree of LC
*                       Class number descriptions
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		11/15/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define LCCTREE
#include <cheshire.h>


/***************************************************************************/
/* buildclasstree - construct the LC Class search tree from the input file */
/*                  assumes that the input file is structured in hierarchic*/
/*                  order, with leading tabs on each line indicating the   */
/*                  level(depth) of the particular entry in the hierarchy  */
/***************************************************************************/
extern FILE *LOGFILE;

int
TKZ_LCCBuildCmd (inSession, interp, argc, argv)
     ClientData inSession;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  char *filename;
  FILE *fd;
  char line[512], *lineptr, *c, *textptr, *fgets();
  char junk;
  int level;
  int matchnum;
  int linelen;
  struct LCCLASSNODE *newnode;
  struct LCCLASSNODE *lastnode;
  char *lastcmin, *lastcmax;
  
  if (argc != 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " LCC_tree_file_name\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  filename = argv[1];

  
  if (NULL == (fd = fopen (filename, "r"))) {
    Tcl_AppendResult(interp, "Unable to open LC Class tree file",
		     (char *)NULL);
    return TCL_ERROR;
  }
  
  lastnode = NULL;
  lastcmin = "";
  lastcmax = "";
  
  while ((lineptr = fgets(line,511,fd))) {
    /* first allocate a new_var structure */
    if ((newnode = CALLOC(struct LCCLASSNODE, 1)) == NULL) {
      Tcl_AppendResult(interp,"couldn't allocate LC tree structure", 
		       (char *) NULL);
      return TCL_ERROR;
    }
    /* remove newline characters from each line */
    linelen = strlen(lineptr);
    if (lineptr[linelen-1] == '\n') 
      lineptr[linelen-1] = '\0';
    if ((newnode->desc = MALLOC(linelen)) == NULL) {
      Tcl_AppendResult(interp,"couldn't allocate LC tree text area",
		       (char *)NULL);
      return TCL_ERROR;
    }
    
    /* get level and point to first char of range */
    for (c = lineptr, level = 0; *c=='\t'; c++, level++);
    newnode->level = (char)level;
    
    /* set a pointer to the text description */
    textptr = strchr(c,':');
    if (textptr) {
      *textptr = '\0'; /* make two strings out of the line */
      textptr += 2;
    }
    else {
      Tcl_AppendResult(interp,"error: no colon in class_var desc. line",
		       (char *)NULL);
      return TCL_ERROR;
    }
    strcpy(newnode->desc,textptr); /* copy the text */
    
    /* read the range */
    if (isdigit(*c)) { /* numbers are always subordinate to chars */
      matchnum = sscanf(c,"%f%c%f",
			&newnode->fvalmin, &junk, &newnode->fvalmax);
      strcpy(newnode->cvalmin,lastcmin);
      strcpy(newnode->cvalmax,lastcmax);
      if (matchnum == 1) {
	newnode->type = FSINGLE;
	newnode->fvalmax = newnode->fvalmin;
      }
      else {
	if (matchnum == 3)
	  newnode->type = FRANGE;
	else {
	  Tcl_AppendResult(interp,"Bad Number: ",
			   c, " ", textptr, (char *)NULL);
	  return TCL_ERROR;
	}
      }
    }
    else {
      matchnum = sscanf(c,"%[ABCDEFGHIJKLMNOPQRSTUVWXYZ]%c%[ABCDEFGHIJKLMNOPQRSTUVWXYZ]",
			newnode->cvalmin,&junk,newnode->cvalmax);
      if (matchnum == 1) {
	newnode->type = CSINGLE;
	newnode->fvalmax = 999999.9;
	newnode->fvalmin = 0.0;
	strcpy(newnode->cvalmax,newnode->cvalmin);
      }
      else {
	if (matchnum == 3) {
	  newnode->type = CRANGE;
	  newnode->fvalmax = 999999.9;
	  newnode->fvalmin = 0.0;
	}
	else {
	  Tcl_AppendResult(interp,"Bad Number: ",
		  c, " ", textptr, (char *)NULL);
	  return TCL_ERROR;
	}
      }
      lastcmin = &newnode->cvalmin[0];
      lastcmax = &newnode->cvalmax[0];
    }
    /* set the root if this is the first line */
    if (lccroot == NULL) {
      lccroot = newnode;
      newnode->parent = NULL;
      lastnode = newnode;
    }
    else { /* insert the newnode into the tree */
      if (lastnode->level == newnode->level) {
	/* siblings */
	newnode->parent = lastnode->parent;
	if (lastnode->sibling == NULL) 
	  lastnode->sibling = newnode;
	else {
	  Tcl_AppendResult(interp,"LCTree Sibling error", (char *)NULL);
	  return TCL_ERROR;
	}
      }
      else if (lastnode->level < newnode->level) {
	/* a child */
	newnode->parent = lastnode;
	if (lastnode->child == NULL) 
	  lastnode->child = newnode;
	else {
	  Tcl_AppendResult(interp,"LCTree child error", (char *)NULL);
	  return TCL_ERROR;
	}
      }
      else if (lastnode->level > newnode->level) {
	/* a cousin or distant relative */
	while (lastnode->level > newnode->level)
	  lastnode = lastnode->parent;
	newnode->parent = lastnode->parent;
	if (lastnode->sibling == NULL) 
	  lastnode->sibling = newnode;
	else {
	  Tcl_AppendResult(interp,"LCTree cousin error", (char *)NULL);
	  return TCL_ERROR;
	}
      }
      /* set lastnode to the current node */
      lastnode = newnode;
    }		
  }
  fclose(fd);
  return TCL_OK; /* all done */
}


int
TKZ_LCCDestroyCmd (inSession, interp, argc, argv)
     ClientData inSession;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  
  if (argc != 1) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     "\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  /* nothing for now */
  return TCL_OK;;
}


int
lcc_inrange(node,alpha,num)
     struct LCCLASSNODE *node;
     char *alpha;
     float num;
{
  if ((strcasecmp(alpha,node->cvalmin) >= 0) && (strcasecmp(alpha,node->cvalmax) <= 0)) {
    /* in the ballpark */
    if (node->type == CSINGLE || node->type == CRANGE)
      return(1); /* a match */
    else { /* numeric range */
      if (node->fvalmin <= num && node->fvalmax >= num) 
	return(1);
      else
	return(0);
    }
  }
  else /* not in range */
    return(0);
  
}



/**************************************************************************/
/* lcc_getdesc - traverses the lcc hierarchy built above and puts strings */
/*               together the hierarchic description of an lc class_var       */
/**************************************************************************/
char *
lcc_getdesc(buffer,root,alpha,num)
     char *buffer;
     struct LCCLASSNODE *root;
     char *alpha;
     float num;
{
  int match;

  if (root == NULL) return(buffer);
  
  match = lcc_inrange(root,alpha,num);
  
  /* recursively traverse the child or sibling hierarchies */
  if (match) {
    strcat(buffer,"*");
    strcat(buffer,root->desc);
    return(lcc_getdesc(buffer,root->child,alpha,num));
  }
  else
    return(lcc_getdesc(buffer,root->sibling,alpha,num));
}


int
TKZ_LCCGetCmd (inSession, interp, argc, argv)
     ClientData inSession;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  
  char buffer[1000];
  char lcalpha[500];
  float lcnumber;

  if (argc != 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " \"LCC_ClassNumber\"\"", (char *) NULL);
    return TCL_ERROR;
  }

  if( sscanf(argv[1],
	     "%[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]%f",
	     lcalpha,
	     &lcnumber) == 2 ) {
    *buffer = '\0';
    lcc_getdesc( buffer, lccroot, lcalpha, lcnumber );
    
    Tcl_AppendResult(interp, buffer, (char *) NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, "Malformed LCC number \"", argv[1],
		     "\"", (char *) NULL);
    return TCL_ERROR;
  }
}




