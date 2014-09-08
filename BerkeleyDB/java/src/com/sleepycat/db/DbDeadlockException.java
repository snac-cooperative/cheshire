/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1999, 2000
 *	Sleepycat Software.  All rights reserved.
 *
 *	$Id: DbDeadlockException.java,v 1.1.1.1 2000/08/12 08:29:12 silkworm Exp $
 */

package com.sleepycat.db;

public class DbDeadlockException extends DbException
{
    // methods
    //

    public DbDeadlockException(String s)
    {
        super(s);
    }

    public DbDeadlockException(String s, int errno)
    {
        super(s, errno);
    }
}

// end of DbDeadlockException.java
