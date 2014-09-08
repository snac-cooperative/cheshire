/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 1998, 1999, 2000
 *	Sleepycat Software.  All rights reserved.
 *
 *	$Id: DbException.java,v 1.1.1.1 2000/08/12 08:29:12 silkworm Exp $
 */

package com.sleepycat.db;

/**
 *
 * @author Donald D. Anderson
 */
public class DbException extends Exception
{
    // methods
    //

    public DbException(String s)
    {
        super(s);
    }

    public DbException(String s, int errno)
    {
        super(s);
        this.errno_ = errno;
    }

    public String toString()
    {
        String s = super.toString();
        if (errno_ == 0)
            return s;
        else
            return s + ": " + DbEnv.strerror(errno_);

    }

    // get/set methods
    //

    public int get_errno()
    {
        return errno_;
    }

    // private data
    //

    private int errno_ = 0;
}

// end of DbException.java
