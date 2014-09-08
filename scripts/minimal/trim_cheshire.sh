

rm -rf BerkeleyDB/
echo "Add -ldb-3.X to linker path"
rm -rf wordnet2/
rm -rf sdlip/
rm -rf jmcd_logtool/
rm -rf scripts/
rm -rf MakefileDM*
rm -rf Makefile.*
rm -rf CVS/
rm -rf config-gui/

cd ac_list
rm -rf CVS/
rm MakefileDM
cd ..

cd ac_utils
rm -rf CVS/
rm MakefileDM
cd ..

cd cheshire2
rm -rf GUI*
rm -rf CVS
rm *scp
rm *.tcl
rm explain*
rm melv*
mv test.c keep_me_test.c
rm test*
mv keep_me_test.c test.c
rm MakefileDM*
rm script*
rm search*
rm pages*
rm tilebars*
rm results NOTES-TODO opacper out.ps CONFIG.DL
cd ..

cd client
rm -rf CVS/
rm MakefileDM
cd ..

cd cmdparse
rm -rf CVS/
rm MakefileDM
cd ..

cd config
rm -rf CVS/
rm -rf OLDPOST/
rm -rf TESTDATA/
rm -rf TESTDL/
rm CONFIG.DL lc_outline.text TREC.DBCONFIGFILE
rm MakefileDM
rm testc*
rm -rf TEST*
cd ..

cd diagnostic
rm -rf CVS/
rm MakefileDM
cd ..

cd fileio
rm -rf CVS/
rm MakefileDM
rm dlrecs.test jmcd.assoc
rm math*
rm test*
cd ..

cd header
rm -rf CVS
rm tkzclient.log
cd ..

cd index
rm -rf CVS
rm -rf TESTDATA
rm -rf TESTDATA2
rm -rf TESTDL
rm -rf IMAGETEST
rm MakefileDM
rm *.dtd *.DTD
rm testrecs*
rm *.scp
rm *.tcl
rm CONFIG*.* *config*
rm DBCONFIGFILE testcatalog test.data
cd ..

cd jserver
rm -rf CVS
rm MakefileDM
rm *bak
cd ..

cd marc2sgml
rm -rf CVS
rm MakefileDM
rm scimarc scimarc2
cd ..

cd marclib
rm -rf CVS
rm MakefileDM
rm math*
cd ..

cd search
rm -rf CVS
rm MakefileDM Makefile.MYC
rm config* *CONFIG* testconfig.new
rm *README
rm tkzclient.log
rm trec_comments
cd ..

cd sgml2marc
rm -rf CVS
rm MakefileDM
rm shorttest2 test testdtd testrecs
rm NOTE recs.out
cd ..

cd sgmlparse
rm -rf CVS
rm -rf EAD_*
rm MakefileDM
rm *catalog
rm *.DTD *.dtd
rm CONFIG.* *config*
rm math*
rm dtd_results
rm test*
cd ..

cd socket
rm -rf CVS
rm MakefileDM
cd ..

cd tclhash
rm -rf CVS
rm MakefileDM
cd ..

cd utils
rm -rf CVS
rm MakefileDM
rm duprecs
rm testrecs* testsgmlmarc
cd ..

cd zpdu
rm -rf CVS
rm MakefileDM
cd ..

cd zserver
rm -rf CVS
rm MakefileDM
rm *.scp
rm server.init*
cd ..

echo "Cleaned directories."
















