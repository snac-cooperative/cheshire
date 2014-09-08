// DO NOT EDIT: automatically built by dist/distrib.

package com.sleepycat.db;

public class DbConstants
{
	public static final int DB_MAX_PAGES = 0xffffffff;
	public static final int DB_MAX_RECORDS = 0xffffffff;
	public static final int DB_DBT_ISSET = 0x001;
	public static final int DB_DBT_MALLOC = 0x002;
	public static final int DB_DBT_PARTIAL = 0x004;
	public static final int DB_DBT_REALLOC = 0x008;
	public static final int DB_DBT_USERMEM = 0x010;
	public static final int DB_DBT_DUPOK = 0x020;
	public static final int DB_CXX_NO_EXCEPTIONS = 0x000001;
	public static final int DB_CLIENT = 0x000002;
	public static final int DB_XA_CREATE = 0x000002;
	public static final int DB_CREATE = 0x000001;
	public static final int DB_NOMMAP = 0x000002;
	public static final int DB_THREAD = 0x000004;
	public static final int DB_FORCE = 0x000008;
	public static final int DB_INIT_CDB = 0x000010;
	public static final int DB_INIT_LOCK = 0x000020;
	public static final int DB_INIT_LOG = 0x000040;
	public static final int DB_INIT_MPOOL = 0x000080;
	public static final int DB_INIT_TXN = 0x000100;
	public static final int DB_RECOVER = 0x000200;
	public static final int DB_RECOVER_FATAL = 0x000400;
	public static final int DB_SYSTEM_MEM = 0x000800;
	public static final int DB_TXN_NOSYNC = 0x001000;
	public static final int DB_USE_ENVIRON = 0x002000;
	public static final int DB_USE_ENVIRON_ROOT = 0x004000;
	public static final int DB_LOCKDOWN = 0x008000;
	public static final int DB_PRIVATE = 0x010000;
	public static final int DB_TXN_SYNC = 0x000001;
	public static final int DB_TXN_NOWAIT = 0x000002;
	public static final int DB_EXCL = 0x000008;
	public static final int DB_RDONLY = 0x000010;
	public static final int DB_TRUNCATE = 0x000020;
	public static final int DB_FCNTL_LOCKING = 0x000040;
	public static final int DB_ODDFILESIZE = 0x000080;
	public static final int DB_RDWRMASTER = 0x000100;
	public static final int DB_AGGRESSIVE = 0x0001;
	public static final int DB_NOORDERCHK = 0x0002;
	public static final int DB_ORDERCHKONLY = 0x0004;
	public static final int DB_PR_PAGE = 0x0008;
	public static final int DB_PR_HEADERS = 0x0010;
	public static final int DB_PR_RECOVERYTEST = 0x0020;
	public static final int DB_SALVAGE = 0x0040;
	public static final int DB_VRFY_FLAGMASK = 0xffff;
	public static final int DB_UPGRADE = 0x000001;
	public static final int DB_LOCK_NORUN = 0;
	public static final int DB_LOCK_DEFAULT = 1;
	public static final int DB_LOCK_OLDEST = 2;
	public static final int DB_LOCK_RANDOM = 3;
	public static final int DB_LOCK_YOUNGEST = 4;
	public static final int DB_DUP = 0x0001;
	public static final int DB_DUPSORT = 0x0002;
	public static final int DB_RECNUM = 0x0004;
	public static final int DB_RENUMBER = 0x0008;
	public static final int DB_REVSPLITOFF = 0x0010;
	public static final int DB_SNAPSHOT = 0x0020;
	public static final int DB_JOIN_NOSORT = 0x0001;
	public static final int DB_VERB_CHKPOINT = 0x0001;
	public static final int DB_VERB_DEADLOCK = 0x0002;
	public static final int DB_VERB_RECOVERY = 0x0004;
	public static final int DB_VERB_WAITSFOR = 0x0008;
	public static final int DB_TEST_PREOPEN = 1;
	public static final int DB_TEST_POSTOPEN = 2;
	public static final int DB_TEST_POSTLOGMETA = 3;
	public static final int DB_TEST_POSTLOG = 4;
	public static final int DB_TEST_POSTSYNC = 5;
	public static final int DB_TEST_PRERENAME = 6;
	public static final int DB_TEST_POSTRENAME = 7;
	public static final int DB_ENV_CDB = 0x00001;
	public static final int DB_ENV_CREATE = 0x00002;
	public static final int DB_ENV_DBLOCAL = 0x00004;
	public static final int DB_ENV_LOCKDOWN = 0x00008;
	public static final int DB_ENV_NOMMAP = 0x00010;
	public static final int DB_ENV_OPEN_CALLED = 0x00020;
	public static final int DB_ENV_PRIVATE = 0x00040;
	public static final int DB_ENV_RPCCLIENT = 0x00080;
	public static final int DB_ENV_STANDALONE = 0x00100;
	public static final int DB_ENV_SYSTEM_MEM = 0x00200;
	public static final int DB_ENV_THREAD = 0x00400;
	public static final int DB_ENV_TXN_NOSYNC = 0x00800;
	public static final int DB_ENV_USER_ALLOC = 0x01000;
	public static final int DB_BTREEVERSION = 8;
	public static final int DB_BTREEOLDVER = 6;
	public static final int DB_BTREEMAGIC = 0x053162;
	public static final int DB_HASHVERSION = 7;
	public static final int DB_HASHOLDVER = 4;
	public static final int DB_HASHMAGIC = 0x061561;
	public static final int DB_QAMVERSION = 2;
	public static final int DB_QAMOLDVER = 1;
	public static final int DB_QAMMAGIC = 0x042253;
	public static final int DB_LOGVERSION = 2;
	public static final int DB_LOGOLDVER = 2;
	public static final int DB_LOGMAGIC = 0x040988;
	public static final int DB_AFTER = 1;
	public static final int DB_APPEND = 2;
	public static final int DB_BEFORE = 3;
	public static final int DB_CACHED_COUNTS = 4;
	public static final int DB_CHECKPOINT = 5;
	public static final int DB_CONSUME = 6;
	public static final int DB_CURLSN = 7;
	public static final int DB_CURRENT = 8;
	public static final int DB_FIRST = 9;
	public static final int DB_FLUSH = 10;
	public static final int DB_GET_BOTH = 11;
	public static final int DB_GET_BOTHC = 12;
	public static final int DB_GET_RECNO = 13;
	public static final int DB_JOIN_ITEM = 14;
	public static final int DB_KEYFIRST = 15;
	public static final int DB_KEYLAST = 16;
	public static final int DB_LAST = 17;
	public static final int DB_NEXT = 18;
	public static final int DB_NEXT_DUP = 19;
	public static final int DB_NEXT_NODUP = 20;
	public static final int DB_NODUPDATA = 21;
	public static final int DB_NOOVERWRITE = 22;
	public static final int DB_NOSYNC = 23;
	public static final int DB_POSITION = 24;
	public static final int DB_POSITIONI = 25;
	public static final int DB_PREV = 26;
	public static final int DB_PREV_NODUP = 27;
	public static final int DB_RECORDCOUNT = 28;
	public static final int DB_SET = 29;
	public static final int DB_SET_RANGE = 30;
	public static final int DB_SET_RECNO = 31;
	public static final int DB_WRITECURSOR = 32;
	public static final int DB_WRITELOCK = 33;
	public static final int DB_OPFLAGS_MASK = 0x000000ff;
	public static final int DB_RMW = 0x80000000;
	public static final int DB_INCOMPLETE = -30999;
	public static final int DB_KEYEMPTY = -30998;
	public static final int DB_KEYEXIST = -30997;
	public static final int DB_LOCK_DEADLOCK = -30996;
	public static final int DB_LOCK_NOTGRANTED = -30995;
	public static final int DB_NOSERVER = -30994;
	public static final int DB_NOSERVER_HOME = -30993;
	public static final int DB_NOSERVER_ID = -30992;
	public static final int DB_NOTFOUND = -30991;
	public static final int DB_OLD_VERSION = -30990;
	public static final int DB_RUNRECOVERY = -30989;
	public static final int DB_VERIFY_BAD = -30988;
	public static final int DB_DELETED = -30899;
	public static final int DB_NEEDSPLIT = -30898;
	public static final int DB_SWAPBYTES = -30897;
	public static final int DB_TXN_CKP = -30896;
	public static final int DB_VERIFY_FATAL = -30895;
	public static final int DB_FILE_ID_LEN = 20;
	public static final int DB_LOGFILEID_INVALID = -1;
	public static final int DB_OK_BTREE = 0x01;
	public static final int DB_OK_HASH = 0x02;
	public static final int DB_OK_QUEUE = 0x04;
	public static final int DB_OK_RECNO = 0x08;
	public static final int DB_AM_DISCARD = 0x00001;
	public static final int DB_AM_DUP = 0x00002;
	public static final int DB_AM_DUPSORT = 0x00004;
	public static final int DB_AM_INMEM = 0x00008;
	public static final int DB_AM_PGDEF = 0x00010;
	public static final int DB_AM_RDONLY = 0x00020;
	public static final int DB_AM_RECOVER = 0x00040;
	public static final int DB_AM_SUBDB = 0x00080;
	public static final int DB_AM_SWAP = 0x00100;
	public static final int DB_BT_RECNUM = 0x00200;
	public static final int DB_BT_REVSPLIT = 0x00400;
	public static final int DB_DBM_ERROR = 0x00800;
	public static final int DB_OPEN_CALLED = 0x01000;
	public static final int DB_RE_DELIMITER = 0x02000;
	public static final int DB_RE_FIXEDLEN = 0x04000;
	public static final int DB_RE_PAD = 0x08000;
	public static final int DB_RE_RENUMBER = 0x10000;
	public static final int DB_RE_SNAPSHOT = 0x20000;
	public static final int DB_RECORD_LOCK = 1;
	public static final int DB_PAGE_LOCK = 2;
	public static final int DB_LOCKVERSION = 1;
	public static final int DB_LOCK_NOWAIT = 0x01;
	public static final int DB_LOCK_RECORD = 0x02;
	public static final int DB_LOCK_UPGRADE = 0x04;
	public static final int DB_LOCK_CONFLICT = 0x01;
	public static final int DB_LOCK_RW_N = 3;
	public static final int DB_LOCK_RIW_N = 6;
	public static final int DB_ARCH_ABS = 0x001;
	public static final int DB_ARCH_DATA = 0x002;
	public static final int DB_ARCH_LOG = 0x004;
	public static final int DB_MPOOL_CREATE = 0x001;
	public static final int DB_MPOOL_LAST = 0x002;
	public static final int DB_MPOOL_NEW = 0x004;
	public static final int DB_MPOOL_NEW_GROUP = 0x008;
	public static final int DB_MPOOL_CLEAN = 0x001;
	public static final int DB_MPOOL_DIRTY = 0x002;
	public static final int DB_MPOOL_DISCARD = 0x004;
	public static final int DB_TXNVERSION = 1;
	public static final int DB_TXN_BACKWARD_ROLL = 1;
	public static final int DB_TXN_FORWARD_ROLL = 2;
	public static final int DB_TXN_OPENFILES = 3;
	public static final int DB_TXN_REDO = 4;
	public static final int DB_TXN_UNDO = 5;
	public static final int DB_DBM_HSEARCH = 0;
	public static final int DB_VERSION_MAJOR = 3;
	public static final int DB_VERSION_MINOR = 1;
	public static final int DB_VERSION_PATCH = 14;
}
