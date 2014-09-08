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
/***************************************************************************
* Cheshire II Online Catalog System
*
* MARCLIB -- USMARC format handling library
*
* Author:	Ray Larson, ray@sherlock.berkeley.edu
*		School of Information Management and Systems, UC Berkeley
* 
* 
* Copyright (c) 1994-5 Regents of the University of California
* 
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose, without fee, and without a written agreement
* is hereby granted, provided that the above copyright notice and this
* paragraph and the following two paragraphs appear in all copies.
* 
* IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
* DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
* LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
* DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* 
* THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
* ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
* PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
* 
**************************************************************************/
/* #define DEBUGMARC 1 */

#ifdef WIN32
#include <io.h>
#define lseek _lseek
#define read _read
#endif
#ifdef MACOSX
#include <stdlib.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#ifndef SOLARIS
#ifndef WIN32
#include "strings.h"
#endif
#endif
#include <string.h>

#include <dmalloc.h>
#include "marc.h"


/***********************************************************/
/*  Maximum number of records count - set in SeekMarc      */
/***********************************************************/

int MaxRecs = 0;


/*********************************************************************/
/* GetNum converts a specifed number of characters to a number       */
/*********************************************************************/

int GetNum(s,n)
     char *s;
     int n;
{  
  char nbuf[10]; /* ints can be no more than 9 digits in current */
  /* architecture -- will need to change for Alpha */
  strncpy(&nbuf[0], s, n);
  nbuf[n] = '\0';
  return(atol(nbuf));
}


/*********************************************************************/
/* SetSubF  - attaches  a list of subfield records to a field record */
/*            returns NULL if there is no more memory                */
/*********************************************************************/
int
SetSubF(f, dat)
     MARC_FIELD *f;
     char *dat;
{
  MARC_SUBFIELD *s;
  char *tmp;

  if (*dat != SUBFDELIM) {
    /* No subfields? make a pseudo subfield A */
    if (f->tag[0] != '0' || (f->tag[0] == '0' && f->tag[1] != '0')) {
      if ((s = (MARC_SUBFIELD *)CALLOC(MARC_SUBFIELD,1)) == NULL) {
	fprintf(stderr, "couldn't allocate marc subfield structure\n");
	return(0);
      }
      f->subfcodes['a'] = (char)1 ;
      s->code = 'a';
      s->data = dat;
      f->subfield = s;
      f->lastsub = s;
      s->next = NULL;
      while (*dat != FIELDTERM && *dat != RECTERM && *dat != SUBFDELIM)
	dat++;
      if (*dat != SUBFDELIM)
	return(1);
    }
    else
      return(1);
  }

  while (*dat != FIELDTERM && *dat != RECTERM) {
    if (*dat == SUBFDELIM) {
      /* check for valid subfield code... */
      if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%",
		 (int)(*(dat+1)))) {
	if ((s = (MARC_SUBFIELD *)CALLOC(MARC_SUBFIELD,1)) == NULL) {
	  fprintf(stderr, "couldn't allocate marc subfield structure\n");
	  return(0);
	}
	f->subfcodes[0]++;
	f->subfcodes[*(dat+1)] = (char)1 ;
	s->code = *(dat+1);
	s->data = dat+2;
	if (f->lastsub != NULL)
	  f->lastsub->next = s;
	if (f->subfield == NULL)
	  f->subfield = s;
	f->lastsub = s;
	s->next = NULL;
      }
      else {
	/* might be a space ??? try to fix... */
	if (*(dat+1) == ' ') {
	  tmp = dat;
	  dat++;
	  while (*dat == ' ') dat++;
	  if (*dat == FIELDTERM) {
	    /* false subfield at end of field -- make it a blank */
	    *tmp = ' ';
	    return(1);
	  }
	  
	  /* otherwise check to see if a blank crept into the beginning */
	  /* % subfield added for Aleph records */
	  if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%",
		     (int)(*dat))) {
	    if ((s = (MARC_SUBFIELD *)CALLOC(MARC_SUBFIELD,1)) == NULL) {
	      fprintf(stderr, "couldn't allocate marc subfield structure\n");
	      return(0);
	    }
	    f->subfcodes[0]++;
	    f->subfcodes[*dat] = (char)1 ;
	    s->code = *(dat);
	    s->data = dat+1;
	    if (f->lastsub != NULL)
	      f->lastsub->next = s;
	    if (f->subfield == NULL)
	      f->subfield = s;
	    f->lastsub = s;
	    s->next = NULL;
	  }
	  else {
	    /* not a legal subfield so ...      */
	    /* could clobber the false subf delimiter and try again ...*/
	    /* but better just to say it is bad and continue */
	    fprintf(stderr,"Illegal MARC subfield in field %s starting: \"%s\"\n", f->tag, dat);
	    return(0);
	  }
	}
	else if (*(dat+1) == FIELDTERM) {
	  /* could clobber the false subf delimiter and continue ...*/
	  *dat = '\040';
	}
	else if (*(dat+1) == '@') {
	  /* this is a MELVYL thing that we change to "2" */
	  /* could clobber the false subf delimiter and continue ...*/
	  *dat = '\040';
	}
	else {
	  /* not a legal subfield so ...      */
	  /* could clobber the false subf delimiter and try again ...*/
	  /* but better just to say it is bad and continue */
	  fprintf(stderr,"Illegal MARC subfield in field %s starting: \"%s\"\n", f->tag, dat);
	  return(0);
	}
      }
    }
    dat++;
  }
  
  return(1);
}


/*********************************************************************/
/* SetField - installs a marc field into a field processing structure*/
/*            returns null if there are problems with allocation     */
/*********************************************************************/

MARC_FIELD *SetField(rec,dir)
     MARC_REC *rec;
     MARC_DIRENTRY_OVER *dir;
{
  MARC_FIELD *f;
  char *p;
  int i;
  
  if ((f = (MARC_FIELD *)CALLOC(MARC_FIELD,1)) == NULL) {
    fprintf(stderr,"couldn't allocate marc field structure\n");
    return(NULL);
  }
  if (rec->nfields == 0)
    rec->fields = f;
  rec->nfields++;
  if (rec->lastfield != NULL)
    rec->lastfield->next = f;
  rec->lastfield = f;
  for (i=0; i<3; i++)
    f->tag[i] = dir->tag[i];
  f->tag[3] = '\0';
  f->length = GetNum(dir->flen,4);
  f->data = rec->BaseAddr + GetNum(dir->fstart,5);
  f->indicator1 = *(f->data);
  f->indicator2 = *(f->data + 1);
  if (f->indicator1 == SUBFDELIM) {
    f->indicator1 = ' ';
    f->indicator2 = ' ';
    p = f->data;
  }
  else
    p = f->data + 2;

  f->next = NULL;
  f->subfield = NULL;
  f->lastsub = NULL;
  if (SetSubF(f,p))
    return(f);
  else
    return(NULL);
}


/*********************************************************************/
/* ReadMARC - reads a marc record into a buffer and returns          */
/*            the length of the record - also appends a NULL to the  */
/*            end the buffer                                         */
/*           Returns 0 at end-of-file                                */
/*********************************************************************/

int ReadMARC(file,buffer,buffsize)
     int file;
     char *buffer; /* input record buffer */
     int buffsize; /* size of input buffer*/
{
  int recoffset, lrecl;
  int i, currentpos;
  int rescan();

 startread:

  if(read(file,buffer,10) == 0)
    return(0); /* end of file */
  
  /* verify that the buffer does contain digits */
  for (i=0; i < 5; i++)
    if (!isdigit(buffer[i]) &&
	(recoffset = rescan(file,buffer)) == 0) {
      fprintf(stderr,"Bad record - unable to rescan\n");
      return(0);
    }

  for (; i < 10; i++) 
    if (buffer[i] == RECTERM) {
      currentpos = lseek(file,-(10-i),SEEK_CUR);
      fprintf(stderr,"Bad record offset? - rescanning\n");
      goto startread;
    }

  lrecl = GetNum(buffer,5);
  if (lrecl >= buffsize) {
    fprintf(stderr,"input buffer too small: buffer %d : record %d\n",
	    buffsize, lrecl);
    return(0);
  }
  if (read(file, buffer+10, lrecl-10) == 0)
    return(0); /* end of file */
  buffer[lrecl] = '\0';
  return(lrecl);
}    



/*********************************************************************/
/* ReadMARC - reads a marc record into a buffer and returns          */
/*            the length of the record - also appends a NULL to the  */
/*            end the buffer                                         */
/*           Returns 0 at end-of-file                                */
/*********************************************************************/

int ReadMARC2(file,buffer,buffsize)
     int file;
     char *buffer; /* input record buffer */
     int buffsize; /* size of input buffer*/
{
  int recoffset, lrecl;
  int i, currentpos;
  int rescan();
  char c;
 startread:

  i = 0;
  

  if(read(file,buffer+i,1) == 0)
    return(0); /* end of file */

  while (buffer[i] == '\n') {
    currentpos = read(file,buffer+i,1);
    if (currentpos == -1) {
      perror("Read fails");
    }
    if (currentpos == 0) {
      return(0);
    }
  }

  while (buffer[i] != RECTERM) {
    i++;
    currentpos = read(file,buffer+i,1);
    if (currentpos == -1) {
      perror("Read fails");
    }
    
  }
  lrecl = i;
  buffer[lrecl] = '\0';
  return(lrecl);
}    



int
rescan2(file,buffer,badpos)
     int file;
     char *buffer;
     int badpos;
{
  char c;
  int badcount, i;
  int currentpos;
  
  badcount = 0;
  
  /* seek back in the file to the record terminator */
  /*currentpos = lseek(file,-6,SEEK_CUR); */
  if (read(file,&c,1) == 0) { /*get one byte*/
#ifdef DEBUGMARC
    fprintf(stderr,"No Byte read in Rescan\n");    
#endif
  }

 
  while (c != RECTERM && read(file,&c,1) > 0) {
    badcount++;
  }

  /* skip non-digits */
  while (isdigit(c) == 0) {
    badcount++;
    if (read(file,&c,1)) {
#ifdef DEBUGMARC
      fprintf(stderr,"%c",c);
#endif
    }
    else /* end of file */ {
      fprintf(stderr,"End of file in rescan - quitting\n");
      exit(0);
    }
  }
#ifdef DEBUGMARC
  fprintf(stderr,"%d bytes skipped at file position %d\n",
	  badcount, currentpos+1);
#endif
  /* start putting digits into the buffer */
  buffer[0] = c;
#ifdef DEBUGMARC
  fprintf(stderr,"new record length = %c",c);
#endif
  for (i=1; i<5; i++) {
    if (read(file,&buffer[i],1) == 0) {
      fprintf(stderr,"End of file in rescan - quitting\n");
      exit(0);
    }
#ifdef DEBUGMARC
    fprintf(stderr,"%c",buffer[i]);
#endif
    if (isdigit(buffer[i]) == 0) {
#ifdef DEBUGMARC
      fprintf(stderr,"Non-digit in suspected record length\n");
#endif
      return(rescan2(file,buffer,badpos+badcount));
    }
  }
#ifdef DEBUGMARC
  fprintf(stderr,"\n\n");
#endif

  /* if we get to here should be a valid record (we hope) */
  return(lseek(file,0,SEEK_CUR) - 5);
}



/*********************************************************************/
/* rescan - pick through garbage preceding a Marc record             */
/*          returns the new record offset in the file when it is     */
/*          successful, returns zero when it fails to fix things     */
/*********************************************************************/
int
rescan(file,buffer,badpos)
     int file;
     char *buffer;
     int badpos;
{
  char c;
  int badcount, i;
  int currentpos;
  
  badcount = 0;
  
  /* seek back in the file to the record terminator */
  currentpos = lseek(file,-6,SEEK_CUR);
  if (read(file,&c,1) == 0) { /*get one byte*/
#ifdef DEBUGMARC
    fprintf(stderr,"No Byte read in Rescan\n");    
#endif
  }

  if (c == RECTERM) {
#ifdef DEBUGMARC
    fprintf(stderr,"Rescanning - previous record OK\n");
#endif
  }
  /* skip non-digits */
  while (isdigit(c) == 0) {
    badcount++;
    if (read(file,&c,1)) {
#ifdef DEBUGMARC
      fprintf(stderr,"%c",c);
#endif
    }
    else /* end of file */ {
      fprintf(stderr,"End of file in rescan - quitting\n");
      exit(0);
    }
  }
#ifdef DEBUGMARC
  fprintf(stderr,"%d bytes skipped at file position %d\n",
	  badcount, currentpos+1);
#endif
  /* start putting digits into the buffer */
  buffer[0] = c;
#ifdef DEBUGMARC
  fprintf(stderr,"new record length = %c",c);
#endif
  for (i=1; i<5; i++) {
    if (read(file,&buffer[i],1) == 0) {
      fprintf(stderr,"End of file in rescan - quitting\n");
      exit(0);
    }
#ifdef DEBUGMARC
    fprintf(stderr,"%c",buffer[i]);
#endif
    if (isdigit(buffer[i]) == 0) {
#ifdef DEBUGMARC
      fprintf(stderr,"Non-digit in suspected record length\n");
#endif
      return(rescan2(file,buffer,badpos+badcount));
    }
  }
#ifdef DEBUGMARC
  fprintf(stderr,"\n\n");
#endif

  /* if we get to here should be a valid record (we hope) */
  return(lseek(file,0,SEEK_CUR) - 5);
}


/*********************************************************************/
/* SeekMARC - uses offsets in an associator file to position the     */
/*            MARC file to a given logical record number -           */
/*            the associator file and MARC files must be open        */
/*            returns -1 on seek errors and the marc file position   */
/*            when successful.                                       */
/*********************************************************************/

int SeekMARC(marcfile,assocfile,recnumber)
     int marcfile, assocfile;
     int recnumber;
{
  int marcoffset;
  int seekreturn;
  
  seekreturn = lseek(assocfile,0,SEEK_CUR);
  read(assocfile,&MaxRecs,sizeof(int));
  
  if (recnumber > MaxRecs) {
    fprintf(stderr,"Bad marc record number in SeekMARC - past end of file\n");
    return (-1);
  }
  
  if (recnumber > 1) { 
    seekreturn = lseek(assocfile,((recnumber) * sizeof(int)),SEEK_SET);
    if (seekreturn == -1) {
      if (errno == EBADF)
	fprintf(stderr,"unable to use associator file\n");
      else if (errno == EINVAL)
	fprintf(stderr,"invalid seek value in SeekMARC\n");
      else
	fprintf(stderr,"associator file error in SeekMARC\n");
      return (-1);
    }
    else
      read(assocfile,&marcoffset,sizeof(int));
  }
  else if (recnumber == 1)
    marcoffset = 0;
  else if (recnumber < 1) {
    fprintf(stderr,"Bad marc record number in SeekMARC - %d\n",recnumber);
    return (-1);
  }
  
  if (marcoffset < 0) {
    return (marcoffset); /* indicating deleted record number */
  }
  
  /* if we get to here the record number must be OK */
  seekreturn = lseek(marcfile,marcoffset,SEEK_SET);
  if (seekreturn == -1) {
    if (errno == EBADF)
      fprintf(stderr,"unable to use marc file\n");
    else if (errno == EINVAL)
      fprintf(stderr,"invalid seek value in SeekMARC\n");
    else
      fprintf(stderr,"marc file error in SeekMARC\n");
    return (-1);
  }
  else
    return(seekreturn); /* return current file position */
}    


/*********************************************************************/
/* GetMARC - Moves a marc record into the marc processing structure  */
/*           and returns a pointer to the structure                  */
/*           if the copy flag is not zero a new record buffer is     */
/*           allocated and the input buffer copied to it             */
/*********************************************************************/
MARC_REC *GetMARC(buffer,lrecl,copy)
     char *buffer; /* input record buffer */
     int lrecl; /* size of input buffer*/
     int copy;
{
  MARC_DIRENTRY_OVER *dir;
  MARC_REC *m;
  char *record;
  
  if (copy) {
    if ((record = CALLOC(char, lrecl+1)) == NULL) {
      fprintf(stderr,"couldn't allocate record\n");
      return(NULL);
    }
    strcpy(record,buffer);
  }
  else
    record = buffer;

  if ((m = (MARC_REC *)CALLOC(MARC_REC, 1)) == NULL) {
    fprintf(stderr,"couldn't allocate marc processing structure\n");
    return(NULL);
  }
  m->length = lrecl;
  m->record = record;
  if (copy)
    m->copy = 1;
  else
    m->copy = 0;
  m->leader = (MARC_LEADER_OVER *)record;
  if (m->leader->IndCount != '2' && m->leader->SubFCount != '2') {
    /* this isn't MARC if it has other indicator counts etc. */
    fprintf(stderr,"Not a MARC record: Indicator Count and Subfield Delimiter count in the leader are not 2\n");
    return(NULL);
  }
  m->BaseAddr = record + GetNum(m->leader->BaseAddr,5);
  dir = (MARC_DIRENTRY_OVER *)(record + sizeof(MARC_LEADER_OVER));
  for (; isdigit(dir->tag[0]); dir++)
    if ( SetField(m,dir) == NULL) {
      fprintf(stderr,"could not setfield in getmarc\n");
      return(NULL);
    }
  return (m);
}     
 

/*********************************************************************/
/* fieldcopy  -  copy string From to string To stopping when either  */
/*            terminal null character, or a field terminator         */
/*            is encountered in the From string.                     */
/*            returns number of chars copied                         */
/*********************************************************************/
int
fieldcopy(To,From)
     char *To, *From;
{
  register char *t, *f;
  register int count;
  
  t = To;
  f = From;
  count = 0;
  
  while (*f && *f != FIELDTERM && *f != RECTERM) {
    *t++ = *f++;
    count++;
  }
  *t = '\0';
  return(count+1);
}


/*********************************************************************/
/* codeconvert  - convert subfield codes and other non-print chars   */
/*                in a null terminated string                        */
/*********************************************************************/
void
codeconvert(string)
     char *string;
{
  while (*string) {
    if (*string == SUBFDELIM) *string = '$';
    if (*string == FIELDTERM) *string = '+';
    if (*string == RECTERM) *string = '|';
    if (*string && iscntrl(*string)) *string = '?';
    string++;
  }
}


/*********************************************************************/
/* charconvert  - convert subfield codes and other non-print chars   */
/*                normal characters are returned unchanged           */
/*********************************************************************/

char charconvert(c)
     char c;
{
  switch (c) {
  case  SUBFDELIM :  return( '$');
  case  FIELDTERM :  return('+');
  case  RECTERM :  return('|');
  case  '\0': return(c);
  default:  return (iscntrl(c) ? '?' : c);
  }
}


/*********************************************************************/
/* subfcopy  -  copy string From to string To stopping when either   */
/*            terminal null character, subfield delim or a field     */
/*            terminator is encountered in the From string.          */
/*            returns number of chars copied                         */
/*            if flag is 1, non-ascii characters are removed         */
/*********************************************************************/
int
subfcopy(To,From,flag)
     char *To, *From;
     int flag;
{
  register char *t, *f;
  register int count;
  
  t = To;
  f = From;
  count = 0;
  
  while(*f && *f != FIELDTERM && *f != RECTERM && *f != SUBFDELIM) {
    if (flag) {
      if (*f > 0) {
	*t++ = *f++;
	count++;
      }
      else f++;
    }
    else {
      *t++ = *f++;
      count++;
    }
  }
  *t = '\0';
  return(count+1);
}


/********************************************************************/
/* tagcmp - compare two marc tags with X or x as wildcards.         */
/*          returns 0 for no match and 1 for match                 */
/********************************************************************/
#ifdef EXTERNAL_TAG_MATCH

/* lets the tagmatching be elsewhere */
extern int tagcmp(char *pattag, char *comptag);

#else
tagcmp(pattag,comptag)
     char *pattag, *comptag;
{
  int i;
  for (i = 0; i < 3; i++) {
    if (pattag[i] == 'x' || pattag[i] == 'X')
      continue;
    if (isdigit(pattag[i]) && pattag[i] == comptag[i])
      continue;
    else
      return(0);
  }
  /* must match */
  return(1);
} 
#endif

/*********************************************************************/
/* GetField - extract a field with a given tag from a marc           */
/*            processing structure. Returns a pointer to the         */
/*            field and copies the field to the buffer(if provided)  */
/*            successful. Returns NULL if it fails to find the       */
/*            field. Startf lets it start from later in a field list.*/
/*            Permits "wildcard" comparisons using tagcmp            */
/*********************************************************************/
MARC_FIELD *GetField(rec,startf,buffer,tag)
     MARC_REC *rec;
     MARC_FIELD *startf;
     char *buffer;
     char *tag;
{
  MARC_FIELD *f;
  
  if (rec == NULL && startf == NULL)
    return (NULL);
  if (buffer)
    buffer[0] = '\0';
  if (startf)
    f = startf;
  else
    f = rec->fields;
  
  while     (f && tagcmp(tag,f->tag) == 0)
    f = f->next;
  if (f && buffer)
    fieldcopy(buffer,f->data);
  return(f);
}
     

/*********************************************************************/
/* GetSubf - get a named subfield from a marc processing structure   */
/*           for a given field. returns a pointer to the subfield    */
/*           and copies it to a buffer (if provided) when successful */
/*           Returns NULL if it fails to find the subfield           */
/*********************************************************************/
MARC_SUBFIELD *GetSubf(f,buffer,code)
     MARC_FIELD *f;
     char *buffer;
     char code;
{
  MARC_SUBFIELD *s;
  
  if (f == NULL) return(NULL);

  if(f->subfcodes[code]) /* go for the data */ 
    { s = f->subfield;
      while(s)  
	{ if (s->code == code)
	    { if (buffer) subfcopy(buffer,s->data,0);
	      return(s);
	    }
	  s = s->next;
	}
    }
  return(NULL); /* didn't find it */
}

/************************************************************************/
/* NORMALIZE -- convert an LC call number to a standardized format      */
/*              returns NULL if call number is abnormal, out otherwise  */
/************************************************************************/
char *normalize(in, out)
     char *in, *out;
{
  char mainclass[4], mainsub[7];
  char decimal[7], subcutter[10];
  int  valmainsub, ofs, decflag;
  char sep;
  
  for (ofs = 0; ofs < sizeof mainclass - 1 && isalpha(*in); ofs++, in++)
    *(mainclass+ofs) = islower(*in) ? toupper(*in):*in;
  if (ofs && ofs < sizeof mainclass)
    *(mainclass+ofs) = '\0';
  else
    return(NULL);
  
  while (*in && isspace(*in))
    in++;                    /* skip blanks */
  
  for (ofs = 0; ofs < sizeof mainsub - 1 && isdigit(*in); ofs++, in++)
    *(mainsub+ofs) = *in;
  if (ofs && ofs < sizeof mainsub)
    *(mainsub+ofs) = '\0';
  else
    return(NULL);
  valmainsub = atoi(mainsub);
  
  if (*in == '\0') {
    decflag = 0;
    decimal[0] = '\0';
    subcutter[0] = '\0';
  }
  else {                      /* still more of the call number */
    while (*in && isspace(*in))
      in++;               /* skip blanks */
    if (*in == '.' && isdigit(*(in+1)))
      decflag = 1;
    else
      decflag = 0;
    in++;
    decimal[0] = '\0';
    if (decflag) {
      while(isspace(*in))
	in++;          /* skip blanks */
      for (ofs = 0; ofs < sizeof decimal - 1 && isdigit(*in); ofs++, in++)
	*(decimal+ofs) = *in;
      if (ofs < sizeof decimal)
	*(decimal+ofs) = '\0';
      else
	return(NULL);
    }
    
    /* this could be changed to separate successive cutters */
    while (*in && isspace(*in))
      in++;               /* skip blanks */
    if (*in == '.' && isalnum(*(in+1)))
      in++;
    for (ofs = 0; ofs < sizeof subcutter - 1 && isalnum(*in); ofs++, in++)
      *(subcutter+ofs) = islower(*in) ? toupper(*in):*in;
    if (ofs < sizeof subcutter)
      *(subcutter+ofs) = '\0';
    else
      return(NULL);
  }

  decflag ? (sep = '.') : (sep = ' ');
  sprintf(out, "%-3s%05d%c%-5s %-10s", mainclass,
	  valmainsub, sep, decimal, subcutter);
  return(out);
}


/* FREEMARC */
void
FreeMARC_SUBFIELD(MARC_SUBFIELD *s)
{

  if (s->next)
    FreeMARC_SUBFIELD(s->next);
  FREE(s);

}

void
FreeMARC_FIELD(MARC_FIELD *f) 
{

  if (f->next)
    FreeMARC_FIELD(f->next);

  if (f->subfield)
    FreeMARC_SUBFIELD(f->subfield);

  FREE(f);
}

void
FreeMARC_REC(MARC_REC *rec)
{
  if (rec->fields)
    FreeMARC_FIELD(rec->fields);

  if (rec->record && rec->copy)
    FREE(rec->record);

  FREE(rec);
}

