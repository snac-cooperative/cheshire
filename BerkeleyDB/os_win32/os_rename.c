/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 1998, 1999, 2000
 *	Sleepycat Software.  All rights reserved.
 */

#include "db_config.h"

#ifndef lint
static const char revid[] = "$Id: os_rename.c,v 1.1.1.1 2000/08/12 08:29:18 silkworm Exp $";
#endif /* not lint */

#include "db_int.h"
#include "os_jump.h"

/*
 * __os_rename --
 *	Rename a file.
 */
int
__os_rename(dbenv, old, new)
	DB_ENV *dbenv;
	const char *old, *new;
{
	int ret;

	ret = 0;
	if (__db_jump.j_rename != NULL) {
		if (__db_jump.j_rename(old, new) == -1)
			ret = __os_get_errno();
	}
	else {
		if (MoveFileEx(old, new, MOVEFILE_REPLACE_EXISTING) != TRUE)
			ret = __os_win32_errno();
	}
	if (ret != 0)
		__db_err(dbenv, "Rename %s %s: %s", old, new, strerror(ret));

	return (ret);
}
