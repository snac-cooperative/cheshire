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
#include <stdio.h>
#include <ctype.h>
#define LCCTREE
#include <cheshire.h>

/***************************************************************************/
/* buildclasstree - construct the LC Class search tree from the input file */
/*                  assumes that the input file is structured in hierarchic*/
/*                  order, with leading tabs on each line indicating the   */
/*                  level(depth) of the particular entry in the hierarchy  */
/***************************************************************************/
lcc_buildclasstree(filename)
char *filename;
{
  FILE *fd;
  char line[512], *lineptr, *c, *textptr, *fgets();
  char junk;
  int level;
  int matchnum;
  int linelen;
  struct LCCLASSNODE *newnode;
  struct LCCLASSNODE *lastnode;
  char *lastcmin, *lastcmax;
  
  if (NULL == (fd = fopen (filename, "r"))) {
    fprintf (LOGFILE, "Unable to open LC Class tree file\n");
    exit (1);
  }
  
  lastnode = NULL;
  lastcmin = "";
  lastcmax = "";
  
  while (lineptr = fgets(line,511,fd)) {
    /* first allocate a new structure */
    if ((newnode = CALLOC(struct LCCLASSNODE, 1)) == NULL) {
      fprintf(LOGFILE,"couldn't allocate LC tree structure\n");
      exit (2);
    }
    /* remove newline characters from each line */
    linelen = strlen(lineptr);
    if (lineptr[linelen-1] == '\n') 
      lineptr[linelen-1] = '\0';
    if ((newnode->desc = MALLOC(linelen)) == NULL) {
      fprintf(LOGFILE,"couldn't allocate LC tree text area\n");
      exit (3);
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
      fprintf(LOGFILE,"error: no colon in class desc. line\n");
      exit (5);
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
	  fprintf(LOGFILE,"Bad Number: %s:%s\n",
		  c,textptr);
	  exit (6);
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
	  fprintf(LOGFILE,"Bad Number: %s:%s\n",
		  c,textptr);
	  exit (6);
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
	  fprintf(LOGFILE,"LCTree Sibling error\n");
	  exit (7);
	}
      }
      else if (lastnode->level < newnode->level) {
	/* a child */
	newnode->parent = lastnode;
	if (lastnode->child == NULL) 
	  lastnode->child = newnode;
	else {
	  fprintf(LOGFILE,"LCTree child error\n");
	  exit (8);
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
	  fprintf(LOGFILE,"LCTree cousin error\n");
	  exit (9);
	}
      }
      /* set lastnode to the current node */
      lastnode = newnode;
    }		
  }
  fclose(fd);
  return (0); /* all done */
}

lcc_destroyclasstree()
{
  /* nothing for now */
}

/**************************************************************************/
/* lcc_getdesc - traverses the lcc hierarchy built above and puts strings */
/*               together the hierarchic description of an lc class       */
/**************************************************************************/
char *lcc_getdesc(buffer,root,alpha,num)
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
		strcat(buffer,"\\");
		strcat(buffer,root->desc);
		return(lcc_getdesc(buffer,root->child,alpha,num));
	}
	else
		return(lcc_getdesc(buffer,root->sibling,alpha,num));
}

lcc_inrange(node,alpha,num)
struct LCCLASSNODE *node;
char *alpha;
float num;
{
	if ((strcmp(alpha,node->cvalmin) >= 0) && (strcmp(alpha,node->cvalmax) <= 0)) {
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


