#!/bin/sh

WD=`pwd`
filename="DBCONFIGFILE-html.top"

if [ "$1" != "" ]; then
  echo "<DBCONFIG>" > $filename
  echo "<DBENV> $1 </DBENV>" >> $filename
  echo "<FILEDEF TYPE=SGML>" >> $filename
  echo "<DEFAULTPATH> $WD </DEFAULTPATH>" >> $filename
  echo "<FILENAME> $WD/DATA </FILENAME>" >> $filename

  cp $filename DBCONFIGFILE-users.top
  
  echo "<FILETAG> cmetadata </FILETAG>" >> $filename
  echo "<FILETAG> cmeditors </FILETAG>" >> DBCONFIGFILE-users

  cp $filename DBCONFIGFILE-deletehtml.top
  cp DBCONFIGFILE-users DBCONFIGFILE-deleteuser.top

  echo "Built configuration files. Run buildnew.sh-html and buildnew.sh-user to build the databases."
  ls
  

else
  echo "Usage:  build_cf.sh DBENV_path"
  echo "  The DBENV_path should be the fully qualified path for the DBENV field."
  echo "  Eg:  /home/cheshire/cheshire/default"
fi




