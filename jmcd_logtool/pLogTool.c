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
*	Header Name:	pLogTool.c
*
*	Programmer:	Jerome McDonough
*
*	Purpose:	Transaction Log Open/Close/View
*
*	Usage:		pLogTool o $fixlogfilename $varlogfilename
*                       pLogTool c
*                       pLogTool r $recnum $fixlogfilename $varlogfilename
*
*	Variables:	dummy -- Tcl clientdata (ignore this)
*                       inter -- Tcl Interpreter
*                       argc -- int, # of arguments in calling function
*                       argv -- char **, arguments in calling function,
*                                    should only be 1, the transaction code
*
*	Return Conditions and Return Codes:	TCL_OK, TCL_ERROR
*
*	Date:		06/20/96
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1996.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "tcl.h"
#include "zlog.h"
#include "tranlog.h"

int pLogTool(dummy, interp, argc, argv)
ClientData dummy; /* not used */
Tcl_Interp *interp;
int argc;
char **argv;
{
     
     char *flog_name;
     char *vlog_name;
     static FILE *flog;                   /* fixed length data log */
     static FILE *vlog;                   /* variable length data log */
     ftrans_rec flog_rec;          /* struct for fixed transaction record */
     vtrans_rec vlog_rec;          /* struct for var. transaction record */
     int i;
     char param1;
     char tmpstr[9];
     char *recstr;
     int recnum = 0;
     long fixseek = 0, varseek = 0;
     long maxrecs;
     long vloc = 0;
     struct stat fstatbuf;
     char *fmode;
#ifdef WIN32
     fmode = "rb";
#else 
     fmode = "r";
#endif

     /* set things up */
     param1 = (char) *argv[1];
     
     memset (&tmpstr, 0, 9);
     
     memset (&flog_rec, 0, sizeof(ftrans_rec));
     memset (&vlog_rec, 0, sizeof(vtrans_rec));
     
     memset (&fstatbuf, 0, sizeof(struct stat));
     
     /* Big Switch for three different cases: 1. open up the specified
	log files; 2. close the currently open log files; and 3. display
	the specified log file record in the given Tcl/Tk text widget */
     switch(param1)
     {
     case 'o':
	  
	  /* check to see if we need to close file pointers and reopen */
	  /* and close out the file pointers if so.  */

	  if (flog != NULL)
	  {
	       fclose(flog);
	  }

	  if (vlog != NULL)
	  {
	       fclose(vlog);
	  }
	  
	  /* Get logfile names & create pointers */
	  
	  if ((flog = fopen(argv[2], fmode)) == NULL) 
	  {
	       Tcl_AppendResult(interp, "Can't open fixed log file in pLogTool.c", (char *) NULL);
	       return TCL_ERROR;
	       
	  }
	  
	  if ((vlog = fopen(argv[3], fmode)) == NULL) 
	  {
	       Tcl_AppendResult(interp, "Can't open variable log file in pLogTool.c", (char *) NULL);
	       return TCL_ERROR;
	  }
	  
	  break;
	  
		 
     case 'c':

	  /* close out the open file pointers */
	  fclose(flog);
	  fclose(vlog);
	  break;
	  
     case 'r':
	  
	  /* Get record number to retrieve */
	  strcpy (tmpstr, argv[2]);
	  recnum = atoi(tmpstr);

	  /* double check that the record number requested makes */
	  /* sense for the current log files and return error if not */
	  
	  stat(argv[3], &fstatbuf);
	  if ((recnum * sizeof(ftrans_rec)) > fstatbuf.st_size) 
	  {
	       Tcl_AppendResult(interp, "Record number goes past end of file.", (char *) NULL);
	       return TCL_ERROR;
	  }
	  
	  /* Get records from fixed and variable files */
	  
	  /* seek to appropriate locations in files and read records*/
	  fixseek = (recnum - 1) * (sizeof(ftrans_rec));
	  fseek(flog, fixseek, SEEK_SET);
	  fread(&flog_rec, sizeof(ftrans_rec), 1, flog);
	  
	  varseek = 0;
	  fseek(vlog, varseek, SEEK_SET);
  
	  for (i = 1; i < recnum; i++)
	  {
	       vloc = ftell(vlog);
	       fread(&varseek, sizeof(long), 1, vlog);
	       fseek(vlog, (vloc + varseek), SEEK_SET);
	  }
	  
	  vloc = ftell(vlog);
	  
	  /* NOTE: while varseek is mainly used to find size of records */
	  /* to move between them, here we're going to use it to get size */
	  /* of record for a malloc operation.  Don't panic. */
	  fread(&varseek, sizeof(long), 1, vlog);
	  vlog_rec.trans_txt = (char *) calloc(varseek, (sizeof(char)+ 1));
	  fseek(vlog, vloc, SEEK_SET);
	  
	  fread(&vlog_rec, sizeof(long), 2, vlog);
	  fread(vlog_rec.trans_txt, ((varseek * sizeof(char)) - (2 * sizeof(long))), 1, vlog);

	  /* allocate string big enough for a 29x80 character display, plus */
	  /* the length of the variable text from vrecord, plus an extra */
	  /* character for a hard return every 79 chars in variable text, */
	  /* plus one to grow on */
	  
	  recstr = (char *) calloc((2320 + strlen(vlog_rec.trans_txt) + \
				    ((strlen(vlog_rec.trans_txt)/79)) + 1), \
				   sizeof(char));

	  if (formRec(recstr, flog_rec, vlog_rec) != 1) 
	  {
	       Tcl_AppendResult(interp, "Problems formatting record in pLogTool.c.", (char *) NULL);
	       return TCL_ERROR;
	  }
	  
	  
	  /* set logrec Tcl/Tk variable to formatted record string */
	  /* DEVELOPMENT NOTE: recstr is a pointer to char -- need to */
	  /* calloc memory for it and then free it */
	  Tcl_SetVar(interp, "logrec", recstr, TCL_LEAVE_ERR_MSG);

	  free(vlog_rec.trans_txt);
	  free(recstr);
	  
     }

     
     return TCL_OK;

}

/* formRec -- takes a fixed length transaction log record and a variable */
/*            length transaction log record and produces a long, formatted */
/*            string used by Tcl/Tk to display record info to user */

int formRec(recstr, frecord, vrecord)
char *recstr;
ftrans_rec frecord;
vtrans_rec vrecord;
{
     int i;
     char tmpstr[81];
     char *tmpptr;
     char *tmpptr2;
     char tmpstr2[81];
     char tmpstr3[81];
     char tmpstr4[81];
     int txtlen = 0;
     
     /* clear out tmp. string */
     memset(tmpstr, 0, 81);
     memset(tmpstr2, 0, 81);
     memset(tmpstr3, 0, 81);
     memset(tmpstr4, 0, 81);
     
     /* set a pointer to point at beginning of finalized record string */
     tmpptr = recstr;
     
     /* print out each line in display, incrementing tmpptr as appropriate */
     /* each time */

     sprintf(tmpstr, "Transaction #: %6.0ld       Transaction Code: %3d %-25s\n\n", frecord.trans_num, frecord.trans_code, code_table[frecord.trans_code]);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "  Terminal ID: %6.0d       Z39.50 Server ID: %3d %-25s\n", frecord.term_id, frecord.server_id, server_table[frecord.server_id]);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "   Session ID: %6.0d   New Z39.50 Server ID: %3d %-25s\n", frecord.session_id, frecord.new_server_id, server_table[frecord.new_server_id]);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);

     sprintf(tmpstr, "                        Old Z39.50 Server ID: %3d %-25s\n\n", frecord.old_server_id, server_table[frecord.old_server_id]);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);

     if (frecord.interface_type == 0)
     {
	  sprintf(tmpstr2, "Bool");
     } else if (frecord.interface_type == 1)
     {
	  sprintf(tmpstr2, "Prob");
     }

#ifdef ALPHA
     ctime_r(&frecord.cmdsent_time, tmpstr3, 81);
#else
#if defined(LINUX) || defined(MACOSX)
     strcpy(tmpstr3,ctime(&frecord.cmdsent_time));
#else
     cftime(tmpstr3, "%X", &frecord.cmdsent_time);
#endif
#endif
     sprintf(tmpstr, "Usr Interface: %6s          Time Cmd Sent: %s\n", tmpstr2, tmpstr3);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     memset(tmpstr2, 0, 81);
     memset(tmpstr3, 0, 81);
     
     if (frecord.display_type == 0)
     {
	  sprintf(tmpstr2, "Review");
     } else if (frecord.display_type == 1)
     {
	  sprintf(tmpstr2, "Short");
     } else if (frecord.display_type == 2)
     {
	  sprintf(tmpstr2, "Long");
     } else 
     {
	  sprintf(tmpstr2, "MARC");
     }
#ifdef ALPHA
     ctime_r(&frecord.outputcomp_time, tmpstr3, 81);
#else
#if defined(LINUX) || defined(MACOSX)
     strcpy(tmpstr3,ctime(&frecord.outputcomp_time));
#else
     cftime(tmpstr3, "%X", &frecord.outputcomp_time);
#endif     
#endif
     sprintf(tmpstr, "  Rec. Format: %6s      Time Cmd Complete: %s\n", tmpstr2, tmpstr3);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     memset(tmpstr2, 0, 81);
     memset(tmpstr3, 0, 81);

     sprintf(tmpstr, "Selec. Rec. #: %6d\n\n", frecord.record_num);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "Boolean Indexes Used:\n\n");
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "   %u Author         %u Date       %u Rectype    %u X_Corp Author    %u Call #\n", frecord.i_author, frecord.i_date, frecord.i_rectype, frecord.i_xca, frecord.i_call);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "   %u Title          %u Language   %u X_Author   %u Uniform Title\n", frecord.i_title, frecord.i_language, frecord.i_xauthor, frecord.i_ut);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "   %u Subject        %u ISBN       %u X_Title    %u X_Subject\n", frecord.i_subject, frecord.i_ISBN, frecord.i_xtitle, frecord.i_xsu);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "   %u Organization   %u LCCN       %u Topic      %u Host Item\n", frecord.i_organization, frecord.i_LCCN, frecord.i_topic, frecord.i_hostitem);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "   %u Series         %u ISSN       %u Keyword    %u Conf\n\n", frecord.i_series, frecord.i_ISSN, frecord.i_kw, frecord.i_conf);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "Boolean Conjunctions:\n\n");
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);

     if (frecord.bool1val == 0)
     {
	  sprintf(tmpstr2, "or");
     } else if (frecord.bool1val == 1)
     {
	  sprintf(tmpstr2, "and");
     }

     if (frecord.bool2val == 0)
     {
	  sprintf(tmpstr3, "OR");
     } else if (frecord.bool2val == 1)
     {
	  sprintf(tmpstr3, "AND");
     }

     if (frecord.bool3val == 0)
     {
	  sprintf(tmpstr4, "or");
     } else if (frecord.bool3val == 1)
     {
	  sprintf(tmpstr4, "and");
     }
     
     
     sprintf(tmpstr, "   Button 1: %-3s   Button 2: %-3s   Button 3: %-3s\n", tmpstr2, tmpstr3, tmpstr4); 
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     memset(tmpstr2, 0, 81);
     memset(tmpstr3, 0, 81);
     memset(tmpstr4, 0, 81);
     
     if (frecord.bool1use == 0)
     {
	  sprintf(tmpstr2, "No");
     } else if (frecord.bool1use == 1)
     {
	  sprintf(tmpstr2, "Yes");
     }
     
     if (frecord.bool2use == 0)
     {
	  sprintf(tmpstr3, "No");
     } else if (frecord.bool2use == 1)
     {
	  sprintf(tmpstr3, "Yes");
     }
     
     if (frecord.bool3use == 0)
     {
	  sprintf(tmpstr4, "No");
     } else if (frecord.bool3use == 1)
     {
	  sprintf(tmpstr4, "yes");
     }
     
     sprintf(tmpstr, "      Used?: %-3s      Used?: %-3s      Used?: %-3s\n\n", tmpstr2, tmpstr3, tmpstr4); 
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     memset(tmpstr2, 0, 81);
     memset(tmpstr3, 0, 81);
     memset(tmpstr4, 0, 81);
     
     sprintf(tmpstr, "   # Keywords: %6d   Error Code: %6d\n", frecord.num_keywords, frecord.error_code);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);

     sprintf(tmpstr, "       # Hits: %6d    Help Code: %6d\n", frecord.num_hits, frecord.help_code);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);

     sprintf(tmpstr, "  # Displayed: %6d      Help ID: %6d\n\n", frecord.num_displayed, frecord.help_id);
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);
     
     sprintf(tmpstr, "Associated Variable Length Text:\n\n");
     sprintf(tmpptr, "%s", tmpstr);
     tmpptr += (strlen(tmpstr));
     memset(tmpstr, 0, 81);

     i = 0;
     
     for (tmpptr2 = vrecord.trans_txt; *tmpptr2 != '\0'; tmpptr2++)
     {
	  tmpstr[i] = *tmpptr2;
	  i++;
	  if (i == 76)
	  {
	       sprintf(tmpptr, "%s\n", tmpstr);
	       tmpptr += (strlen(tmpstr) + 1);
	       memset(tmpstr, 0, 81);
	       i = 0;
	  }
	  
     }
     
     sprintf(tmpptr, "%s", tmpstr);
     
     return 1;

}







