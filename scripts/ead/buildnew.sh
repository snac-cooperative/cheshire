#!/bin/sh

cd ~/cheshire/ead_components
export PATH=~/cheshire/bin:$PATH

rm ead_components.assoc DATA.cont indexes/*.index indexes/*TEMP
buildassoc -r ~/cheshire/ead_components/DATA ead_components.assoc ead

cat CONF.TOP > DBCONFIGFILE
cat DATA.cont >> DBCONFIGFILE
cat CONF.BOTTOM >> DBCONFIGFILE


echo "Beginning Indexing..."
index_cheshire -b DBCONFIGFILE
echo "Done."

