/*
 * Defines for the dmalloc library
 *
 * Copyright 2000 by Gray Watson
 *
 * This file is part of the dmalloc package.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose and without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies, and that the name of Gray Watson not be used in
 * advertising or publicity pertaining to distribution of the document
 * or software without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The author may be contacted via http://dmalloc.com/
 *
 * $Id: dmalloc_dist.h,v 1.3 2001/12/26 18:25:58 ray Exp $
 */

#ifndef __DMALLOC_H__
#define __DMALLOC_H__

/* this is dmalloc.h.2 */
/* produced by configure, inserted into dmalloc.h */

/* const is available */
/* strdup is not a macro */
#undef DMALLOC_STRDUP_MACRO

/*
 * the definition of DMALLOC_SIZE
 *
 * NOTE: some architectures have malloc, realloc, etc.
 * using unsigned instead of unsigned long.  You may
 * have to edit this by hand to fix any compilation
 * warnings or errors.
 */
#include <sys/types.h>
#define DMALLOC_SIZE size_t

/*
 * We use stdarg.h for the dmalloc_message and
 * dmalloc_vmessage functions.
 */
#include <stdarg.h>
#define DMALLOC_STDARG 1

/* NOTE: start of $Id: dmalloc_dist.h,v 1.3 2001/12/26 18:25:58 ray Exp $ */

/* dmalloc version defines */
#define DMALLOC_VERSION_MAJOR	4	/* X.0.0-b0 */
#define DMALLOC_VERSION_MINOR	8	/* 0.X.0-b0 */
#define DMALLOC_VERSION_PATCH	2	/* 0.0.X-b0 */
#define DMALLOC_VERSION_BETA	0	/* 0.0.0-bX */

/* this defines what type the standard void memory-pointer is */
#if (defined(__STDC__) && __STDC__ == 1) || defined(__cplusplus)
#define DMALLOC_PNT		void *
#define DMALLOC_FREE_RET	void
#else
#define DMALLOC_PNT		char *
#define DMALLOC_FREE_RET	int
#endif

/*
 * Malloc function return codes
 */
#define CALLOC_ERROR		0L		/* error from calloc */
#define MALLOC_ERROR		0L		/* error from malloc */
#define REALLOC_ERROR		0L		/* error from realloc */

/* NOTE: this if for non- __STDC__ systems only */
#define FREE_ERROR		0		/* error from free */
#define FREE_NOERROR		1		/* no error from free */

#define DMALLOC_ERROR		0		/* function failed */
#define DMALLOC_NOERROR		1		/* function succeeded */

#define DMALLOC_VERIFY_ERROR	0		/* function failed */
#define DMALLOC_VERIFY_NOERROR	1		/* function succeeded */
#define MALLOC_VERIFY_ERROR	DMALLOC_VERIFY_ERROR
#define MALLOC_VERIFY_NOERROR	DMALLOC_VERIFY_NOERROR

#ifdef __cplusplus
extern "C" {
#endif

/* logfile for dumping dmalloc info, DMALLOC_LOGFILE env var overrides this */
extern	char		*dmalloc_logpath;

/* internal dmalloc error number for reference purposes only */
extern	int		dmalloc_errno;

/* address to look for.  when discovered call dmalloc_error() */
extern	DMALLOC_PNT	dmalloc_address;

/*
 * argument to dmalloc_address, if 0 then never call dmalloc_error()
 * else call it after seeing dmalloc_address for this many times.
 */
extern	int		dmalloc_address_count;

/*
 * Shutdown memory-allocation module, provide statistics if necessary
 */
extern
void	dmalloc_shutdown(void);

/*
 * Allocate and return a SIZE block of bytes.
 *
 * Returns 0L on error.
 */
extern
DMALLOC_PNT	malloc(DMALLOC_SIZE size);

/*
 * Allocate and return a block of _zeroed_ bytes able to hold
 * NUM_ELEMENTS, each element contains SIZE bytes.
 *
 * Returns 0L on error.
 */
extern
DMALLOC_PNT	calloc(DMALLOC_SIZE num_elements, DMALLOC_SIZE size);

/*
 * Resizes OLD_PNT to NEW_SIZE bytes and return the new space after
 * either copying all of OLD_PNT to the new area or truncating.  If
 * OLD_PNT is 0L then it will do the equivalent of malloc(NEW_SIZE).
 * If NEW_SIZE is 0 and OLD_PNT is not 0L then it will do the
 * equivalent of free(OLD_PNT) and will return 0L.
 *
 * Returns 0L on error.
 */
extern
DMALLOC_PNT	realloc(DMALLOC_PNT old_pnt, DMALLOC_SIZE new_size);

/*
 * Resizes OLD_PNT to NEW_SIZE bytes and return the new space after
 * either copying all of OLD_PNT to the new area or truncating.  If
 * OLD_PNT is 0L then it will do the equivalent of malloc(NEW_SIZE).
 * If NEW_SIZE is 0 and OLD_PNT is not 0L then it will do the
 * equivalent of free(OLD_PNT) and will return 0L.  Any extended
 * memory space will be zeroed like calloc.
 *
 * Returns 0L on error.
 */
extern
DMALLOC_PNT	recalloc(DMALLOC_PNT old_pnt, DMALLOC_SIZE new_size);

/*
 * Allocate and return a SIZE block of bytes that has been aligned to
 * ALIGNMENT bytes.  ALIGNMENT must be a power of two and must be less
 * than or equal to the block-size.
 *
 * Returns 0L on error.
 */
extern
DMALLOC_PNT	memalign(DMALLOC_SIZE alignment, DMALLOC_SIZE size);

/*
 * Allocate and return a SIZE block of bytes that has been aligned to
 * a page-size.
 *
 * Returns 0L on error.
 */
extern
DMALLOC_PNT	valloc(DMALLOC_SIZE size);

#ifndef DMALLOC_STRDUP_MACRO
/*
 * Allocate and return a block of bytes that contains the string STR
 * including the \0.
 *
 * Returns 0L on error.
 */
extern
char	*strdup(const char *str);
#endif /* ifndef DMALLOC_STRDUP_MACRO */

/*
 * Release PNT in the heap.
 *
 * Returns FREE_ERROR, FREE_NOERROR or void depending on whether STDC
 * is defined by your compiler.
 */
extern	DMALLOC_FREE_RET	free(DMALLOC_PNT pnt);

/*
 * same as free PNT
 */
extern	DMALLOC_FREE_RET	cfree(DMALLOC_PNT pnt);

/*
 * Log the heap structure plus information on the blocks if necessary.
 */
extern
void	dmalloc_log_heap_map(void);

/*
 * Dump dmalloc statistics to logfile.
 */
extern	void	dmalloc_log_stats(void);

/*
 * Dump unfreed-memory info to logfile.
 */
extern	void	dmalloc_log_unfreed(void);

/*
 * Verify pointer PNT, if PNT is 0 then check the entire heap.
 *
 * Returns MALLOC_VERIFY_ERROR or MALLOC_VERIFY_NOERROR
 */
extern	int	dmalloc_verify(const DMALLOC_PNT pnt);

/*
 * Verify pointer PNT, if PNT is 0 then check the entire heap.
 *
 * Returns MALLOC_VERIFY_ERROR or MALLOC_VERIFY_NOERROR
 */
extern	int	malloc_verify(const DMALLOC_PNT pnt);

/*
 * Set the global debug functionality FLAGS (0 to disable all
 * debugging).
 *
 * NOTE: you cannot remove certain flags such as signal handlers since
 * they are setup at initialization time only.  Also you cannot add
 * certain flags such as fence-post or free-space checking since they
 * must be on from the start.
 *
 * Returns the old debug flag value.
 */
extern	unsigned int	dmalloc_debug(const unsigned int flags);

/*
 * Returns the current debug functionality flags.  This allows you to
 * save a dmalloc library state to be restored later.
 */
extern	unsigned int	dmalloc_debug_current(void);

/*
 * int dmalloc_examine
 *
 * DESCRIPTION:
 *
 * Examine a pointer and return information on its allocation size as
 * well as the file and line-number where it was allocated.  If the
 * file and line number is not available, then it will return the
 * allocation location's return-address if available.
 *
 * RETURNS:
 *
 * Success - DMALLOC_NOERROR
 *
 * Failure - DMALLOC_ERROR
 *
 * ARGUMENTS:
 *
 * file -> File were we are examining the pointer.
 *
 * line -> Line-number from where we are examining the pointer.
 *
 * pnt -> Pointer we are checking.
 *
 * size_p <- Pointer to an unsigned int which, if not NULL, will be
 * set to the size of bytes from the pointer.
 *
 * file_p <- Pointer to a character pointer which, if not NULL, will
 * be set to the file where the pointer was allocated.
 *
 * line_p <- Pointer to a character pointer which, if not NULL, will
 * be set to the line-number where the pointer was allocated.
 *
 * ret_attr_p <- Pointer to a void pointer, if not NULL, will be set
 * to the return-address where the pointer was allocated.
 */
extern	int	dmalloc_examine(const DMALLOC_PNT pnt, DMALLOC_SIZE *size_p,
			 char **file_p, unsigned int *line_p,
			 DMALLOC_PNT *ret_attr_p);

#if DMALLOC_STDARG
/*
 * message writer with vprintf like arguments
 */
extern	void	dmalloc_vmessage(const char *format, va_list args);

/*
 * message writer with printf like arguments
 */
extern	void	dmalloc_message(const char *format, ...);
#endif

/*
 * Dmalloc function IDs for the dmalloc_track_t callback function.
 */
#define DMALLOC_FUNC_MALLOC	10	/* malloc function called */
#define DMALLOC_FUNC_CALLOC	11	/* calloc function called */
#define DMALLOC_FUNC_REALLOC	12	/* realloc function called */
#define DMALLOC_FUNC_RECALLOC	13	/* recalloc called */
#define DMALLOC_FUNC_MEMALIGN	14	/* memalign function called */
#define DMALLOC_FUNC_VALLOC	15	/* valloc function called */
#define DMALLOC_FUNC_STRDUP	16	/* strdup function called */
#define DMALLOC_FUNC_FREE	17	/* free function called */

typedef void  (*dmalloc_track_t)(const char *file, const unsigned int line,
				 const int func_id,
				 const DMALLOC_SIZE byte_size,
				 const DMALLOC_SIZE alignment,
				 const DMALLOC_PNT old_addr,
				 const DMALLOC_PNT new_addr);

/*
 * Register an allocation tracking function which will be called each
 * time an allocation occurs.  Pass in NULL to disable.
 */
extern	void	dmalloc_track(const dmalloc_track_t track_func);

/*
 * Return to the caller the current ``mark'' which can be used later
 * to dmalloc_log_changed pointers since this point.  Multiple marks
 * can be saved and used.
 */
extern	unsigned long	dmalloc_mark(void);

/*
 * Dump the pointers that have changed since the mark which was
 * returned by dmalloc_mark.  If not_freed_b is set to non-0 then log
 * the new pointers that are non-freed.  If free_b is set to non-0
 * then log the new pointers that are freed.  If details_b set to
 * non-0 then dump the individual pointers that have changed otherwise
 * just dump the summaries.
 */
extern	void	dmalloc_log_changed(const unsigned long mark, const int not_freed_b,
			     const int free_b, const int details_b);

/*
 * Dmalloc version of strerror to return the string version of
 * ERROR_NUM.
 *
 * Returns an invaid errno string if ERROR_NUM is out-of-range.
 */
extern
const char	*dmalloc_strerror(const int error_num);

/*
 * leap routine to malloc
 */
extern
DMALLOC_PNT	_malloc_leap(const char *file, const int line,
			     DMALLOC_SIZE size);

/*
 * leap routine to calloc
 */
extern
DMALLOC_PNT	_calloc_leap(const char *file, const int line,
			     DMALLOC_SIZE ele_n, DMALLOC_SIZE size);

/*
 * leap routine to realloc
 */
extern
DMALLOC_PNT	_realloc_leap(const char *file, const int line,
			      DMALLOC_PNT old_p, DMALLOC_SIZE new_size);

/*
 * leap routine to recalloc
 */
extern
DMALLOC_PNT	_recalloc_leap(const char *file, const int line,
			       DMALLOC_PNT old_p, DMALLOC_SIZE new_size);

/*
 * leap routine to memalign
 */
extern
DMALLOC_PNT	_memalign_leap(const char *file, const int line,
			       DMALLOC_SIZE alignment, DMALLOC_SIZE size);

/*
 * leap routine to valloc
 */
extern
DMALLOC_PNT	_valloc_leap(const char *file, const int line,
			     DMALLOC_SIZE size);

/*
 * leap routine to strdup
 */
extern
char	*_strdup_leap(const char *file, const int line, const char *str);

/*
 * leap routine to free
 */
extern
DMALLOC_FREE_RET	_free_leap(const char *file, const int line,
				   DMALLOC_PNT pnt);

/*
 * leap routine to malloc with error checking
 */
extern
DMALLOC_PNT	_xmalloc_leap(const char *file, const int line,
			      DMALLOC_SIZE size);

/*
 * leap routine to calloc with error checking
 */
extern
DMALLOC_PNT	_xcalloc_leap(const char *file, const int line,
			      DMALLOC_SIZE ele_n, DMALLOC_SIZE size);

/*
 * leap routine to realloc with error checking
 */
extern
DMALLOC_PNT	_xrealloc_leap(const char *file, const int line,
			       DMALLOC_PNT old_p, DMALLOC_SIZE new_size);

/*
 * leap routine to recalloc with error checking
 */
extern
DMALLOC_PNT	_xrecalloc_leap(const char *file, const int line,
				DMALLOC_PNT old_p, DMALLOC_SIZE new_size);

/*
 * leap routine to memalign with error checking
 */
extern
DMALLOC_PNT	_xmemalign_leap(const char *file, const int line,
				DMALLOC_SIZE alignment, DMALLOC_SIZE size);

/*
 * leap routine to valloc with error checking
 */
extern
DMALLOC_PNT	_xvalloc_leap(const char *file, const int line,
			      DMALLOC_SIZE size);

/*
 * leap routine for strdup with error checking
 */
extern
char 	*_xstrdup_leap(const char *file, const int line,
		       const char *str);

/*
 * leap routine to free
 */
extern
DMALLOC_FREE_RET	_xfree_leap(const char *file, const int line,
				    DMALLOC_PNT pnt);

#ifdef __cplusplus
}
#endif

/*
 * alloc macros to provide for memory FILE/LINE debugging information.
 */

#ifndef DMALLOC_DISABLE

#undef malloc
#define malloc(size) \
  _malloc_leap(__FILE__, __LINE__, size)
#undef calloc
#define calloc(count, size) \
  _calloc_leap(__FILE__, __LINE__, count, size)
#undef realloc
#define realloc(ptr, size) \
  _realloc_leap(__FILE__, __LINE__, ptr, size)
#undef recalloc
#define recalloc(ptr, size) \
  _recalloc_leap(__FILE__, __LINE__, ptr, size)
#undef memalign
#define memalign(alignment, size) \
  _memalign_leap(__FILE__, __LINE__, alignment, size)
#undef valloc
#define valloc(size) \
  _valloc_leap(__FILE__, __LINE__, size)
#ifndef DMALLOC_STRDUP_MACRO
#undef strdup
#define strdup(str) \
  _strdup_leap(__FILE__, __LINE__, str)
#endif
#undef free
#define free(ptr) \
  _free_leap(__FILE__, __LINE__, ptr)

#undef xmalloc
#define xmalloc(size) \
  _xmalloc_leap(__FILE__, __LINE__, size)
#undef xcalloc
#define xcalloc(count, size) \
  _xcalloc_leap(__FILE__, __LINE__, count, size)
#undef xrealloc
#define xrealloc(ptr, size) \
  _xrealloc_leap(__FILE__, __LINE__, ptr, size)
#undef xrecalloc
#define xrecalloc(ptr, size) \
  _xrecalloc_leap(__FILE__, __LINE__, ptr, size)
#undef xmemalign
#define xmemalign(alignment, size) \
  _xmemalign_leap(__FILE__, __LINE__, alignment, size)
#undef xvalloc
#define xvalloc(size) \
  _xvalloc_leap(__FILE__, __LINE__, size)
#undef xstrdup
#define xstrdup(str) \
  _xstrdup_leap(__FILE__, __LINE__, str)
#undef xfree
#define xfree(ptr) \
  _xfree_leap(__FILE__, __LINE__, ptr)

#ifdef DMALLOC_FUNC_CHECK

/*
 * do debugging on the following functions.  this may cause compilation or
 * other problems depending on your architecture.
 */
#undef bcmp
#define bcmp(b1, b2, len)		_dmalloc_bcmp(b1, b2, len)
#undef bcopy
#define bcopy(from, to, len)		_dmalloc_bcopy(from, to, len)

#undef memcmp
#define memcmp(b1, b2, len)		_dmalloc_memcmp(b1, b2, len)
#undef memcpy
#define memcpy(to, from, len)		_dmalloc_memcpy(to, from, len)
#undef memset
#define memset(buf, ch, len)		_dmalloc_memset(buf, ch, len)

#undef index
#define index(str, ch)			_dmalloc_index(str, ch)
#undef rindex
#define rindex(str, ch)			_dmalloc_rindex(str, ch)

#undef strcat
#define strcat(to, from)		_dmalloc_strcat(to, from)
#undef strcmp
#define strcmp(s1, s2)			_dmalloc_strcmp(s1, s2)
#undef strlen
#define strlen(str)			_dmalloc_strlen(str)
#undef strtok
#define strtok(str, sep)		_dmalloc_strtok(str, sep)

#undef bzero
#define bzero(buf, len)			_dmalloc_bzero(buf, len)

#undef memccpy
#define memccpy(s1, s2, ch, len)	_dmalloc_memccpy(s1, s2, ch, len)
#undef memchr
#define memchr(s1, ch, len)		_dmalloc_memchr(s1, ch, len)

#undef strchr
#define strchr(str, ch)			_dmalloc_strchr(str, ch)
#undef strrchr
#define strrchr(str, ch)		_dmalloc_strrchr(str, ch)

#undef strcpy
#define strcpy(to, from)		_dmalloc_strcpy(to, from)
#undef strncpy
#define strncpy(to, from, len)		_dmalloc_strncpy(to, from, len)
#undef strcasecmp
#define strcasecmp(s1, s2)		_dmalloc_strcasecmp(s1, s2)
#undef strncasecmp
#define strncasecmp(s1, s2, len)	_dmalloc_strncasecmp(s1, s2, len)
#undef strspn
#define strspn(str, list)		_dmalloc_strspn(str, list)
#undef strcspn
#define strcspn(str, list)		_dmalloc_strcspn(str, list)
#undef strncat
#define strncat(to, from, len)		_dmalloc_strncat(to, from, len)
#undef strncmp
#define strncmp(s1, s2, len)		_dmalloc_strncmp(s1, s2, len)
#undef strpbrk
#define strpbrk(str, list)		_dmalloc_strpbrk(str, list)
#undef strstr
#define strstr(str, pat)		_dmalloc_strstr(str, pat)

#endif /* DMALLOC_FUNC_CHECK */
#endif /* ! DMALLOC_DISABLE */

/* we start FUNC_CHECK again because we had to stop it above */
#ifdef DMALLOC_FUNC_CHECK

/*
 * feel free to add your favorite functions here and to arg_check.[ch]
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * copied in from arg_check.h with all comments removed
 */
extern
int	_dmalloc_bcmp(const void *b1, const void *b2, const DMALLOC_SIZE len);
extern
void	_dmalloc_bcopy(const void *from, void *to, const DMALLOC_SIZE len);
extern
int	_dmalloc_memcmp(const void *b1, const void *b2, const DMALLOC_SIZE len);
extern
void	*_dmalloc_memcpy(void *to, const void *from, const DMALLOC_SIZE len);
extern
void	*_dmalloc_memset(void *buf, const int ch, const DMALLOC_SIZE len);
extern
char	*_dmalloc_index(const char *str, const char ch);
extern
char	*_dmalloc_rindex(const char *str, const char ch);
extern
char	*_dmalloc_strcat(char *to, const char *from);
extern
int	_dmalloc_strcmp(const char *s1, const char *s2);
extern
DMALLOC_SIZE	_dmalloc_strlen(const char *str);
extern
char	*_dmalloc_strtok(char *str, const char *sep);
extern
void	_dmalloc_bzero(void *buf, const DMALLOC_SIZE len);
extern
void	*_dmalloc_memccpy(void *s1, const void *s2, const int ch,
			  const DMALLOC_SIZE len);
extern
void	*_dmalloc_memchr(const void *s1, const int ch, const DMALLOC_SIZE len);
extern
char	*_dmalloc_strchr(const char *str, const int ch);
extern
char	*_dmalloc_strrchr(const char *str, const int ch);
extern
char	*_dmalloc_strcpy(char *to, const char *from);
extern
char	*_dmalloc_strncpy(char *to, const char *from, const DMALLOC_SIZE len);
extern
int	_dmalloc_strcasecmp(const char *s1, const char *s2);
extern
int	_dmalloc_strncasecmp(const char *s1, const char *s2,
			     const DMALLOC_SIZE len);
extern
int	_dmalloc_strspn(const char *str, const char *list);
extern
int	_dmalloc_strcspn(const char *str, const char *list);
extern
char	*_dmalloc_strncat(char *to, const char *from, const DMALLOC_SIZE len);
extern
int	_dmalloc_strncmp(const char *s1, const char *s2,
			 const DMALLOC_SIZE len);
extern
char	*_dmalloc_strpbrk(const char *str, const char *list);
extern
char	*_dmalloc_strstr(const char *str, const char *pat);

#ifdef __cplusplus
}
#endif

#endif /* DMALLOC_FUNC_CHECK */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copied directly from dmalloc_lp.h, removed some routines
 */

/* internal dmalloc error number for reference purposes only */
extern
int	dmalloc_errno;

#ifdef __cplusplus
}
#endif

#endif /* ! __DMALLOC_H__ */
