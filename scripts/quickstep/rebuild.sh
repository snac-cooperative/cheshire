#!/bin/sh

rm INDEX_LOGFILE
buildassoc quickstep.sgml quickstep.assoc message
index_cheshire -b DBCONFIGFILE $1
