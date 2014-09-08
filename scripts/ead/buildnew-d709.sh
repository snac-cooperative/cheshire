#!/bin/sh

cd ~/cheshire/ead_components
export PATH=~/cheshire/bin:$PATH

rm ead_components.assoc DATA.cont indexes/*.index indexes/*TEMP
buildassoc  ~/cheshire/ead_components/DATA/d709.sgm ead_components.assoc ead

echo "Beginning Indexing..."
index_cheshire -b DBCONFIGFILE.owen
echo "Done."

