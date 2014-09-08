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
*	Header Name:	pTranLog.c
*
*	Programmer:	Jerome McDonough
*
*	Purpose:	Transaction Logging for Cheshire II client
*
*	Usage:		pTranLog (transaction_code)
*
*	Variables:	dummy -- Tcl clientdata (ignore this)
*                       inter -- Tcl Interpreter
*                       argc -- int, # of arguments in calling function
*                       argv -- char **, arguments in calling function,
*                                    should only be 1, the transaction code
*
*	Return Conditions and Return Codes:	TCL_OK, TCL_ERROR
*
*	Date:		10/23/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include <stdio.h>
#include "tcl.h"
#include "zlog.h"
#ifndef WIN32
#include <netdb.h>
#endif
#include <string.h>
#ifndef WIN32
#ifndef SOLARIS
#include <strings.h>
#endif
#endif
#ifdef WIN32
#include <stdlib.h>
#include <winsock2.h>
#include <sys/types.h>
#include <sys/stat.h>
#define STAT _stat
#else
#define STAT stat
#endif
#include "dmalloc.h"
#include "alpha.h"

int pTranLog(dummy, interp, argc, argv)
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
     char *transcode;              /* transaction code from Tcl/Tk */
     char *tmpstr=NULL;                 /* temp. string for translation of 
				      Tcl/Tk variables */
     char *tmpstr2=NULL;
     hosts tmphost;
     char hostname[100];
     struct hostent *hostinfo;
     static char ipadd[5];
     time_t curtime;               /* time log function executed */
     FILE *tmpfile;
     struct STAT buf;
     int vlogprint = FALSE;
     int vlogfree = FALSE;
     int i;
     char *fmode;

     /* set things up */
     time(&curtime);
     memset (&flog_rec, 0, sizeof(ftrans_rec));
     memset (&vlog_rec, 0, sizeof(vtrans_rec));
	 memset (hostname, 0, (100 * sizeof(char)));
     
     /* Get transcode & logfile names & create pointers */

     transcode = argv[1];

     if (flog == NULL || vlog == NULL) 
     {
	  
	  flog_name = Tcl_GetVar(interp, "fixedlogfilename", 0);
	  vlog_name = Tcl_GetVar(interp, "varlogfilename", 0);
#ifdef WIN32
	  fmode = "a+b";
#else 
	  fmode = "a+";
#endif
	  if ((flog = fopen(flog_name, fmode)) == NULL) 
	  {
	       fprintf(stderr, "Can't open fixed logfile in pTranLog.\n");
	  }
	  
	  if ((vlog = fopen(vlog_name, fmode)) == NULL) 
	  {
	       fprintf(stderr, "Can't open variable logfile in pTranLog.\n");
	  }
     }
     
     /* fill in universal parts of trans. rec. structures */
     
     tmpstr = Tcl_GetVar(interp, "transnum", 0);
     vlog_rec.trans_num = flog_rec.trans_num = atol(tmpstr);

     flog_rec.trans_code = atoi(argv[1]);

     if (*ipadd == 0) 
     {
	  tmpstr = (char *) getenv("DISPLAY");
#ifdef WIN32
	  gethostname(hostname, 100);
#else 
	  if ((tmpstr != NULL) && (*tmpstr != '\0') && (tmpstr[0] != ':')) 
	    {
	      i = strcspn(tmpstr, ":");
	      strncpy(hostname, tmpstr, i);
	      hostname[i] = '\0';
	    } else 
	      {
#if defined ALPHA || defined SOLARIS
	       sysinfo(SI_HOSTNAME, hostname, 100);
#endif
#if defined LINUX || defined HP
	       gethostname(hostname, 100);
#endif
	  }	  
#endif
	  hostinfo = gethostbyname(hostname);
	  memset(ipadd, 0, 5);
	  strncpy(ipadd, hostinfo->h_addr_list[0], 4);
     }
#ifdef WIN32
	flog_rec.term_id = 0;
#else
     flog_rec.term_id = inet_lnaof(ipadd);
#endif

     /* for the curious, the Tcl/Tk 'userid' isn't one -- it's a number
        identifying the number of times the client has been reset (as 
	close as we currently have to identifying individual user sessions) */
     tmpstr = Tcl_GetVar(interp, "userid", 0);
     flog_rec.session_id = atoi(tmpstr);

     tmpstr = Tcl_GetVar(interp, "currHost", 0);
     for (i = 0; i <= NUM_HOSTS; i++)
     {
	  tmphost = knownhosts[i];
	  
	  if (strcmp(tmphost.hostname, tmpstr) == 0) 
	  {
	       flog_rec.server_id = tmphost.hostid;
	       break;
	  }

     }

     tmpstr = Tcl_GetVar(interp, "currSearch", 0);
     if (strcmp("bl", tmpstr) == 0) 
     {
	  flog_rec.interface_type = 0;
     }
     else if (strcmp("pr", tmpstr) == 0) 
     {
	  flog_rec.interface_type = 1;
     }
     
     
     tmpstr = Tcl_GetVar(interp, "currFormat", 0);
     if (strcmp("review", tmpstr) == 0) 
     {
	  flog_rec.display_type = 0;
     }
     else if (strcmp("short", tmpstr) == 0)
     {
	  flog_rec.display_type = 1;
     }
     else if (strcmp("long", tmpstr) == 0)
     {
	  flog_rec.display_type = 2;
     }
     else if (strcmp("marc", tmpstr) == 0)
     {
	  flog_rec.display_type = 3;
     }
     
     getBSwitches(interp, &flog_rec);

	 
     /* depending on current transaction code, get appropriate data from
	Tcl/Tk global variables to fill in two trans. rec. structures */

     switch (atoi(transcode))
     {
     /* New Host Selected */
     case 0:
	  flog_rec.cmdsent_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "hostKLUDGE", TCL_GLOBAL_ONLY);
	  for (i = 0; i <= NUM_HOSTS; i++)
	  {
	       tmphost = knownhosts[i];
	       if (strcmp(tmphost.longname, tmpstr) == 0)
	       {
		    flog_rec.new_server_id = tmphost.hostid;
		    break;
	       }
	  }
	  tmpstr = Tcl_GetVar(interp, "currHost", 0);
	  for (i = 0; i <= NUM_HOSTS; i++)
	  {
	       tmphost = knownhosts[i];
	       if (strcmp(tmphost.hostname, tmpstr) == 0)
	       {
		    flog_rec.old_server_id = tmphost.hostid;
		    break;
	       }
	  }
	  break;
	  
     /* New Search Interface Type Selected */
     case 1:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* New Display Type Selected */
     case 2:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* Client Manually Restarted */
     case 3:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* Help Viewed */
     case 4:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "C2_helpinfo1", TCL_GLOBAL_ONLY);
	  flog_rec.help_code = atoi(tmpstr);
	  tmpstr = Tcl_GetVar(interp, "C2_helpinfo2", TCL_GLOBAL_ONLY);
	  flog_rec.help_id = atoi(tmpstr);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "C2_helpinfo3", TCL_GLOBAL_ONLY);
	  vlogprint = TRUE;
	  break;
	  
     /* Boolean Search Performed */
     case 5:
	  flog_rec.cmdsent_time = curtime;
	  getBIndexes(interp, &flog_rec);
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Direct Ranking Search Performed */
     case 6:
	  flog_rec.cmdsent_time = curtime;
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Cluster Ranking  Search Performed */
     case 7:
	  flog_rec.cmdsent_time = curtime;
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Mixed Boolean/Direct Ranking Search Performed */
     case 8:
	  flog_rec.cmdsent_time = curtime;
	  getBIndexes(interp, &flog_rec);
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Mixed Boolean/Cluster Ranking Search Performed */
     case 9:
	  flog_rec.cmdsent_time = curtime;
	  getBIndexes(interp, &flog_rec);
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Hypertext Author Search Performed */
     case 10:
	  flog_rec.cmdsent_time = curtime;
	  flog_rec.i_author = 1;
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Hypertext Subject Search Performed */
     case 11:
	  flog_rec.cmdsent_time = curtime;
	  flog_rec.i_subject = 1;
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Record Relevance Feedback Search Performed */
     case 12:
	  flog_rec.cmdsent_time = curtime;
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "FBcmd", TCL_GLOBAL_ONLY);
	  vlogprint = TRUE;
	  break;
	  
     /* Cluster Relevance Feedback Search Performed */
     case 13:
	  flog_rec.cmdsent_time = curtime;
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Search Results Displayed */
     case 14:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "nHits", TCL_GLOBAL_ONLY);
	  flog_rec.num_hits = atoi(tmpstr);
	  tmpstr = Tcl_GetVar(interp, "totRcvd", TCL_GLOBAL_ONLY);
	  flog_rec.num_displayed = atoi(tmpstr);
	  
/*	  flog_rec.num_hits = atoi(Tcl_GetVar(interp, "nHits", TCL_GLOBAL_ONLY));
	  flog_rec.num_displayed = atoi(Tcl_GetVar(interp, "totRcvd", 0)); */
	  break;
	  
     /* Search Terms Cleared */
     case 15:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* History Viewed */
     case 16:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* Search Redone from history window */
     /* NOTE -- this call can be invoked from either pHistoryBoolRedo or
	pHistoryProbRedo, and so it's cmdsent_time is a little delayed
	from the actual button press, but who really cares? */
     case 17:
	  flog_rec.cmdsent_time = curtime;
	  getBIndexes(interp, &flog_rec);
	  getKWcount(interp, &flog_rec);
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "cmd", 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Search History Cleared */
     case 18:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* Record Selected */
     case 19:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "SLi", TCL_GLOBAL_ONLY);
	  flog_rec.record_num = atoi(tmpstr);
	  vlog_rec.trans_txt = Tcl_GetVar2(interp, "searchSet", tmpstr, TCL_GLOBAL_ONLY);
	  vlogprint = TRUE;
	  break;

     /* Record(s) Printed */
     case 20:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "tmpfname", 0);
	  if ((tmpfile = fopen(tmpstr, "r")) == NULL)
	  {
	       fprintf(stderr, "pTranLog: can't open Tcl print file.\n");
	  }
	  STAT(tmpstr, &buf);
	  vlog_rec.trans_txt = (char *) calloc((buf.st_size + 1), sizeof(char));
	  fread(vlog_rec.trans_txt, sizeof(char), buf.st_size, tmpfile);
	  vlogprint = TRUE;
	  vlogfree = TRUE;
	  break;
	  
     /* record(s) e-mailed */
     case 21:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "tmpfname", 0);
	  if ((tmpfile = fopen(tmpstr, "r")) == NULL)
	  {
	       fprintf(stderr, "pTranLog: can't open Tcl email file.\n");
	  }
	  STAT(tmpstr, &buf);
	  vlog_rec.trans_txt = (char *) calloc((buf.st_size + 1), sizeof(char));
	  fread(vlog_rec.trans_txt, sizeof(char), buf.st_size, tmpfile);
	  vlogprint = TRUE;
	  vlogfree = TRUE;
	  break;
	  
     /* record saved */
     case 22:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "SLi", 0);
	  flog_rec.record_num = atoi(tmpstr);
	  vlog_rec.trans_txt = Tcl_GetVar2(interp, "searchSet", tmpstr, 0);
	  vlogprint = TRUE;
	  break;
	  
     /* Saved records viewed */
     case 23:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* Saved records window iconified */
     case 24:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* saved records e-mailed */
     case 25:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "tmpfname", 0);
	  if ((tmpfile = fopen(tmpstr, "r")) == NULL)
	  {
	       fprintf(stderr, "pTranLog: can't open Tcl email file.\n");
	  }
	  STAT(tmpstr, &buf);
	  vlog_rec.trans_txt = (char *) calloc((buf.st_size + 1), sizeof(char));
	  fread(vlog_rec.trans_txt, sizeof(char), buf.st_size, tmpfile);
	  vlogprint = TRUE;
	  vlogfree = TRUE;
	  break;
	  
     /* saved records cleared */
     case 26:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* saved records window exited */
     case 27:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* print cancelled */
     case 28:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* e-mail cancelled */
     case 29:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* relevance feedback search cancelled */
     case 30:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* save records cancelled */
     case 31:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* Change of server cancelled */
     case 32:
	  flog_rec.outputcomp_time = curtime;
	  break;
	  
     /* client restart */
     case 33:
	  flog_rec.outputcomp_time = curtime;
	  break;

     /* help window exited */
     case 34:
	  flog_rec.outputcomp_time = curtime;
  	  /* XXX - Need to fill in help_code and help_id here once Paul's stuff
	     is integrated */
	  break;
	  
     /* new_var host connected */
     case 35:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "lastHost", TCL_GLOBAL_ONLY);
	  for (i = 0; i <= NUM_HOSTS; i++)
	  {
	       tmphost = knownhosts[i];
	       
	       if (strcmp(tmphost.hostname, tmpstr) == 0) 
	       {
		    flog_rec.old_server_id = tmphost.hostid;
		    break;
	       }
	       
	  }

	  break;
	  
     /* TCL error */
     /* XXX This doesn't work.  Currently, all errors are logged as general */
     case 36:
	  flog_rec.outputcomp_time = curtime;
/* XXX following doesn't work.  Anyway to get error codes? */
/*	  tmpstr = Tcl_GetVar(interp, "TCL_ERROR", 0); */
/*	  flog_rec.error_code = atoi(tmpstr); */
	  tmpstr = interp->result;
	  tmpstr2 = Tcl_GetVar(interp, "errorInfo", 0);
	  vlog_rec.trans_txt = (char *) calloc((strlen(tmpstr) + strlen(tmpstr2) + 2), sizeof(char));
	  sprintf(vlog_rec.trans_txt, "%s|%s", tmpstr, tmpstr2);
	  vlogprint = TRUE;
	  vlogfree=TRUE;
	  break;
	  
     /* General error */
     case 37:
	  flog_rec.outputcomp_time = curtime;
	  vlog_rec.trans_txt = Tcl_GetVar(interp, "ERROR", TCL_GLOBAL_ONLY);
	  vlogprint = TRUE;
	  break;

     /* Record Deselected */
     case 38:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "SLi", TCL_GLOBAL_ONLY);
	  flog_rec.record_num= atoi(tmpstr);
	  vlog_rec.trans_txt = Tcl_GetVar2(interp, "searchSet", tmpstr, TCL_GLOBAL_ONLY);
	  vlogprint = TRUE;
	  break;
	  

     /* Record Displayed in Separate data window (TREC) */
     case 39:
	  flog_rec.outputcomp_time = curtime;
	  tmpstr = Tcl_GetVar(interp, "SLi", TCL_GLOBAL_ONLY);
	  flog_rec.record_num= atoi(tmpstr);
	  vlog_rec.trans_txt = Tcl_GetVar2(interp, "searchSet", tmpstr, TCL_GLOBAL_ONLY);
	  vlogprint = TRUE;
	  break;
	  
     }
     


     /* write records out to files */
     if (vlogprint)
     {
	  vlog_rec.rec_len = strlen(vlog_rec.trans_txt) + (2 * sizeof(long)) + 1;
     } else 
     {
	  vlog_rec.rec_len = (2 * sizeof(long)) + 1;
     }
     
     
     fwrite(&flog_rec, sizeof(ftrans_rec), 1, flog);
/*     fwrite(&vlog_rec, vlog_rec.rec_len, 1, vlog); */
     fwrite(&vlog_rec.rec_len, sizeof(long), 1, vlog);
     fwrite(&vlog_rec.trans_num, sizeof(long), 1, vlog);
     if (vlogprint) 
     {
	  fprintf(vlog, "%s\n", vlog_rec.trans_txt);
     } else 
     {
	  fprintf(vlog, "\n");
     }
     
     
     if (vlogfree)
     {
	  free(vlog_rec.trans_txt);
     }
     
     
     /* close files */
     fflush(flog);
     fflush(vlog);

     return TCL_OK;

}
/* Get Boolean Indexes from Tcl/Tk */
/* Has brains to know if pure boolean or mixed boolean/prob */
int getBIndexes(Tcl_Interp *interp, ftrans_rec *flog_rec)
{

     int num_idx = 0;
     int i;
     char *i_char;
     char *tmp_idx;
     int idx_used[4];
     
     memset(idx_used, 1, (4 * sizeof(int)));
     i_char = (char *) calloc(2, sizeof(char));
     
     
     if (flog_rec->interface_type == 0) /* if boolean interface ... */
     {
	  num_idx = 4;                  /* then there's 4 indexes ... */
     } else 
     {
	  num_idx = 2;                  /* otherwise, there's 2 */
     }	  

     /* for each index, get the index value, and flip appropriate bit in flog_rec */
     for (i = 0; i < num_idx; i++)
     {
	  sprintf(i_char, "%d", i);

	  tmp_idx = Tcl_GetVar2(interp, "index", i_char, TCL_GLOBAL_ONLY);

	  if ((strcmp("unused", tmp_idx)) == 0)
	  {
	       idx_used[i] = 0;
	  } else if ((strcmp("author", tmp_idx)) == 0)
	  {
	       flog_rec->i_author = 1;
	  } else if ((strcmp("title", tmp_idx)) == 0)
	  {
	       flog_rec->i_title = 1;
	  } else if ((strcmp("subject", tmp_idx)) == 0)
	  {
	       flog_rec->i_subject = 1;
	  } else if ((strcmp("organization", tmp_idx)) == 0)
	  {
	       flog_rec->i_organization = 1;
	  } else if ((strcmp("series", tmp_idx)) == 0)
	  {
	       flog_rec->i_series =1;
	  } else if ((strcmp("date", tmp_idx)) == 0)
	  {
	       flog_rec->i_date =1;
	  } else if ((strcmp("lang", tmp_idx)) == 0)
	  {
	       flog_rec->i_language =1;
	  } else if ((strcmp("isbn", tmp_idx)) == 0)
	  {
	       flog_rec->i_ISBN =1;
	  } else if ((strcmp("lccn", tmp_idx)) == 0)
	  {
	       flog_rec->i_LCCN =1;
	  } else if ((strcmp("issn", tmp_idx)) == 0)
	  {
	       flog_rec->i_ISSN =1;
	  } else if ((strcmp("record_type_key", tmp_idx)) == 0)
	  {
	       flog_rec->i_rectype =1;
	  } else if ((strcmp("xauthor", tmp_idx)) == 0)
	  {
	       flog_rec->i_xauthor =1;
	  } else if ((strcmp("xtitle", tmp_idx)) == 0)
	  {
	       flog_rec->i_xtitle =1;
	  } else if ((strcmp("topic", tmp_idx)) == 0)
	  {
	       flog_rec->i_topic =1;
	  } else if (((strcmp("kw", tmp_idx)) == 0) || ((strcmp("any", tmp_idx)) == 0))
	  {
	       flog_rec->i_kw =1;
	  } else if ((strcmp("xca", tmp_idx)) == 0)
	  {
	       flog_rec->i_xca =1;
	  } else if ((strcmp("ut", tmp_idx)) == 0)
	  {
	       flog_rec->i_ut =1;
	  } else if ((strcmp("xsu", tmp_idx)) == 0)
	  {
	       flog_rec->i_xsu =1;
	  } else if ((strcmp("host-item", tmp_idx)) == 0)
	  {
	       flog_rec->i_hostitem =1;
	  } else if ((strcmp("conf", tmp_idx)) == 0)
	  {
	       flog_rec->i_conf =1;
	  } else if ((strcmp("call", tmp_idx)) == 0)
	  {
	       flog_rec->i_call =1;
	  }
     }
     

     /* figure out which indexes used and set bool#used bits */
     /* first the boolean pair that's always around... */
     if (idx_used[0] && idx_used[1])
     {
	  flog_rec->bool1use = 1;
     }
     /* then the boolean sets specific to the boolean interface */
     if (flog_rec->interface_type == 0)
     {
	  if ((idx_used[0] || idx_used[1]) && (idx_used[2] || idx_used[3]))
	  {
	       flog_rec->bool2use = 1;
	  }
	  if (idx_used[2] && idx_used[3])
	  {
	       flog_rec->bool3use = 1;
	  }
     }
     free(i_char);
     
     return;
     
}

/* Get Boolean Switch values (i.e., and/or) from Tcl/Tk */
/* Has brains to know if pure boolean or mixed boolean/prob */
int getBSwitches(Tcl_Interp *interp, ftrans_rec *flog_rec)
{
     int num_switch = 0;
     int i;
     char *tmp_switch;
     char switchname[12];
     
     if (flog_rec->interface_type == 0) /* if boolean interface ... */
     {
	  num_switch = 3;               /* then there's 3 boolean buttons */
     } else 
     {
	  num_switch = 1;               /* otherwise, there's 1 */
     }	  

/* for each switch, get boolean value, and flip appropriate bit in flog_rec */
/* NOTE -- boolean value for all switches is initially set to 0 (OR) so we */
/* only need to change the bit if the switch has a value of AND (1) */
     for (i = 0; i < num_switch; i++)
     {
	  sprintf(switchname, "boolSwitch%d", i+1);

	  tmp_switch = Tcl_GetVar(interp, switchname, TCL_GLOBAL_ONLY);
     
	  if (((strcmp("and", tmp_switch)) == 0) || ((strcmp("AND", tmp_switch)) == 0))
	  {
	       switch (i) 
	       {
	       case 0:
		    flog_rec->bool1val = 1;
		    break;
	       case 1:
		    flog_rec->bool2val = 1;
		    break;
	       case 2:
		    flog_rec->bool3val = 1;
		    break;
	       default:
		    break;
	       }
	  }		  
     }
     return;
}

/* Count all of the Keywords in the search */
int getKWcount(Tcl_Interp *interp, ftrans_rec *flog_rec)
{
     int num_bflds = 0;
     char *i_char;
     char *searchTerms;
     int num_words = 0;
     int i;

     i_char = (char *) calloc(2, sizeof(char));

     if (flog_rec->interface_type == 0) /* if boolean interface ... */
     {
	  num_bflds = 4;                /* 4 bool. fields to extract ... */
     }
     else 
     {
	  num_bflds = 2;               /* otherwise, 2 boolean fields */
     }
     
     for (i = 0; i < num_bflds; i++)
     {
	  /* getting $searchTerm($i) for each boolean entry field */
	  
	  sprintf(i_char, "%d", i);

	  searchTerms = Tcl_GetVar2(interp, "searchTerm", i_char, TCL_GLOBAL_ONLY);
	  wordcount(searchTerms, &num_words);
	  
     }

     if (flog_rec->interface_type == 1) /* if probabilistic interface ... */
     {
	  searchTerms = Tcl_GetVar(interp, "probSearchTerm", TCL_GLOBAL_ONLY);
	  wordcount(searchTerms, &num_words);
     }
     
     flog_rec->num_keywords = num_words;
     
     free(i_char);
     
     return (num_words);
     
}

/* Count all of the words in a string */
/* totally braindead minimalist version of Unix wc program */
/* we don't initialize count because we're using it for a running total */
/* the '-' character counts as white space so double dashes in LC */
/* subject headings don't get counted as terms */
int wordcount(char *terms, int *count)
{
     int state = 0;
     char *cptr;
     
     cptr = terms;
     
     while (*cptr != '\0') 
     {
	  if (cptr[0] == ' ' || cptr[0] == '\n' || cptr[0] == '\t' || cptr[0] == '-')
	       state = 0;
	  else if (state == 0) 
	  {
	       state = 1;
	       (*count)++;
	  }
	  cptr++;
     }
     return (*count);
}

