rm *.assoc *.DOCSIZES core
rm indexes/*.index
rm INDEX_LOGFILE

buildassoc quickstep.sgml quickstep.assoc message
index_cheshire -b DBCONFIGFILE
