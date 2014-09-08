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
/*************************************************************************
*
*	Header Name:	sResults.c
*
*	Programmer:	Lucy Kuntz
*
*	Purpose:	Capturing Survey Results
*
*	Usage:	sResults()
*
*	Variables:
*
*	Return Conditions and Return Codes:  TCL_OK, TCL_ERROR
*
*	Date:	2/18/96
*	Revised:
*	Version: 1.0
*	Copyright(c) 1996.  The Regents of the University of California
*		All Rights Reserved
*
*************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#ifndef SOLARIS
#ifndef WIN32
#include <strings.h>
#endif
#endif
#include "survey.h"
#include "tcl.h"

#define FALSE 0
#define TRUE 1

int sResults(dummy, interp, argc, argv)
ClientData dummy; /* not used */
Tcl_Interp *interp;
int argc;
char **argv;
{

  char *fres_name;
  char *vres_name;
  FILE *fres;	/* fixed length results */
  FILE *vres;	/* variable length results */
  fsurvey_rec fres_rec;	/* struct for fixed results */
  vsurvey_rec vres_rec;	/* struct for variable results */
  char *tmpstr;		/* temp string */
  time_t curtime;	/* time stamp */
  int vresprint = FALSE;
  char *nullstr = "NULL";
  char *fmode;
  /* set up */
  time(&curtime);
  memset(&fres_rec,0,sizeof(fsurvey_rec));
  memset(&vres_rec,0,sizeof(vsurvey_rec));

  /* get logfile names & create pointers */

  fres_name = Tcl_GetVar(interp,"fixedresfilename",TCL_GLOBAL_ONLY);
  vres_name = Tcl_GetVar(interp,"varresfilename",TCL_GLOBAL_ONLY);
#ifdef WIN32
	  fmode = "a+b";
#else 
	  fmode = "a+";
#endif

  if ((fres = fopen(fres_name, fmode)) == NULL)
  {
	fprintf(stderr, "Can't open fixed resfile in sResults.\n");
  }

  if ((vres = fopen(vres_name, fmode)) == NULL)
  {
	fprintf(stderr, "Can't open variable resfile in sResults.\n");
  }

  /* get data */

  tmpstr = Tcl_GetVar(interp,"recordnum",TCL_GLOBAL_ONLY);
  fres_rec.frec_num = atoi(tmpstr);
  vres_rec.vrec_num = atoi(tmpstr);

  /* Question 1 */

  tmpstr = Tcl_GetVar(interp,"compauth",TCL_GLOBAL_ONLY); 
  fres_rec.compauth = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"partauth",TCL_GLOBAL_ONLY); 
  fres_rec.partauth = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"topic",TCL_GLOBAL_ONLY); 
  fres_rec.topic = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"subject",TCL_GLOBAL_ONLY); 
  fres_rec.subject = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"comptitle",TCL_GLOBAL_ONLY); 
  fres_rec.comptitle = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"parttitle",TCL_GLOBAL_ONLY); 
  fres_rec.parttitle = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"compcall",TCL_GLOBAL_ONLY); 
  fres_rec.compcall = atoi(tmpstr);

  /* Question 2 */

  tmpstr = Tcl_GetVar(interp,"specific",TCL_GLOBAL_ONLY); 
  fres_rec.specific = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"ontopic",TCL_GLOBAL_ONLY); 
  fres_rec.ontopic = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"byauthor",TCL_GLOBAL_ONLY); 
  fres_rec.byauthor = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"information",TCL_GLOBAL_ONLY); 
  fres_rec.information = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"library",TCL_GLOBAL_ONLY); 
  fres_rec.library = atoi(tmpstr);

  /* Question 3 */

  tmpstr = Tcl_GetVar(interp,"class",TCL_GLOBAL_ONLY); 
  fres_rec.class_var = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"paper",TCL_GLOBAL_ONLY); 
  fres_rec.paper = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"thesis",TCL_GLOBAL_ONLY); 
  fres_rec.thesis = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"teach",TCL_GLOBAL_ONLY); 
  fres_rec.teach = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"update",TCL_GLOBAL_ONLY); 
  fres_rec.update = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"fix",TCL_GLOBAL_ONLY); 
  fres_rec.fix = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"recreation",TCL_GLOBAL_ONLY); 
  fres_rec.recreation = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"work",TCL_GLOBAL_ONLY); 
  fres_rec.work = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"personal",TCL_GLOBAL_ONLY); 
  fres_rec.personal = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"hobby",TCL_GLOBAL_ONLY); 
  fres_rec.hobby = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"publish",TCL_GLOBAL_ONLY); 
  fres_rec.publish = atoi(tmpstr);

  tmpstr = Tcl_GetVar(interp,"other",TCL_GLOBAL_ONLY); 
  fres_rec.other = atoi(tmpstr);
  if (fres_rec.other == 1)
  {
	vres_rec.whyother = Tcl_GetVar(interp,"whyother",TCL_GLOBAL_ONLY);
	vresprint = TRUE;
  } else
  {
	vres_rec.whyother = nullstr;
  }

  /* Question 4 */

  tmpstr = Tcl_GetVar(interp,"infoamount",TCL_GLOBAL_ONLY); 
  fres_rec.infoamount = atoi(tmpstr);

  /* Question 5 */

  tmpstr = Tcl_GetVar(interp,"satisfaction",TCL_GLOBAL_ONLY); 
  fres_rec.satisfaction = atoi(tmpstr);

  /* Question 6 */

  tmpstr = Tcl_GetVar(interp,"serendipity",TCL_GLOBAL_ONLY); 
  fres_rec.serendipity = atoi(tmpstr);

  /* Question 7 */

  tmpstr = Tcl_GetVar(interp,"na7",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na7 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"amount",TCL_GLOBAL_ONLY); 
  	fres_rec.amount = atoi(tmpstr);
  } else 
  {
	fres_rec.amount = 0;
  }

  /* Question 8 */

  tmpstr = Tcl_GetVar(interp,"na8",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na8 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"arrange",TCL_GLOBAL_ONLY); 
  	fres_rec.arrange = atoi(tmpstr);
  } else 
  {
	fres_rec.arrange = 0;
  }

  /* Question 9 */

  tmpstr = Tcl_GetVar(interp,"na9",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na9 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"learn",TCL_GLOBAL_ONLY); 
  	fres_rec.learn = atoi(tmpstr);
  } else 
  {
	fres_rec.learn = 0;
  }

  /* Question 10 */

  tmpstr = Tcl_GetVar(interp,"na10",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na10 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"ranked",TCL_GLOBAL_ONLY); 
  	fres_rec.ranked = atoi(tmpstr);
  } else 
  {
	fres_rec.ranked = 0;
  }

  /* Question 11 */

  tmpstr = Tcl_GetVar(interp,"na11",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na11 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"feedback",TCL_GLOBAL_ONLY); 
  	fres_rec.feedback = atoi(tmpstr);
  } else 
  {
	fres_rec.feedback = 0;
  }

  /* Question 12 */

  tmpstr = Tcl_GetVar(interp,"na12",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na12 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"hypertext",TCL_GLOBAL_ONLY); 
  	fres_rec.hypertext = atoi(tmpstr);
  } else 
  {
	fres_rec.hypertext = 0;
  }

  /* Question 13 */

  tmpstr = Tcl_GetVar(interp,"na13",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na13 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"email",TCL_GLOBAL_ONLY); 
  	fres_rec.email = atoi(tmpstr);
  } else 
  {
	fres_rec.email = 0;
  }

  /* Question 14 */

  tmpstr = Tcl_GetVar(interp,"na14",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na14 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"save",TCL_GLOBAL_ONLY); 
  	fres_rec.save = atoi(tmpstr);
  } else 
  {
	fres_rec.save = 0;
  }

  /* Question 15 */

  tmpstr = Tcl_GetVar(interp,"na15",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na15 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"ltime",TCL_GLOBAL_ONLY); 
  	fres_rec.ltime = atoi(tmpstr);
  } else 
  {
	fres_rec.ltime = 0;
  }

  /* Question 16 */

  tmpstr = Tcl_GetVar(interp,"na16",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na16 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"helpmess",TCL_GLOBAL_ONLY); 
  	fres_rec.helpmess = atoi(tmpstr);
  } else 
  {
	fres_rec.helpmess = 0;
  }

  /* Question 17 */

  tmpstr = Tcl_GetVar(interp,"na17",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na17 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"helpamount",TCL_GLOBAL_ONLY); 
  	fres_rec.helpamount = atoi(tmpstr);
  } else 
  {
	fres_rec.helpamount = 0;
  }

  /* Question 18 */

  tmpstr = Tcl_GetVar(interp,"na18",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na18 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"speed",TCL_GLOBAL_ONLY); 
  	fres_rec.speed = atoi(tmpstr);
  } else 
  {
	fres_rec.speed = 0;
  }

  /* Question 19 */

  tmpstr = Tcl_GetVar(interp,"na19",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na19 = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"correct",TCL_GLOBAL_ONLY); 
  	fres_rec.correct = atoi(tmpstr);
  } else 
  {
	fres_rec.correct = 0;
  }

  /* Question 20 */

  tmpstr = Tcl_GetVar(interp,"na20a",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na20a = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"overalltw",TCL_GLOBAL_ONLY); 
  	fres_rec.overalltw = atoi(tmpstr);
  } else 
  {
	fres_rec.overalltw = 0;
  }

  tmpstr = Tcl_GetVar(interp,"na20b",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na20b = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"overallfs",TCL_GLOBAL_ONLY); 
  	fres_rec.overallfs = atoi(tmpstr);
  } else 
  {
	fres_rec.overallfs = 0;
  }

  tmpstr = Tcl_GetVar(interp,"na20c",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na20c = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"overallde",TCL_GLOBAL_ONLY); 
  	fres_rec.overallde = atoi(tmpstr);
  } else 
  {
	fres_rec.overallde = 0;
  }

  tmpstr = Tcl_GetVar(interp,"na20d",TCL_GLOBAL_ONLY); 
  if ((fres_rec.na20d = atoi(tmpstr)) == 0)
  {
  	tmpstr = Tcl_GetVar(interp,"overallrf",TCL_GLOBAL_ONLY); 
  	fres_rec.overallrf = atoi(tmpstr);
  } else 
  {
	fres_rec.overallrf = 0;
  }

  /* Question 21 */

  vres_rec.decision = Tcl_GetVar(interp,"decision",TCL_GLOBAL_ONLY);
  vresprint = TRUE;
 
  /* Question 22 */

  vres_rec.best = Tcl_GetVar(interp,"best",TCL_GLOBAL_ONLY);
  vresprint = TRUE;
 
  /* Question 23 */

  vres_rec.worst = Tcl_GetVar(interp,"worst",TCL_GLOBAL_ONLY);
  vresprint = TRUE;

  /* Question 24 */

  vres_rec.confusing = Tcl_GetVar(interp,"confusing",TCL_GLOBAL_ONLY);
  vresprint = TRUE;

  /* Question 25 */

  vres_rec.problems = Tcl_GetVar(interp,"problems",TCL_GLOBAL_ONLY);
  vresprint = TRUE;

  /* Question 26 */

  vres_rec.improve = Tcl_GetVar(interp,"improve",TCL_GLOBAL_ONLY);
  vresprint = TRUE;

  /* Question 27 */

  vres_rec.comments = Tcl_GetVar(interp,"comments",TCL_GLOBAL_ONLY);
  vresprint = TRUE;

  /* Question 28 */

  tmpstr = Tcl_GetVar(interp,"age",TCL_GLOBAL_ONLY); 
  fres_rec.age = atoi(tmpstr);

  /* Question 29 */

  tmpstr = Tcl_GetVar(interp,"gender",TCL_GLOBAL_ONLY); 
  fres_rec.gender = atoi(tmpstr);

  /* Question 30 */

  tmpstr = Tcl_GetVar(interp,"education",TCL_GLOBAL_ONLY); 
  fres_rec.education = atoi(tmpstr);

  /* Question 31 */

  tmpstr = Tcl_GetVar(interp,"area",TCL_GLOBAL_ONLY); 
  fres_rec.area = atoi(tmpstr);
  if (fres_rec.area == 11)
  {
	vres_rec.q31other = Tcl_GetVar(interp,"q31other",TCL_GLOBAL_ONLY);
  	vresprint = TRUE;
  } else
  {
	vres_rec.q31other = nullstr;
  }

  /* Question 32 */

  tmpstr = Tcl_GetVar(interp,"focus",TCL_GLOBAL_ONLY); 
  fres_rec.focus = atoi(tmpstr);

  /* Question 33 */

  tmpstr = Tcl_GetVar(interp,"affiliation",TCL_GLOBAL_ONLY); 
  fres_rec.affiliation = atoi(tmpstr);

  /* Question 34 */

  tmpstr = Tcl_GetVar(interp,"othersoft",TCL_GLOBAL_ONLY); 
  fres_rec.othersoft = atoi(tmpstr);

  /* Question 35 */

  tmpstr = Tcl_GetVar(interp,"experience",TCL_GLOBAL_ONLY); 
  fres_rec.experience = atoi(tmpstr);

  /* Writing to files */

  if (vresprint)
  {
	vres_rec.rec_len = strlen(vres_rec.whyother) + 
	  strlen(vres_rec.decision) + strlen(vres_rec.best) + 
	  strlen(vres_rec.worst) + strlen(vres_rec.confusing) + 
	  strlen(vres_rec.problems) + strlen(vres_rec.improve) + 
	  strlen(vres_rec.comments) + strlen(vres_rec.q31other) + 
	  (2 * sizeof(int)) + 1;
  } else
  {
	vres_rec.rec_len = (2 * sizeof(long)) + 1;
  }

  fwrite (&fres_rec,sizeof(fsurvey_rec),1,fres);
  fwrite (&vres_rec.rec_len,sizeof(long),1,vres);
  fwrite (&vres_rec.vrec_num,sizeof(long),1,vres);

  if(vresprint)
  {
	fprintf(vres,"%s\n",vres_rec.whyother);
	fprintf(vres,"%s\n",vres_rec.decision);
	fprintf(vres,"%s\n",vres_rec.best);
	fprintf(vres,"%s\n",vres_rec.worst);
	fprintf(vres,"%s\n",vres_rec.confusing);
	fprintf(vres,"%s\n",vres_rec.problems);
	fprintf(vres,"%s\n",vres_rec.improve);
	fprintf(vres,"%s\n",vres_rec.comments);
	fprintf(vres,"%s\n",vres_rec.q31other);
  } else
  {
	fprintf(vres,"\n");
  }

  /* Close Files */

  fclose(fres);
  fclose(vres);

  return TCL_OK;

}

