/*
 *  Copyright (c) 1990-2001 [see Other Notes, below]. The Regents of the
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

/* these structures are used for "Flattened records */


typedef struct sdata_info {
  int start_tag_offset;    /* offset to beginning of start tag in buffer  */
  int content_start_offset;/* offset to beginning of contents in buffer   */ 
  int content_end_offset;  /* offset to end  of contents in buffer        */ 
  int end_tag_offset;      /* offset to beginning of end tag in buffer    */ 
  int data_end_offset;     /* offset to end of end tag in buffer          */ 
  int processing_flags;    /* for handling processing instructions        */
  int data_element_id;
  int occurrence_no;
  int parent;
  int subdata;
  int nextdata;
  int n_attr;
  int attr_start_offset;
  SGML_Data *data;         /* used during construction of flattened record */
} sdata_info;

typedef struct pi_info {
  int Instruction_Type ;    /* INDEXING or DISPLAY processing */
  int Instruction ;         /* subst_tag or subst_attr (for now) */
  char tag[100]; 
  char attr[100];
  char index_name[200];
} pi_info;

typedef struct output_record {
  char digest[16];
  int docid; /* should be the same as the key */
  int n_sgml_data;
  int n_pi;
  int attr_size;
  int var_area;
  int hash_tag_count;
  int buffersize;
  sdata_info sdata[1];
} Output_Record;


