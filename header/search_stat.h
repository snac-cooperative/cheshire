/*
 *  Copyright (c) 1990-2002 [see Other Notes, below]. The Regents of the
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

/* these structures are used for accumulating stats on ranked retrieval */
#ifdef STATMAIN
int statflag = 0;
#else
extern int statflag;
#endif 

typedef struct sstat_info {
  int docid;             /* Document to which the measures apply          */
  int compid;            /* component id */                     
  int doclength;         /* computed document length                      */
  int querylength;       /* query length (in words without stopwords)     */
  int num_match_terms;   /* number of terms matching between q and doc    */
  int ndocs;             /* number of documents (records) in collection   */
  int dist_ndocs;        /* number of documents in distrib. collection    */
  int min_cf;            /* min collfreq of matching term in query        */
  int max_cf;            /* max collfreq of matching term in query        */
  int min_tf;            /* min docfreq of term found in document         */
  int max_tf;            /* max docfreq of term found in document        */
  int sum_entries;       /* sum of docs/collections for matching terms    */
  int min_entries;       /* min of docs/collections for matching terms    */
  int max_entries;       /* max of docs/collections for matching terms    */
  double X1;             /* average log of Query term freq over match     */ 
  double X2;             /* SQRT of querysize                             */ 
  double X3;             /* average log of Doc term freq over match       */ 
  double X4;             /* SQRT of doclength                             */ 
  double X5;             /* average of log IDF over all terms             */
  double X6;             /* log of number of matching terms               */ 
  double logodds;        /* calculated logodds of relevance               */
  double docwt;          /* final doc weight (probability of relevance    */

  
} sstat_info;

typedef struct query_stats {
  int totaldocs;
  char *comp_name;
  sstat_info docstat[1];
} query_stats;

#ifdef STATMAIN
query_stats *Query_Stats = NULL;
#else
extern query_stats *Query_Stats;
#endif



