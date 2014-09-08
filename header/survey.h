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
/* Structure for Survey Results */

typedef struct fsurvey_rec {
  int frec_num;
  int compauth;
  int partauth;
  int topic;
  int subject;
  int comptitle;
  int parttitle;
  int compcall;
  int partcall;
  int specific;
  int ontopic;
  int byauthor;
  int information;
  int library;
  int class_var;
  int paper;
  int thesis;
  int teach;
  int update;
  int fix;
  int recreation;
  int work;
  int personal;
  int hobby;
  int publish;
  int other;
  int infoamount;
  int satisfaction;
  int serendipity;
  int na7;
  int amount;
  int na8;
  int arrange;
  int na9;
  int learn;
  int na10;
  int ranked;
  int na11;
  int feedback;
  int na12;
  int hypertext;
  int na13;
  int email;
  int na14;
  int save;
  int na15;
  int ltime;
  int na16;
  int helpmess;
  int na17;
  int helpamount;
  int na18;
  int speed;
  int na19;
  int correct;
  int na20a;
  int overalltw;
  int na20b;
  int overallfs;
  int na20c;
  int overallde;
  int na20d;
  int overallrf;
  int age;
  int gender;
  int education;
  int area;
  int focus;
  int affiliation;
  int othersoft;
  int experience;
} fsurvey_rec; 

typedef struct vsurvey_rec {
  int rec_len;
  int vrec_num;
  char *whyother;	
  char *decision;
  char *best;
  char *worst;
  char *confusing;
  char *problems;
  char *improve;
  char *comments;
  char *q31other;
} vsurvey_rec;

