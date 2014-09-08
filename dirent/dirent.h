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
/*
 * dirent.h
 */

#ifndef _DIRENT_H
#  define _DIRENT_H

#  include <sys/types.h>
#  include <limits.h>

typedef unsigned short _ino_t;		/* i-node number (not used on DOS) */
typedef long _off_t;			/* file offset value */
#define ino_t _ino_t
#define off_t _off_t

#define MAXNAMLEN	255	/* maximum filename length		*/

#ifndef NAME_MAX
#define	NAME_MAX	(MAXNAMLEN - 1)
#endif

struct dirent			/* data from getdents()/readdir()	*/
{
    ino_t	d_ino;		/* inode number of entry		*/
    off_t	d_off;		/* offset of disk directory entry	*/
    wchar_t	d_reclen;	/* length of this record		*/
    char	d_name[MAXNAMLEN + 1];
};


/* The following nonportable ugliness could have been avoided by defining
 * DIRENTSIZ and DIRENTBASESIZ to also have (struct dirent *) arguments.
 * There shouldn't be any problem if you avoid using the DIRENTSIZ() macro.
 */

#define	DIRENTBASESIZ		(((struct dirent *)0)->d_name \
				- (char *)&((struct dirent *)0)->d_ino)

#define	DIRENTSIZ(namlen)	((DIRENTBASESIZ + sizeof(long) + (namlen)) \
				/ sizeof(long) * sizeof(long))



#  ifndef _BOOL_T_DEFINED
typedef unsigned char	bool;
#  define _BOOL_T_DEFINED
#  endif

#  define DIRBUF	8192	/* buffer size for fs-indep. dirs	*/
				/* must in general be larger than the	*/
				/* filesystem buffer size		*/

struct _dircontents {
    char		*_d_entry;
    struct _dircontents	*_d_next;
};

typedef struct _dirdesc {
    int			dd_id;	/* uniquely identify each open directory */
    long		dd_loc;	/* where we are in directory entry is this */
    struct _dircontents	*dd_contents;	/* pointer to contents of dir	*/
    struct _dircontents	*dd_cp;		/* pointer to current position	*/
} DIR;


#if defined (__STDC__)
#  define _PROTO(p)	p
#else
#  define _PROTO(p)	()
#  undef  const
#  undef  volatile
#endif

/* Functions */

extern DIR *		opendir	_PROTO ((const char *));
extern struct dirent * 	readdir _PROTO ((DIR *));
extern void		rewinddir _PROTO ((DIR *));

extern int		closedir _PROTO ((DIR *));
extern void		seekdir	_PROTO ((DIR *, off_t));
extern off_t		telldir	_PROTO ((DIR *));

extern int		chdir _PROTO ((const char *));
extern char * 		getcwd _PROTO ((char *, size_t));

extern int		mkdir _PROTO ((const char *));

extern int		rmdir _PROTO ((const char *));
extern int		scandir _PROTO ((char *,
			 struct dirent ***,
			 int (*)(const void *, const void *),
			 int (*)(const void *, const void *)));

extern int		_chdrive _PROTO ((int));
extern int		_getdrive _PROTO ((void));
extern char * 		_getdcwd _PROTO ((int, char *, int));

extern bool		IsHPFSFileSystem _PROTO ((char *));

#endif

