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
#include <stdio.h>
#include <time.h>
#ifdef LINUX
#include <sys/sysinfo.h>
#endif
#if (defined SOLARIS || defined ALPHA)
#include <sys/systeminfo.h>
#endif

#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>
#include "alpha.h"
#endif

#define FALSE 0
#define TRUE 1
#define DELIM 034

typedef struct {
     int trans_num;                 /* Transaction Number */
     int trans_code;                 /* Transaction Code -- see table below */
     int term_id;                    /* Terminal ID -- last part of IP addr */
     int session_id;                 /* Session ID */
     int server_id;                 /* Current Server ID from known_hosts
                                        below */
     int interface_type;             /* current Interface (bool 0 or prob 1) */     
     int display_type;               /* display type (review, short, etc.) --
					see table below */
     time_t cmdsent_time;            /* Command sent time */
     time_t outputcomp_time;         /* output complete time */
     int record_num;                 /* # of record selected */
     unsigned i_author : 1;          /* bit field indicating indexes used */
     unsigned i_title : 1;
     unsigned i_subject : 1;
     unsigned i_organization : 1;
     unsigned i_series : 1;
     unsigned i_date : 1;
     unsigned i_language : 1;
     unsigned i_ISBN : 1;
     unsigned i_LCCN : 1;
     unsigned i_ISSN : 1;
     unsigned i_rectype : 1;
     unsigned i_xauthor : 1;
     unsigned i_xtitle : 1;
     unsigned i_topic : 1;
     unsigned i_kw : 1;
     unsigned i_xca : 1;
     unsigned i_ut : 1;
     unsigned i_xsu : 1;
     unsigned i_hostitem : 1;
     unsigned i_conf : 1;
     unsigned i_call : 1;
     int new_server_id;              /* new_var selected server from known_hosts */
     int old_server_id;              /* old selected Server from known_hosts */
     unsigned bool1val : 1;          /* bit field indicating bool. settings */
     unsigned bool2val : 1;          /* where bool1-3val have 0=OR, 1=AND */
     unsigned bool3val : 1;          /* and bool1-3use have 0=UNUSED and */
     unsigned bool1use : 1;          /* 1=USED */
     unsigned bool2use : 1;
     unsigned bool3use : 1;
     int num_keywords;               /* number of keywords in search */
     int num_hits;                  /* Number of hits from search */
     int num_displayed;             /* number of records displayed */
     int error_code;                 /* Tcl or other error */
     int help_code;                  /* type of help invoked */
     int help_id;                    /* page id from help file */
} ftrans_rec;

typedef struct 
{
     int rec_len;                   /* record length */
     int trans_num;                 /* Transaction Number */
     char *trans_txt;                /* text describing transaction */
} vtrans_rec;

typedef struct
{
     char *hostname;                 /* name of host from Tcl/Tk list */
     char *longname;
     int hostid;                     /* arbitrary ID number for host */
} hosts;

#define NUM_HOSTS 19

hosts knownhosts[]= {
{"cheshire", "Astro/Math/Stat", 0},
{"melvyl", "Melvyl", 1},
{"ten", "Melvyl Ten Year", 2},
{"pe", "Melvyl Periodicals", 3},
{"mags", "Melvyl MAGS", 4},
{"comp", "Melvyl COMP", 5},
{"news", "Melvyl NEWS", 6},
{"penn", "Penn State", 7},
{"abi", "Melvyl ABI", 8},
{"cmu", "Carnegie Mellon U", 9},
{"att_cia", "CIA World Factbook", 10},
{"duke", "Duke Univ", 11},
{"gils", "Govt Info Locator", 12},
{"hkust", "Hong Kong U Sci Tech", 13},
{"ins", "Melvyl INS", 14},
{"lc_books", "Library of Congress", 15},
{"uncc", "UNC Chapel Hill", 16},
{"att", "ATT Library", 17},
{"cheshire3", "Test Cheshire A", 18},
{"trec", "TREC", 19}
}; 
     
/* TABLE OF TRANSACTION CODES FOR REFERENCE PURPOSES                        */
/*                                                                          */
/* TRANS_CODE     MEANING                                                   */
/*          0     New Host Selected                                         */
/*          1     New Search Interface Selected                             */
/*          2     New Display Type Selected                                 */
/*          3     Client Manually Restarted                                 */
/*          4     Help Viewed                                               */
/*          5     Boolean Search Performed                                  */
/*          6     Direct Ranking Search Performed                           */
/*          7     Cluster Ranking Search Performed                          */
/*          8     Mixed Boolean/Direct Ranking Search Performed             */
/*          9     Mixed Boolean/Cluster Ranking Search Performed            */
/*         10     Hypertext Author Search Performed                         */
/*         11     Hypertext Subject Search Performed                        */
/*         12     Record Relevance Feedback Search Performed                */
/*         13     Cluster Relevance Feedback Search Performed               */
/*         14     Search Results Displayed                                  */
/*         15     Search Terms Cleared                                      */
/*         16     History Viewed                                            */
/*         17     Search Redone from history window                         */
/*         18     Search History cleared                                    */
/*         19     Record Selected                                           */
/*         20     Record(s) Printed                                         */
/*         21     Record(s) e-mailed                                        */
/*         22     Record(s) saved                                           */
/*         23     Saved Records Viewed                                      */
/*         24     Saved Records window iconified                            */
/*         25     Saved Records e-mailed                                    */
/*         26     Saved Records Cleared                                     */
/*         27     Saved Records Window exited                               */
/*         28     Print Cancelled                                           */
/*         29     E-mail Cancelled                                          */
/*         30     Relevance Feedback Search Cancelled                       */
/*         31     Save Records Cancelled                                    */
/*         32     Change of Server Cancelled                                */
/*         33     Client Auto-restart                                       */
/*         34     Help Window Exited                                        */
/*         35     New Host Connected (after new_var host selected)              */
/*         36     Tcl Error                                                 */
/*         37     General (non-Tcl) Error                                   */
/*         38     Record de-selected                                        */
/*         39     Record displayed in separate doc window (TREC)            */

/* INDEXES USED BIT FIELD                                                   */
/*   position     Meaning                                                   */
/*          0     Author                                          */
/*          1     Title */
/*          2     subject                         */
/*          3     organization                      */
/*          4     series                              */
/*          5     date                             */
/*          6     Language                                          */
/*          7     ISBN                                         */
/*          8     LCCN                                 */
/*          9     ISSN */
/*         10     Record type */
/*         11     Exact Author */
/*         12     Exact Title */
/*         13     Topic Words */
/*         14     Keyword */
/*         15     Exact Organization */
/*         16     Uniform Title */
/*         17     Exact Subject */
/*         18     Periodical Source */
/*         19     Conference */
/*         20-31  Undefined                                                 */

/* HELP CODE MEANINGS                               */
/*          1     Quick Start                       */
/*          2     Help on Cheshire 2 through tkinfo */
/*          3     Help on Searching through tkinfo  */
/*          4     Send Developers a complaint       */
/*          5     Look at credits                   */

/* HELP ID # MEANINGS                               */
/* Nodes from cheshire2.info file are numbered 1XX  */
/* Nodes from search.info file are numbered 2XX     */
/* Miscellaneous (like quickstart) are numbered 0XX */
/*                                                  */
/*     001     Quick Start text                     */
/*     002     User supplied text                   */
/*     003     Cheshire II Credits                  */
/*                                                  */
/*     100     cheshire2.info Top                   */
/*     101     cheshire2.info Introduction          */
/*     102     cheshire2.info FAQ                   */
/*     103     cheshire2.info FAQ-1                 */
/*     104     cheshire2.info FAQ-2                 */
/*     105     cheshire2.info FAQ-3                 */
/*     106     cheshire2.info FAQ-4                 */
/*     107     cheshire2.info FAQ-5                 */
/*     108     cheshire2.info FAQ-6                 */
/*     109     cheshire2.info FAQ-7                 */
/*     110     cheshire2.info Gettting Started      */
/*     111     cheshire2.info Selecting a Host      */
/*     112     cheshire2.info Changing the Interface */
/*     113     cheshire2.info Changing Display Format */
/*     114     cheshire2.info Changing the Size of Downloads */
/*     115     cheshire2.info Conducting a Search   */
/*     116     cheshire2.info Boolean Searches      */
/*     117     cheshire2.info Ranked Searches       */
/*     118     cheshire2.info Hypertext Searches    */
/*     119     cheshire2.info Search History        */
/*     120     cheshire2.info Viewing Results       */
/*     121     cheshire2.info Moving Around         */
/*     122     cheshire2.info Changing the Display  */
/*     123     cheshire2.info Using your Results    */
/*     124     cheshire2.info Printing Results      */
/*     125     cheshire2.info E-mailing Results     */
/*     126     cheshire2.info Searching with your Results */
/*     127     cheshire2.info Saving Records        */
/*     128     cheshire2.info Viewing Saved Records */
/*     129     cheshire2.info Miscellany            */
/*                                                  */
/*     200     search.info Top                      */
/*     201     search.info Introduction             */
/*     202     search.info Where to Search          */
/*     203     search.info How to Search            */
/*     204     search.info Results equals 0??!!     */
/*     205     search.info Results equals 31287??!! */
/*     206     search.info So close and yet so far  */











