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
/* z_init.c -- handle initialization of the cheshire2 client
 *             sets up tables and initializes important parts of the
 *             data structures for the system.
 *
 * Copyright (c) 1994-6 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Information Management and Systems, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef SOLARIS
#ifndef WIN32
#include "strings.h"
#endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifndef WIN32
#include <sys/param.h>
#endif
#include "tcl.h"

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "session.h"
#include "z_parameters.h"
#include "z3950_intern.h"

/* the following define will initialize the bib-1 attributes */
/* this is currently done in the cmdparse interface code     */
/* #define Z_PARSE_INTERFACE */
#include "z_parse.h"
#include "cheshire_version.h"

HOST_ENTRY hosts_table[] =
{
  "SUNSITE",    "bibfile", "sunsite.berkeley.edu", 2100, "",
  "ACADIA",      "AULS",     "131.162.1.70", 210, "",
  "DENVER",      "DPL",      "192.54.81.12", 210, "",
  "MINES",       "MIN",      "192.54.81.12", 210, "",
  "UDENVER",     "DNU",      "192.54.81.12", 210, "",
  "BOULDER",     "CUB",      "192.54.81.12", 210, "",
  "NCOLORADO",   "UNC",      "192.54.81.12", 210, "",
  "WYOMING",     "WYO",      "192.54.81.12", 210, "",
  "INDIANA",     "LUIN",     "libtag.ucs.indiana.edu", 210, "",
  "WISCONSIN",   "madison",  "z3950.adp.wisc.edu", 210, "",
  "CLEVELAND",   "cpl",      "clevxg.cpl.org",   210, "",
  "MELVYL",      "CDL90",      "melvyl.cdlib.org",  210, "",
  "COPAC",      "COPAC",      "z3950.copac.ac.uk",  2100, "",
  "RLIN",        "BIB",      "zinc.rlg.org",  200, "",
  "RLG",         "BIB",      "zinc.rlg.org",  200, "",
  "RLGTST",         "BIB",      "zinc.rlg.org",  210, "",
  "AUT",         "AUT",      "zinc.rlg.org",  200, "",
  "RLGSAM",         "SAM",      "zinc.rlg.org",  200, "",
  "WLI",         "WLI",      "zinc.rlg.org", 200, "", /* World law index */
  "FLP",         "FLP",      "zinc.rlg.org", 200, "", /* Foreign Legal per. */
  "PENN",        "catalog",  "128.118.88.200",  210, "",
  "ATT",         "books",    "z3950.bell-labs.com",  210, "",
  "OCLC",        "OLUC",     "rdsd-rs6000.dev.oclc.org",  210, "",
  "ERIC",        "ERIC",     "rdsd-rs6000.dev.oclc.org",  210, "",
  "NOTIS",       "ERIC",     "192.101.184.12",  210, "",
  "IREG",        "IREG",     "192.101.184.12",  210, "",
  "MATHSTAT",    "netlib",   "research.att.com",  210, "",
  "GAYLORD",     "bib",      "saturn.gaylord.com",  210, "",
  "TEST",        "cat",      "sherlock.berkeley.edu",  210, "",
  "TEST2",       "cat",      "briet.berkeley.edu",  210, "",
  "LC",    "voyager",    "z3950.loc.gov",  7090, "",
  "LC_BOOKS",    "voyager",    "z3950.loc.gov",  7090, "",
  "CNIDR",       "eric-digests", "kudzu.cnidr.org",  2210, "",
  "CNIDR2",       "USPAT", "kudzu.cnidr.org",  2211, "",
  "CHESHIRE",    "bibfile", "cheshire.lib.berkeley.edu", 2100, "",
  "CHESHIRE3",    "bibfile", "cheshire.lib.berkeley.edu", 2223, "",
  "INFOCAL",     "cat",      "infocal.berkeley.edu",   210, "",
  "UWASH",       "LCAT",     "denali.lib.washington.edu", 210, "",
  "ATT_CIA",     "factbook", "z3950.bell-labs.com", 210, "",/*CIA World Facts */
  "GILS",     "GILS",      "z3950.bell-labs.com", 210, "", /* GILS */
  "UMICH",     "MCAT",      "tag1.lib.umich.edu", 210, "", /* Univ. of Michigan */
  "UMICHBUS",     "INNOPAC",      "141.211.227.11", 210, "", /* Univ. od Mich. Business */
  "PENNLAW",     "INNOPAC",      "130.91.144.191", 210, "", /* U. Penn Law */
  "PURDUE",     "LUPW",      "tag.cc.purdue.edu", 210, "", /* Purdue */
  "OHIOLINK",     "INNOPAC",      "130.108.120.20", 210, "", /* OHIOLINK*/
  "HONGKONG","INNOPAC","202.40.216.17",210, "", /* Chinese Univ. of Hong Kong*/
  "HKAPA","INNOPAC","202.40.151.2",210, "", /* Hong Kong Acad Perf Arts */
  "HKUST","INNOPAC","ustlib.ust.hk",210, "", /* Hong Kong Univ Sci Tech */
  "DUKE","MARION","ducatalog.lib.duke.edu",210, "", /* */
  "BU","INNOPAC","128.197.130.200",210, "", /* Boston University */
  "CMU","unicorn","128.2.232.10",210, "", /* Carnegie Mellon */
  "UNCC","MARION","unclib.lib.unc.edu",210, "", /* UNC Chapel Hill */
  "WEL","INNOPAC","149.130.90.2",210, "", /* Wellesley */
  "SHERLOCK","bibfile","sherlock.berkeley.edu",2100, "", /* sherlock test server */
  "MYCROFT","bibfile","mycroft-homes.hip.berkeley.edu",2100, "", /* mycroft test server */
  "LIVERPOOL", "sf", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "SF", "sf", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "CUNARD", "Cunard", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "ARCHIVES", "archives", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "SFSUBJCLUSTER", "sfsubjcluster", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "CUNARDSUBJCLUSTER", "cunardsubjcluster", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "OWEN", "owen", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "OWEN3", "owen3", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "SF", "sf", "sca.lib.liv.ac.uk", 210, "", /* Univ. of Liverpool */
  "HDS", "HDS_Catalogue", "155.245.254.46", 210, "", /* Univ. of Essex HDS */
  "ESSEX", "HDS_Catalogue", "155.245.254.46", 210, "", /* Univ. of Essex HDS */
  "TREC", "trec", "otlet.sims.berkeley.edu", 2100, "", /* TREC Interactive */
  "RMIT", "ZIW", "z3950.mds.rmit.edu.au", 210, "", /* RMIT test database */
  "","","",0, "" /* Must be last */
};


/* TKZ_ClientInit --
 *
 *  This routine sets up some tables used in Z39.50 processing
 *
 * Results:
 *	none.
 *
 *----------------------------------------------------------------------
 */

int TKZ_ClientInit (interp)
     Tcl_Interp *interp;			/* Current interpreter. */
{
  int i;
  char tempstring[100];
  INDEX_REFERENCE b;
  extern INDEX all_indexes[];
  HOST_ENTRY *h;
  extern ZSESSION TKZ_User_Session;

  /* set the global variable for version */
  Tcl_SetVar(interp,"cheshire_version", CHESHIRE_VERSION, TCL_GLOBAL_ONLY);


  /* set up some default servers in the hosts table */
  for (h = hosts_table; h->server[0] != '\0'; h++) {
    Tcl_SetVar2(interp,"Z_HOSTS",h->server, h->server,
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT );
    Tcl_SetVar2(interp,"Z_HOSTS",h->server, h->hostname,
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    Tcl_SetVar2(interp,"Z_HOSTS",h->server, h->dbname, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    sprintf(tempstring,"%d",h->port);
    Tcl_SetVar2(interp,"Z_HOSTS",h->server, tempstring, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    Tcl_SetVar2(interp,"Z_HOSTS",h->server, h->authentication, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
  }

  /* set up the all_indexes table */
  for (b = all_indexes, i=0; b->name[0] != '\0' ; b++, i++) {
    
    Tcl_SetVar2(interp,"ALL_INDEXES",b->name, b->name,
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT );
    Tcl_SetVar2(interp,"ALL_INDEXES",b->name, b->desc,
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    
    sprintf(tempstring,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d",i, 
	    b->attributes[0] /* this is the attribute set type id */,
	    b->attributes[1], b->attributes[2], b->attributes[3], 
	    b->attributes[4], b->attributes[5], b->attributes[6],
	    b->attributes[7], b->attributes[8], b->attributes[9],
	    b->attributes[10], b->attributes[11], b->attributes[12]);

    Tcl_SetVar2(interp,"ALL_INDEXES",b->name, tempstring, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
  }

  init_display_formats();

}



