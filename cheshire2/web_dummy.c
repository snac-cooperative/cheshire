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
*       Header Name:    se_web_dummy.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        fake stub for driving search engine as a cgi script
*                      
*       Usage:          webcheshire searchkey
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           11/1/96
*       Revised:        
*       Version:        1.0
*       Copyright (c) 1996.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include <cheshire.h>
#include <dmalloc.h>

#define STATMAIN
#include "search_stat.h"


int
Cheshire_CloseUp(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}
int
Cheshire_Search(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}

int
Cheshire_Scan(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}

int
Cheshire_Sort(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}

int
Cheshire_Fetch_Results(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}

int
Cheshire_Delete_Results(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}

int
Cheshire_Get_Result_Names(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}

int
TileBar_Search(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}
int
TermFreq_Search(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

  Tcl_AppendResult(interp, "\"", argv[0], "\" not implemented in this " 
		   " version -- use the webcheshire interpreter", 
		   (char *) NULL);
  return TCL_ERROR;
}



